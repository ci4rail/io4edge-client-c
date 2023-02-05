#include "io4edge_internal.h"
#include "binaryiotypea.h"

static const char *TAG = "binaryiotypea";

io4e_err_t io4edge_binaryiotypea_upload_configuration(io4edge_functionblock_client_t *client,
    const BinaryIoTypeA__ConfigurationSet *configuration)
{
    IO4E_PACK_PROTOMSG(configuration, packed_req, binary_io_type_a, configuration_set);

    return io4e_functionblock_upload_configuration(client, &packed_req);
}

// caller must free the response in *configuration_p
io4e_err_t io4edge_binaryiotypea_download_configuration(io4edge_functionblock_client_t *client,
    BinaryIoTypeA__ConfigurationGetResponse **configuration_p)
{
    io4e_err_t err;
    IO4E_MAKE_EMPTY_REQUEST(empty_req, BinaryIoTypeA, BINARY_IO_TYPE_A, ConfigurationGet, CONFIGURATION_GET);
    IO4E_PACK_PROTOMSG(empty_req, packed_req, binary_io_type_a, configuration_get);

    if ((err = io4e_functionblock_download_configuration(client,
             &packed_req,
             (io4e_unpack_t)binary_io_type_a__configuration_get_response__unpack,
             (void **)configuration_p)) != IO4E_OK) {
        return err;
    }
    return IO4E_OK;
}
