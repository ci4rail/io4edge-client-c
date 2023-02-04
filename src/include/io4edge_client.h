#pragma once
#define IO4E_OK 0
#define IO4E_FAIL -1
#define IO4E_ERR_TIMEOUT 1
#define IO4E_ERR_NO_MEM 2
#define IO4E_ERR_PROTOBUF 3

typedef int io4e_err_t;

#ifndef IO4EDGE_INTERNAL_H
typedef void io4edge_functionblock_client_t;
#endif

io4e_err_t io4edge_functionblock_client_new(io4edge_functionblock_client_t **handle_p, const char *host, int port);
io4e_err_t io4edge_functionblock_client_delete(io4edge_functionblock_client_t **handle_p);
