#ifndef IO4EDGE_CLIENT_H

#define IO4E_OK 0
#define IO4E_FAIL -1
#define IO4E_ERR_TIMEOUT 1
#define IO4E_ERR_NO_MEM 2

typedef int io4e_err_t;

#ifndef IO4EDGE_INTERNAL_H
typedef void io4edge_functionblock_client_t;
#endif

io4e_err_t io4edge_client_new(io4edge_functionblock_client_t **handle_p, const char *host, int port);

#endif
