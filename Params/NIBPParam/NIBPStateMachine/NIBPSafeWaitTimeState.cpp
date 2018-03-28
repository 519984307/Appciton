#include "NIBPSafeWaitTimeState.h"
#include "NIBPParam.h"
#include "IConfig.h"

/**************************************************************************************************
 * 主运行。
 *************************************************************************************************/
void NIBPSafeWaitTime::run(void)
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

        // 超时到了切换到启动状态。
        if (isTimeOut())
        {
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
                nibpParam.switchState(NIBP_STATE_STARTING);
            }
            else
            {
                //额外一次测量
                if (nibpParam.isAdditionalMeasure())
                {
                    nibpParam.switchState(NIBP_STATE_STARTING);
                }
                else
                {
                    nibpParam.switchState(NIBP_STATE_STANDBY);
                }
            }
//            nibpParam.setShowMeasureCount();
        }
    }
}

/**************************************************************************************************
 * 进入该状态。
 *************************************************************************************************/
void NIBPSafeWaitTime::enter(void)
{
    provider->stopMeasure();
    _safeFlag = false;
    _safeTime = nibpEventTrigger.safeWaitTime() * 1000;
}

/**************************************************************************************************
 * 解包。
 *************************************************************************************************/
void NIBPSafeWaitTime::unPacket(unsigned char *packet, int /*len*/)
{
    if (!_safeFlag)
    {
        // 获取压力数据。
        short pressure;
        QString str;
        machineConfig.getStrValue("NIBP", str);
        if (str == "SUNTECH_NIBP")
        {
            pressure = provider->cuffPressure(packet);
        }
        else
        {
            pressure = provider->lowPressure(packet);
        }

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
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPSafeWaitTime::NIBPSafeWaitTime() : NIBPState(NIBP_STATE_SAFEWAITTIME)
{
    _safeWaitTiming = 0;
    _safeTime = 5;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPSafeWaitTime::~NIBPSafeWaitTime()
{

}
