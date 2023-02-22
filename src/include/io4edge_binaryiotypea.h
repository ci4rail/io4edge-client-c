/*
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "io4edge_client.h"
#include "io4edge_api/binaryIoTypeA.pb-c.h"
#include "io4edge_api/io4edge_functionblock.pb-c.h"

/*
 * @brief Upload configuration to the binaryiotypea function block.
 * @param client Client handle.
 * @param configuration Configuration to upload.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 * NOTE: You must initialize the configuration struct with BINARY_IO_TYPE_A__CONFIGURATION_SET__INIT
 * before calling this function.
 */
io4e_err_t io4edge_binaryiotypea_upload_configuration(io4edge_functionblock_client_t *client,
    const BinaryIoTypeA__ConfigurationSet *configuration);

/*
 * @brief Download configuration from the binaryiotypea function block.
 * @param client Client handle.
 * @param configuration_p[out] Pointer to the configuration struct that is filled with the configuration.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 * NOTE: You must free the configuration struct with binary_io_type_a__configuration_get_response__free_unpacked() after
 * use.
 */
io4e_err_t io4edge_binaryiotypea_download_configuration(io4edge_functionblock_client_t *client,
    BinaryIoTypeA__ConfigurationGetResponse **configuration_p);

/*
 * @brief Describe the binaryiotypea function block.
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
 * @param state State to set. A "true" state turns on the outputs switch, a "false" state turns it off.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_binaryiotypea_set_output(io4edge_functionblock_client_t *client, uint32_t channel, bool state);

/*
 * @brief Set the output state of all channels.
 * @param client Client handle.
 * @param states binary coded map of outputs. 0 means switch off, 1 means switch on, LSB is Channel0
 * @param mask binary coded map of outputs to be set. 0 means do not change, 1 means change, LSB is Channel0
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_binaryiotypea_set_all_outputs(io4edge_functionblock_client_t *client,
    uint32_t states,
    uint32_t mask);

/*
 * @brief Exit the error state.
 *
 * Try to recover the binary output controller from error state.
 * The binary output controller enters error state when there is an overurrent condition for a long time.
 * In the error state, no outputs can be set; inputs can still be read.
 * This call tells the binary output controller to try again.
 * This call does however not wait if the recovery was successful or not.
 * @param client Client handle.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_binaryiotypea_exit_error_state(io4edge_functionblock_client_t *client);

/*
 * @brief Get the input state of a channel.
 *
 * Get the state of a single channel, regardless whether its configured as input or output)
 * State "true" is returned if the input level is above switching threshold, "false" otherwise.
 *
 * @param client Client handle.
 * @param channel Channel number.
 * @param state_p[out] Pointer to the state variable that is filled with the state.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_binaryiotypea_input(io4edge_functionblock_client_t *client, uint32_t channel, bool *state_p);

/*
 * @brief Get the input state of all channels.
 *
 * Get the state of all channels, regardless whether they are configured as input or output.
 * Each bit in the returned state corresponds to one channel, bit0 being channel 0.
 * The bit is "true" if the input level is above switching threshold, "false" otherwise.
 *
 * @param client Client handle.
 * @param mask binary coded map of inputs to be read. 0 means do not read, 1 means read, LSB is Channel0
 * @param states_p[out] Pointer to the state variable that is filled with the state.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_binaryiotypea_all_inputs(io4edge_functionblock_client_t *client, uint32_t mask, uint32_t *states_p);

/*
 * @brief Start streaming of transitions
 *
 * @param client Client handle.
 * @param channelfiltermask binary coded map of inputs that shall trigger a new sample
 */
io4e_err_t io4edge_binaryiotypea_start_stream(io4edge_functionblock_client_t *client,
    uint32_t channelfiltermask,
    Functionblock__StreamControlStart *fb_config);
