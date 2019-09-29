/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/7
 **/

#include "NIBPMonitorErrorState.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include "NIBPCountdownTime.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "PatientManager.h"

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPMonitorErrorState::enter(void)
{
    nibpParam.provider().stopMeasure();

    //通信中断在超时中处理
    //停止的触发时间
    nibpParam.createSnapshot(NIBP_ONESHOT_ABORT);

    nibpParam.setAdditionalMeasure(false);

    nibpParam.setText(trs("NIBPModule") + "\n" + trs("NIBPDisable"));
    nibpParam.setModelText("");
    nibpParam.clearResult();

    //
    nibpCountdownTime.STATMeasureStop();
    nibpParam.setSTATMeasure(false);
    nibpParam.setSTATClose(false);
    //自动测量模式中的手动触发关闭
    nibpParam.setAutoMeasure(false);
    //安全间隔结束，临时状态被关闭
    nibpParam.setSTATOpenTemp(false);
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPMonitorErrorState::handleNIBPEvent(NIBPEvent event, const unsigned char */*args*/, int /*argLen*/)
{
    switch (event)
    {
    case NIBP_EVENT_CONNECTION_NORMAL:
    {
        int enable = 0;
        machineConfig.getModuleInitialStatus("NIBPNEOMeasureEnable", reinterpret_cast<bool*>(&enable));
        if (patientManager.getType() == PATIENT_TYPE_NEO && !enable)
        {
            break;
        }
        // 恢复STAT按钮
        nibpParam.setSTATMeasure(false);
        // 防止在错误状态时间内，产生的倒计时测量触发
        nibpCountdownTime.setAutoMeasureTimeout(false);
        switchState(NIBP_MONITOR_STANDBY_STATE);
        nibpParam.setText(InvStr());
        break;
    }
    case NIBP_EVENT_TRIGGER_MODEL:
        if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
        {
            nibpParam.switchToAuto();
        }
        else
        {
            nibpParam.switchToManual();
        }
        break;
    case NIBP_EVENT_MODULE_ERROR:
        nibpParam.setText(trs("NIBPModule") + "\n" + trs("NIBPDisable"));
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMonitorErrorState::NIBPMonitorErrorState() : NIBPState(NIBP_MONITOR_ERROR_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMonitorErrorState::~NIBPMonitorErrorState()
{
}
