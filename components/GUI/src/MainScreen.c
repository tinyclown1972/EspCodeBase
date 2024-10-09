/*************************************************************************/
/** Copyright.                                                          **/
/** FileName: HMI_Demo04_Graph.c                                        **/
/** Author: Polarix                                                     **/
/** Version: 1.0.0.0                                                    **/
/** Description: HMI demo for graph interface.                          **/
/*************************************************************************/
//=======================================================================//
//= Include files.                                                      =//
//=======================================================================//
#include "DemoProc.h"
#include "SGUI_VariableBox.h"
#include "SGUI_FontResource.h"
#include <string.h>
#include <esp_log.h>
#include <esp_system.h>
#ifdef CONFIG_RTE_EN
#endif
#ifdef CONFIG_PUMP_EN
#include "pump.h"
#endif

//=======================================================================//
//= Macro definition.                                                   =//
//=======================================================================//

static int i4Count;

//=======================================================================//
//= Static function declaration.                                        =//
//=======================================================================//
static HMI_ENGINE_RESULT    MainScreen_Initialize(SGUI_SCR_DEV* pstDeviceIF);
static HMI_ENGINE_RESULT    MainScreen_Prepare(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters);
static HMI_ENGINE_RESULT    MainScreen_RefreshScreen(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters);
static HMI_ENGINE_RESULT    MainScreen_ProcessEvent(SGUI_SCR_DEV* pstDeviceIF, const HMI_EVENT_BASE* pstEvent, SGUI_INT* piActionID);
static HMI_ENGINE_RESULT    MainScreen_PostProcess(SGUI_SCR_DEV* pstDeviceIF, HMI_ENGINE_RESULT eProcResult, SGUI_INT iActionID);
static void                 MainScreen_DrawStaticText(SGUI_SCR_DEV *pstDeviceIF);

//=======================================================================//
//= Static variable declaration.                                        =//
//=======================================================================//
HMI_SCREEN_ACTION               s_stMainScreenActions     = {
                                                                MainScreen_Initialize,
                                                                MainScreen_Prepare,
                                                                MainScreen_RefreshScreen,
                                                                MainScreen_ProcessEvent,
                                                                MainScreen_PostProcess,
                                                            };

//=======================================================================//
//= Global variable declaration.                                        =//
//=======================================================================//
HMI_SCREEN_OBJECT               g_stMain_Screen         =   {   HMI_SCREEN_ID_MAIN_SCREEN,
                                                                &s_stMainScreenActions
                                                            };

//=======================================================================//
//= Function define.                                                    =//
//=======================================================================//
HMI_ENGINE_RESULT MainScreen_Initialize(SGUI_SCR_DEV* pstDeviceIF)
{
    /*----------------------------------*/
    /* Variable Declaration             */
    /*----------------------------------*/
    i4Count = 0;
    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/

    return HMI_RET_NORMAL;
}

HMI_ENGINE_RESULT MainScreen_Prepare(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters)
{
    /*----------------------------------*/
    /* Variable Declaration             */
    /*----------------------------------*/

    /*----------------------------------*/
    /* Initialize                       */
    /*----------------------------------*/

    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/
    MainScreen_DrawStaticText(pstDeviceIF);
    MainScreen_UpdateWaterLevel(pstDeviceIF, (uint8_t)0);

    return HMI_RET_NORMAL;
}

HMI_ENGINE_RESULT MainScreen_RefreshScreen(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters)
{
    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/

    return HMI_RET_NORMAL;
}

HMI_ENGINE_RESULT MainScreen_ProcessEvent(SGUI_SCR_DEV* pstDeviceIF, const HMI_EVENT_BASE* pstEvent, SGUI_INT* piActionID)
{
    /*----------------------------------*/
    /* Variable Declaration             */
    /*----------------------------------*/
    HMI_ENGINE_RESULT           eProcessResult;
    SGUI_UINT16                 uiKeyValue;
    KEY_PRESS_EVENT*            pstKeyEvent;
    SGUI_INT                    iProcessAction;

    /*----------------------------------*/
    /* Initialize                       */
    /*----------------------------------*/
    eProcessResult =            HMI_RET_NORMAL;
    iProcessAction =            HMI_DEMO_PROC_NO_ACT;

    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/
    if(EVENT_ID_KEY_PRESS == pstEvent->iID)
    {
        pstKeyEvent = (KEY_PRESS_EVENT*)pstEvent;
        uiKeyValue = KEY_CODE_VALUE(pstKeyEvent->Data.uiKeyValue);

        switch(uiKeyValue)
        {
            case KEY_VALUE_ENTER:
            {
                /* Should goto configure screen */
                iProcessAction = HMI_DEMO_PROC_CONFIRM;
                break;
            }
            case KEY_VALUE_ESC:
            {
                iProcessAction = HMI_DEMO_PROC_CANCEL;
                break;
            }
            case KEY_VALUE_UP:
            {
                /* Should Add water manually, goto next to handle */
                esp_restart();
                break;
            }
            case KEY_VALUE_DOWN:
            {
                /* Should Add water manually */
                /* Call refresh for debug use */
                if((RTEGetgePumpStateMachine() == PUMP_RUN) || (RTEGetgePumpStateMachine() == PUMP_RUN_MANUAL))
                {
                    RTESetgePumpStateMachine(PUMP_INIT);
                }
                else
                {
                    RTESetgePumpStateMachine(PUMP_RUN_MANUAL);
                }
                break;
            }
            default:
            {
                /* No process. */
                break;
            }
        }
    }

    if(NULL != piActionID)
    {
        *piActionID = iProcessAction;
    }

    return eProcessResult;
}

HMI_ENGINE_RESULT MainScreen_PostProcess(SGUI_SCR_DEV* pstDeviceIF, HMI_ENGINE_RESULT eProcResult, SGUI_INT iActionID)
{
    if(HMI_PROCESS_SUCCESSFUL(eProcResult))
    {
        if(iActionID == HMI_DEMO_PROC_CONFIRM)
        {
            HMI_SwitchScreen(HMI_SCREEN_ID_DEMO_VARIABLE_BOX, NULL);
        }
        else if(iActionID == HMI_DEMO_PROC_CANCEL)
        {
            HMI_SwitchScreen(HMI_SCREEN_ID_DEMO_TEXT_NOTICE, "Confirm to store parameter and RESTART!");
        }
    }

    return HMI_RET_NORMAL;
}

void MainScreen_UpdateWaterLevel(SGUI_SCR_DEV* pstDeviceIF, uint8_t u8WaterLevel)
{
    /*----------------------------------*/
    /* Variable Declaration             */
    /*----------------------------------*/
    SGUI_RECT               stTextWaterDisplayArea;
    SGUI_POINT              stInnerPos;
    char                    DisplayText[6] = "";

    /*----------------------------------*/
    /* Initialize                       */
    /*----------------------------------*/
    /* Use full screen */
    stTextWaterDisplayArea.iX = 0;
    stTextWaterDisplayArea.iY = 32;
    stTextWaterDisplayArea.iHeight = 32;
    stTextWaterDisplayArea.iWidth = 128;

    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/
    if(NULL != pstDeviceIF)
    {
        sprintf(DisplayText, "%d", u8WaterLevel);

        /* Centralize */
        stInnerPos.iX = (128 - (strlen(DisplayText) * 8)) / 2;
        stInnerPos.iY = 0;

        SGUI_Text_DrawText(pstDeviceIF, DisplayText, &SGUI_DEFAULT_FONT_16, &stTextWaterDisplayArea, &stInnerPos, SGUI_DRAW_NORMAL);
    }
}

void MainScreen_DrawStaticText(SGUI_SCR_DEV* pstDeviceIF)
{
    /*----------------------------------*/
    /* Variable Declaration             */
    /*----------------------------------*/
    SGUI_RECT               stTextWaterDisplayArea;
    SGUI_POINT              stInnerPos;

    /*----------------------------------*/
    /* Initialize                       */
    /*----------------------------------*/
    /* Use full screen */
    stTextWaterDisplayArea.iX = 0;
    stTextWaterDisplayArea.iY = 0;
    stTextWaterDisplayArea.iHeight = 32;
    stTextWaterDisplayArea.iWidth = 128;

    /* Centralize */
    stInnerPos.iX = 0;
    stInnerPos.iY = 0;

    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/
    if(NULL != pstDeviceIF)
    {
        SGUI_Text_DrawText(pstDeviceIF, "Water level:", &SGUI_DEFAULT_FONT_16, &stTextWaterDisplayArea, &stInnerPos, SGUI_DRAW_NORMAL);
    }
}
