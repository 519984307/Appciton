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
#include "ECGMenu.h"
#include "WindowManager.h"

/***************************************************************************************************
 * 所有的快捷按键定义。
 **************************************************************************************************/
static KeyActionDesc _monitorKeys[] =
{
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_ECG_LEAD_CHANGE),
//    KeyActionDesc("", "DiagECG.png", MonitorSoftkeyAction::diagECG), //12L
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_CODE_MARKER),
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_CO2_ONOFF),
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NEXT_PAGE),

//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_ALARM_LIMIT),
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_PAT_INFO),
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_RESCUE_DATA),
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NR, Qt::black, Qt::black, Qt::black, false),
//    KeyActionDesc("", "", NULL, SOFT_BASE_KEY_NEXT_PAGE),

    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_INFO),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PAT_NEW),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_PREVIOUS_PAGE),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_ALARM_LIMIT),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_CODE_MARKER),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_RESCUE_DATA),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_WINDOWLAYOUT),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_CALCULATION),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_LOCK_SCREEN),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_SYS_MODE_STANDARD),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_SYS_MODE_12LEAD),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_SYS_MODE_OXYCRG),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_SYS_MODE_TREND),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_SYS_MODE_BIGFONT),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_SYS_MODE_CUSTOM),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_SYS_MODE_UNKNOW),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_NEXT_PAGE),
    KeyActionDesc("", "", "", NULL, SOFT_BASE_KEY_MAIN_SETUP)
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
