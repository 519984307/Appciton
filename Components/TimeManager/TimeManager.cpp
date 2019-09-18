/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/19
 **/

#include "TimeManager.h"
#include "DateTimeWidget.h"
#include "ElapseTimeWidget.h"
#include "TimeDate.h"
#include "IConfig.h"
#include "SystemTick.h"
#include <QProcess>
#include <QDateTime>

#define MAXDATETIMEVALUE  2145916800  // 2037-12-31 23:59:59秒对应的时间戳
#define SHUT_DOWN_HINT_TIME  (120)
/**************************************************************************************************
 * 功能： 刷新界面。
 *************************************************************************************************/
void TimeManager::_refreshWidgets()
{
    // 刷新日期和时间。
    QString text;
    QString t;
    timeDate.getTime(_curTime, text, _showSecond);
    timeDate.getDate(_curTime, t, true);
    t += " ";
    t += text;
    t += " ";
    if (_dateTimeWidget)
    {
        _dateTimeWidget->setText(t);
    }

    // 刷新系统运行的时间。
    unsigned diffTime = _curTime - _elapseStartTime;
//    int min = diffTime / 60;
//    int sec = diffTime % 60;
//    t.sprintf("%.2d:%.2d", min, sec);
    int hour = diffTime / 3600;
    int min = (diffTime - hour * 3600) / 60;
    int sec = diffTime % 60;

    // 99小时59分59妙后还原
    if (hour < 99)
    {
    }
    else if (hour > 99)
    {
        hour = 0;
        min = 0;
        sec = 0;
        _elapseStartTime = _curTime;
        systemConfig.setNumValue("ElapseStartTime", _elapseStartTime);
    }
    else if (hour == 99 && min == 59 && sec == 59)
    {
        _elapseStartTime = _curTime + 1;
        systemConfig.setNumValue("ElapseStartTime", _elapseStartTime);
    }

    t.sprintf("%.2d:%.2d:%.2d", hour, min, sec); // NOLINT
    if (_elapsedWidget)
    {
        _elapsedWidget->setText(t);
    }
}

TimeManager &TimeManager::getInstance()
{
    static TimeManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new TimeManager();
        TimeManagerInterface *old = registerTimeManager(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

/**************************************************************************************************
 * 功能： 注册两个窗体控件。
 * 参数：
 *      widget: 日期时间显示控件。
 *      elapseWidget: 运行计时控件。
 *************************************************************************************************/
void TimeManager::registerWidgets(DateTimeWidget *widget, ElapseTimeWidget *elapseWidget)
{
    _dateTimeWidget = widget;
    _elapsedWidget = elapseWidget;

    // 初始设置。
    _refreshWidgets();
}

/**************************************************************************************************
 * 主运行入口。
 *************************************************************************************************/
void TimeManager::mainRun(unsigned t)
{
    if (t >= MAXDATETIMEVALUE)
    {
        _curTime = t - 100 * 3600;
        _elapseStartTime = _elapseStartTime - 100 * 3600;
        QDateTime dt = QDateTime::fromTime_t(_curTime);
        setSystemTime(dt);
        systemTick.resetLastTime();
    }
    else
    {
        _curTime = t;
    }

    _refreshWidgets();

    // 更新时间戳。
    systemConfig.setNumValue("Timestamp", _curTime);
}

void TimeManager::recordRunTime()
{
    _runTime++;
}

unsigned TimeManager::getRunTime() const
{
   return _runTime;
}

void TimeManager::setElapsedTime()
{
    _elapseStartTime = _curTime;
    systemConfig.setNumValue("ElapseStartTime", _elapseStartTime);
}

void TimeManager::roloadConfig()
{
    // 读配置是否显示秒
    systemConfig.getNumValue("DateTime|DisplaySecond", _showSecond);
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
TimeManager::TimeManager()
{
    _dateTimeWidget = NULL;
    _elapsedWidget = NULL;
    _runTime = 0;

    // 读出运行起始时间。
    systemConfig.getNumValue("ElapseStartTime", _elapseStartTime);

    // 读配置是否显示秒
    systemConfig.getNumValue("DateTime|DisplaySecond", _showSecond);

    // 判断上次开机与本次开机的时间间隔。
    unsigned timestamp = 0;
    systemConfig.getNumValue("Timestamp", timestamp);
    _curTime = timeDate.time();
    unsigned diffTime = (_curTime < timestamp) ? 901 : (_curTime - timestamp);

    // 判断开机时间间隔(关机到开机再次看到界面)。
    if (diffTime <= SHUT_DOWN_HINT_TIME + 7) // 两次开机时间小于SHUT_DOWN_HINT_TIME + 7(开机时间大约7s)。
    {
        _powerOnSession = POWER_ON_SESSION_CONTINUE;
    }
    else
    {
        _powerOnSession = POWER_ON_SESSION_NEW;
        _elapseStartTime = _curTime;
        systemConfig.setNumValue("ElapseStartTime", _elapseStartTime);
    }
}

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
TimeManager::~TimeManager()
{
}

void TimeManager::setSystemTime(const QDateTime &datetime)
{
    QString cmd = QString("date -s \"%1\"").arg(datetime.toString("yyyy-MM-dd hh:mm:ss"));
    QProcess process;
    process.execute(cmd);
    process.execute("hwclock --systohc");
    process.execute("sync");
    process.waitForFinished();
    _curTime = datetime.toTime_t();
}

void TimeManager::checkAndFixSystemTime()
{
    QDateTime cur = QDateTime::currentDateTime();

    if (cur.toTime_t() < 10)
    {
        setSystemTime(QDateTime(QDate(2018, 1, 1), QTime(0, 0)));
    }
    else if (cur.date().year() > 2037)
    {
        setSystemTime(QDateTime(QDate(2037, 12, 27), QTime(20, 0)));
    }
}

bool TimeManager::isShowSecond()
{
    return _showSecond;
}
