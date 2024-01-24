/*
 * SPDX-FileCopyrightText: 2024 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "io4edge_client.h"
#include "io4edge_api/mvbSniffer.pb-c.h"
#include "io4edge_api/telegram.pb-c.h"
#include "io4edge_api/io4edge_functionblock.pb-c.h"

/*
 * @brief Send a pattern to the mvbSniffer's internal mvb frame generator.
 * See https://github.com/ci4rail/io4edge-client-go/blob/main/mvbsniffer/generator.go how to create the pattern.
 * @param client Client handle.
 * @param msg Pattern to send.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_mvbsniffer_send_pattern(io4edge_functionblock_client_t *client, char *msg);

/*
 * @brief Start streaming of samples.
 *
 * @param client Client handle.
 * @param config MVB specific configuration for the stream.
 * @param fb_config Generic configuration for the stream.
 * @return IO4E_OK on success, IO4E_xxx error code on failure.
 */
io4e_err_t io4edge_mvbsniffer_start_stream(io4edge_functionblock_client_t *client,
    MvbSniffer__StreamControlStart *config,
    Functionblock__StreamControlStart *fb_config);
