/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/


#include "NIBPMonitorStopState.h"
#include "NIBPParamInterface.h"
#include "NIBPCountdownTimeInterface.h"
#include "NIBPState.h"
#include "LanguageManager.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPMonitorStopState::enter(void)
{
    //通信中断在超时中处理
    //停止的触发时间
    NIBPParamInterface* nibpParam = NIBPParamInterface::getNIBPParam();
    if (!nibpParam)
    {
        return;
    }
    nibpParam->createSnapshot(NIBP_ONESHOT_ABORT);

    // 启动定时器，不能无限期地等待下去。
    setTimeOut();

    nibpParam->provider().stopMeasure();

    nibpParam->setAdditionalMeasure(false);

    if (nibpParam->getMeasurMode() == NIBP_MODE_STAT)
    {
        NIBPCountdownTimeInterface* nibpCountdownTime = NIBPCountdownTimeInterface::getNIBPCountdownTime();
        if (nibpCountdownTime)
        {
            nibpCountdownTime->STATMeasureStop();
        }
        nibpParam->setSTATMeasure(false);               // 退出STAT
        nibpParam->setSTATClose(true);                  // 置STAT关闭标志
        nibpParam->setModelText(trs("STATSTOPPED"));
    }
    else
    {
        if (nibpParam->isAutoMeasure())
        {
            nibpParam->setModelText(trs("NIBPManual"));
        }
    }

    // 如果不需要启动应答则立即切换到测量状态。
    if (!nibpParam->provider().needStopACK())
    {
        nibpParam->setText(trs("NIBPREADING") + "\n" + trs("NIBPSTOPPED"));
        nibpParam->clearResult();
        switchState(NIBP_MONITOR_SAFEWAITTIME_STATE);
        return;
    }
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPMonitorStopState::handleNIBPEvent(NIBPEvent event, const unsigned char */*args*/, int /*argLen*/)
{
    NIBPParamInterface* nibpParam = NIBPParamInterface::getNIBPParam();
    if (!nibpParam)
    {
        return;
    }
    switch (event)
    {
    case NIBP_EVENT_MONITOR_STOP:
        nibpParam->setText(trs("NIBPREADING") + "\n" + trs("NIBPSTOPPED"));
        nibpParam->clearResult();
        switchState(NIBP_MONITOR_SAFEWAITTIME_STATE);
        break;
    case NIBP_EVENT_TIMEOUT:
        nibpParam->setText(trs("NIBPREADING") + "\n" + trs("NIBPSTOPPED"));
        nibpParam->clearResult();
        switchState(NIBP_MONITOR_SAFEWAITTIME_STATE);
        break;
    case NIBP_EVENT_MODULE_ERROR:
        switchState(NIBP_MONITOR_ERROR_STATE);
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMonitorStopState::NIBPMonitorStopState() : NIBPState(NIBP_MONITOR_STOPE_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMonitorStopState::~NIBPMonitorStopState()
{
}
