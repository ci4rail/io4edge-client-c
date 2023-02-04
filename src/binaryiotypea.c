#include "io4edge_internal.h"
#include "api/binaryIoTypeA/protobuf-c/binaryIoTypeA/v1alpha1/binaryIoTypeA.pb-c.h"

io4e_err_t io4edge_binaryiotypea_upload_configuration(io4edge_functionblock_client_t *client,
    const BinaryIoTypeA__ConfigurationSet *configuration)
{
    io4e_err_t err;
    size_t packed_size = binary_io_type_a__configuration_set__get_packed_size(configuration);
    uint8_t buffer[packed_size];
    binary_io_type_a__configuration_set__pack(configuration, buffer);

    return io4e_functionblock_upload_configuration(
        client, buffer, packed_size, binary_io_type_a__configuration_set__descriptor.name);
}
