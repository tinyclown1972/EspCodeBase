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
#include "Resource.h"
#include "SGUI_Notice.h"
#include "SGUI_VariableBox.h"
#include "SGUI_FontResource.h"
#include "SGUI_IconResource.h"

#ifdef CONFIG_UTILS_EN
#include "utils.h"
#endif

#ifdef CONFIG_RTE_EN
#include "RTE.h"
#endif

//=======================================================================//
//= Macro definition.                                                   =//
//=======================================================================//

#define                     VARIABLE_BOX_WIDTH                  (10)
#define                     VARIABLE_NUMBER_BOX_HEIGHT          (12)
#define                     VARIABLE_BOX_POSX                   (60)
#define                     VARIABLE_BOX_NUMBER_HIGH_POSY       (24)
#define                     VARIABLE_BOX_NUMBER_LOW_POSY        (40)

//=======================================================================//
//= Static function declaration.                                        =//
//=======================================================================//
static HMI_ENGINE_RESULT    HMI_DemoVariableBox_Initialize(SGUI_SCR_DEV* pstDeviceIF);
static HMI_ENGINE_RESULT    HMI_DemoVariableBox_Prepare(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters);
static HMI_ENGINE_RESULT    HMI_DemoVariableBox_RefreshScreen(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters);
static HMI_ENGINE_RESULT    HMI_DemoVariableBox_ProcessEvent(SGUI_SCR_DEV* pstDeviceIF, const HMI_EVENT_BASE* pstEvent, SGUI_INT* piActionID);
static HMI_ENGINE_RESULT    HMI_DemoVariableBox_PostProcess(SGUI_SCR_DEV* pstDeviceIF, HMI_ENGINE_RESULT eProcResult, SGUI_INT iActionID);
static void                 HMI_DemoVariableBox_DrawFrame(SGUI_SCR_DEV* pstDeviceIF, SGUI_SZSTR szTitle);

//=======================================================================//
//= Static variable declaration.                                        =//
//=======================================================================//
static SGUI_INT                 s_HighAndLowFlag;
SGUI_NUM_VARBOX_STRUCT          s_stNumberHighBox =     {0x00};
SGUI_NUM_VARBOX_STRUCT          s_stNumberLowBox  =     {0x00};

static SGUI_CHAR                s_szDefaultFrameTitle[] =   SCR4_VAR_BOX_TITLE;
static SGUI_SZSTR               s_szFrameTitle =            s_szDefaultFrameTitle;

HMI_SCREEN_ACTION               s_stDemoVariableBoxActions = {
                                                                HMI_DemoVariableBox_Initialize,
                                                                HMI_DemoVariableBox_Prepare,
                                                                HMI_DemoVariableBox_RefreshScreen,
                                                                HMI_DemoVariableBox_ProcessEvent,
                                                                HMI_DemoVariableBox_PostProcess,
                                                            };

//=======================================================================//
//= Global variable declaration.                                        =//
//=======================================================================//
HMI_SCREEN_OBJECT               g_stHMIDemo_VariableBox =   {   HMI_SCREEN_ID_DEMO_VARIABLE_BOX,
                                                                &s_stDemoVariableBoxActions
                                                            };


//=======================================================================//
//= Function define.                                                    =//
//=======================================================================//
HMI_ENGINE_RESULT HMI_DemoVariableBox_Initialize(SGUI_SCR_DEV* pstDeviceIF)
{
    /*----------------------------------*/
    /* Variable Declaration             */
    /*----------------------------------*/
    SGUI_NUM_VARBOX_PARAM   stHighParam;
    SGUI_NUM_VARBOX_PARAM   stLowParam;
    int32_t i32TempVal = 0;

    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/
    s_HighAndLowFlag = -1;

    stHighParam.eAlignment = SGUI_CENTER;
    stHighParam.iMin = 0;
    stHighParam.iMax = 65535;
    stHighParam.pstFontRes = &SGUI_DEFAULT_FONT_12;
    stHighParam.stLayout.iX = VARIABLE_BOX_POSX+2;
    stHighParam.stLayout.iY = VARIABLE_BOX_NUMBER_HIGH_POSY;
    stHighParam.stLayout.iWidth = 60;
    stHighParam.stLayout.iHeight = SGUI_DEFAULT_FONT_12.iHeight;

    SGUI_NumberVariableBox_Initialize(&s_stNumberHighBox, &stHighParam);

    stLowParam.eAlignment = SGUI_CENTER;
    stLowParam.iMin = 0;
    stLowParam.iMax = 65535;
    stLowParam.pstFontRes = &SGUI_DEFAULT_FONT_12;
    stLowParam.stLayout.iX = VARIABLE_BOX_POSX+2;
    stLowParam.stLayout.iY = VARIABLE_BOX_NUMBER_LOW_POSY;
    stLowParam.stLayout.iWidth = 60;
    stLowParam.stLayout.iHeight = SGUI_DEFAULT_FONT_12.iHeight;

    SGUI_NumberVariableBox_Initialize(&s_stNumberLowBox, &stLowParam);

    HMI_DemoVariableBox_DrawFrame(pstDeviceIF, (SGUI_SZSTR)s_szFrameTitle);
    SGUI_NumberVariableBox_SetValue(&s_stNumberHighBox, RTEGetHighThreshold());
    SGUI_NumberVariableBox_SetValue(&s_stNumberLowBox, RTEGetLowThreshold());

    return HMI_RET_NORMAL;
}

HMI_ENGINE_RESULT HMI_DemoVariableBox_Prepare(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters)
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
    // Draw frame
    s_szFrameTitle = s_szDefaultFrameTitle;
    HMI_DemoVariableBox_DrawFrame(pstDeviceIF, (SGUI_SZSTR)s_szFrameTitle);
    VariableBox_UpdateWaterLevel(pstDeviceIF, (uint8_t)RTEGetWaterLevel());

    SGUI_NumberVariableBox_SetValue(&s_stNumberHighBox, RTEGetHighThreshold());
    SGUI_NumberVariableBox_SetValue(&s_stNumberLowBox, RTEGetLowThreshold());

    RTCTimerEnable(true);
    return HMI_RET_NORMAL;
}

HMI_ENGINE_RESULT HMI_DemoVariableBox_RefreshScreen(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters)
{
    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/

    // Draw frame
    HMI_DemoVariableBox_DrawFrame(pstDeviceIF, (SGUI_SZSTR)s_szFrameTitle);
    // Draw number box
    SGUI_Basic_DrawRectangle(pstDeviceIF, VARIABLE_BOX_POSX, VARIABLE_BOX_NUMBER_HIGH_POSY,
                            s_stNumberHighBox.stParam.stLayout.iWidth+4, s_stNumberHighBox.stParam.stLayout.iHeight+4,
                             SGUI_COLOR_FRGCLR, SGUI_COLOR_BKGCLR);
    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberHighBox, (0 == s_HighAndLowFlag)?SGUI_DRAW_REVERSE:SGUI_DRAW_NORMAL);

    SGUI_Basic_DrawRectangle(pstDeviceIF, VARIABLE_BOX_POSX, VARIABLE_BOX_NUMBER_LOW_POSY,
                            s_stNumberHighBox.stParam.stLayout.iWidth+4, s_stNumberHighBox.stParam.stLayout.iHeight+4,
                             SGUI_COLOR_FRGCLR, SGUI_COLOR_BKGCLR);
    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberLowBox, (1 == s_HighAndLowFlag)?SGUI_DRAW_REVERSE:SGUI_DRAW_NORMAL);

    // Draw text box
    // SGUI_Basic_DrawRectangle(pstDeviceIF, VARIABLE_BOX_POSX, VARIABLE_BOX_TEXT_POSY,
    //                         s_stTextVariableBox.stParam.stLayout.iWidth+4, s_stTextVariableBox.stParam.stLayout.iHeight+4,
    //                         SGUI_COLOR_FRGCLR, SGUI_COLOR_BKGCLR);
    // SGUI_TextVariableBox_Repaint(pstDeviceIF, &s_stTextVariableBox, VARIABLE_MASK_CHARACTER, (0 == s_HighAndLowFlag)?SGUI_DRAW_NORMAL:SGUI_DRAW_REVERSE);

    return HMI_RET_NORMAL;
}

HMI_ENGINE_RESULT HMI_DemoVariableBox_ProcessEvent(SGUI_SCR_DEV* pstDeviceIF, const HMI_EVENT_BASE* pstEvent, SGUI_INT* piActionID)
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
                s_HighAndLowFlag = ((s_HighAndLowFlag+1)%2);
                if(1 == s_HighAndLowFlag)
                {
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberHighBox, SGUI_DRAW_REVERSE);
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberLowBox, SGUI_DRAW_NORMAL);
                }
                else
                {
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberHighBox, SGUI_DRAW_NORMAL);
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberLowBox, SGUI_DRAW_REVERSE);
                }
                break;
            }
            case KEY_VALUE_ESC:
            {
                iProcessAction = HMI_DEMO_PROC_CANCEL;
                break;
            }
            case KEY_VALUE_UP:
            {
                if(1 == s_HighAndLowFlag)
                {
                    /* High Water Level refersh */
                    SGUI_NumberVariableBox_SetValue(&s_stNumberHighBox, SGUI_NumberVariableBox_GetValue(&s_stNumberHighBox)+1);
                    RTESetHighThreshold(SGUI_NumberVariableBox_GetValue(&s_stNumberHighBox));
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberHighBox, SGUI_DRAW_REVERSE);

                    /* Low Water Level refresh */
                    /* No need to re-set water level,since no change happend, if display incorrect uncomment this line */
                    // SGUI_NumberVariableBox_SetValue(&s_stNumberLowBox, SGUI_NumberVariableBox_GetValue(&s_stNumberLowBox));
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberLowBox, SGUI_DRAW_NORMAL);
                }
                else if(0 == s_HighAndLowFlag)
                {
                    /* High Water Level refersh */
                    /* No need to re-set water level,since no change happend, if display incorrect uncomment this line */
                    // SGUI_NumberVariableBox_SetValue(&s_stNumberHighBox, SGUI_NumberVariableBox_GetValue(&s_stNumberHighBox));
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberHighBox, SGUI_DRAW_NORMAL);

                    /* Low Water Level refresh */
                    SGUI_NumberVariableBox_SetValue(&s_stNumberLowBox, SGUI_NumberVariableBox_GetValue(&s_stNumberLowBox)+1);
                    RTESetLowThreshold(SGUI_NumberVariableBox_GetValue(&s_stNumberLowBox));
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberLowBox, SGUI_DRAW_REVERSE);
                }
                else
                {
                    /* Do nothing */
                }
                break;
            }
            case KEY_VALUE_DOWN:
            {
                if(1 == s_HighAndLowFlag)
                {
                    /* High Water Level refersh */
                    SGUI_NumberVariableBox_SetValue(&s_stNumberHighBox, SGUI_NumberVariableBox_GetValue(&s_stNumberHighBox)-1);
                    RTESetHighThreshold(SGUI_NumberVariableBox_GetValue(&s_stNumberHighBox));
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberHighBox, SGUI_DRAW_REVERSE);

                    /* Low Water Level refresh */
                    /* No need to re-set water level,since no change happend, if display incorrect uncomment this line */
                    // SGUI_NumberVariableBox_SetValue(&s_stNumberLowBox, SGUI_NumberVariableBox_GetValue(&s_stNumberLowBox));
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberLowBox, SGUI_DRAW_NORMAL);
                }
                else if(0 == s_HighAndLowFlag)
                {
                    /* High Water Level refersh */
                    /* No need to re-set water level,since no change happend, if display incorrect uncomment this line */
                    // SGUI_NumberVariableBox_SetValue(&s_stNumberHighBox, SGUI_NumberVariableBox_GetValue(&s_stNumberHighBox));
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberHighBox, SGUI_DRAW_NORMAL);

                    /* Low Water Level refresh */
                    SGUI_NumberVariableBox_SetValue(&s_stNumberLowBox, SGUI_NumberVariableBox_GetValue(&s_stNumberLowBox)-1);
                    RTESetLowThreshold(SGUI_NumberVariableBox_GetValue(&s_stNumberLowBox));
                    SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberLowBox, SGUI_DRAW_REVERSE);
                }
                else
                {
                    /* Do nothing */
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

HMI_ENGINE_RESULT HMI_DemoVariableBox_PostProcess(SGUI_SCR_DEV* pstDeviceIF, HMI_ENGINE_RESULT eProcResult, SGUI_INT iActionID)
{
    if(HMI_PROCESS_SUCCESSFUL(eProcResult))
    {
        if(HMI_DEMO_PROC_CANCEL == iActionID)
        {
            HMI_GoBack(NULL);
        }
    }

    return HMI_RET_NORMAL;
}

void HMI_DemoVariableBox_DrawFrame(SGUI_SCR_DEV* pstDeviceIF, SGUI_SZSTR szTitle)
{
    /*----------------------------------*/
    /* Variable Declaration             */
    /*----------------------------------*/
    SGUI_RECT               stTextDisplayArea;
    SGUI_RECT               stTextHighDisplayArea;
    SGUI_RECT               stTextLowDisplayArea;
    SGUI_POINT              stInnerPos;
    SGUI_POINT              stTextInnerPos;

    /*----------------------------------*/
    /* Initialize                       */
    /*----------------------------------*/
    stTextDisplayArea.iX = 4;
    stTextDisplayArea.iY = 4;
    stTextDisplayArea.iHeight = 12;

    stTextHighDisplayArea.iX = 4;
    stTextHighDisplayArea.iY = VARIABLE_BOX_NUMBER_HIGH_POSY;
    stTextHighDisplayArea.iHeight = 12;

    stTextLowDisplayArea.iX = 4;
    stTextLowDisplayArea.iY = VARIABLE_BOX_NUMBER_LOW_POSY;
    stTextLowDisplayArea.iHeight = 12;

    stInnerPos.iX = 15;
    stInnerPos.iY = 0;

    stTextInnerPos.iX = 5;
    stTextInnerPos.iY = 0;

    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/
    if(NULL != pstDeviceIF)
    {
        stTextDisplayArea.iWidth = pstDeviceIF->stSize.iWidth-8;
        stTextHighDisplayArea.iWidth = 55;
        stTextLowDisplayArea.iWidth = 55;
        SGUI_Basic_DrawRectangle(pstDeviceIF, 0, 0, SGUI_RECT_WIDTH(pstDeviceIF->stSize), SGUI_RECT_HEIGHT(pstDeviceIF->stSize), SGUI_COLOR_FRGCLR, SGUI_COLOR_BKGCLR);
        SGUI_Basic_DrawRectangle(pstDeviceIF, 2, 2, SGUI_RECT_WIDTH(pstDeviceIF->stSize)-4, SGUI_RECT_HEIGHT(pstDeviceIF->stSize)-4, SGUI_COLOR_FRGCLR, SGUI_COLOR_TRANS);
        SGUI_Basic_DrawLine(pstDeviceIF, 3, 17, 124, 17, SGUI_COLOR_FRGCLR);
        SGUI_Text_DrawText(pstDeviceIF, "Water Level:", &SGUI_DEFAULT_FONT_12, &stTextDisplayArea, &stTextInnerPos, SGUI_DRAW_NORMAL);

        SGUI_Text_DrawText(pstDeviceIF, "High", &SGUI_DEFAULT_FONT_12, &stTextHighDisplayArea, &stInnerPos, SGUI_DRAW_REVERSE);
        SGUI_Text_DrawText(pstDeviceIF, "Low ", &SGUI_DEFAULT_FONT_12, &stTextLowDisplayArea, &stInnerPos, SGUI_DRAW_REVERSE);
        SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberHighBox, SGUI_DRAW_NORMAL);
        SGUI_NumberVariableBox_Repaint(pstDeviceIF, &s_stNumberLowBox, SGUI_DRAW_NORMAL);
    }
}

void VariableBox_UpdateWaterLevel(SGUI_SCR_DEV* pstDeviceIF, uint8_t u8WaterLevel)
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
    stTextWaterDisplayArea.iX = 90;
    stTextWaterDisplayArea.iY = 5;
    stTextWaterDisplayArea.iHeight = 12;
    stTextWaterDisplayArea.iWidth = 35;

    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/
    if(NULL != pstDeviceIF)
    {
        sprintf(DisplayText, "%d", u8WaterLevel);

        /* Centralize */
        stInnerPos.iX = 0;
        stInnerPos.iY = 0;

        SGUI_Text_DrawText(pstDeviceIF, DisplayText, &SGUI_DEFAULT_FONT_12, &stTextWaterDisplayArea, &stInnerPos, SGUI_DRAW_NORMAL);
    }
}
