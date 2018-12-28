/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/11
 **/

#pragma once
#include "NIBPState.h"
#include <QTime>

#define STAT_Time    300       // STAT连续测量时间
#define AUTO_TIME    300       // 自动测量时间

class QTimer;
class NIBPCountdownTime : public QObject
{
    Q_OBJECT

public:
    static NIBPCountdownTime &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new NIBPCountdownTime();
        }
        return *_selfObj;
    }
    static NIBPCountdownTime *_selfObj;
    ~NIBPCountdownTime();

public:
    void run(void);

    void autoMeasureStart(int sec);
    void autoMeasureChange(int sec);
    bool isAutoMeasureTimeout(void);
    void setAutoMeasureTimeout(bool flag);
    int autoMeasureElapseTime(void);
    void autoMeasureStop(void);

    void STATMeasureStart(void);
    void STATMeasureStop(void);
    bool isSTATMeasureTimeout(void);
    int STATMeasureElapseTime(void);

private slots:
    void _autoMeasureTimeout(void);
    void _STATMeasureTimeout(void);

private:
    NIBPCountdownTime();
    NIBPState *_currentState;
    QTimer *_autoMeasureTimer;           // 自动测量倒计时
    QTimer *_STATMeasureTimer;           // STAT测量倒计时
    QTime _autoMeasureElapseTimer;        // 计时
    QTime _STATMeasureElapseTimer;
    bool _isAutoMeasureTimeout;
    bool _isSTATMeasureTimeout;

    int _autoTime;                // 自动倒计时时间
    int _autoElapseTime;          // 自动计时器时间

    int _STATTime;                // STAT倒计时时间
    int _STATElapseTime;          // STAT计时器时间
};
#define nibpCountdownTime (NIBPCountdownTime::construction())
#define deleteNIBPCountdownTime() (delete NIBPCountdownTime::_selfObj)
