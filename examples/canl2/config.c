/*
 * Demo application for the CanL2 function block
 *
 * SPDX-FileCopyrightText: 2024 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "io4edge_client.h"
#include <io4edge_canl2.h>
#include <pthread.h>
#include "can_example.h"

// set your desired CAN parameters here
#define CAN_BITRATE 1000000
#define CAN_SAMPLE_POINT 800  // 80%
#define CAN_SJW 1
#define CAN_LISTEN_ONLY true

int main(void)
{
    io4e_err_t err;
    io4edge_functionblock_client_t *client;
    if ((err = io4edge_functionblock_client_new_from_host_port(&client, DEVICE_IP, DEVICE_PORT, 5)) != IO4E_OK) {
        printf("Failed to create client: %d\n", err);
        return 1;
    }

    CanL2__ConfigurationSet config = CAN_L2__CONFIGURATION_SET__INIT;
    config.baud = CAN_BITRATE;
    config.samplepoint = CAN_SAMPLE_POINT;
    config.sjw = CAN_SJW;
    config.listenonly = CAN_LISTEN_ONLY;

    if ((err = io4edge_canl2_upload_configuration(client, &config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }

    CanL2__ConfigurationGetResponse *dl_config;
    if ((err = io4edge_canl2_download_configuration(client, &dl_config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }
    printf("Actual configuration: bitrate=%d, samplepoint=%d, sjw=%d, listenonly=%d\n",
        dl_config->baud,
        dl_config->samplepoint,
        dl_config->sjw,
        dl_config->listenonly);
    // free the downloaded configuration
    can_l2__configuration_get_response__free_unpacked(dl_config, NULL);

    io4edge_functionblock_client_delete(&client);

    return 0;
}
