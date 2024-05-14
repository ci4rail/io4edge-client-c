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
#include <inttypes.h>

#define ACCEPTANCE_MASK 0x000
#define ACCEPTANCE_CODE 0x000
#define LOW_LATENCY_MODE true

static char *sample_to_string(char *buf, size_t buf_len, CanL2__Sample *sample);

int main(void)
{
    io4e_err_t err;
    io4edge_functionblock_client_t *client;
    if ((err = io4edge_functionblock_client_new_from_host_port(&client, DEVICE_IP, DEVICE_PORT, 5)) != IO4E_OK) {
        printf("Failed to create client: %d\n", err);
        return 1;
    }

    // start stream
    Functionblock__StreamControlStart stream_attr = FUNCTIONBLOCK__STREAM_CONTROL_START__INIT;
    stream_attr.bucketsamples = 100;
    stream_attr.bufferedsamples = 200;
    stream_attr.keepaliveinterval = 1000;
    stream_attr.low_latency_mode = LOW_LATENCY_MODE;

    // configure filter: accept all telegrams with f_code 0x000 to 0xfff, ignore timed out frames
    CanL2__StreamControlStart config = CAN_L2__STREAM_CONTROL_START__INIT;
    config.acceptancecode = ACCEPTANCE_CODE;
    config.acceptancemask = ACCEPTANCE_MASK;

    if ((err = io4edge_canl2_start_stream(client, &config, &stream_attr)) != IO4E_OK) {
        printf("Failed to start stream: %d\n", err);
        return 1;
    }

    // read stream
    for (;;) {
        Functionblock__StreamData *stream_data;
        CanL2__StreamData *fs_stream_data;
        if ((err = io4edge_functionblock_read_stream(
                 client, (io4edge_unpack_t)can_l2__stream_data__unpack, &stream_data, (void **)&fs_stream_data, 2)) !=
            IO4E_OK) {
            printf("Failed to read stream: %d\n", err);
            return 1;
        }
        printf("Received %d telegrams, seq=%d\n", (int)fs_stream_data->n_samples, stream_data->sequence);

        for (int i = 0; i < fs_stream_data->n_samples; i++) {
            CanL2__Sample *sample = fs_stream_data->samples[i];

            char buf[200];
            printf("%s\n", sample_to_string(buf, sizeof(buf), sample));
        }
    }
    io4edge_functionblock_client_delete(&client);

    return 0;
}

static char *sample_to_string(char *buf, size_t buf_len, CanL2__Sample *sample)
{
    char *p = buf;
    size_t buf_left = buf_len;
    int ret;

    ret = snprintf(p, buf_left, "%10" PRIu64 " us: ", sample->timestamp);
    p += ret;
    buf_left -= ret;

    if (sample->isdataframe) {
        CanL2__Frame *frame = sample->frame;
        ret = snprintf(p, buf_left, frame->extendedframeformat ? "ID: %08X" : "ID: %03X", frame->messageid);
        p += ret;
        buf_left -= ret;

        if (frame->remoteframe) {
            ret = snprintf(p, buf_left, " R");
            p += ret;
            buf_left -= ret;
        }

        ret = snprintf(p, buf_left, " DATA:");
        p += ret;
        buf_left -= ret;

        for (int i = 0; i < frame->data.len; i++) {
            ret = snprintf(p, buf_left, " %02X", frame->data.data[i]);
            p += ret;
            buf_left -= ret;
        }
        ret = snprintf(p, buf_left, " ");
        p += ret;
        buf_left -= ret;
    }

    ret = snprintf(p,
        buf_left,
        "ERROR: %s",
        sample->error < can_l2__error_event__descriptor.n_values
            ? can_l2__error_event__descriptor.values[sample->error].name
            : "unknown");
    p += ret;
    buf_left -= ret;

    ret = snprintf(p,
        buf_left,
        " STATE: %s",
        sample->controllerstate < can_l2__controller_state__descriptor.n_values
            ? can_l2__controller_state__descriptor.values[sample->controllerstate].name
            : "unknown");
    p += ret;
    buf_left -= ret;

    return buf;
}
