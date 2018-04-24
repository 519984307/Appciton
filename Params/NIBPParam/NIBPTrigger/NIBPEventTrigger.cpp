#include "NIBPEventTrigger.h"
#include "NIBPCountdownTime.h"
#include "NIBPParam.h"
#include "Debug.h"
#include "SummaryStorageManager.h"


NIBPEventTrigger *NIBPEventTrigger::_selfObj = NULL;

/**************************************************************************************************
 * 状态转为AUTO后，开始倒计时。
 *************************************************************************************************/
void NIBPEventTrigger::_switchToAuto(void)
{
    _timing = nibpParam.getAutoIntervalTime();
    //判断是切换的模式还是修改了自动测量的时间
    if (nibpParam.isSwitchTime())
    {
        // 自动测量，获取延时的时间。
        nibpCountdownTime.autoMeasureChange(_timing);
    }
    else
    {
        nibpCountdownTime.autoMeasureStart(_timing);
    }

    nibpParam.setSwitchFlagTime(false);
    if (nibpParam.currentState() != NIBP_STATE_ERROR)
    {
        if (nibpParam.getMeasurMode() != NIBP_MODE_STAT)
        {
            nibpParam.setModelText(trs("NIBPAUTO") + ":" +
                                   trs(NIBPSymbol::convert((NIBPIntervalTime)nibpParam.getAutoInterval())));
        }
    }
}

/**************************************************************************************************
 * 状态转为手动后，倒计时时间停止，且不显示。
 *************************************************************************************************/
void NIBPEventTrigger::_switchToManual(void)
{
    nibpCountdownTime.autoMeasureStop();
    nibpParam.setCountdown(-1);

    nibpParam.setAutoMeasure(false);
    if (nibpParam.currentState() != NIBP_STATE_ERROR)
    {
        if (nibpParam.getMeasurMode() != NIBP_MODE_STAT)
        {
            nibpParam.setModelText(trs("NIBPManual"));
        }
    }
}

/**************************************************************************************************
 * STAT在安全间隔内被关闭,临时STAT状态也被关闭。
 *************************************************************************************************/
void NIBPEventTrigger::_safeWaitTimeSTATStop()
{
    if (!nibpParam.isSTATOpenTemp())
    {
        nibpCountdownTime.STATMeasureStop();
        nibpParam.setSTATClose(true);
    }
    nibpParam.setSTATMeasure(false);
    nibpParam.setText(trs("STATSTOPPED"));
    nibpParam.setModelText(trs("STATSTOPPED"));
}

/**************************************************************************************************
 * 按钮触发处理。
 *************************************************************************************************/
void NIBPEventTrigger::triggerbutton()
{
    switch(nibpParam.currentState())
    {
    case NIBP_STATE_STANDBY:
        if (nibpParam.getMeasurMode() == NIBP_MODE_AUTO)
        {
            //自动测量模式的手动触发标志
            nibpParam.setAutoMeasure(true);
        }
        // 转换到测量状态。
        nibpParam.switchState(NIBP_STATE_STARTING);
        break;
    case NIBP_STATE_STARTING:
    case NIBP_STATE_MEASURING:
    case NIBP_STATE_GET_RESULT:
        nibpParam.switchState(NIBP_STATE_STOPING);
        break;
    case NIBP_STATE_SAFEWAITTIME:
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
    case NIBP_STATE_ERROR:
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 病人类型修改处理。
 *************************************************************************************************/
void NIBPEventTrigger::triggerpatienttype()
{
    //每个状态需要setText
    //切换病人类型就清除结果
    nibpParam.clearResult();
    nibpParam.setText(InvStr());
    switch(nibpParam.currentState())
    {
    case NIBP_STATE_STANDBY:
        break;
    case NIBP_STATE_STARTING:
    case NIBP_STATE_MEASURING:
    case NIBP_STATE_GET_RESULT:
        nibpParam.switchState(NIBP_STATE_STOPING);
        break;
    case NIBP_STATE_SAFEWAITTIME:
        if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
        {
            _safeWaitTimeSTATStop();
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
    case NIBP_STATE_ERROR:
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 测量模式修改触发处理。
 *************************************************************************************************/
void NIBPEventTrigger::triggermodel(bool STAT_Flag)
{
    switch(nibpParam.currentState())
    {
    case NIBP_STATE_STANDBY:
        if (STAT_Flag)
        {
            nibpParam.setSTATMeasure(true);
            nibpCountdownTime.STATMeasureStart();// 只测量5分钟。
            nibpParam.switchState(NIBP_STATE_STARTING);
            break;
        }
        if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
        {
            _switchToAuto();
        }
        else
        {
            _switchToManual();
        }
        break;
    case NIBP_STATE_STARTING:
        if (STAT_Flag)
        {
            if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
            {
                nibpParam.switchState(NIBP_STATE_STOPING);
            }
            else
            {
                nibpParam.setSTATMeasure(true);
                nibpCountdownTime.STATMeasureStart(); // 只测量5分钟。
            }
            break;
        }
        if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
        {
            _switchToAuto();
        }
        else
        {
            _switchToManual();
        }
        break;
    case NIBP_STATE_MEASURING:
        if (STAT_Flag)
        {
            if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
            {
                nibpParam.switchState(NIBP_STATE_STOPING);
            }
            else
            {
                nibpParam.setSTATMeasure(true);
                nibpCountdownTime.STATMeasureStart(); // 只测量5分钟。
            }
            break;
        }

        if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
        {
            _switchToAuto();
        }
        else
        {
            _switchToManual();
        }
        break;
    case NIBP_STATE_GET_RESULT:
        if (STAT_Flag)
        {
            if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
            {
                nibpParam.switchState(NIBP_STATE_STOPING);
            }
            else
            {
                nibpParam.setSTATMeasure(true);
                nibpCountdownTime.STATMeasureStart(); // 只测量5分钟。
            }
            break;
        }

        if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
        {
            _switchToAuto();
        }
        else
        {
            _switchToManual();
        }
        nibpParam.switchState(NIBP_STATE_SAFEWAITTIME);
        break;
    case NIBP_STATE_SAFEWAITTIME:
        nibpParam.setAutoMeasure(false);
        if (STAT_Flag)
        {
            if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
            {
                _safeWaitTimeSTATStop();
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
            _switchToAuto();
        }
        else
        {
            _switchToManual();
        }
        break;
    case NIBP_STATE_ERROR:
        if (nibpParam.getSuperMeasurMode() == NIBP_MODE_AUTO)
        {
            _switchToAuto();
        }
        else
        {
            _switchToManual();
        }
    default:
        break;
    }
    //    nibpParam.setShowMeasureCount();
}

/**************************************************************************************************
 * PD充电触发处理。
 *************************************************************************************************/
void NIBPEventTrigger::triggerPD(void)
{
    switch(nibpParam.currentState())
    {
    case NIBP_STATE_STANDBY:
        break;
    case NIBP_STATE_STARTING:
        if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
        {
            nibpParam.switchState(NIBP_STATE_SAFEWAITTIME);
        }
        else
        {
            nibpParam.switchState(NIBP_STATE_STOPING);
        }
        break;
    case NIBP_STATE_MEASURING:
        if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
        {
            nibpParam.switchState(NIBP_STATE_SAFEWAITTIME);
        }
        else
        {
            nibpParam.switchState(NIBP_STATE_STOPING);
        }
        break;
    case NIBP_STATE_GET_RESULT:
        if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
        {
            nibpParam.switchState(NIBP_STATE_SAFEWAITTIME);
        }
        else
        {
            nibpParam.switchState(NIBP_STATE_STOPING);
        }
        break;
    case NIBP_STATE_SAFEWAITTIME:
        //额外测量处理
        if (nibpParam.isAdditionalMeasure())
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
        break;
    case NIBP_STATE_ERROR:
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 通信中断处理。
 *************************************************************************************************/
void NIBPEventTrigger::triggerError(void)
{
    switch(nibpParam.currentState())
    {
    case NIBP_STATE_STANDBY:
        nibpParam.switchState(NIBP_STATE_ERROR);
        break;
    case NIBP_STATE_STARTING:
        nibpParam.switchState(NIBP_STATE_ERROR);
        break;
    case NIBP_STATE_MEASURING:
        nibpParam.switchState(NIBP_STATE_ERROR);
        break;
    case NIBP_STATE_GET_RESULT:
        nibpParam.switchState(NIBP_STATE_ERROR);
        break;
    case NIBP_STATE_SAFEWAITTIME:
        nibpParam.switchState(NIBP_STATE_ERROR);
        break;
    case NIBP_STATE_ERROR:
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 复位处理。
 *************************************************************************************************/
void NIBPEventTrigger::triggerReset()
{
    nibpParam.setText(InvStr());
    switch(nibpParam.currentState())
    {
    case NIBP_STATE_STANDBY:
        break;
    case NIBP_STATE_STARTING:
    case NIBP_STATE_MEASURING:
    case NIBP_STATE_GET_RESULT:
        nibpParam.switchState(NIBP_STATE_STOPING);
        break;
    case NIBP_STATE_SAFEWAITTIME:
        if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
        {
            _safeWaitTimeSTATStop();
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
    case NIBP_STATE_ERROR:
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 获取安全间隔的时间。
 *************************************************************************************************/
int NIBPEventTrigger::safeWaitTime()
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
 * 状态机主运行。
 *************************************************************************************************/
NIBPEventTrigger::NIBPEventTrigger()
{
}

/**************************************************************************************************
 * 状态机主运行。
 *************************************************************************************************/
NIBPEventTrigger::~NIBPEventTrigger()
{

}



