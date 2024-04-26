#include "mygpio.h"
#ifdef CONFIG_MYGPIO_EN
#include "argtable3/argtable3.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef CONFIG_UTILS_ENABLE
#include "utils.h"
#endif

#ifdef CONFIG_CONSOLE_ENABLE
#include "esp_console.h"
#endif

#ifdef CONFIG_RTE_ENABLE
#include "RTE.h"
#endif

tGpioConf gGpioConf[] = {
#ifdef CONFIG_HX711_EN
    {"HX711DT", GPIO_E_HX711_DT, 0, GPIO_MODE_INPUT, GPIO_INTR_DISABLE},
    {"HX711SCK", GPIO_E_HX711_SCK, 0, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE},
#endif
#ifdef CONFIG_PUMP_EN
    {"PumpEn", GPIO_E_PUMP_EN, 0, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE, GPIO_PULLUP_ENABLE, GPIO_PULLDOWN_DISABLE},
#endif
#ifdef CONFIG_ESP32_C3_LED_ENABLE
    {"LedD4", GPIO_E_LED_D4, 0, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE, GPIO_PULLUP_ENABLE, GPIO_PULLDOWN_DISABLE},
    {"LedD5", GPIO_E_LED_D5, 0, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE, GPIO_PULLUP_ENABLE, GPIO_PULLDOWN_DISABLE},
#endif

};
const int gGpioConfSize = sizeof(gGpioConf) / sizeof(gGpioConf[0]);

#ifdef CONFIG_CONSOLE_ENABLE
/** Arguments used by 'join' function */
static struct
{
    struct arg_int *gpioNum;
    struct arg_int *gpioVal;
    struct arg_end *end;
} GPIO_join_args;

gpio_config_t ioConfig;

int GpioCli(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&GPIO_join_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, GPIO_join_args.end, argv[0]);
        return 1;
    }
    if ((GPIO_join_args.gpioNum->count) && (GPIO_join_args.gpioVal->count))
    {
        ioConfig.pin_bit_mask = (1ULL << GPIO_join_args.gpioNum->ival[0]);
        // ioConfig.mode = GPIO_MODE_OUTPUT;
        // gpio_config(&ioConfig);
        gpio_set_level(GPIO_join_args.gpioNum->ival[0],
                       GPIO_join_args.gpioVal->ival[0]);
        GPIO_DEBUG_PRINT_ARG2("GPIO%d set to %d\n",
                              GPIO_join_args.gpioNum->ival[0],
                              GPIO_join_args.gpioVal->ival[0]);
    }
    else
    {
        return 1;
    }
    return 0;
}

void register_Gpio(void)
{
    GPIO_join_args.gpioNum =
        arg_int0("n", "gpio", "<n>", "Gpio num to configure");
    GPIO_join_args.gpioVal =
        arg_int1("v", "value", "<v>", "Gpio Value to set, 0 or 1");
    GPIO_join_args.end = arg_end(2);

    const esp_console_cmd_t join_cmd = {.command = "gpio",
                                        .help = "Set gpio value",
                                        .hint = NULL,
                                        .func = &GpioCli,
                                        .argtable = &GPIO_join_args};

    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
#endif

void GpioInit()
{
    int i = 0;
    for (i = 0; i < gGpioConfSize; i++)
    {
        gpio_config_t ioConfig;
        ioConfig.pin_bit_mask = (1ULL << gGpioConf[i].gpioNum);
        ioConfig.mode = gGpioConf[i].mode;
        ioConfig.intr_type = gGpioConf[i].intr;
        ioConfig.pull_up_en = gGpioConf[i].pull_up_en;
        ioConfig.pull_down_en = gGpioConf[i].pull_down_en;
        gpio_config(&ioConfig);
        gpio_set_level(gGpioConf[i].gpioNum, gGpioConf[i].gpioVal);
    }
};
#endif