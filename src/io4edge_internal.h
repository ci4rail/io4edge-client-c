#pragma once
#define IO4EDGE_INTERNAL_H

typedef struct io4edge_functionblock_client_t io4edge_functionblock_client_t;

#include <io4edge_client.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "logging.h"
#include "api/io4edge/protobuf-c/functionblock/v1alpha1/io4edge_functionblock.pb-c.h"

// Transport interface
typedef struct transport_t transport_t;
struct transport_t {
    io4e_err_t (*write_msg)(transport_t *self, uint8_t *buf, uint32_t len);
    io4e_err_t (*read_msg)(transport_t *self, uint8_t *buf, uint32_t buf_size, uint32_t *len_p, int timeout_seconds);
    uint32_t (*get_write_offset)(transport_t *self);
    void (*destroy)(transport_t **self_p);
};
io4e_err_t io4edge_transport_new(const char *host, int port, transport_t **handle_p);

struct io4edge_functionblock_client_t {
    transport_t *transport;
    int cmd_context;  // id of the last command sent
    int cmd_timeout;  // timeout for commands in seconds
};

// functionblock client
typedef struct {
    uint8_t *marshaled_msg;
    size_t marshaled_msg_len;
    const char *proto_name;
} io4e_protomsg_t;

io4e_err_t io4e_functionblock_upload_configuration(io4edge_functionblock_client_t *client, io4e_protomsg_t *fs_config);

#define IO4E_ALLOC_ON_STACK_AND_PACK_PROTOMSG(m, PREFIX, PROTONAME)   \
    size_t _packed_len = PREFIX##__##PROTONAME##__get_packed_size(m); \
    uint8_t _buffer[_packed_len];                                     \
    PREFIX##__##PROTONAME##__pack(m, _buffer);                        \
    io4e_protomsg_t packed_msg = {_buffer, _packed_len, PREFIX##__configuration_set__descriptor.name}
