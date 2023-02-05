#include "io4edge_client.h"
#include "binaryIoTypeA/protobuf-c/binaryIoTypeA/v1alpha1/binaryIoTypeA.pb-c.h"

io4e_err_t io4edge_binaryiotypea_upload_configuration(io4edge_functionblock_client_t *client,
    const BinaryIoTypeA__ConfigurationSet *configuration);
io4e_err_t io4edge_binaryiotypea_download_configuration(io4edge_functionblock_client_t *client,
    BinaryIoTypeA__ConfigurationGetResponse *configuration);
