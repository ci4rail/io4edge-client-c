/*
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "io4edge_internal.h"
#include "io4edge_analogintypea.h"

static const char *TAG = "analogintypea";

io4e_err_t io4edge_analogintypea_upload_configuration(io4edge_functionblock_client_t *client,
    const AnalogInTypeA__ConfigurationSet *configuration)
{
    IO4E_PACK_PROTOMSG(configuration, packed_req, analog_in_type_a, configuration_set);

    return io4e_functionblock_upload_configuration(client, &packed_req);
}

// caller must free the response in *configuration_p
io4e_err_t io4edge_analogintypea_download_configuration(io4edge_functionblock_client_t *client,
    AnalogInTypeA__ConfigurationGetResponse **configuration_p)
{
    io4e_err_t err;
    IO4E_MAKE_EMPTY_REQUEST(empty_req, AnalogInTypeA, ANALOG_IN_TYPE_A, ConfigurationGet, CONFIGURATION_GET);
    IO4E_PACK_PROTOMSG(empty_req, packed_req, analog_in_type_a, configuration_get);

    if ((err = io4e_functionblock_download_configuration(client,
             &packed_req,
             (io4edge_unpack_t)analog_in_type_a__configuration_get_response__unpack,
             (void **)configuration_p)) != IO4E_OK) {
        return err;
    }
    return IO4E_OK;
}

io4e_err_t io4edge_analogintypea_value(io4edge_functionblock_client_t *client, float *value_p)
{
    io4e_err_t err;
    IO4E_MAKE_EMPTY_REQUEST(empty_req, AnalogInTypeA, ANALOG_IN_TYPE_A, FunctionControlGet, FUNCTION_CONTROL_GET);
    IO4E_PACK_PROTOMSG(empty_req, packed_req, analog_in_type_a, function_control_get);

    AnalogInTypeA__FunctionControlGetResponse *response;
    if ((err = io4e_functionblock_function_control_get(client,
             &packed_req,
             (io4edge_unpack_t)analog_in_type_a__function_control_get_response__unpack,
             (void **)&response)) != IO4E_OK) {
        return err;
    }
    *value_p = response->value;
    analog_in_type_a__function_control_get_response__free_unpacked(response, NULL);
    return IO4E_OK;
}

io4e_err_t io4edge_analogintypea_start_stream(io4edge_functionblock_client_t *client,
    Functionblock__StreamControlStart *fb_config)
{
    AnalogInTypeA__StreamControlStart start = ANALOG_IN_TYPE_A__STREAM_CONTROL_START__INIT;
    IO4E_PACK_PROTOMSG_P(start, packed_req, analog_in_type_a, stream_control_start);

    return io4e_functionblock_start_stream(client, &packed_req, fb_config);
}
