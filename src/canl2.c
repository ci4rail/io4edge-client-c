/*
 * SPDX-FileCopyrightText: 2024 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "io4edge_internal.h"
#include "io4edge_canl2.h"

static const char *TAG = "canl2";

io4e_err_t io4edge_canl2_upload_configuration(io4edge_functionblock_client_t *client,
    const CanL2__ConfigurationSet *configuration)
{
    IO4E_PACK_PROTOMSG(configuration, packed_req, can_l2, configuration_set);

    return io4e_functionblock_upload_configuration(client, &packed_req);
}

// caller must free the response in *configuration_p
io4e_err_t io4edge_canl2_download_configuration(io4edge_functionblock_client_t *client,
    CanL2__ConfigurationGetResponse **configuration_p)
{
    io4e_err_t err;
    IO4E_MAKE_EMPTY_REQUEST(empty_req, CanL2, CAN_L2, ConfigurationGet, CONFIGURATION_GET);
    IO4E_PACK_PROTOMSG(empty_req, packed_req, can_l2, configuration_get);

    if ((err = io4e_functionblock_download_configuration(client,
             &packed_req,
             (io4edge_unpack_t)can_l2__configuration_get_response__unpack,
             (void **)configuration_p)) != IO4E_OK) {
        return err;
    }
    return IO4E_OK;
}

io4e_err_t io4edge_canl2_send_frames(io4edge_functionblock_client_t *client, CanL2__Frame **frames, size_t n_frames)
{
    io4e_err_t err;
    CanL2__FunctionControlSet set = CAN_L2__FUNCTION_CONTROL_SET__INIT;
    set.frame = frames;
    set.n_frame = n_frames;
    IO4E_PACK_PROTOMSG_P(set, packed_req, can_l2, function_control_set);

    return io4e_functionblock_function_control_set(client, &packed_req, NULL, NULL);
}

io4e_err_t io4edge_canl2_ctrl_state(io4edge_functionblock_client_t *client, CanL2__ControllerState *state_p)
{
    io4e_err_t err;
    IO4E_MAKE_EMPTY_REQUEST(empty_req, CanL2, CAN_L2, FunctionControlGet, FUNCTION_CONTROL_GET);
    IO4E_PACK_PROTOMSG(empty_req, packed_req, can_l2, function_control_get);

    CanL2__FunctionControlGetResponse *response;
    if ((err = io4e_functionblock_function_control_get(client,
             &packed_req,
             (io4edge_unpack_t)can_l2__function_control_get_response__unpack,
             (void **)&response)) != IO4E_OK) {
        return err;
    }
    *state_p = response->controllerstate;
    can_l2__function_control_get_response__free_unpacked(response, NULL);
    return IO4E_OK;
}

io4e_err_t io4edge_canl2_start_stream(io4edge_functionblock_client_t *client,
    CanL2__StreamControlStart *config,
    Functionblock__StreamControlStart *fb_config)
{
    IO4E_PACK_PROTOMSG(config, packed_req, can_l2, stream_control_start);

    return io4e_functionblock_start_stream(client, &packed_req, fb_config);
}
