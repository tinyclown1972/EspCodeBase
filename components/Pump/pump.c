#include "argtable3/argtable3.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "pump.h"

#ifdef CONFIG_PUMP_EN
#ifdef CONFIG_CONSOLE_EN
#include "esp_console.h"
#endif

#ifdef CONFIG_RTE_EN
#include "RTE.h"
#endif

#ifdef CONFIG_UTILS_EN
#include "utils.h"
#endif

#ifdef CONFIG_MYGPIO_EN
#include "mygpio.h"
#endif

#ifdef CONFIG_HX711_EN
#include "HX711.h"
#endif

#ifdef CONFIG_CONSOLE_EN
/** Arguments used by 'join' function */
static struct
{
    struct arg_str *gpioVal;
    struct arg_end *end;
} PUMP_join_args;

int PumpCli(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&PUMP_join_args);
    if (nerrors != 0)
    {
        arg_print_errors(stderr, PUMP_join_args.end, argv[0]);
        return 1;
    }
    if ((strcmp(PUMP_join_args.gpioVal->sval[0], "en") == 0) ||
        (strcmp(PUMP_join_args.gpioVal->sval[0], "enable") == 0))
    {
        // GPIO_PULL_HIGH(GPIO_E_PUMP_EN);
        gePumpStateMachine = PUMP_RUN;
    }
    else if ((strcmp(PUMP_join_args.gpioVal->sval[0], "dis") == 0) ||
             (strcmp(PUMP_join_args.gpioVal->sval[0], "disable") == 0))
    {
        GPIO_PULL_LOW(GPIO_E_PUMP_EN);
        gePumpStateMachine = PUMP_WAIT;
    }
    return ESP_OK;
}

void register_Pump(void)
{
    PUMP_join_args.gpioVal =
        arg_str1("v", "en", "<v>", "Enable or Disable Pump [en/dis]");
    PUMP_join_args.end = arg_end(1);

    const esp_console_cmd_t join_cmd = {.command = "pump",
                                        .help = "Set pump state enable/disable",
                                        .hint = NULL,
                                        .func = &PumpCli,
                                        .argtable = &PUMP_join_args};

    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
#endif

#define PUMP_STATE_MACHINE_PERIOD (250)
#define PUMP_CONTINUE_PUMP_TIME   (60 * (1000/PUMP_STATE_MACHINE_PERIOD))  /* 60S */

#ifdef CONFIG_SR04_EN
#define SR04_WATER_THRESHOLD    (4)  /* In Range(ow+SR04_WATER_THRESHOLD, Low) will add water */
#define SR04_ADD_CONFIRM        (10 * (1000/PUMP_STATE_MACHINE_PERIOD))
#define SR04_SHUTDOWN_CONFIRM   (4 * (1000/PUMP_STATE_MACHINE_PERIOD))
#endif

RTE_VAR(ePumpState, gePumpStateMachine, PUMP_INIT);

void PumpThread(void *pvParameter)
{
    ESP_DELAY(3000);
    static tPumpWaterSituation bowlSituation = PUMP_WATER_BOWL_NORMAL;
    static uint32_t continuePumpWaterTime = 0;

    while (1)
    {
#ifdef CONFIG_HX711_EN
        bowlSituation = (tPumpWaterSituation)HX711CheckWeight();
#endif
#ifdef CONFIG_SR04_EN
        static uint8_t u8ConfirmAddWater = 0U;
        static uint8_t u8ConfirmShut = 0U;
        uint8_t u8WaterLevel = RTEGetWaterLevel();
        int32_t i32HighVal = 0;
        int32_t i32LowVal  = 0;

        i32HighVal = RTEGetHighThreshold();
        i32LowVal = RTEGetLowThreshold();
        if((int32_t)u8WaterLevel >= (i32LowVal + SR04_WATER_THRESHOLD))
        {
            bowlSituation = PUMP_WATER_BOWL_DOWN;
        }
        else if(((int32_t)u8WaterLevel < (i32LowVal + SR04_WATER_THRESHOLD)) && ((int32_t)u8WaterLevel >= i32LowVal))
        {
            bowlSituation = PUMP_WATER_BOWL_EMPTY;
        }
        else if(((int32_t)u8WaterLevel < i32LowVal) && ((int32_t)u8WaterLevel >= i32HighVal))
        {
            bowlSituation = PUMP_WATER_BOWL_NORMAL;
        }
        else if((int32_t)u8WaterLevel < i32HighVal)
        {
            bowlSituation = PUMP_WATER_BOWL_FULL;
        }
        else
        {
            bowlSituation = PUMP_WATER_BOWL_CRITICAL;
        }

        if(bowlSituation == PUMP_WATER_BOWL_EMPTY)
        {
            if(u8ConfirmAddWater != SR04_ADD_CONFIRM) /* 5 times of add water will add water, otherwise treat as trick */
            {
                u8ConfirmAddWater++;
                bowlSituation = PUMP_WATER_BOWL_NORMAL;
            }
            else
            {
                u8ConfirmAddWater = 0;
            }
        }
        else if((bowlSituation == PUMP_WATER_BOWL_DOWN) ||
                (bowlSituation == PUMP_WATER_BOWL_FULL) ||
                (bowlSituation == PUMP_WATER_BOWL_CRITICAL))
        {
            if(u8ConfirmShut != SR04_SHUTDOWN_CONFIRM) /* 3 times of shut pump, otherwise treat as trick */
            {
                u8ConfirmShut++;
                bowlSituation = PUMP_WATER_BOWL_NORMAL;
            }
            else
            {
                u8ConfirmShut = 0;
            }
        }
        else
        {
            u8ConfirmAddWater = 0;
            u8ConfirmShut = 0;
        }
#endif

        switch (gePumpStateMachine)
        {
        case PUMP_INIT:
            /* Do something */
            PUMP_SHUTDOWN;
            PUMP_DEBUG_PRINT("Pump in PUMP_INIT situation\n");
            continuePumpWaterTime = 0;
            if ((bowlSituation == PUMP_WATER_BOWL_CRITICAL) ||
                (bowlSituation == PUMP_WATER_BOWL_DOWN))
            {
                gePumpStateMachine = PUMP_INIT;
            }
            else
            {
                gePumpStateMachine = PUMP_WAIT;
            }
            break;
        case PUMP_WAIT:
            /* Do something */
            PUMP_SHUTDOWN;
            PUMP_DEBUG_PRINT("Pump in PUMP_WAIT situation\n");
            continuePumpWaterTime = 0;
            if ((bowlSituation == PUMP_WATER_BOWL_CRITICAL) ||
                (bowlSituation == PUMP_WATER_BOWL_DOWN))
            {
                gePumpStateMachine = PUMP_INIT;
            }
            else if ((bowlSituation == PUMP_WATER_BOWL_FULL) ||
                     (bowlSituation == PUMP_WATER_BOWL_NORMAL))
            {
                gePumpStateMachine = PUMP_WAIT;
            }
            else if (bowlSituation == PUMP_WATER_BOWL_EMPTY)
            {
                gePumpStateMachine = PUMP_RUN;
            }
            break;
        case PUMP_RUN:
            /* Do something */
            PUMP_ENABLE;
            PUMP_DEBUG_PRINT("Pump in PUMP_RUN situation\n");
            continuePumpWaterTime += 1;
            if ((bowlSituation == PUMP_WATER_BOWL_CRITICAL) ||
                (bowlSituation == PUMP_WATER_BOWL_DOWN))
            {
                gePumpStateMachine = PUMP_INIT;
            }
            else if ((bowlSituation == PUMP_WATER_BOWL_EMPTY) ||
                     (bowlSituation == PUMP_WATER_BOWL_NORMAL))
            {
                gePumpStateMachine = PUMP_RUN;
            }
            else if (bowlSituation == PUMP_WATER_BOWL_FULL)
            {
                gePumpStateMachine = PUMP_INIT;
            }
            break;
        case PUMP_END:
            /* Do nothing, waiting for someone to wake up */
            PUMP_SHUTDOWN;
            break;
        case PUMP_RESTART:
            /* Restart self */
            PUMP_SHUTDOWN;
            SystemRestart();
            break;
        case PUMP_RUN_MANUAL:
            /* Trigger manually */
            continuePumpWaterTime += 1;
            if((PUMP_WATER_BOWL_CRITICAL == bowlSituation) ||
                (PUMP_WATER_BOWL_FULL == bowlSituation))
            {
                gePumpStateMachine = PUMP_INIT;
            }
            PUMP_ENABLE;
            break;
        default:
            /* Critical go to infinity loop */
            PUMP_SHUTDOWN;
            printf("Pump in PUMP_WATER_BOWL_CRITICAL situation\n");
#ifdef CONFIG_HX711_EN
            while (HX711_WATER_BOWL_DOWN != HX711CheckWeight())
            {
                ESP_DELAY(100);
            }
#endif
            break;
        }

        if (continuePumpWaterTime > PUMP_CONTINUE_PUMP_TIME)
        {
            PUMP_SHUTDOWN;
            gePumpStateMachine = PUMP_INIT;
            // InfinityLoop("Can not Pump water more than 15 Seconds\n");
        }

        ESP_DELAY(PUMP_STATE_MACHINE_PERIOD/portTICK_PERIOD_MS);
    }
}

void PumpInit()
{
    /* Do nothing here */
    /* Init Pump thread */
    printf("Start to init Pump\n");
#ifdef CONFIG_CONSOLE_EN
    register_Pump();
#endif
    xTaskCreate(PumpThread, "PumpTask", 2048, NULL, 1, NULL);
};

#endif
