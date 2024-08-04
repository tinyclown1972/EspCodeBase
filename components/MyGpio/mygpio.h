#ifndef _MYGPIO_H
#define _MYGPIO_H

#include "driver/gpio.h"
#include "utils.h"
#include <stdint.h>

#define GPIO_DEBUG_PRINT(x) DEBUG_PRINT(DEBUG_PRINT_GPIO_FLAG, x)
#define GPIO_DEBUG_PRINT_ARG1(x, arg1)                                         \
    DEBUG_PRINT(DEBUG_PRINT_GPIO_FLAG, x, arg1)
#define GPIO_DEBUG_PRINT_ARG2(x, arg1, arg2)                                   \
    DEBUG_PRINT(DEBUG_PRINT_GPIO_FLAG, x, arg1, arg2)
#define GPIO_DEBUG_PRINT_ARG3(x, arg1, arg2, arg3)                             \
    DEBUG_PRINT(DEBUG_PRINT_GPIO_FLAG, x, arg1, arg2, arg3)

typedef struct
{
    char *name;
    int gpioNum;
    int gpioVal;
    char defVal;
    gpio_mode_t mode;
    gpio_int_type_t intr;
    gpio_pullup_t pull_up_en;
    gpio_pulldown_t pull_down_en;
} tGpioConf;

#define MY_GPIO_HIGH 1
#define MY_GPIO_LOW  0

#ifdef CONFIG_HX711_EN
#define GPIO_E_HX711_DT  12
#define GPIO_E_HX711_SCK 18
#endif
#ifdef CONFIG_PUMP_EN
#define GPIO_E_PUMP_EN   3
#endif
#ifdef CONFIG_ESP32_C3_LED_ENABLE
#define GPIO_E_LED_D4    12
#define GPIO_E_LED_D5    13
#endif
#ifdef CONFIG_ADC_KEY_ENABLE
#define GPIO_E_ADC_PIN    GPIO_NUM_0
#endif

#define GPIO_E_SR04_TRIG    1
#define GPIO_E_SR04_ECHO    2

#ifdef CONFIG_CONSOLE_EN
void register_Gpio(void);
#endif

void GpioInit();

#endif
