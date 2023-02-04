#include "io4edge_internal.h"
#include "api/io4edge/protobuf-c/functionblock/v1alpha1/io4edge_functionblock.pb-c.h"

io4e_err_t io4edge_functionblock_client_new(io4edge_functionblock_client_t **handle_p, const char *host, int port)
{
    io4e_err_t err;
    // allocate memory for the functionblock client
    io4edge_functionblock_client_t *h = calloc(1, sizeof(io4edge_functionblock_client_t));
    if (h == NULL) {
        return IO4E_ERR_NO_MEM;
    }

    if ((err = io4edge_transport_new(host, port, &h->transport)) != IO4E_OK)
        goto ERREXIT;

    *handle_p = h;
ERREXIT:
    if (err != IO4E_OK) {
        free(h);
    }
    return err;
}

io4e_err_t io4edge_functionblock_client_delete(io4edge_functionblock_client_t **handle_p)
{
    if (*handle_p != NULL) {
        io4edge_functionblock_client_t *h = *handle_p;
        h->transport->destroy(&h->transport);
        free(h);
        *handle_p = NULL;
    }
    return IO4E_OK;
}
