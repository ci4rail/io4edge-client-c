/*
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#define IO4EDGE_INTERNAL_H

typedef struct io4edge_functionblock_client_t io4edge_functionblock_client_t;

#include <io4edge_client.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include "logging.h"

#define CONFIG_STREAMQ_CAPACITY 100
#define IO4E_FOREVER 10000000000  // more than 200 years

// Transport interface
typedef struct transport_t transport_t;
struct transport_t {
    io4e_err_t (*write_msg)(transport_t *self, uint8_t *buf, uint32_t len);
    io4e_err_t (*read_msg)(transport_t *handle, uint8_t **buf_p, uint32_t *len_p, int timeout_seconds);
    uint32_t (*get_write_offset)(transport_t *self);
    void (*destroy)(transport_t **self_p);
};
io4e_err_t io4edge_transport_new(const char *host, int port, transport_t **handle_p);

// stream queue
typedef struct {
    void **msg;             // message array
    size_t capacity;        // capacity of the array
    int write_idx;          // index of the next message to be written
    int read_idx;           // index of the next message to be read
    int nentries;           // current number of entries in the queue
    sem_t write_sem;        // semaphore that counts the number of free messages in the queue
    sem_t read_sem;         // semaphore that counts the number of messages in the queue
    pthread_mutex_t mutex;  // mutex to protect the queue
} streamq_t;

// functionblock client
struct io4edge_functionblock_client_t {
    transport_t *transport;
    char cmd_context[2];                    // id of the last command sent as a single char string
    int cmd_timeout;                        // timeout for commands in seconds
    pthread_t read_thread_id;               // thread id to handle responses
    pthread_mutex_t cmd_mutex;              // mutex to protect command from reentrance
    bool read_thread_stop;                  // flag to stop the read thread
    Functionblock__Response *cmd_response;  // response of the last command
    sem_t cmd_sem;                          // semaphore to signal cmd_response
    streamq_t *streamq;                     // queue for incoming stream messages
};

// Protobuf marshaling
typedef struct {
    uint8_t *marshaled_msg;
    size_t marshaled_msg_len;
    const char *proto_name;
} io4e_protomsg_t;

typedef void *(*io4edge_unpack_t)(ProtobufCAllocator *allocator, size_t len, const uint8_t *data);

io4e_err_t io4e_functionblock_upload_configuration(io4edge_functionblock_client_t *client, io4e_protomsg_t *fs_config);
io4e_err_t io4e_functionblock_download_configuration(io4edge_functionblock_client_t *client,
    io4e_protomsg_t *fs_req,
    io4edge_unpack_t unpack,
    void **fs_res_p);
io4e_err_t io4e_functionblock_describe(io4edge_functionblock_client_t *client,
    io4e_protomsg_t *fs_req,
    io4edge_unpack_t unpack,
    void **fs_res_p);
io4e_err_t io4e_functionblock_function_control_set(io4edge_functionblock_client_t *client,
    io4e_protomsg_t *fs_req,
    io4edge_unpack_t unpack,
    void **fs_res_p);
io4e_err_t io4e_functionblock_function_control_get(io4edge_functionblock_client_t *client,
    io4e_protomsg_t *fs_req,
    io4edge_unpack_t unpack,
    void **fs_res_p);
io4e_err_t io4e_functionblock_start_stream(io4edge_functionblock_client_t *h,
    io4e_protomsg_t *fs_config,
    Functionblock__StreamControlStart *fb_config);

// Pack the UNPACKEDMSG into a buffer on the stack and put the pack results in PACKEDMSG.
#define IO4E_PACK_PROTOMSG(UNPACKEDMSG, PACKEDMSG, PREFIX, PROTONAME)                        \
    size_t UNPACKEDMSG##_packed_len = PREFIX##__##PROTONAME##__get_packed_size(UNPACKEDMSG); \
    uint8_t UNPACKEDMSG##_buffer[UNPACKEDMSG##_packed_len];                                  \
    PREFIX##__##PROTONAME##__pack(UNPACKEDMSG, UNPACKEDMSG##_buffer);                        \
    io4e_protomsg_t PACKEDMSG = {                                                            \
        UNPACKEDMSG##_buffer, UNPACKEDMSG##_packed_len, PREFIX##__##PROTONAME##__descriptor.name}

// Pack the UNPACKEDMSG pointer into a buffer on the stack and put the pack results in PACKEDMSG.
#define IO4E_PACK_PROTOMSG_P(UNPACKEDMSG, PACKEDMSG, PREFIX, PROTONAME)                       \
    size_t UNPACKEDMSG##_packed_len = PREFIX##__##PROTONAME##__get_packed_size(&UNPACKEDMSG); \
    uint8_t UNPACKEDMSG##_buffer[UNPACKEDMSG##_packed_len];                                   \
    PREFIX##__##PROTONAME##__pack(&UNPACKEDMSG, UNPACKEDMSG##_buffer);                        \
    io4e_protomsg_t PACKEDMSG = {                                                             \
        UNPACKEDMSG##_buffer, UNPACKEDMSG##_packed_len, PREFIX##__##PROTONAME##__descriptor.name}

// Make an empty request message for the given PROTONAME.
#define IO4E_MAKE_EMPTY_REQUEST(REQ, PREFIX_CAMELCASE, PREFIX_UPPERCASE, PROTONAME_CAMELCASE, PROTONAME_UPPERCASE) \
    PREFIX_CAMELCASE##__##PROTONAME_CAMELCASE _##REQ = PREFIX_UPPERCASE##__##PROTONAME_UPPERCASE##__INIT;          \
    PREFIX_CAMELCASE##__##PROTONAME_CAMELCASE *REQ;                                                                \
    REQ = &_##REQ

io4e_err_t io4e_streamq_new(size_t nentries, streamq_t **q_p);
void io4e_streamq_delete(streamq_t **q_p);
io4e_err_t io4e_streamq_push(streamq_t *q, void *msg, long timeout);
io4e_err_t io4e_streamq_pop(streamq_t *q, void **msg_p, long timeout);
size_t io4e_streamq_entries(streamq_t *q);
