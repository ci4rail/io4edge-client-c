#pragma once
#include <stdint.h>
#include <stdarg.h>

typedef enum {
    IO4E_LOG_NONE,  /*!< No log output */
    IO4E_LOG_ERROR, /*!< Critical errors, software module can not recover on its own */
    IO4E_LOG_WARN,  /*!< Error conditions from which recovery measures have been taken */
    IO4E_LOG_INFO,  /*!< Information messages which describe normal flow of events */
    IO4E_LOG_DEBUG, /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
} io4e_log_level_t;

#define LOG_LOCAL_LEVEL IO4E_LOG_INFO  // TODO: Make this configurable

void io4e_log_write(io4e_log_level_t level, const char* tag, const char* format, ...)
    __attribute__((format(printf, 3, 4)));

uint32_t io4e_log_timestamp(void);

#define LOG_COLOR_BLACK "30"
#define LOG_COLOR_RED "31"
#define LOG_COLOR_GREEN "32"
#define LOG_COLOR_BROWN "33"
#define LOG_COLOR_BLUE "34"
#define LOG_COLOR_PURPLE "35"
#define LOG_COLOR_CYAN "36"
#define LOG_COLOR(COLOR) "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR) "\033[1;" COLOR "m"
#define LOG_RESET_COLOR "\033[0m"
#define LOG_COLOR_E LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D
#define LOG_COLOR_V
#define LOG_FORMAT(letter, format) LOG_COLOR_##letter #letter " (%u) %s: " format LOG_RESET_COLOR "\n"

#define IO4E_LOG_LEVEL(level, tag, format, ...)                                                                    \
    do {                                                                                                           \
        if (level == IO4E_LOG_ERROR) {                                                                             \
            io4e_log_write(                                                                                        \
                IO4E_LOG_ERROR, tag, LOG_FORMAT(E, format), io4e_log_timestamp(), tag __VA_OPT__(, ) __VA_ARGS__); \
        } else if (level == IO4E_LOG_WARN) {                                                                       \
            io4e_log_write(                                                                                        \
                IO4E_LOG_WARN, tag, LOG_FORMAT(W, format), io4e_log_timestamp(), tag __VA_OPT__(, ) __VA_ARGS__);  \
        } else if (level == IO4E_LOG_DEBUG) {                                                                      \
            io4e_log_write(                                                                                        \
                IO4E_LOG_DEBUG, tag, LOG_FORMAT(D, format), io4e_log_timestamp(), tag __VA_OPT__(, ) __VA_ARGS__); \
        } else {                                                                                                   \
            io4e_log_write(                                                                                        \
                IO4E_LOG_INFO, tag, LOG_FORMAT(I, format), io4e_log_timestamp(), tag __VA_OPT__(, ) __VA_ARGS__);  \
        }                                                                                                          \
    } while (0)

#define IO4E_LOG_LEVEL_LOCAL(level, tag, format, ...)          \
    do {                                                       \
        if (LOG_LOCAL_LEVEL >= level)                          \
            IO4E_LOG_LEVEL(level, tag, format, ##__VA_ARGS__); \
    } while (0)

#define IO4E_LOGE(tag, format, ...) IO4E_LOG_LEVEL_LOCAL(IO4E_LOG_ERROR, tag, format, ##__VA_ARGS__)
#define IO4E_LOGW(tag, format, ...) IO4E_LOG_LEVEL_LOCAL(IO4E_LOG_WARN, tag, format, ##__VA_ARGS__)
#define IO4E_LOGI(tag, format, ...) IO4E_LOG_LEVEL_LOCAL(IO4E_LOG_INFO, tag, format, ##__VA_ARGS__)
#define IO4E_LOGD(tag, format, ...) IO4E_LOG_LEVEL_LOCAL(IO4E_LOG_DEBUG, tag, format, ##__VA_ARGS__)
#define IO4E_LOGV(tag, format, ...) IO4E_LOG_LEVEL_LOCAL(IO4E_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
