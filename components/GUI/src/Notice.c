/*************************************************************************/
/** Copyright.                                                          **/
/** FileName: Notice.c                                                  **/
/** Author: Polarix                                                     **/
/** Description: HMI demo for notice box interface.                     **/
/*************************************************************************/
//=======================================================================//
//= Include files.                                                      =//
//=======================================================================//
#include "DemoProc.h"
#include "SGUI_Notice.h"
#include "SGUI_FontResource.h"
#include "SGUI_IconResource.h"
#include <esp_system.h>
#include "utils.h"
#include <esp_log.h>
#include "SGUI_VariableBox.h"

//=======================================================================//
//= Macro definition.                                                   =//
//=======================================================================//
#define                 NOTICE_TEXT_BUFFER_SIZE             (64)

//=======================================================================//
//= Static function declaration.                                        =//
//=======================================================================//
static HMI_ENGINE_RESULT    HMI_DemoNotice_Initialize(SGUI_SCR_DEV* pstDeviceIF);
static HMI_ENGINE_RESULT    HMI_DemoNotice_Prepare(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters);
static HMI_ENGINE_RESULT    HMI_DemoNotice_RefreshScreen(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters);
static HMI_ENGINE_RESULT    HMI_DemoNotice_ProcessEvent(SGUI_SCR_DEV* pstDeviceIF, const HMI_EVENT_BASE* pstEvent, SGUI_INT* piActionID);
static HMI_ENGINE_RESULT    HMI_DemoNotice_PostProcess(SGUI_SCR_DEV* pstDeviceIF, HMI_ENGINE_RESULT eProcResult, SGUI_INT iActionID);

//=======================================================================//
//= Static variable declaration.                                        =//
//=======================================================================//
static SGUI_NOTICT_BOX  s_stDemoNoticeBox =             {0x00};
static SGUI_CHAR        s_szDemoNoticeText[NOTICE_TEXT_BUFFER_SIZE+1] = {0x00};

HMI_SCREEN_ACTION       s_stDemoNoticeActions =         {   HMI_DemoNotice_Initialize,
                                                            HMI_DemoNotice_Prepare,
                                                            HMI_DemoNotice_RefreshScreen,
                                                            HMI_DemoNotice_ProcessEvent,
                                                            HMI_DemoNotice_PostProcess,
                                                        };

//=======================================================================//
//= Global variable declaration.                                        =//
//=======================================================================//
HMI_SCREEN_OBJECT       g_stHMIDemo_Notice =            {   HMI_SCREEN_ID_DEMO_TEXT_NOTICE,
                                                            &s_stDemoNoticeActions
                                                        };
extern SGUI_NUM_VARBOX_STRUCT s_stNumberHighBox;
extern SGUI_NUM_VARBOX_STRUCT s_stNumberLowBox;
//=======================================================================//
//= Function define.                                                    =//
//=======================================================================//
HMI_ENGINE_RESULT HMI_DemoNotice_Initialize(SGUI_SCR_DEV* pstDeviceIF)
{
    SGUI_SystemIF_MemorySet(s_szDemoNoticeText, 0x00, sizeof(SGUI_CHAR)*(NOTICE_TEXT_BUFFER_SIZE+1));
    s_stDemoNoticeBox.cszNoticeText = s_szDemoNoticeText;
    s_stDemoNoticeBox.pstIcon = &SGUI_RES_ICON_INFORMATION_16;
    SGUI_Notice_FitArea(pstDeviceIF, &(s_stDemoNoticeBox.stLayout));
    return HMI_RET_NORMAL;
}

HMI_ENGINE_RESULT HMI_DemoNotice_Prepare(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters)
{
    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/
    if(NULL == pstParameters)
    {
        SGUI_SystemIF_StringLengthCopy(s_szDemoNoticeText, "No Parameter.", NOTICE_TEXT_BUFFER_SIZE);
    }
    else
    {
        SGUI_SystemIF_StringLengthCopy(s_szDemoNoticeText, (SGUI_SZSTR)pstParameters, NOTICE_TEXT_BUFFER_SIZE);
        s_szDemoNoticeText[NOTICE_TEXT_BUFFER_SIZE] = '\0';
    }
    SGUI_Notice_Repaint(pstDeviceIF, &s_stDemoNoticeBox, &GB2312_FZXS12, 0);
    return HMI_RET_NORMAL;
}

HMI_ENGINE_RESULT HMI_DemoNotice_RefreshScreen(SGUI_SCR_DEV* pstDeviceIF, const void* pstParameters)
{
    SGUI_Notice_Repaint(pstDeviceIF, &s_stDemoNoticeBox, &GB2312_FZXS12, 0);
    return HMI_RET_NORMAL;
}

HMI_ENGINE_RESULT HMI_DemoNotice_ProcessEvent(SGUI_SCR_DEV* pstDeviceIF, const HMI_EVENT_BASE* pstEvent, SGUI_INT* piActionID)
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
                iProcessAction = HMI_DEMO_PROC_CONFIRM;
                break;
            case KEY_VALUE_ESC:
            {
                iProcessAction = HMI_DEMO_PROC_CANCEL;
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

HMI_ENGINE_RESULT HMI_DemoNotice_PostProcess(SGUI_SCR_DEV* pstDeviceIF, HMI_ENGINE_RESULT eProcResult, SGUI_INT iActionID)
{
    int32_t i32TempVal = 0;
    /*----------------------------------*/
    /* Process                          */
    /*----------------------------------*/
    if(HMI_PROCESS_SUCCESSFUL(eProcResult))
    {
        if(HMI_DEMO_PROC_CANCEL == iActionID)
        {
            HMI_GoBack(NULL);
        }
        else if(HMI_DEMO_PROC_CONFIRM == iActionID)
        {
            /* Should store high/low level settings */
            i32TempVal = SGUI_NumberVariableBox_GetValue(&s_stNumberHighBox);
            ESP_LOGE("Notice", "High Water Level: %d\n", i32TempVal);
            NvsFlashWriteInt32("nvs", "HighL", &i32TempVal);
            i32TempVal = SGUI_NumberVariableBox_GetValue(&s_stNumberLowBox);
            ESP_LOGE("Notice", "Low Water Level: %d\n", i32TempVal);
            NvsFlashWriteInt32("nvs", "LowL", &i32TempVal);
            /* And Go back */
            /* I think we could restart to inform that store succeed */
            esp_restart();
        }
    }
    return HMI_RET_NORMAL;
}

