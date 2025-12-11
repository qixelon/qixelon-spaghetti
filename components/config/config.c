#include "config.h"
#include "esp_littlefs.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"

typedef struct kv {
    char *key;
    char *value;
    struct kv *next;
} kv_t;

typedef struct section {
    char *name;
    kv_t *kv_list;
    struct section *next;
} section_t;

static section_t *sections = NULL;

static kv_t* kv_create(const char *key, const char *value) {
    kv_t *kv = malloc(sizeof(kv_t));
    kv->key = strdup(key);
    kv->value = strdup(value);
    kv->next = NULL;

    return kv;
}

static section_t* section_create(const char *name) {
    section_t *sec = malloc(sizeof(section_t));
    sec->name = strdup(name);
    sec->kv_list = NULL;
    sec->next = NULL;

    return sec;
}

static section_t* section_find(const char *name) {
    section_t *sec = sections;

    while(sec) {
        if(strcmp(sec->name, name) == 0) return sec;
        sec = sec->next;
    }

    return NULL;
}

static kv_t* kv_find(section_t *sec, const char *key) {
    kv_t *kv = sec->kv_list;

    while(kv) {
        if(strcmp(kv->key, key) == 0) return kv;
        kv = kv->next;
    }

    return NULL;
}

bool config_init(void) {
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/mnt/config",
        .partition_label = "config",
        .format_if_mount_failed = true,
    };

    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if(ret != ESP_OK) {
        LOG_ERROR("config", "Little FS mount failed: %s", esp_err_to_name(ret));
        return false;
    }

    return true;
}

bool config_load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if(!f) return false;

    char line[256];

    section_t *current_sec = NULL;

    while(fgets(line, sizeof(line), f)) {
        char *trim = line;

        while(*trim == ' ' || *trim == '\t') trim++;

        if(*trim == '#' || *trim == '\n' || *trim == '\0') continue;
        if(trim[0] == '[') {
            char *end = strchr(trim, ']');
            if(!end) continue;
            *end = 0;

            current_sec = section_create(trim+1);
            current_sec->next = sections;

            sections = current_sec;
        } else if(current_sec) {
            char *sep = strchr(trim, ':');
            if(!sep) continue;
            *sep = 0;

            char *key = trim;
            char *value = sep+1;

            while(*value == ' ' || *value == '\t') value++;
            char *newline = strchr(value, '\n');

            if(newline) *newline = 0;

            kv_t *kv = kv_create(key, value);
            kv->next = current_sec->kv_list;
            current_sec->kv_list = kv;
        }
    }

    fclose(f);

    return true;
}

void* config_get_section(const char *section_name) {
    return section_find(section_name);
}

bool config_get_string(const char *section_name, const char *key, const char **out, const char *default_val) {
    section_t *sec = section_find(section_name);

    if(!sec) {
        *out = default_val;
        return false;
    }

    kv_t *kv = kv_find(sec, key);

    if(!kv) {
        *out = default_val;

        return false;
    }
    *out = kv->value;

    return true;
}

bool config_get_bool(const char *section_name, const char *key, bool *out, bool default_val) {
    const char *val;

    if(!config_get_string(section_name, key, &val, NULL)) {
        *out = default_val;
        return false;
    }

    if(strcmp(val, "true") == 0 || strcmp(val, "1") == 0) *out = true;

    else *out = false;

    return true;
}

bool config_get_int(const char *section_name, const char *key, int *out, int default_val) {
    const char *val;

    if(!config_get_string(section_name, key, &val, NULL)) {
        *out = default_val;
        return false;
    }
    *out = atoi(val);

    return true;
}

bool config_get_float(const char *section_name, const char *key, float *out, float default_val) {
    const char *val;
    if(!config_get_string(section_name, key, &val, NULL)) {
        *out = default_val;
        return false;
    }
    *out = atof(val);
    return true;
}

bool config_reload_file(const char *filename) {
    section_t *sec = sections;

    while(sec) {
        kv_t *kv = sec->kv_list;

        while(kv) {
            kv_t *tmp = kv;
            kv = kv->next;
            free(tmp->key);
            free(tmp->value);
            free(tmp);
        }

        section_t *tmp_sec = sec;
        sec = sec->next;
        free(tmp_sec->name);
        free(tmp_sec);
    }
    sections = NULL;

    return config_load_file(filename);
}