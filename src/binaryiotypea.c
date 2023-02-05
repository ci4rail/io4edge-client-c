#include "google/protobuf/any.pb-c.h"
#include "io4edge_internal.h"
#include "binaryiotypea.h"

static const char *TAG = "binaryiotypea";

io4e_err_t io4edge_binaryiotypea_upload_configuration(io4edge_functionblock_client_t *client,
    const BinaryIoTypeA__ConfigurationSet *configuration)
{
    IO4E_ALLOC_ON_STACK_AND_PACK_PROTOMSG(configuration, packed_req, binary_io_type_a, configuration_set);

    return io4e_functionblock_upload_configuration(client, &packed_req);
}

io4e_err_t io4edge_binaryiotypea_download_configuration(io4edge_functionblock_client_t *client,
    BinaryIoTypeA__ConfigurationGetResponse *configuration)
{
    io4e_err_t err;
    BinaryIoTypeA__ConfigurationGet empty_req = BINARY_IO_TYPE_A__CONFIGURATION_GET__INIT;
    BinaryIoTypeA__ConfigurationGet *empty_req_p = &empty_req;
    IO4E_ALLOC_ON_STACK_AND_PACK_PROTOMSG(empty_req_p, packed_req, binary_io_type_a, configuration_get);

    if ((err = io4e_functionblock_download_configuration(client,
             &packed_req,
             (io4e_unpack_t)binary_io_type_a__configuration_get_response__unpack,
             (io4e_free_unpacked_t)binary_io_type_a__configuration_get_response__free_unpacked,
             (void *)&configuration)) != IO4E_OK) {
        return err;
    }
    return IO4E_OK;
}
