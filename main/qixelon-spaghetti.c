#include <config.h>
#include <stdio.h>
#include <logger.h>

void app_main(void)
{
    LOG_INFO(0, "Qixelon Spaghetti");

    config_init();
}
