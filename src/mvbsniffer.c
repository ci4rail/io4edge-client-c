/*
 * SPDX-FileCopyrightText: 2024 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "io4edge_internal.h"
#include "io4edge_mvbsniffer.h"

static const char *TAG = "mvbsniffer";

io4e_err_t io4edge_mvbsniffer_send_pattern(io4edge_functionblock_client_t *client, char *msg)
{
    MvbSniffer__FunctionControlSet set = MVB_SNIFFER__FUNCTION_CONTROL_SET__INIT;
    set.generator_pattern = msg;
    IO4E_PACK_PROTOMSG_P(set, packed_req, mvb_sniffer, function_control_set);
    return io4e_functionblock_function_control_set(client, &packed_req, NULL, NULL);
}

io4e_err_t io4edge_mvbsniffer_start_stream(io4edge_functionblock_client_t *client,
    MvbSniffer__StreamControlStart *config,
    Functionblock__StreamControlStart *fb_config)
{
    IO4E_PACK_PROTOMSG(config, packed_req, mvb_sniffer, stream_control_start);

    return io4e_functionblock_start_stream(client, &packed_req, fb_config);
}
