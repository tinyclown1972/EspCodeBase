#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include "nvs_flash.h"
#include "nvs.h"

#define VAR_AUTO_GEN(module, type, name, initValue)                            \
    static type name = initValue;                                              \
    type module##Get##name(void) { return name; }                              \
    void module##Set##name(type newValue) { name = newValue; }

#define STAT_AUTO_GEN(module, type, name)                                      \
    type module##Get##name(void);                                              \
    void module##Set##name(type newValue);

#define ESP_DELAY(x) vTaskDelay(x / portTICK_PERIOD_MS)
#define ESP_YIELD() xTaskNotifyGive(xTaskGetCurrentTaskHandle())
#define GPIO_PULL_HIGH(x) gpio_set_level(x, true);
#define GPIO_PULL_LOW(x) gpio_set_level(x, false);
#define GPIO_GET_VALUE(x) gpio_get_level(x)

typedef enum
{
    DEBUG_PRINT_RTE_FLAG = 0,
    DEBUG_PRINT_HX711_FLAG,
    DEBUG_PRINT_GPIO_FLAG,
    DEBUG_PRINT_PUMP_FLAG,
    DEBUG_PRINT_MQTT_FLAG,
}eDebugPrintFlag;

void register_RTE(void);
extern void TestHello();
void DEBUG_PRINT(uint32_t flag, const char *format, ...);
void InfinityLoop();
int GetDigitLength(int number);

void NvsFlashInit(void);
esp_err_t NvsFlashReadInt32(const char *nameSpace, const char *key,
                            int32_t *pi32Val);
esp_err_t NvsFlashWriteInt32(const char *nameSpace, const char *key,
                             int32_t *pi32Val);
void SystemRestart(void);
#endif // UTILS_H
