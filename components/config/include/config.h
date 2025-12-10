#pragma once
#include <stdbool.h>

bool config_init(void);
bool config_load_file(const char *filename);
void* config_get_section(const char *section_name);
bool config_get_string(const char *section_name, const char *key, const char **out, const char *default_val);
bool config_get_bool(const char *section_name, const char *key, bool *out, bool default_val);
bool config_get_int(const char *section_name, const char *key, int *out, int default_val);
bool config_get_float(const char *section_name, const char *key, float *out, float default_val);
bool config_reload_file(const char *filename);