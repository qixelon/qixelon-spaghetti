#include "logger.h"
#include "esp_timer.h"
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>


#define LOG_COLOR_INFO  "\x1b[32m"
#define LOG_COLOR_WARN  "\x1b[33m"
#define LOG_COLOR_ERROR "\x1b[31m"
#define LOG_COLOR_RESET "\x1b[0m"

void logger_log(log_level_t level, const char *tag, const char *fmt, ...) {
    uint64_t us = esp_timer_get_time();
    uint32_t sec = us / 1000000ULL;

    const char *level_str;
    const char *color;

    switch (level) {
        case LOG_LEVEL_INFO:  level_str = "INFO";  color = LOG_COLOR_INFO;  break;
        case LOG_LEVEL_WARN:  level_str = "WARN";  color = LOG_COLOR_WARN;  break;
        case LOG_LEVEL_ERROR: level_str = "ERROR"; color = LOG_COLOR_ERROR; break;
        default:              level_str = "???";   color = LOG_COLOR_RESET; break;
    }

    printf("%s[%" PRIu32 " s] %s", color, sec, level_str);

    if (tag != NULL)
        printf(" (%s)", tag);

    printf(": ");

    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    printf("%s\n", LOG_COLOR_RESET);
}
