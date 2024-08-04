#ifndef RTE_H
#define RTE_H

#include "utils.h"

#define RTE_VAR(type, name, initVal) VAR_AUTO_GEN(RTE, type, name, initVal)
#define RTE_STAT(type, name, initVal) STAT_AUTO_GEN(RTE, type, name)

#define RTE_DEBUG_PRINT(x) DEBUG_PRINT(DEBUG_PRINT_RTE_FLAG, x)
#define RTE_DEBUG_PRINT_ARG1(x, arg1) DEBUG_PRINT(DEBUG_PRINT_RTE_FLAG, x, arg1)
#define RTE_DEBUG_PRINT_ARG2(x, arg1, arg2)                                    \
    DEBUG_PRINT(DEBUG_PRINT_RTE_FLAG, x, arg1, arg2)
#define RTE_DEBUG_PRINT_ARG3(x, arg1, arg2, arg3)                              \
    DEBUG_PRINT(DEBUG_PRINT_RTE_FLAG, x, arg1, arg2, arg3)

RTE_STAT(long, lCurrentWeight, 0);
RTE_STAT(uint32_t, u32DebugPrintFlag, 0);
RTE_STAT(int32_t, i32EmptyBowl, 0);
RTE_STAT(int32_t, i32LowWaterBowl, 0);
RTE_STAT(int32_t, i32HighWaterBowl, 0);
RTE_STAT(uint8_t, WaterLevel, 0);

extern void RTEInit(void);

#endif // RTE_H
