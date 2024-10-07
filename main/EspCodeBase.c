#include <esp_event.h>
#include <esp_log.h>
#include <esp_netif_types.h>
#include <esp_system.h>
#include <stdio.h>
#include <driver/adc.h>

#include "esp_task.h"
#include <freertos/task.h>
#include <driver/gpio.h>
#include "DemoProc.h"
#include "driver_ssd1306_basic.h"
#include "AdcKey.h"

#ifdef CONFIG_UTILS_EN
#include "utils.h"
#endif

#ifdef CONFIG_MYGPIO_EN
#include "mygpio.h"
#endif

#ifdef CONFIG_PUMP_EN
#include "pump.h"
#endif

#ifdef CONFIG_SR04_EN
#include "SR04.h"
#endif

#ifdef CONFIG_WiFi_EN
#include "MyWiFi.h"
#endif

#ifdef CONFIG_WEB_EN
#include "MyWeb.h"
#endif

#include "freertos/event_groups.h"

#define TAG "EspCodeBase"

EventGroupHandle_t s_wifi_event_group;
static uint8_t u8WiFiConnected = (uint8_t)0;

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
    EventBits_t  u32Bits = 0U;
#ifdef CONFIG_UTILS_EN
    NvsFlashInit();
#endif
#ifdef CONFIG_WiFi_EN
    MyWiFiInit();
#endif
#ifdef CONFIG_MYGPIO_EN
    GpioInit();
#endif
#ifdef CONFIG_PUMP_EN
    PumpInit();
#endif
#ifdef CONFIG_RTE_EN
    RTEInit();
#endif
#ifdef CONFIG_SR04_EN
    SR04Init();
#endif

    xTaskCreate(DisplayTask,"DisplayTask",2048,NULL,1,NULL);

#ifdef CONFIG_ADC_KEY_ENABLE
    xTaskCreate(AdcTask,"AdcTask",2048,NULL,2,NULL);
#endif

    while(true)
    {
#ifdef CONFIG_WiFi_EN
        u32Bits = xEventGroupGetBits(s_wifi_event_group); /* Get connected/Fail bit */
        if(u8WiFiConnected == (uint8_t)0)
        {
            if(((u32Bits & BIT0) == 0x1U) && ( (u32Bits & BIT1) == 0x0U ))
            {
                /* WiFi Connected succeed */
                ESP_LOGI(TAG, "Wifi connected succeed!");
                u8WiFiConnected = (uint8_t)1;

                /* Init task which should run after wifi */
                MyWebInit();
            }
        }
        else
        {
            if((u32Bits & BIT0) == 0x0U)
            {
                /* WiFi Connected succeed */
                ESP_LOGI(TAG, "Wifi disconnected");
                u8WiFiConnected = (uint8_t)0;

                /* DeInit task which should use wifi */
            }
        }
#endif
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    /* Should never run into here */
    esp_restart();
}
