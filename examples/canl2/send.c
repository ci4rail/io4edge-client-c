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

#define NUM_BUCKETS 10
#define NUM_MESSAGES_PER_BUCKET 5
#define GAP_BETWEEN_BUCKETS_US 10000
#define USE_EXTENDED_FRAME false

int main(void)
{
    io4e_err_t err;
    io4edge_functionblock_client_t *client;
    if ((err = io4edge_functionblock_client_new_from_host_port(&client, DEVICE_IP, DEVICE_PORT, 5)) != IO4E_OK) {
        printf("Failed to create client: %d\n", err);
        return 1;
    }

    for (int bucket = 0; bucket < NUM_BUCKETS; bucket++) {
        CanL2__Frame *frames[NUM_MESSAGES_PER_BUCKET];
        for (int message = 0; message < NUM_MESSAGES_PER_BUCKET; message++) {
            CanL2__Frame *frame = malloc(sizeof(CanL2__Frame) + 8);
            can_l2__frame__init(frame);
            frame->data.data = (uint8_t *)(frame + 1);

            frame->messageid = 0x100 + (bucket % 0xFF);
            frame->extendedframeformat = USE_EXTENDED_FRAME;
            frame->remoteframe = false;
            frame->data.len = message % 8;
            for (int i = 0; i < frame->data.len; i++) {
                frame->data.data[i] = message % 0xFF;
            }
            frames[message] = frame;
        }
        err = io4edge_canl2_send_frames(client, (CanL2__Frame **)&frames, NUM_MESSAGES_PER_BUCKET);
        for (int i = 0; i < NUM_MESSAGES_PER_BUCKET; i++) {
            free(frames[i]);
        }
        if (err != IO4E_OK) {
            printf("Failed to send message: %d\n", err);
            return 1;
        }
        usleep(GAP_BETWEEN_BUCKETS_US);
    }

    io4edge_functionblock_client_delete(&client);

    return 0;
}
