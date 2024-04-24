#include "esp_task.h"
#include <driver/adc.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_system.h>
#include <freertos/task.h>
#include <stdio.h>

#include "DemoProc.h"
#include "driver_ssd1306_basic.h"
#include "AdcKey.h"

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
    {DisplayTask, "DisplayTask", 2048, NULL, 1, NULL},
    {AdcTask,     "AdcTask",     2048, NULL, 2, NULL}
};

static const uint8_t u8TaskTblLen = sizeof(TaskTbl)/sizeof(tTaskNode);

void InitGpio()
{
    gpio_config_t ioConfig;
    memset(&ioConfig, 0, sizeof(gpio_config_t));
    ioConfig.pin_bit_mask = (1ULL << 3);
    ioConfig.mode = GPIO_MODE_OUTPUT;
    ioConfig.pull_up_en = 1;
    ioConfig.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&ioConfig);
    gpio_set_level(3, 0);

#ifdef CONFIG_ADC_KEY_ENABLE
    // GPIO 0 ADC
    memset(&ioConfig, 0, sizeof(gpio_config_t));
    ioConfig.pin_bit_mask = 1ULL << GPIO_ADC_PIN,
    ioConfig.mode = GPIO_MODE_INPUT,
    gpio_config(&ioConfig);
#endif

}

uint8_t InitDisplay()
{
    uint8_t retVal;

    retVal = ssd1306_basic_init(SSD1306_INTERFACE_IIC, SSD1306_ADDR_SA0_2);
    if (retVal != 0)
    {
        ssd1306_interface_debug_print("ssd1306: clear screen failed.\n");
    }
    else
    {
        retVal = ssd1306_basic_clear();
        if (retVal != 0)
        {
            ssd1306_interface_debug_print("ssd1306: clear screen failed.\n");
            (void)ssd1306_basic_deinit();
        }
    }

    return retVal;
}

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
}

void app_main(void)
{
    InitGpio();
    // xTaskCreate(DisplayTask,"DisplayTask",2048,NULL,1,NULL);
    // xTaskCreate(AdcTask,"AdcTask",2048,NULL,2,NULL);
    CreateTask();

    while(true)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    /* Should never run into here */
    esp_restart();
}
