/*
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "io4edge_client.h"
#include "io4edge_api/binaryIoTypeA.pb-c.h"
#include "io4edge_api/io4edge_functionblock.pb-c.h"

/*
 * @brief Upload configuration to the binaryiotype a function block.
 * @param client Client handle.
 * @param configuration Configuration to upload.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 * NOTE: You must initialize the configuration struct with BINARY_IO_TYPE_A__CONFIGURATION_SET__INIT
 * before calling this function.
 */
io4e_err_t io4edge_binaryiotypea_upload_configuration(io4edge_functionblock_client_t *client,
    const BinaryIoTypeA__ConfigurationSet *configuration);

/*
 * @brief Download configuration from the binaryiotype a function block.
 * @param client Client handle.
 * @param configuration_p[out] Pointer to the configuration struct that is filled with the configuration.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 * NOTE: You must free the configuration struct with binary_io_type_a__configuration_get_response__free_unpacked() after
 * use.
 */
io4e_err_t io4edge_binaryiotypea_download_configuration(io4edge_functionblock_client_t *client,
    BinaryIoTypeA__ConfigurationGetResponse **configuration_p);

/*
 * @brief Describe the binaryiotype a function block.
 * @param client Client handle.
 * @param description_p[out] Pointer to the description struct that is filled with the description.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 * NOTE: You must free the description struct with binary_io_type_a__configuration_describe_response__free_unpacked()
 * after use.
 */
io4e_err_t io4edge_binaryiotypea_describe(io4edge_functionblock_client_t *client,
    BinaryIoTypeA__ConfigurationDescribeResponse **description_p);

/*
 * @brief Set the output state of a channel.
 * @param client Client handle.
 * @param channel Channel number.
 * @param state State to set.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */

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
