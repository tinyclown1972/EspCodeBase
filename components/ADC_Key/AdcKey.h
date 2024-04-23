//
// Created by leonard on 2024/4/23.
//

#ifndef ADCKEY_H
#define ADCKEY_H
#define CONFIG_ADC_KEY_ENABLE
#ifdef CONFIG_ADC_KEY_ENABLE
#include <driver/gpio.h>

#define GPIO_ADC_PIN GPIO_NUM_0

typedef struct
{
    int iKeyCode;
    int iLowVal;
    int iHighVal;
}tKeyNode;

void AdcTask(void *pvParameter);

#endif
#endif //ADCKEY_H
