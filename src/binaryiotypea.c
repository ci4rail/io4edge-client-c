#include "binaryIoTypeA/protobuf-c/binaryIoTypeA/v1alpha1/binaryIoTypeA.pb-c.h"
#include "io4edge_client.h"
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

io4e_err_t io4edge_binaryiotypea_describe(io4edge_functionblock_client_t *client,
    BinaryIoTypeA__ConfigurationDescribeResponse **description_p)
{
    io4e_err_t err;
    IO4E_MAKE_EMPTY_REQUEST(empty_req, BinaryIoTypeA, BINARY_IO_TYPE_A, ConfigurationDescribe, CONFIGURATION_DESCRIBE);
    IO4E_PACK_PROTOMSG(empty_req, packed_req, binary_io_type_a, configuration_describe);

    if ((err = io4e_functionblock_describe(client,
             &packed_req,
             (io4e_unpack_t)binary_io_type_a__configuration_describe_response__unpack,
             (void **)description_p)) != IO4E_OK) {
        return err;
    }
    return IO4E_OK;
}

io4e_err_t io4edge_binaryiotypea_set_output(io4edge_functionblock_client_t *client, uint32_t channel, bool state)
{
    BinaryIoTypeA__FunctionControlSet set = BINARY_IO_TYPE_A__FUNCTION_CONTROL_SET__INIT;
    set.type_case = BINARY_IO_TYPE_A__FUNCTION_CONTROL_SET__TYPE_SINGLE;
    BinaryIoTypeA__SetSingle set_single = BINARY_IO_TYPE_A__SET_SINGLE__INIT;
    set.single = &set_single;
    set_single.channel = channel;
    set_single.state = state;
    IO4E_PACK_PROTOMSG_P(set, packed_req, binary_io_type_a, function_control_set);

    return io4e_functionblock_function_control_set(client, &packed_req, NULL, NULL);
}

io4e_err_t io4edge_binaryiotypea_set_all_outputs(io4edge_functionblock_client_t *client, uint32_t states, uint32_t mask)
{
    BinaryIoTypeA__FunctionControlSet set = BINARY_IO_TYPE_A__FUNCTION_CONTROL_SET__INIT;
    set.type_case = BINARY_IO_TYPE_A__FUNCTION_CONTROL_SET__TYPE_ALL;
    BinaryIoTypeA__SetAll set_all = BINARY_IO_TYPE_A__SET_ALL__INIT;
    set.all = &set_all;
    set_all.values = states;
    set_all.mask = mask;
    IO4E_PACK_PROTOMSG_P(set, packed_req, binary_io_type_a, function_control_set);

    return io4e_functionblock_function_control_set(client, &packed_req, NULL, NULL);
}

io4e_err_t io4edge_binaryiotypea_input(io4edge_functionblock_client_t *client, uint32_t channel, bool *state_p)
{
    io4e_err_t err;
    BinaryIoTypeA__FunctionControlGet get = BINARY_IO_TYPE_A__FUNCTION_CONTROL_GET__INIT;
    get.type_case = BINARY_IO_TYPE_A__FUNCTION_CONTROL_GET__TYPE_SINGLE;
    BinaryIoTypeA__GetSingle get_single = BINARY_IO_TYPE_A__GET_SINGLE__INIT;
    get.single = &get_single;
    get_single.channel = channel;
    IO4E_PACK_PROTOMSG_P(get, packed_req, binary_io_type_a, function_control_get);

    BinaryIoTypeA__FunctionControlGetResponse *response;
    if ((err = io4e_functionblock_function_control_get(client,
             &packed_req,
             (io4e_unpack_t)binary_io_type_a__function_control_get_response__unpack,
             (void **)&response)) != IO4E_OK) {
        return err;
    }
    if (response->type_case != BINARY_IO_TYPE_A__FUNCTION_CONTROL_GET_RESPONSE__TYPE_SINGLE) {
        IO4E_LOGE(TAG, "Unexpected response type %d", response->type_case);
        binary_io_type_a__function_control_get_response__free_unpacked(response, NULL);
        return IO4E_ERR_PROTOBUF;
    }

    *state_p = response->single->state;
    binary_io_type_a__function_control_get_response__free_unpacked(response, NULL);

    return IO4E_OK;
}
