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

void RTEInit(void)
{
    /* Maybe init some variable here */
    printf("Initializing RTE...\n");
}
#endif
