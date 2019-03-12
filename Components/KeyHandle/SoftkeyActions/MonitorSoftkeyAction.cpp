/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/15
 **/



#include "MonitorSoftkeyAction.h"
#include "SoftKeyManager.h"
#include "ECGParam.h"
#include "WindowManager.h"

/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _monitorKeys[] =
{
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_INFO),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_NEW),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_DISCHARGE),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_ECG_LEAD_MODE),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_ALARM_LIMIT),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_CODE_MARKER),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_TREND_GRAPH),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_EVENT_REVIEW),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_TREND_TABLE),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_SYS_MODE),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_WINDOWLAYOUT),
#ifndef HIDE_PARAM_SWITCH
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PARA_SWITCH),
#endif
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_SCREEN_BAN),
#ifndef HIDE_STANDBY_FUNCTION
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_STANDBY),
#endif
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_CO2_CALIBRATION),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_CO2_HANDLE),
#ifndef HIDE_IBP_CALIBRATE_ZERO
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_IBP_CALIBRATION),
#endif
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_CALCULATION),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_KEYBOARD_VOLUMN),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_SCREEN_BRIGHTNESS),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NIGHT_MODE),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PRINTER_SET),
};

/***************************************************************************************************
 * 获取动作描述总个数。
 **************************************************************************************************/
int MonitorSoftkeyAction::getActionDescNR(void)
{
    return sizeof(_monitorKeys) / sizeof(KeyActionDesc);
}

/***************************************************************************************************
 * 获取动作描述句柄。
 **************************************************************************************************/
KeyActionDesc *MonitorSoftkeyAction::getActionDesc(int index)
{
    if (index >= static_cast<int>(sizeof(_monitorKeys) / sizeof(KeyActionDesc)))
    {
        return NULL;
    }

    if (_monitorKeys[index].type < SOFT_BASE_KEY_NR)
    {
        return SoftkeyActionBase::getActionDesc(_monitorKeys[index].type);
    }

    return &_monitorKeys[index];
}

/***************************************************************************************************
 * 构造。
 **************************************************************************************************/
MonitorSoftkeyAction::MonitorSoftkeyAction() : SoftkeyActionBase(SOFTKEY_ACTION_STANDARD)
{
}

/***************************************************************************************************
 * 析构。
 **************************************************************************************************/
MonitorSoftkeyAction::~MonitorSoftkeyAction()
{
}
