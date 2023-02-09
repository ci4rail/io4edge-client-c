#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <protobuf-c/protobuf-c.h>
#include "io4edge_api/io4edge_functionblock.pb-c.h"

#define IO4E_OK 0
#define IO4E_FAIL -1
#define IO4E_ERR_TIMEOUT 1
#define IO4E_ERR_NO_MEM 2
#define IO4E_ERR_PROTOBUF 3
#define IO4E_ERR_CONNECTION_CLOSED 4
#define IO4E_ERR_FB_UNSPECIFIC_ERROR 10
#define IO4E_ERR_FB_UNKNOWN_COMMAND 11
#define IO4E_ERR_FB_NOT_IMPLEMENTED 12
#define IO4E_ERR_FB_WRONG_CLIENT 13
#define IO4E_ERR_FB_INVALID_PARAMETER 14
#define IO4E_ERR_FB_HW_FAULT 15
#define IO4E_ERR_FB_STREAM_ALREADY_STARTED 16
#define IO4E_ERR_FB_STREAM_ALREADY_STOPPED 17
#define IO4E_ERR_FB_STREAM_START_FAILED 18
#define IO4E_ERR_FB_TEMPORARILY_UNAVAILABLE 19
#define IO4E_ERR_FB_UNKNOWN 99

typedef int io4e_err_t;

#ifndef IO4EDGE_INTERNAL_H
typedef void io4edge_functionblock_client_t;
#endif

typedef void *(*io4edge_unpack_t)(ProtobufCAllocator *allocator, size_t len, const uint8_t *data);

io4e_err_t io4edge_functionblock_client_new_from_host_port(io4edge_functionblock_client_t **handle_p,
    const char *host,
    int port,
    int cmd_timeout_seconds);
io4e_err_t io4edge_functionblock_client_delete(io4edge_functionblock_client_t **handle_p);
io4e_err_t io4edge_functionblock_stop_stream(io4edge_functionblock_client_t *h);
io4e_err_t io4edge_functionblock_read_stream(io4edge_functionblock_client_t *h,
    io4edge_unpack_t unpack,
    Functionblock__StreamData **sd_p,
    void **fs_data_p,
    long timeout);

#ifdef __cplusplus
}
#endif
