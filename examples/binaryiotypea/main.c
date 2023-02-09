#include <stdio.h>
#include "io4edge_client.h"
#include <io4edge_binaryiotypea.h>
#include <pthread.h>

static void *stim_thread(void *arg)
{
    io4edge_functionblock_client_t *client = (io4edge_functionblock_client_t *)arg;
    io4e_err_t err;
    while (1) {
        if ((err = io4edge_binaryiotypea_set_output(client, 0, true)) != IO4E_OK) {
            printf("Failed to set output: %d\n", err);
            return NULL;
        }
        usleep(300000);
        if ((err = io4edge_binaryiotypea_set_output(client, 0, false)) != IO4E_OK) {
            printf("Failed to set output: %d\n", err);
            return NULL;
        }
        usleep(300000);
    }
    return NULL;
}

int main(void)
{
    io4e_err_t err;
    io4edge_functionblock_client_t *client;
    if ((err = io4edge_functionblock_client_new_from_host_port(&client, "192.168.24.154", 10002, 5)) != IO4E_OK) {
        printf("Failed to create client: %d\n", err);
        return 1;
    }

    BinaryIoTypeA__ConfigurationSet config = BINARY_IO_TYPE_A__CONFIGURATION_SET__INIT;
    config.outputwatchdogmask = 0x1;
    config.outputwatchdogtimeout = 1000;

    if ((err = io4edge_binaryiotypea_upload_configuration(client, &config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }

    BinaryIoTypeA__ConfigurationGetResponse *dl_config;
    if ((err = io4edge_binaryiotypea_download_configuration(client, &dl_config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }
    printf("Downloaded configuration: %d %d\n", dl_config->outputwatchdogmask, dl_config->outputwatchdogtimeout);
    binary_io_type_a__configuration_get_response__free_unpacked(dl_config, NULL);

    pthread_t stim_thread_id;
    pthread_create(&stim_thread_id, NULL, stim_thread, client);

#if 0
    if ((err = io4edge_binaryiotypea_set_output(client, 0, true)) != IO4E_OK) {
        printf("Failed to set output: %d\n", err);
        return 1;
    }

    if ((err = io4edge_binaryiotypea_set_all_outputs(client, 0x1, 0x1)) != IO4E_OK) {
        printf("Failed to set all outputs: %d\n", err);
        return 1;
    }
#endif
    // start stream
    Functionblock__StreamControlStart stream_attr = FUNCTIONBLOCK__STREAM_CONTROL_START__INIT;
    stream_attr.bucketsamples = 10;
    stream_attr.bufferedsamples = 20;
    stream_attr.keepaliveinterval = 1000;
    stream_attr.low_latency_mode = true;
    if ((err = io4edge_binaryiotypea_start_stream(client, 0xf, &stream_attr)) != IO4E_OK) {
        printf("Failed to start stream: %d\n", err);
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        bool state;
        if ((err = io4edge_binaryiotypea_input(client, 0, &state)) != IO4E_OK) {
            printf("Failed to get input: %d\n", err);
            return 1;
        }
        printf("Input state: %d\n", state);

        uint32_t states;
        if ((err = io4edge_binaryiotypea_all_inputs(client, 0xf, &states)) != IO4E_OK) {
            printf("Failed to get all inputs: %d\n", err);
            return 1;
        }
        printf("All input states: 0x%x\n", states);
        usleep(100000);
    }

    // read stream
    for (int i = 0; i < 10; i++) {
        Functionblock__StreamData *stream_data;
        BinaryIoTypeA__StreamData *fs_stream_data;
        if ((err = io4edge_functionblock_read_stream(client,
                 (io4edge_unpack_t)binary_io_type_a__stream_data__unpack,
                 &stream_data,
                 (void **)&fs_stream_data,
                 2000)) != IO4E_OK) {
            printf("Failed to read stream: %d\n", err);
            return 1;
        }
        printf("Received stream data ts=%ld, seq=%d\n", stream_data->deliverytimestampus, stream_data->sequence);
        for (int i = 0; i < fs_stream_data->n_samples; i++) {
            BinaryIoTypeA__Sample *sample = fs_stream_data->samples[i];
            printf(" Channel %d -> %d\n", sample->channel, sample->value);
        }
        binary_io_type_a__stream_data__free_unpacked(fs_stream_data, NULL);
        functionblock__stream_data__free_unpacked(stream_data, NULL);
    }
    pthread_cancel(stim_thread_id);
    pthread_join(stim_thread_id, NULL);
    io4edge_functionblock_client_delete(&client);

    return 0;
}
