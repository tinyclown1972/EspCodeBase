#include <esp_log.h>
#include <esp_system.h>
#include <stdio.h>
#include <driver/adc.h>

#include "esp_task.h"
#include <freertos/task.h>
#include <driver/gpio.h>
#include "DemoProc.h"
#include "driver_ssd1306_basic.h"
#include "AdcKey.h"

#ifdef CONFIG_MYGPIO_EN
#include "mygpio.h"
#endif

#ifdef CONFIG_PUMP_EN
#include "pump.h"
#endif

#define TAG "EspCodeBase"

uint8_t InitDisplay()
{
    uint8_t retVal;

    retVal = ssd1306_basic_init(SSD1306_INTERFACE_IIC, 0x3c);
    if (retVal != 0)
    {
        printf("[%s:%d]\n",__FUNCTION__,__LINE__);
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

void DisplayTask(void* param)
{
    if(InitDisplay() == 0)
    {
        DemoMainProcess();
    }
    else
    {
        ESP_LOGE(TAG, "Can  not init display!");
        return;
    }
}

void app_main(void)
{
    GpioInit();
    PumpInit();

    RTEInit();

    xTaskCreate(DisplayTask,"DisplayTask",2048,NULL,1,NULL);
    xTaskCreate(AdcTask,"AdcTask",2048,NULL,2,NULL);

    while(true)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    /* Should never run into here */
    esp_restart();
}
