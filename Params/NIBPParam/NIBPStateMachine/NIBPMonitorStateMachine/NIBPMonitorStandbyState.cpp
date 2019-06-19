/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/


#include "NIBPMonitorStandbyState.h"
#include "NIBPParamInterface.h"
#include "LanguageManager.h"
#include "NIBPSymbol.h"
#include "NIBPCountdownTimeInterface.h"

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPMonitorStandbyState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
{
    NIBPParamInterface* nibpParam = NIBPParamInterface::getNIBPParam();
    if (!nibpParam)
    {
        return;
    }
    switch (event)
    {
    case NIBP_EVENT_MODULE_ERROR:
        switchState(NIBP_MONITOR_ERROR_STATE);
        break;

    case NIBP_EVENT_TIMEOUT:
        switchState(NIBP_MONITOR_ERROR_STATE);
        break;

    case NIBP_EVENT_TRIGGER_BUTTON:
        if (nibpParam->getMeasurMode() == NIBP_MODE_AUTO)
        {
            //自动测量模式的手动触发标志
            nibpParam->setAutoMeasure(true);
            nibpParam->setFirstAuto(true);
        }
        // 转换到测量状态。
        switchState(NIBP_MONITOR_STARTING_STATE);
        break;

    case NIBP_EVENT_TRIGGER_MODEL:
        if (args[0] == 0x01)
        {
            NIBPCountdownTimeInterface* nibpCountdownTime = NIBPCountdownTimeInterface::getNIBPCountdownTime();
            nibpParam->setSTATMeasure(true);
            if (nibpParam->isFirstAuto())
            {
                nibpCountdownTime->setSTATMeasureTimeout(false);
                nibpParam->setAutoStat(true);
                switchState(NIBP_MONITOR_SAFEWAITTIME_STATE);
                break;
            }
            if (nibpCountdownTime)
            {
                nibpCountdownTime->STATMeasureStart();
            }
            switchState(NIBP_MONITOR_STARTING_STATE);
            break;
        }
        if (nibpParam->getSuperMeasurMode() == NIBP_MODE_AUTO)
        {
            nibpParam->switchToAuto();
        }
        else
        {
            if (nibpParam->isFirstAuto())
            {
                switchState(NIBP_MONITOR_SAFEWAITTIME_STATE);
            }
            nibpParam->switchToManual();
        }
        break;
    case NIBP_EVENT_ZERO_SELFTEST:
        nibpParam->setCuffPressure(args[0]);
        if (args[0] == 0x01)                               //  开机正在校准
        {
            nibpParam->setZeroSelfTestState(true);
            nibpParam->setText(trs("NIBPZEROING"));       //  显示正在较零
        }
        else if (args[0] == 0x02)                         //  开机校准成功
        {
            nibpParam->setZeroSelfTestState(false);
            nibpParam->recoverInitTrendWidgetData();      //  显示上一次信息
        }
        else if (args[0] == 0x03)                        //  校零失败，进入禁用状态
        {
            nibpParam->setZeroSelfTestState(false);
            nibpParam->errorDisable();
        }
    default:
        break;
    }
}

/**************************************************************************************************
 * 状态进入
 *************************************************************************************************/
void NIBPMonitorStandbyState::enter()
{
    NIBPParamInterface* nibpParam = NIBPParamInterface::getNIBPParam();
    if (nibpParam)
    {
        if (nibpParam->getMeasurMode() == NIBP_MODE_AUTO)
        {
            nibpParam->setAutoMeasure(false);
            nibpParam->setModelText(trs("NIBPAUTO") + ":" +
                                    trs(NIBPSymbol::convert((NIBPAutoInterval)nibpParam->getAutoInterval())));
        }
        else if (nibpParam->getMeasurMode() == NIBP_MODE_MANUAL)
        {
            nibpParam->setModelText(trs("NIBPManual"));
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMonitorStandbyState::NIBPMonitorStandbyState() : NIBPState(NIBP_MONITOR_STANDBY_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMonitorStandbyState::~NIBPMonitorStandbyState()
{
}
