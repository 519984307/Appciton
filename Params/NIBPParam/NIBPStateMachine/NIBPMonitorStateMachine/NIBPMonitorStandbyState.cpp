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
#include <QTimerEvent>
#include "SystemManager.h"

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
        if (systemManager.getCurWorkMode() != WORK_MODE_DEMO)
        {
            switchState(NIBP_MONITOR_ERROR_STATE);
        }
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
            if (nibpParam->getSuperMeasurMode() == NIBP_MODE_AUTO
                    && nibpParam->isFirstAuto())   // 判断是否在AUTO倒计时时候开启stat测量
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
            if (nibpParam->isFirstAuto())   // AUTO倒计时时候进入手动模式要有5秒放气期
            {
                switchState(NIBP_MONITOR_SAFEWAITTIME_STATE);
            }
            nibpParam->switchToManual();
        }
        break;
    case NIBP_EVENT_ZERO_SELFTEST:
        nibpParam->setCuffPressure(args[0]);
        if (args[0] == NIBP_ZERO_ONGOING_STATE)           //  开机正在校准
        {
            nibpParam->setZeroSelfTestState(true);
            nibpParam->setText(trs("NIBPZEROING"));       //  显示正在较零
        }
        else if (args[0] == NIBP_ZERO_SUCCESS_STATE)      //  开机校准成功
        {
            nibpParam->setZeroSelfTestState(false);
            nibpParam->recoverInitTrendWidgetData();      //  显示上一次信息
            _timerID = startTimer(1000);
            nibpParam->setCuffPressure(0);
        }
        else if (args[0] == NIBP_ZERO_FAIL_STATE)         //  校零失败，进入禁用状态
        {
            nibpParam->setZeroSelfTestState(false);
            nibpParam->setDisableState(true);
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
            if (nibpParam->isFirstAuto())
            {
                nibpParam->setModelText(trs("NIBPAUTO") + ":" +
                                        trs(NIBPSymbol::
                                            convert(static_cast<NIBPAutoInterval>(nibpParam->getAutoInterval()))));
            }
            else
            {
                nibpParam->setModelText(trs("NIBPManualStart") + ":" +
                                        trs(NIBPSymbol::
                                            convert(static_cast<NIBPAutoInterval>(nibpParam->getAutoInterval()))));
            }
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
NIBPMonitorStandbyState::NIBPMonitorStandbyState() : NIBPState(NIBP_MONITOR_STANDBY_STATE), _timerID(-1)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMonitorStandbyState::~NIBPMonitorStandbyState()
{
}

void NIBPMonitorStandbyState::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == _timerID)
    {
        NIBPParamInterface* nibpParam = NIBPParamInterface::getNIBPParam();
        if (nibpParam)
        {
            nibpParam->setZeroSelfTestState(false);
            nibpParam->recoverInitTrendWidgetData();      //  显示上一次信息
        }
        killTimer(_timerID);
        _timerID = -1;
    }
}
