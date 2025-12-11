idf_component_register(SRCS "config.c"
                    INCLUDE_DIRS "include"
                    REQUIRES logger
)

littlefs_create_partition_image(config ../../flasher/config FLASH_IN_PROJECT)