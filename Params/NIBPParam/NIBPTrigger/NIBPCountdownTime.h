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
#include "NIBPCountdownTimeInterface.h"
#include <QTime>

#define STAT_Time    300       // STAT连续测量时间
#define AUTO_TIME    300       // 自动测量时间

class QTimer;
class NIBPCountdownTime : public QObject , public NIBPCountdownTimeInterface
{
    Q_OBJECT

public:
    static NIBPCountdownTime &getInstance(void);
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
    void setSTATMeasureTimeout(bool flag);
    int STATMeasureElapseTime(void);
    void timeChange(bool flag);

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
    bool _timeChangeFlag;         // 系统时钟改变标志
};
#define nibpCountdownTime (NIBPCountdownTime::getInstance())
