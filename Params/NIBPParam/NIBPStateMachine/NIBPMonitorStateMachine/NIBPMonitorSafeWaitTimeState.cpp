#include "NIBPMonitorSafeWaitTimeState.h"
#include "NIBPParam.h"
#include "IConfig.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPMonitorSafeWaitTimeState::run(void)
{
    if (_safeFlag)
    {
        if (nibpParam.isSTATOpenTemp())
        {
            resetTimeOut(5 * 1000);
        }
        //STAT 5min倒计时。
        if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
        {
            if (!nibpParam.isSTATOpenTemp())
            {
                // STAT总测量时间超时，不再测量。
                if (nibpCountdownTime.isSTATMeasureTimeout())
                {
                    enter();
                    return;
                }
            }
        }
        else if (nibpParam.getMeasurMode() == NIBP_MODE_MANUAL)
        {
            resetTimeOut(5 * 1000);
        }
    }
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPMonitorSafeWaitTimeState::enter(void)
{
    nibpParam.provider().stopMeasure();
    _safeFlag = false;
    _safeTime = _safeWaitTime() * 1000;
}

/**************************************************************************************************
 * 处理事件。
 *************************************************************************************************/
void NIBPMonitorSafeWaitTimeState::handleNIBPEvent(NIBPEvent event, const unsigned char *args, int /*argLen*/)
{
    switch (event)
    {
    case NIBP_EVENT_MODULE_RESET:
        if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
        {
            nibpParam.safeWaitTimeSTATStop();
        }
        else
        {
            //停止额外测量
            if (nibpParam.isAdditionalMeasure())
            {
                nibpParam.setAdditionalMeasure(false);
                nibpParam.setText(trs("NIBPMEASURE") + "\n" + trs("NIBPSTOPPED"));
                if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
                {
                    if (nibpParam.isAutoMeasure())
                    {
                        nibpParam.setModelText(trs("NIBPManual"));
                    }
                }
            }
        }
        break;

    case NIBP_EVENT_MODULE_ERROR:
        switchState(NIBP_MONITOR_ERROR_STATE);
        break;

    case NIBP_EVENT_TRIGGER_BUTTON:
        if (!nibpParam.isAdditionalMeasure())
        {
            nibpParam.setText(trs("NIBPWAITING"));
        }
        //停止额外测量
        else
        {
            nibpParam.setAdditionalMeasure(false);
            nibpParam.setText(trs("NIBPMEASURE") + "\n" + trs("NIBPSTOPPED"));
            nibpParam.clearResult();

            if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
            {
                if (nibpParam.isAutoMeasure())
                {
                    nibpParam.setModelText(trs("NIBPManual"));
                    nibpParam.setAutoMeasure(false);
                }
            }
        }
        if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
        {
            if (nibpParam.isSTATOpenTemp())
            {
                nibpParam.setSTATMeasure(false);
                nibpParam.setText(trs("STATSTOPPED"));
                nibpParam.setModelText(trs("STATSTOPPED"));
            }
        }
        break;

    case NIBP_EVENT_TRIGGER_PATIENT_TYPE:
        if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
        {
            nibpParam.safeWaitTimeSTATStop();
        }
        else
        {
            //停止额外测量
            if (nibpParam.isAdditionalMeasure())
            {
                nibpParam.setAdditionalMeasure(false);
                nibpParam.setText(trs("NIBPMEASURE") + "\n" + trs("NIBPSTOPPED"));
                if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
                {
                    if (nibpParam.isAutoMeasure())
                    {
                        nibpParam.setModelText(trs("NIBPManual"));
                        nibpParam.setAutoMeasure(false);
                    }
                }
            }
        }
        break;

    case NIBP_EVENT_TRIGGER_MODEL:
        nibpParam.setAutoMeasure(false);
        if (args[0] == 0x01)
        {
            if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
            {
                nibpParam.safeWaitTimeSTATStop();
            }
            else
            {
                nibpParam.setSTATOpenTemp(true);
                nibpParam.setSTATMeasure(true);
                nibpParam.setText(trs("NIBPWAITING"));
                nibpParam.setModelText(trs("STATOPEN"));
            }
            break;
        }
        //停止额外测量
        if (nibpParam.isAdditionalMeasure())
        {
            nibpParam.setAdditionalMeasure(false);
            nibpParam.setText(trs("NIBPMEASURE") + "\n" + trs("NIBPSTOPPED"));
            nibpParam.clearResult();
        }
        if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
        {
            nibpParam.switchToAuto();
        }
        else
        {
            nibpParam.switchToManual();
        }
        break;

    case NIBP_EVENT_TIMEOUT:
        //防止在安全间隔时间内，产生的倒计时测量触发
        nibpCountdownTime.setAutoMeasureTimeout(false);
        nibpParam.setText(InvStr());

        //STAT5s间隔期内，STAT被关闭
        //(先判断STAT是否被关闭，否则存在STAT在安全间隔内被关闭后，
        //再临时打开的情况，会存在状态为STAT，但安全间隔只有5s的情况)
        nibpParam.setSTATClose(false);
        //安全间隔结束，临时状态被关闭
        nibpParam.setSTATOpenTemp(false);

        if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
        {
            //判断STAT倒计时是否在使用中
            if (nibpCountdownTime.isSTATMeasureTimeout())
            {
                nibpCountdownTime.STATMeasureStart(); // 只测量5分钟。
            }
            switchState(NIBP_MONITOR_STARTING_STATE);
        }
        else
        {
            //额外一次测量
            if (nibpParam.isAdditionalMeasure())
            {
                switchState(NIBP_MONITOR_STARTING_STATE);
            }
            else
            {
                switchState(NIBP_MONITOR_STANDBY_STATE);
            }
        }
//            nibpParam.setShowMeasureCount();
        break;
    case NIBP_EVENT_CURRENT_PRESSURE:
    {
        short pressure = 0;
        pressure = (args[1]<<8)+args[0];
        if (pressure != -1)
        {
            if (patientManager.getType() == PATIENT_TYPE_NEO)
            {
                if (pressure >= 0 && pressure < 5)
                {
                    setTimeOut(_safeTime);
                    _safeFlag = true;
                }
            }
            else
            {
                if (pressure >= 0 && pressure < 15)
                {
                    setTimeOut(_safeTime);
                    _safeFlag = true;
                }
            }
        }
    }
        break;
    default:
        break;
    }
}


/**************************************************************************************************
 * 获取安全间隔的时间。
 *************************************************************************************************/
int NIBPMonitorSafeWaitTimeState::_safeWaitTime()
{
    //额外一次测量
    if (nibpParam.isAdditionalMeasure())
    {
        nibpParam.setText(trs("NIBPADDITIONAL") + "\n" + trs("NIBPMEASURE"));
        if (nibpParam.getMeasurMode() == NIBP_MODE_MANUAL)
        {
            _safeWaitTiming = 5;
        }
        else if (nibpParam.getMeasurMode() == NIBP_MODE_AUTO)
        {
            if (nibpParam.isAutoMeasure())
            {
                _safeWaitTiming = 5;
            }
            else
            {
                _safeWaitTiming = 30;
            }
        }
        return _safeWaitTiming;
    }

    // 自动测量的等待时间为30秒。
    if (nibpParam.getMeasurMode() == NIBP_MODE_AUTO)
    {
        //STAT手动退出后进入AUTO模式的安全间隔时间
        if (nibpParam.isSTATClose())
        {
//            _safeWaitTiming = 30;
            _safeWaitTiming = 5;
//            nibpParam.setSTATClose(false);
            return _safeWaitTiming;
        }
        //AUTO模式中手动测量的安全间隔时间
        if (nibpParam.isAutoMeasure())
        {
            if (!nibpParam.isSwitchType())
            {
                nibpParam.setModelText(trs("NIBPManual"));
            }
            _safeWaitTiming = 5;
            nibpParam.setAutoMeasure(false);
        }
        //AUTO模式中自动触发的安全间隔时间
        else
        {
            _safeWaitTiming = 30;
        }
    }
    // 连续测量的等待时间为5秒。
    else if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
    {
        //STAT模式5min时间到后的安全间隔时间，且STAT不再测量。
        if (nibpCountdownTime.isSTATMeasureTimeout() && !nibpParam.isSTATOpenTemp())
        {
            nibpParam.setModelText(trs("STATDONE"));
            nibpParam.setSTATMeasure(false);
            nibpParam.setSTATClose(false);
//            _safeWaitTiming = 30;
            _safeWaitTiming = 5;
            return _safeWaitTiming;
        }

        //STAT模式中正常测量的安全间隔时间
        _safeWaitTiming = 5;
    }

    // 手动测量的等待时间为5秒。
    else
    {
        //STAT手动退出后进入手动模式的安全间隔时间
        if (nibpParam.isSTATClose())
        {
//            _safeWaitTiming = 30;
            _safeWaitTiming = 5;
//            nibpParam.setSTATClose(false);
            return _safeWaitTiming;
        }

        //手动模式的安全间隔时间
        _safeWaitTiming = 5;
    }
    return _safeWaitTiming;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMonitorSafeWaitTimeState::NIBPMonitorSafeWaitTimeState() : NIBPState(NIBP_MONITOR_SAFEWAITTIME_STATE)
{
    _safeWaitTiming = 0;
    _safeTime = 5;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMonitorSafeWaitTimeState::~NIBPMonitorSafeWaitTimeState()
{

}