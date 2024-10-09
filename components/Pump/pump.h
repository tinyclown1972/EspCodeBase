#ifndef _PUMP_H
#define _PUMP_H

#include "RTE.h"
#include "utils.h"
#include <stdint.h>

#define PUMP_DEBUG_PRINT(x) DEBUG_PRINT(DEBUG_PRINT_PUMP_FLAG, x)
#define PUMP_DEBUG_PRINT_ARG1(x, arg1)                                         \
    DEBUG_PRINT(DEBUG_PRINT_PUMP_FLAG, x, arg1)
#define PUMP_DEBUG_PRINT_ARG2(x, arg1, arg2)                                   \
    DEBUG_PRINT(DEBUG_PRINT_PUMP_FLAG, x, arg1, arg2)
#define PUMP_DEBUG_PRINT_ARG3(x, arg1, arg2, arg3)                             \
    DEBUG_PRINT(DEBUG_PRINT_PUMP_FLAG, x, arg1, arg2, arg3)

#define PUMP_SHUTDOWN GPIO_PULL_LOW(GPIO_E_PUMP_EN)
#define PUMP_ENABLE GPIO_PULL_HIGH(GPIO_E_PUMP_EN)

typedef enum
{
    PUMP_INIT,
    PUMP_WAIT,
    PUMP_RUN,
    PUMP_RUN_MANUAL,
    PUMP_END,
    PUMP_RESTART
} ePumpState;

typedef enum
{
    PUMP_WATER_BOWL_DOWN = 0,
    PUMP_WATER_BOWL_EMPTY,
    PUMP_WATER_BOWL_NORMAL,
    PUMP_WATER_BOWL_FULL,
    PUMP_WATER_BOWL_CRITICAL
} tPumpWaterSituation;

RTE_STAT(ePumpState, gePumpStateMachine, PUMP_INIT);

void PumpThread(void *pvParameter);
extern void PumpInit();

#endif
