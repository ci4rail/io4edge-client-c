/*
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "io4edge_client.h"
#include "io4edge_api/analogInTypeA.pb-c.h"
#include "io4edge_api/io4edge_functionblock.pb-c.h"

/*
 * @brief Upload configuration to the analogintypea function block.
 * @param client Client handle.
 * @param configuration Configuration to upload.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 * NOTE: You must initialize the configuration struct with ANALOG_IN_TYPE_A__CONFIGURATION_SET__INIT
 * before calling this function.
 */
io4e_err_t io4edge_analogintypea_upload_configuration(io4edge_functionblock_client_t *client,
    const AnalogInTypeA__ConfigurationSet *configuration);

/*
 * @brief Download configuration from the analogintypea function block.
 * @param client Client handle.
 * @param configuration_p[out] Pointer to the configuration struct that is filled with the configuration.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 * NOTE: You must free the configuration struct with analog_in_type_a__configuration_get_response__free_unpacked() after
 * use.
 */
io4e_err_t io4edge_analogintypea_download_configuration(io4edge_functionblock_client_t *client,
    AnalogInTypeA__ConfigurationGetResponse **configuration_p);

/*
 * @brief Read the current analog input level. Range -1 .. +1 (for min/max voltage or current)
 * @param client Client handle.
 * @param value_p[out] Pointer to the value struct that is filled with the value.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_analogintypea_value(io4edge_functionblock_client_t *client, float *value_p);

/*
 * @brief Start streaming of samples.
 *
 * @param client Client handle.
 */
io4e_err_t io4edge_analogintypea_start_stream(io4edge_functionblock_client_t *client,
    Functionblock__StreamControlStart *fb_config);
