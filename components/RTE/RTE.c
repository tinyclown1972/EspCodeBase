#include "RTE.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef CONFIG_UTILS_EN
#include "utils.h"
#endif

#ifdef CONFIG_RTE_EN
/* RTE Private variable */
RTE_VAR(long, lCurrentWeight, 0);
RTE_VAR(uint32_t, u32DebugPrintFlag, 0);
RTE_VAR(int32_t, i32EmptyBowl, 450);
RTE_VAR(int32_t, i32LowWaterBowl, 600);
RTE_VAR(int32_t, i32HighWaterBowl, 850);
RTE_VAR(uint8_t, WaterLevel, 0);

#ifdef CONFIG_SR04_EN
static int32_t gi32HighThreshold = 0;
int32_t RTEGetHighThreshold()
{
    if(gi32HighThreshold == 0)
    {
        NvsFlashReadInt32("nvs", "HighL", &gi32HighThreshold);
    }
    return gi32HighThreshold;
}

void RTESetHighThreshold(int32_t i32Val)
{
    gi32HighThreshold = i32Val;
}

static int32_t gi32LowThreshold = 0;
int32_t RTEGetLowThreshold()
{
    if(gi32LowThreshold == 0)
    {
        NvsFlashReadInt32("nvs", "LowL", &gi32LowThreshold);
    }
    return gi32LowThreshold;
}

void RTESetLowThreshold(int32_t i32Val)
{
    gi32LowThreshold = i32Val;
}
#endif

void RTEInit(void)
{
    /* Maybe init some variable here */
    printf("Initializing RTE...\n");
}
#endif
