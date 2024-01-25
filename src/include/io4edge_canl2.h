/*
 * SPDX-FileCopyrightText: 2024 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "io4edge_client.h"
#include "io4edge_api/canL2.pb-c.h"
#include "io4edge_api/io4edge_functionblock.pb-c.h"

/*
 * @brief Upload configuration to the canl2 function block.
 * @param client Client handle.
 * @param configuration Configuration to upload.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 * NOTE: You must initialize the configuration struct with CAN_L2__CONFIGURATION_SET__INIT
 * before calling this function.
 */
io4e_err_t io4edge_canl2_upload_configuration(io4edge_functionblock_client_t *client,
    const CanL2__ConfigurationSet *configuration);

/*
 * @brief Download configuration from the canl2 function block.
 * @param client Client handle.
 * @param configuration_p[out] Pointer to the configuration struct that is filled with the configuration.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 * NOTE: You must free the configuration struct with can_l2__configuration_get_response__free_unpacked() after
 * use.
 */
io4e_err_t io4edge_canl2_download_configuration(io4edge_functionblock_client_t *client,
    CanL2__ConfigurationGetResponse **configuration_p);

/*
 * @brief Send frames to the CAN bus.
 * if the queue on the device is not large enough to contain all frames, send nothing and raise temporarily unavailable
 * error.
 * @param client Client handle.
 * @param frames array of frame pointers
 * @param n_frames number of frames in the array
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_canl2_send_frames(io4edge_functionblock_client_t *client, CanL2__Frame **frames, size_t n_frames);

/*
 * @brief Get the current state of the CAN controller.
 * @param client Client handle.
 * @param state_p[out] Pointer to the variable that is filled with the controller state.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_canl2_ctrl_state(io4edge_functionblock_client_t *client, CanL2__ControllerState *state_p);

/*
 * @brief Start streaming of samples.
 *
 * @param client Client handle.
 * @param config MVB specific configuration for the stream.
 * @param fb_config Generic configuration for the stream.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_canl2_start_stream(io4edge_functionblock_client_t *client,
    CanL2__StreamControlStart *config,
    Functionblock__StreamControlStart *fb_config);
