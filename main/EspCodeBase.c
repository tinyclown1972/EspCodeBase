#include "esp_task.h"
#include <esp_log.h>
#include <esp_system.h>
#include <freertos/task.h>
#include <stdio.h>

#ifdef CONFIG_ADC_KEY_ENABLE
#include "AdcKey.h"
#endif

#ifdef CONFIG_MYGPIO_EN
#include "MyGpio.h"
#endif

#ifdef CONFIG_RTE_EN
#include "RTE.h"
#endif

#ifdef CONFIG_UTILS_EN
#include "utils.h"
#endif

#ifdef CONFIG_SSD1306_X_ENABLE
#include "driver_ssd1306_basic.h"
#endif

#ifdef CONFIG_SIMPLE_GUI_ENABLE
#include "DemoProc.h"
#endif

#ifdef CONFIG_ADC_KEY_ENABLE
#include <driver/adc.h>
#include "AdcKey.h"
#endif

#ifdef CONFIG_PUMP_EN
#include "pump.h"
#endif

#define TAG "EspCodeBase"

typedef struct
{
    TaskFunction_t pxTaskFunctionPointer;
    const char *const pcTaskName;
    const configSTACK_DEPTH_TYPE usStackDepth;
    void *const pvParameters;
    UBaseType_t uxPriority;
    TaskHandle_t *const pxCreatedTask;
}tTaskNode;

static tTaskNode TaskTbl[] = {
#if defined(CONFIG_SIMPLE_GUI_ENABLE) && defined(CONFIG_SSD1306_X_ENABLE)
    {DisplayTask, "DisplayTask", 2048, NULL, 1, NULL},
#endif
#ifdef CONFIG_ADC_KEY_ENABLE
    {AdcTask,     "AdcTask",     2048, NULL, 2, NULL},
#endif
#ifdef CONFIG_PUMP_EN
    {PumpThread, "PumpTask", 2048, NULL, 1, NULL},
#endif

};

static const uint8_t u8TaskTblLen = sizeof(TaskTbl)/sizeof(tTaskNode);

uint8_t CreateTask()
{
    uint8_t u8Index = 0;

    for(; u8Index < u8TaskTblLen; u8Index++)
    {
        xTaskCreate(TaskTbl[u8Index].pxTaskFunctionPointer, \
                    TaskTbl[u8Index].pcTaskName, \
                    TaskTbl[u8Index].usStackDepth, \
                    TaskTbl[u8Index].pvParameters, \
                    TaskTbl[u8Index].uxPriority, \
                    TaskTbl[u8Index].pxCreatedTask);
    }

    return 0;
}

void app_main(void)
{
    GpioInit();
    CreateTask();

    while(true)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    /* Should never run into here */
    esp_restart();
}
