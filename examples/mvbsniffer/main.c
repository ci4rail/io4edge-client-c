/*
 * Demo application for the MvbSniffer function block
 *
 * SPDX-FileCopyrightText: 2024 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "io4edge_client.h"
#include <io4edge_mvbsniffer.h>
#include <pthread.h>

// change this to your device's IP address
#define DEVICE_IP "192.168.24.102"
#define DEVICE_PORT 10000

//  This is a test pattern that generates the following telegrams.
//  Address    Type                   Data
//  0x001      Process Data 16 Bit    0x01 0x22
//  0x002      Process Data 16 Bit    0x02 0x22
//  0x003      Process Data 16 Bit    0x03 0x22
//  0x004      Process Data 16 Bit    0x04 0x22
//  Spacing between telegrams is 106µs

// Comment out the following line to capture the physical MVB bus
#define TEST_PATTERN \
    "034H`@`@`@`@a@`@`Cl@`BbH`@`@`@`@b@`@`Cl@aBbH`@`@`@`@c@`@`Cl@bBbH`@`@`@`@d@`@`Cl@cBbH`@`@`@`@e@a@`Cl@dBb1"

static char *telegram_to_string(char *buf, size_t buf_len, MvbSniffer__Telegram *telegram);

int main(void)
{
    io4e_err_t err;
    io4edge_functionblock_client_t *client;
    if ((err = io4edge_functionblock_client_new_from_host_port(&client, DEVICE_IP, DEVICE_PORT, 5)) != IO4E_OK) {
        printf("Failed to create client: %d\n", err);
        return 1;
    }
    char *pattern = NULL;
#ifdef TEST_PATTERN
    pattern = TEST_PATTERN;
#else
    // ensure we use the external input in case the internal generator has been selected before
    pattern = "2";
#endif
    if ((err = io4edge_mvbsniffer_send_pattern(client, TEST_PATTERN)) != IO4E_OK) {
        printf("Failed to send pattern: %d\n", err);
        return 1;
    }

    // start stream
    Functionblock__StreamControlStart stream_attr = FUNCTIONBLOCK__STREAM_CONTROL_START__INIT;
    stream_attr.bucketsamples = 100;
    stream_attr.bufferedsamples = 200;
    stream_attr.keepaliveinterval = 1000;
    stream_attr.low_latency_mode = false;

    // configure filter: accept all telegrams with f_code 0x000 to 0xfff, ignore timed out frames
    MvbSniffer__StreamControlStart config = MVB_SNIFFER__STREAM_CONTROL_START__INIT;
    MvbSniffer__FilterMask filter_mask0 = MVB_SNIFFER__FILTER_MASK__INIT;
    MvbSniffer__FilterMask *filter_masks[] = {
        &filter_mask0,
    };
    config.n_filter = sizeof(filter_masks) / sizeof(filter_masks[0]);
    config.filter = (MvbSniffer__FilterMask **)&filter_masks;
    filter_mask0.f_code_mask = 0xfff;
    filter_mask0.address = 0x0;
    filter_mask0.mask = 0x0;
    filter_mask0.include_timedout_frames = false;

    if ((err = io4edge_mvbsniffer_start_stream(client, &config, &stream_attr)) != IO4E_OK) {
        printf("Failed to start stream: %d\n", err);
        return 1;
    }

    // read stream
    for (int i = 0; i < 5; i++) {
        Functionblock__StreamData *stream_data;
        MvbSniffer__TelegramCollection *fs_stream_data;
        if ((err = io4edge_functionblock_read_stream(client,
                 (io4edge_unpack_t)mvb_sniffer__telegram_collection__unpack,
                 &stream_data,
                 (void **)&fs_stream_data,
                 2)) != IO4E_OK) {
            printf("Failed to read stream: %d\n", err);
            return 1;
        }
        printf("Received %d telegrams, seq=%d\n", (int)fs_stream_data->n_entry, stream_data->sequence);
        for (int i = 0; i < fs_stream_data->n_entry; i++) {
            MvbSniffer__Telegram *telegram = fs_stream_data->entry[i];
            // check for errors
            if (telegram->state != MVB_SNIFFER__TELEGRAM__STATE__kSuccessful) {
                if (telegram->state & MVB_SNIFFER__TELEGRAM__STATE__kTimedOut) {
                    printf("No slave frame has been received to a master frame\n");
                }
                if (telegram->state & MVB_SNIFFER__TELEGRAM__STATE__kMissedMVBFrames) {
                    printf("one or more MVB frames are lost in the device since the last telegram\n");
                }
                if (telegram->state & MVB_SNIFFER__TELEGRAM__STATE__kMissedTelegrams) {
                    printf("one or more telegrams are lost\n");
                }
            }

            char buf[200];
            printf("%s\n", telegram_to_string(buf, sizeof(buf), telegram));
        }
        mvb_sniffer__telegram_collection__free_unpacked(fs_stream_data, NULL);
        functionblock__stream_data__free_unpacked(stream_data, NULL);
    }
    io4edge_functionblock_client_delete(&client);

    return 0;
}

static char *telegram_to_string(char *buf, size_t buf_len, MvbSniffer__Telegram *telegram)
{
    char *p = buf;
    int len;
    len = snprintf(buf,
        buf_len,
        "Telegram: ts=%ld addr=%03x, %s,",
        telegram->timestamp,
        telegram->address,
        telegram->type < mvb_sniffer__telegram__type__descriptor.n_values
            ? mvb_sniffer__telegram__type__descriptor.values[telegram->type].name
            : "unknown");
    buf_len -= len;
    p += len;

    if (telegram->data.len > 0) {
        len = snprintf(p, buf_len, " data=");
        buf_len -= len;
        p += len;
        for (int i = 0; i < telegram->data.len; i++) {
            len = snprintf(p, buf_len, " %02x", telegram->data.data[i]);
            buf_len -= len;
            p += len;
        }
    }
    return buf;
}
