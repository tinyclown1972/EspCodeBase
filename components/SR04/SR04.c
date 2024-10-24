#include "driver/gpio.h"
#include "SR04.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#ifdef CONFIG_SR04_EN

#ifdef CONFIG_MYGPIO_EN
#include "mygpio.h"
#endif

#ifdef CONFIG_RTE_EN
#include "RTE.h"
#endif

#include "HMI_Engine.h"
#include "DemoProc.h"

#define MAX_DISTANCE 200       // 最大测量距离，单位厘米
#define TIMEOUT_US 30000       // 超时时间，单位微秒

#define ARRAY_SIZE 10

static double measure_distance()
{
    gpio_set_level(GPIO_E_SR04_TRIG, 0);                    // 拉低触发引脚
    vTaskDelay(10 / portTICK_PERIOD_MS);                // 等待1ms

    gpio_set_level(GPIO_E_SR04_TRIG, 1);                    // 拉高触发引脚，发送10us的脉冲
    vTaskDelay(1 / portTICK_PERIOD_MS);               // 等待10us
    gpio_set_level(GPIO_E_SR04_TRIG, 0);                    // 拉低触发引脚

    double distance = 0.0;
    uint64_t startTime = esp_timer_get_time();         // 获取当前时间

    while (gpio_get_level(GPIO_E_SR04_ECHO) == 0) {            // 等待回声引脚变高
        if ((esp_timer_get_time() - startTime) > TIMEOUT_US) {
            return -1; // 超时，返回-1
        }
    }

    startTime = esp_timer_get_time();                   // 记录回声开始时间

    while (gpio_get_level(GPIO_E_SR04_ECHO) == 1) {            // 等待回声引脚变低
        if ((esp_timer_get_time() - startTime) > TIMEOUT_US) {
            return -1; // 超时，返回-1
        }
    }

    uint64_t pulseDuration = esp_timer_get_time() - startTime; // 计算脉冲持续时间

    if (pulseDuration == 0) {
        return 0; // 没有检测到回声，返回0
    }

    distance = pulseDuration / 29.5; // 计算距离，29.5是声速（cm/us）在空气里

    return distance > MAX_DISTANCE ? MAX_DISTANCE : distance; // 限制最大距离
}

// 计算平均值
static double calculate_mean(uint8_t array[], int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += array[i];
    }
    return sum / size;
}

void SR04Init(void)
{
    xTaskCreate(SR04Thread,"SR04Thread",2048,NULL,2,NULL);
}

void SR04Thread(void *pvParameter)
{
    uint8_t u8Counter = 0;
    uint8_t u8Tmp     = 0;
    uint8_t au8TempArray[10] = {0};
    int     i4ScreenID = 0;

    while(1)
    {
        for (u8Counter = 0; u8Counter < 10; u8Counter++)
        {
            u8Tmp = (uint8_t)measure_distance();
            if(u8Tmp == 0)
            {
                u8Counter--;
            }
            else
            {
                au8TempArray[u8Counter] = u8Tmp;
            }
            vTaskDelay(50 / portTICK_PERIOD_MS); // 等待500ms再次测量
        }

        double mean = calculate_mean(au8TempArray, 10);
        RTESetWaterLevel((uint8_t)mean);

        i4ScreenID = HMI_GetActiveScreenId();
        // printf("ScreenID: %d\n",i4ScreenID);
        if(i4ScreenID == 1011)
        {
            MainScreen_UpdateWaterLevel(DemoGetScrDev(), (uint8_t)mean);
        }
        else if(i4ScreenID == 1006)
        {
            VariableBox_UpdateWaterLevel(DemoGetScrDev(), (uint8_t)mean);
        }
        else
        {
            /* Do not update */
        }
    }
}

#endif
