#include "utils.h"
#include "RTE.h"
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef CONFIG_UTILS_EN
uint16_t gPrintFlag[] = {0,
#ifdef CONFIG_RTE_EN
                         1u << DEBUG_PRINT_RTE_FLAG,
#endif
#ifdef CONFIG_HX711_EN
                         1u << DEBUG_PRINT_HX711_FLAG,
#endif
#ifdef CONFIG_MYGPIO_EN
                         1u << DEBUG_PRINT_GPIO_FLAG,
#endif
#ifdef CONFIG_PUMP_EN
                         1u << DEBUG_PRINT_PUMP_FLAG,
#endif
#ifdef CONFIG_MQTT_EN
                         1u << DEBUG_PRINT_MQTT_FLAG
#endif
};

uint16_t gPrintFlahSz = sizeof(gPrintFlag) / sizeof(gPrintFlag[0]);

#ifdef CONFIG_CONSOLE_EN
/** Arguments used by 'join' function */
static struct
{
    struct arg_int *debugFlag;
    // struct arg_str *password;
    struct arg_end *end;
} RTE_join_args;

int UtilsUpdateDebugPrintFlag(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&RTE_join_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, RTE_join_args.end, argv[0]);
        return ESP_FAIL;
    }
    if (RTE_join_args.debugFlag->count)
    {
        if (RTE_join_args.debugFlag->ival[0] > 0 &&
            RTE_join_args.debugFlag->ival[0] < gPrintFlahSz)
        {
            RTESetu32DebugPrintFlag(
                gPrintFlag[RTE_join_args.debugFlag->ival[0]]);
        }
        else
        {
            printf("Wrong parameter\n");
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

void register_RTE(void)
{
    RTE_join_args.debugFlag = arg_int0("f", "flag", "<f>", "Debug Print Flag");
    RTE_join_args.end = arg_end(1);

    const esp_console_cmd_t join_cmd = {.command = "debug",
                                        .help = "Set Debug Print flag",
                                        .hint = NULL,
                                        .func = &UtilsUpdateDebugPrintFlag,
                                        .argtable = &RTE_join_args};

    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
#endif

void DEBUG_PRINT(uint32_t flag, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if (((1u << flag) & RTEGetu32DebugPrintFlag()))
    {
        vprintf(format, args);
    }
    va_end(args);
}

void InfinityLoop(char *msg)
{
    while (1)
    {
        ESP_DELAY(1000);
        printf(msg);
    }
}

int GetDigitLength(int number)
{
    int length = 0;
    do
    {
        length++;
        number /= 10;
    } while (number != 0);
    return length;
}
#endif