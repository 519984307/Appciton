#include "NIBPCountdownTime.h"
#include "NIBPParam.h"
#include "NIBPMonitorStateDefine.h"
#include "Debug.h"
#include <QTimer>

NIBPCountdownTime *NIBPCountdownTime::_selfObj = NULL;

void NIBPCountdownTime::run()
{
//debug("%d\n",(_countdownTime - countdownElapseTime()) / 1000);
    if (nibpParam.getMeasurMode() == NIBP_MODE_AUTO && !nibpParam.isAdditionalMeasure())
    {
        // 更新倒计时。
        nibpParam.setCountdown((_autoTime - autoMeasureElapseTime()) / 1000);
        // 倒计时是否完成(在STANDBY状态才能启动)
        if ((nibpParam.curStatusType()== NIBP_MONITOR_STANDBY_STATE) && isAutoMeasureTimeout())
        {
            // 转换到测量状态。
            nibpParam.switchState(NIBP_MONITOR_STARTING_STATE);
            setAutoMeasureTimeout(false);
        }
    }
    else if (nibpParam.getMeasurMode() == NIBP_MODE_STAT)
    {
        if (!nibpParam.isSTATOpenTemp())
        {
            int t = STATMeasureElapseTime();
            if (t == 0)
            {
                if (nibpParam.curStatusType() == NIBP_MONITOR_SAFEWAITTIME_STATE)
                {
                    nibpParam.setModelText(trs("STATDONE"));
                }
                else
                {
                    nibpParam.setModelText(trs("STATCOMPLETING"));
                }
            }
            else
            {
                int min = t / 60;
                int sec = t % 60;
                QString str;
                str.sprintf("%.2d:%.2d", min, sec);
                nibpParam.setModelText(trs("STAT") + " " + str);
            }
        }
    }
}

/**************************************************************************************************
 * 自动测量超时调用。
 *************************************************************************************************/
void NIBPCountdownTime::_autoMeasureTimeout(void)
{
    _autoMeasureTimer->stop();
    _isAutoMeasureTimeout = true;
    _autoMeasureTimer->start(_autoTime);
    _autoMeasureElapseTimer.restart();
}

/**************************************************************************************************
 * STAT测量超时调用。
 *************************************************************************************************/
void NIBPCountdownTime::_STATMeasureTimeout()
{
    _isSTATMeasureTimeout = true;
    _STATMeasureTimer->stop();
}

/**************************************************************************************************
 * 启动定时
 *************************************************************************************************/
void NIBPCountdownTime::autoMeasureStart(int sec)
{
    _autoMeasureTimer->stop();
    _autoTime = sec * 1000;
    _autoMeasureTimer->start(_autoTime);
    _autoMeasureElapseTimer.restart();
    _isAutoMeasureTimeout = false;
}

/**************************************************************************************************
 * 倒计时时间被更改
 *************************************************************************************************/
void NIBPCountdownTime::autoMeasureChange(int sec)
{
    _autoMeasureTimer->stop();
    _autoTime = sec * 1000;

    //对计时器 取 倒计时的余数,
    _autoElapseTime = _autoMeasureElapseTimer.elapsed() % _autoTime;
    _autoMeasureTimer->start(_autoTime - _autoElapseTime);
    _isAutoMeasureTimeout = false;
}

/**************************************************************************************************
 * 获取时间流逝。
 *************************************************************************************************/
int NIBPCountdownTime::autoMeasureElapseTime(void)
{
    _autoElapseTime = _autoMeasureElapseTimer.elapsed() % _autoTime;
    return _autoElapseTime;
}

/**************************************************************************************************
 * 状态机主运行。
 *************************************************************************************************/
void NIBPCountdownTime::autoMeasureStop(void)
{
    _autoMeasureTimer->stop();
    _isAutoMeasureTimeout = false;
}

/**************************************************************************************************
 * 启动STAT定时
 *************************************************************************************************/
void NIBPCountdownTime::STATMeasureStart(void)
{
    _STATMeasureTimer->start(_STATTime);
    _STATMeasureElapseTimer.restart();
    _isSTATMeasureTimeout = false;
}

void NIBPCountdownTime::STATMeasureStop(void)
{
    _STATMeasureTimer->stop();
    _isSTATMeasureTimeout = true;
}

/**************************************************************************************************
 * STAT是否超时。
 *************************************************************************************************/
bool NIBPCountdownTime::isSTATMeasureTimeout(void)
{
    return _isSTATMeasureTimeout;
}

/**************************************************************************************************
 * STAT倒计时时间。
 *************************************************************************************************/
int NIBPCountdownTime::STATMeasureElapseTime(void)
{
    _STATElapseTime = _STATTime - _STATMeasureElapseTimer.elapsed();
    if (_STATElapseTime > 0)
    {
        return (_STATElapseTime/1000);
    }
    else
    {
        return 0;
    }
}

/**************************************************************************************************
 * 是否超时。
 *************************************************************************************************/
bool NIBPCountdownTime::isAutoMeasureTimeout(void)
{
    return _isAutoMeasureTimeout;
}

void NIBPCountdownTime::setAutoMeasureTimeout(bool flag)
{
    _isAutoMeasureTimeout = flag;
}

/**************************************************************************************************
 * 状态机主运行。
 *************************************************************************************************/
NIBPCountdownTime::NIBPCountdownTime()
{
    _isAutoMeasureTimeout = false;
    _isSTATMeasureTimeout = true;
    _STATTime = STAT_Time * 1000;


    _autoMeasureTimer = new QTimer();
    connect(_autoMeasureTimer, SIGNAL(timeout()), this, SLOT(_autoMeasureTimeout()));

    _STATMeasureTimer = new QTimer();
    connect(_STATMeasureTimer, SIGNAL(timeout()), this, SLOT(_STATMeasureTimeout()));
}

/**************************************************************************************************
 * 状态机主运行。
 *************************************************************************************************/
NIBPCountdownTime::~NIBPCountdownTime()
{
    if (NULL != _autoMeasureTimer)
    {
        delete _autoMeasureTimer;
        _autoMeasureTimer = NULL;
    }

    if (NULL != _STATMeasureTimer)
    {
        delete _STATMeasureTimer;
        _STATMeasureTimer = NULL;
    }
}



