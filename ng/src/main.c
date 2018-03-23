/*
TODO:
- setup cscope

*/

// ESP SDK (include/)
#include <espressif/esp_common.h>
#include <espressif/user_interface.h>
#include <espressif/esp_system.h>

// FreeRTOS (FreeRTOS/Source/include/)
#include <FreeRTOS.h>
#include <task.h>

// open RTOS (core/include/)
#include <esp/iomux.h>
#include <esp/gpio.h>

#include "stuff.h"
#include "debug.h"
#include "mon.h"
#include "version_gen.h"

const int gpio = 2;

/* This task uses the high level GPIO API (esp_gpio.h) to blink an LED.
 *
 */
void blinkenTask(void *pvParameters)
{
    gpio_enable(gpio, GPIO_OUTPUT);
    while (true)
    {
        gpio_write(gpio, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_write(gpio, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        PRINT("blink %u", portTICK_PERIOD_MS);

        uint32_t foo = 10000000;
        uint32_t bar = 0;
        while (foo--)
        {
            bar++;
        }
        PRINT("bar=%u", bar);
    }
}

void blaTask(void *pvParameters)
{
    static int count;
    while(1) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        DEBUG("bla %i", count++);
    }
}



void user_init(void)
{
    // initialise stuff
    debugInit();
    stuffInit();
    monInit();

    // say hello
    NOTICE("------------------------------------------------------------------------------------------");
    NOTICE("Tschenggins Lämpli NG ("FF_BUILDVER" "FF_BUILDDATE")");
    NOTICE("Copyright (c) 2018 Philippe Kehl & flipflip industries <flipflip at oinkzwurgl dot org>");
    NOTICE("Parts copyright by others. See source code.");
    NOTICE("https://oinkzwurgl.org/projaeggd/tschenggins-laempli");
    NOTICE("------------------------------------------------------------------------------------------");

    // print some system information
    DEBUG("SDK %s, Chip 0x%08x", sdk_system_get_sdk_version(), sdk_system_get_chip_id());
    DEBUG("GCC " FF_GCCVERSION);
    DEBUG("Boot ver: %u, mode: %u", sdk_system_get_boot_version(), sdk_system_get_boot_mode());
    DEBUG("Frequency: %uMHz", sdk_system_get_cpu_freq()); // MHz
    //static const char flashMap[][10] =
    //{
    //    { "4M_256\0" }, { "2M\0" }, { "8M_512\0" }, { "16M_512\0" },
    //   { "32M_512\0" }, { "16M_1024\0" }, { "32M_1024\0" }
    //};
    //DEBUG("Flash: %s", flashMap[sdk_system_get_flash_size_map()]);
    static const char resetReason[][10] =
    {
        { "default\0" }, { "watchdog\0" }, { "exception\0" }, { "soft\0" }
    };
    const struct sdk_rst_info *pkResetInfo = sdk_system_get_rst_info();
    switch ((enum sdk_rst_reason)pkResetInfo->reason)
    {
        case WDT_RST:
        case EXCEPTION_RST:
        case SOFT_RST:
            ERROR("Reset: %s", resetReason[pkResetInfo->reason]);
            if (pkResetInfo->reason == WDT_RST)
            {
                const char *exceptDesc = "other exception";
                switch (pkResetInfo->exccause)
                {
                    case  0: exceptDesc = "invalid instruction"; break;
                    case  6: exceptDesc = "division by zero";    break;
                    case  9: exceptDesc = "unaligned access";    break;
                    case 28:
                    case 29: exceptDesc = "invalid address";     break;
                }
                ERROR("Fatal exception: %s", exceptDesc);
                ERROR("epc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x",
                    pkResetInfo->epc1, pkResetInfo->epc2, pkResetInfo->epc3,
                    pkResetInfo->excvaddr, pkResetInfo->depc);
            }
            break;
        case DEFAULT_RST:
            DEBUG("Reset: default");
            break;
        default:
            WARNING("Reset: %u",pkResetInfo->reason);
            break;
    }

    xTaskCreate(blinkenTask, "foo", 256, NULL, 20, NULL);
    //xTaskCreate(blaTask, "bar", 256, NULL, 2, NULL);
    //xTaskCreate(blinkenRegisterTask, "blinkenRegisterTask", 256, NULL, 2, NULL);
    NOTICE("here we go...");
}