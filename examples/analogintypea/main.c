/*
 * Demo application for the AnalogInTypeA function block
 *
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "io4edge_client.h"
#include <io4edge_analogintypea.h>
#include <pthread.h>
#include <inttypes.h>

// change this to your device's IP address
#define DEVICE_IP "192.168.24.154"
#define DEVICE_PORT 10000
#define SAMPLE_RATE_HZ 1000

int main(void)
{
    io4e_err_t err;
    io4edge_functionblock_client_t *client;
    if ((err = io4edge_functionblock_client_new_from_host_port(&client, DEVICE_IP, DEVICE_PORT, 5)) != IO4E_OK) {
        printf("Failed to create client: %d\n", err);
        return 1;
    }

    AnalogInTypeA__ConfigurationSet config = ANALOG_IN_TYPE_A__CONFIGURATION_SET__INIT;
    config.sample_rate = SAMPLE_RATE_HZ;

    if ((err = io4edge_analogintypea_upload_configuration(client, &config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }

    AnalogInTypeA__ConfigurationGetResponse *dl_config;
    if ((err = io4edge_analogintypea_download_configuration(client, &dl_config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }
    printf("Downloaded configuration: sample_rate: %d\n", dl_config->sample_rate);
    // free the downloaded configuration
    analog_in_type_a__configuration_get_response__free_unpacked(dl_config, NULL);

    // start stream
    Functionblock__StreamControlStart stream_attr = FUNCTIONBLOCK__STREAM_CONTROL_START__INIT;
    stream_attr.bucketsamples = 100;
    stream_attr.bufferedsamples = 200;
    stream_attr.keepaliveinterval = 1000;
    stream_attr.low_latency_mode = false;
    if ((err = io4edge_analogintypea_start_stream(client, &stream_attr)) != IO4E_OK) {
        printf("Failed to start stream: %d\n", err);
        return 1;
    }

    // Demonstrate how to read current input level
    for (int i = 0; i < 10; i++) {
        float value;
        if ((err = io4edge_analogintypea_value(client, &value)) != IO4E_OK) {
            printf("Failed to get value: %d\n", err);
            return 1;
        }
        printf("Input value: %f\n", value);

        usleep(100000);
    }

    // read stream
    for (int i = 0; i < 100; i++) {
        Functionblock__StreamData *stream_data;
        AnalogInTypeA__StreamData *fs_stream_data;
        if ((err = io4edge_functionblock_read_stream(client,
                 (io4edge_unpack_t)analog_in_type_a__stream_data__unpack,
                 &stream_data,
                 (void **)&fs_stream_data,
                 2000)) != IO4E_OK) {
            printf("Failed to read stream: %d\n", err);
            return 1;
        }
        printf("Received stream data ts=%" PRIu64 ", seq=%d\n", stream_data->deliverytimestampus, stream_data->sequence);
        for (int i = 0; i < fs_stream_data->n_samples; i++) {
            AnalogInTypeA__Sample *sample = fs_stream_data->samples[i];
            printf(" Value: %f\n", sample->value);
        }
        analog_in_type_a__stream_data__free_unpacked(fs_stream_data, NULL);
        functionblock__stream_data__free_unpacked(stream_data, NULL);
    }
    io4edge_functionblock_client_delete(&client);

    return 0;
}
