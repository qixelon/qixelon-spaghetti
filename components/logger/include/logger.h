#pragma once
#include <stdio.h>

typedef enum {
    LOG_LEVEL_INFO = 0,
    LOG_LEVEL_WARN = 1,
    LOG_LEVEL_ERROR = 2
} log_level_t;

void logger_log(log_level_t level, const char *tag, const char *fmt, ...);

#define LOG_INFO(tag, fmt, ...)  logger_log(LOG_LEVEL_INFO, tag, fmt, ##__VA_ARGS__)
#define LOG_WARN(tag, fmt, ...)  logger_log(LOG_LEVEL_WARN, tag, fmt, ##__VA_ARGS__)
#define LOG_ERROR(tag, fmt, ...) logger_log(LOG_LEVEL_ERROR, tag, fmt, ##__VA_ARGS__)
