//
// Created by leonard on 2024/4/23.
//

#include "AdcKey.h"

#include <freertos/freertos.h>
#include <driver/adc.h>
#include <freertos/task.h>

#include "DemoActions.h"

#define CONFIG_ADC_KEY_ENABLE
#ifdef CONFIG_ADC_KEY_ENABLE

volatile int  gKeyCode = KEY_VALUE_NONE;
volatile bool keyTriggered = false;

int GetKeyCodeWithValue(int keyVal)
{
    int iIndex;
    const tKeyNode tKeyTbl[] = {
        {KEY_VALUE_ESC, 1300, 1450},
        {KEY_VALUE_UP, 800, 1000},
        {KEY_VALUE_DOWN, 600, 750},
        {KEY_VALUE_ENTER, 400, 580}
    };

    const int iKeyTblLen = sizeof(tKeyTbl)/sizeof(tKeyNode);

    for(iIndex = 0; iIndex < iKeyTblLen; iIndex++)
    {
        if((keyVal > tKeyTbl[iIndex].iLowVal) && (keyVal < tKeyTbl[iIndex].iHighVal))
        {
            return tKeyTbl[iIndex].iKeyCode;
        }
    }

    return KEY_VALUE_NONE;
}

void AdcTask(void *pvParameter)
{
    static int preKeyCode = KEY_VALUE_NONE;
    int currentKeyCode = KEY_VALUE_NONE;

    // 配置 ADC 通道
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);

    while (1)
    {
        // 读取 ADC 值
        uint32_t adc_value = adc1_get_raw(ADC1_CHANNEL_0);
        // printf("ADC value: %.2f\n", ((adc_value* 2500.00)/4095.00));

        currentKeyCode = GetKeyCodeWithValue((int)(adc_value* 2500.00)/4095.00);
        if((currentKeyCode != KEY_VALUE_NONE) \
            && preKeyCode == KEY_VALUE_NONE)
        {
            /* Key change */
            preKeyCode = currentKeyCode;
        }

        if(currentKeyCode == KEY_VALUE_NONE \
            && preKeyCode != KEY_VALUE_NONE)
        {
            /* We need to send action to some other module */
            /* Last Action still not handle yet */
            gKeyCode = preKeyCode;
            keyTriggered = true;
            preKeyCode = currentKeyCode;
            printf("Key enter\n");
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


#endif