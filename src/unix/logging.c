/*
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <bits/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t io4e_log_timestamp(void)
{
    struct timespec spec;
    if (clock_gettime(CLOCK_MONOTONIC, &spec) == -1) {
        return 0;
    }
    return spec.tv_sec * 1000 + spec.tv_nsec / 1e6;
}
