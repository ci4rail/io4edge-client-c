#include "io4edge_client.h"
#include "io4edge_api/binaryIoTypeA.pb-c.h"
#include "io4edge_api/io4edge_functionblock.pb-c.h"

io4e_err_t io4edge_binaryiotypea_upload_configuration(io4edge_functionblock_client_t *client,
    const BinaryIoTypeA__ConfigurationSet *configuration);
io4e_err_t io4edge_binaryiotypea_download_configuration(io4edge_functionblock_client_t *client,
    BinaryIoTypeA__ConfigurationGetResponse **configuration_p);
io4e_err_t io4edge_binaryiotypea_describe(io4edge_functionblock_client_t *client,
    BinaryIoTypeA__ConfigurationDescribeResponse **description_p);
io4e_err_t io4edge_binaryiotypea_set_output(io4edge_functionblock_client_t *client, uint32_t channel, bool state);
io4e_err_t io4edge_binaryiotypea_set_all_outputs(io4edge_functionblock_client_t *client,
    uint32_t states,
    uint32_t mask);
io4e_err_t io4edge_binaryiotypea_exit_error_state(io4edge_functionblock_client_t *client);
io4e_err_t io4edge_binaryiotypea_input(io4edge_functionblock_client_t *client, uint32_t channel, bool *state_p);
io4e_err_t io4edge_binaryiotypea_all_inputs(io4edge_functionblock_client_t *client, uint32_t mask, uint32_t *states_p);
io4e_err_t io4edge_binaryiotypea_start_stream(io4edge_functionblock_client_t *client,
    uint32_t channelfiltermask,
    Functionblock__StreamControlStart *fb_config);
