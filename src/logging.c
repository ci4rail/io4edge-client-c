/*
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "io4edge_internal.h"
#include <stdio.h>
#include <stdarg.h>

void io4e_log_writev(io4e_log_level_t level, const char *tag, const char *format, va_list args)
{
    vprintf(format, args);
}

void io4e_log_write(io4e_log_level_t level, const char *tag, const char *format, ...)
{
    va_list list;
    va_start(list, format);
    io4e_log_writev(level, tag, format, list);
    va_end(list);
}
