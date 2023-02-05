#include "io4edge_internal.h"
#include "binaryiotypea.h"

io4e_err_t io4edge_binaryiotypea_upload_configuration(io4edge_functionblock_client_t *client,
    const BinaryIoTypeA__ConfigurationSet *configuration)
{
    IO4E_ALLOC_ON_STACK_AND_PACK_PROTOMSG(configuration, binary_io_type_a, configuration_set);

    return io4e_functionblock_upload_configuration(client, &packed_msg);
}
