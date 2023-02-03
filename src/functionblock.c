#include "io4edge_internal.h"

io4e_err_t io4edge_client_new(io4edge_functionblock_client_t **handle_p, const char *host, int port)
{
    transport_t *transport;
    return io4edge_transport_new(host, port, &transport);
}
