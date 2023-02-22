/*
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <io4edge_internal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#define FSTREAM_HEADER_SIZE 6 /* 2 bytes magic, 4 bytes length */
static uint8_t magic_bytes[] = {0xFE, 0xED};

typedef struct {
    transport_t methods;
    int socket;
} handle_t;

static const char *TAG = "transport";

static io4e_err_t read_complete(int sock, uint8_t *buf, uint32_t len)
{
    int ret;
    while (len > 0) {
        ret = read(sock, buf, len);

        if (ret == 0) {
            return IO4E_ERR_CONNECTION_CLOSED;
        }
        if (ret < 0) {
            return errno;
        }
        buf += ret;
        len -= ret;
    }
    return IO4E_OK;
}

static io4e_err_t read_msg(transport_t *handle, uint8_t **buf_p, uint32_t *len_p, int timeout_seconds)
{
    uint8_t hdr[FSTREAM_HEADER_SIZE];
    io4e_err_t ret;
    uint32_t len;
    handle_t *h = (handle_t *)handle;
    uint8_t *payload = NULL;

    *buf_p = NULL;

    // use select to check if there is data to read
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(h->socket, &readfds);
    struct timeval timeout;
    timeout.tv_sec = timeout_seconds;
    timeout.tv_usec = 0;
    ret = select(h->socket + 1, &readfds, NULL, NULL, &timeout);
    if (ret == 0) {
        return IO4E_ERR_TIMEOUT;
    }

    ret = read_complete(h->socket, hdr, FSTREAM_HEADER_SIZE);
    if (ret == IO4E_ERR_CONNECTION_CLOSED) {
        return IO4E_ERR_CONNECTION_CLOSED;
    }
    if ((ret == EAGAIN) || (ret == EWOULDBLOCK)) {
        return IO4E_ERR_TIMEOUT;
    }
    if (ret != IO4E_OK) {
        IO4E_LOGE(TAG, "read_msg failed to read header: %d", ret);
        return ret;
    }

    if (memcmp(hdr, magic_bytes, sizeof(magic_bytes)) != 0) {
        IO4E_LOGE(TAG, "read_msg magic bytes not found");
        return IO4E_FAIL;
    }

    len = hdr[sizeof(magic_bytes)];
    len |= ((uint32_t)hdr[sizeof(magic_bytes) + 1]) << 8;
    len |= ((uint32_t)hdr[sizeof(magic_bytes) + 2]) << 16;
    len |= ((uint32_t)hdr[sizeof(magic_bytes) + 3]) << 24;

    if (len > 0) {
        payload = malloc(len);
        if (payload == NULL) {
            IO4E_LOGE(TAG, "read_msg failed to allocate payload");
            return IO4E_ERR_NO_MEM;
        }

        ret = read_complete(h->socket, payload, len);
        if (ret == IO4E_ERR_CONNECTION_CLOSED) {
            free(payload);
            return IO4E_ERR_CONNECTION_CLOSED;
        }
        if (ret != IO4E_OK) {
            IO4E_LOGE(TAG, "read_msg failed to read payload: %d", ret);
            free(payload);
            return ret;
        }
    }
    *len_p = len;
    *buf_p = payload;
    return IO4E_OK;
}

static uint32_t get_write_offset(transport_t *handle)
{
    return FSTREAM_HEADER_SIZE;
}

static io4e_err_t write_msg(transport_t *handle, uint8_t *buf, uint32_t len)
{
    handle_t *h = (handle_t *)handle;
    if (len < FSTREAM_HEADER_SIZE)
        return IO4E_FAIL;

    memcpy(buf, magic_bytes, sizeof(magic_bytes));
    // Assuming caller left space for FSTREAM header
    uint32_t payload_len = len - FSTREAM_HEADER_SIZE;
    buf[sizeof(magic_bytes)] = payload_len & 0xff;
    buf[sizeof(magic_bytes) + 1] = (payload_len >> 8) & 0xff;
    buf[sizeof(magic_bytes) + 2] = (payload_len >> 16) & 0xff;
    buf[sizeof(magic_bytes) + 3] = (payload_len >> 16) & 0xff;

    while (len > 0) {
        int ret = write(h->socket, buf, len);

        if (ret < 0) {
            if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
                IO4E_LOGE(TAG, "write_msg failed to write: %d", errno);
                return IO4E_FAIL;
            } else
                ret = 0;
        }
        buf += ret;
        len -= ret;
    }

    return IO4E_OK;
}

static void destroy(transport_t **handle_p)
{
    if (*handle_p != NULL) {
        handle_t *h = (handle_t *)(*handle_p);
        close(h->socket);
        free(h);
        *handle_p = NULL;
    }
}

static io4e_err_t transport_connect(const char *host, int port, int *socket_p)
{
    struct hostent *hostent;
    struct sockaddr_in dest_addr;
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    hostent = gethostbyname(host);
    if (hostent == NULL) {
        IO4E_LOGE(TAG, "gethostbyname failed");
        return IO4E_FAIL;
    }
    struct in_addr **addr_list = (struct in_addr **)hostent->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++) {
        IO4E_LOGD(TAG, "Host IP addr %s ", inet_ntoa(*addr_list[i]));
    }

    dest_addr.sin_addr.s_addr = (*addr_list[0]).s_addr;  // just take the first IP address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    IO4E_LOGD(TAG, "Socket creation");
    int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        IO4E_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return IO4E_FAIL;
    }
    IO4E_LOGD(TAG, "Socket created, connecting to port %d", port);

    if (connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in)) != 0) {
        IO4E_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(sock);
        return IO4E_FAIL;
    }
    IO4E_LOGI(TAG, "Successfully connected to %s:%d", host, port);

    int nodelay = 1;
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&nodelay, sizeof(int)) < 0) {
        IO4E_LOGW(TAG, "Can't disable nagle");
    }
    *socket_p = sock;
    return IO4E_OK;
}

io4e_err_t io4edge_transport_new(const char *host, int port, transport_t **handle_p)
{
    handle_t *h = calloc(1, sizeof(handle_t));
    *handle_p = NULL;
    if (h == NULL) {
        IO4E_LOGE(TAG, "Could not allocate memory for io4edge_new_fstream handle");
        return IO4E_ERR_NO_MEM;
    }
    h->methods.read_msg = read_msg;
    h->methods.write_msg = write_msg;
    h->methods.get_write_offset = get_write_offset;
    h->methods.destroy = destroy;

    io4e_err_t ret = transport_connect(host, port, &h->socket);
    if (ret != IO4E_OK) {
        IO4E_LOGE(TAG, "Could not connect to %s:%d", host, port);
        free(h);
        return ret;
    }
    *handle_p = (transport_t *)h;
    return IO4E_OK;
}
