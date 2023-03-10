/*
 * User header file for the io4edge client library.
 *
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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

// Error codes returned by the io4edge client library
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

// Type for return values
typedef int io4e_err_t;

#ifndef IO4EDGE_INTERNAL_H
typedef void io4edge_functionblock_client_t;
#endif

// Function pointer to unpack a protobuf message
typedef void *(*io4edge_unpack_t)(ProtobufCAllocator *allocator, size_t len, const uint8_t *data);

/*
 * @brief Create a new io4edge functionblock client.
 *
 * Connects to an io4edge function block server and creates a new client. The client handle must
 * be passed to all other functions.
 *
 * @param handle_p[out] Pointer to the handle that is filled with the new client.
 * @param host Hostname or IP address of the io4edge server.
 * @param port Port of the io4edge server.
 * @param cmd_timeout_seconds Sets the timeout for commands in seconds.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_functionblock_client_new_from_host_port(io4edge_functionblock_client_t **handle_p,
    const char *host,
    int port,
    int cmd_timeout_seconds);

/*
 * @brief Delete an io4edge functionblock client.
 * @param handle_p[in,out] Pointer to the handle that is filled with NULL.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_functionblock_client_delete(io4edge_functionblock_client_t **handle_p);

/*
 * @brief Stop Functionblock Stream
 * @param h[in] Functionblock client handle
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_functionblock_stop_stream(io4edge_functionblock_client_t *h);

/*
 * @brief Read next bucket from stream
 * @param h Functionblock client handle
 * @param unpack Function pointer to unpack the functionblock specific stream data message
 * @param sd_p[out] Pointer to the functionblock generic stream data message (filled with NULL if no data is available)
 * @param fs_data_p[out] Pointer to the unpacked functionblock specific stream data message (filled with NULL if no data
 * is available)
 * @param timeout Timeout in seconds to wait for next bucket
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_functionblock_read_stream(io4edge_functionblock_client_t *h,
    io4edge_unpack_t unpack,
    Functionblock__StreamData **sd_p,
    void **fs_data_p,
    long timeout);

#ifdef __cplusplus
}
#endif
