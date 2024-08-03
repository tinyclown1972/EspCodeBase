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

#define TAG "EspCodeBase"

void InitGpio()
{
    gpio_config_t ioConfig;
    ioConfig.pin_bit_mask = (1ULL << 3);
    ioConfig.mode = GPIO_MODE_INPUT_OUTPUT;
    ioConfig.pull_up_en = 1;
    ioConfig.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&ioConfig);
    gpio_set_level(3, 0);

#ifdef CONFIG_ADC_KEY_ENABLE
    // 配置 GPIO 0 作为 ADC 输入
    gpio_config_t gpio_config1 = {
        .pin_bit_mask = 1ULL << GPIO_ADC_PIN,
        .mode = GPIO_MODE_INPUT,
    };
    gpio_config(&gpio_config1);
#endif

}

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
    InitGpio();
    xTaskCreate(DisplayTask,"DisplayTask",2048,NULL,1,NULL);
    xTaskCreate(AdcTask,"AdcTask",2048,NULL,2,NULL);

    while(true)
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    /* Should never run into here */
    esp_restart();
}
