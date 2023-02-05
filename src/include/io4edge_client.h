#pragma once
#include <stdbool.h>
#define IO4E_OK 0
#define IO4E_FAIL -1
#define IO4E_ERR_TIMEOUT 1
#define IO4E_ERR_NO_MEM 2
#define IO4E_ERR_PROTOBUF 3
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

io4e_err_t io4edge_functionblock_client_new(io4edge_functionblock_client_t **handle_p, const char *host, int port);
io4e_err_t io4edge_functionblock_client_delete(io4edge_functionblock_client_t **handle_p);
