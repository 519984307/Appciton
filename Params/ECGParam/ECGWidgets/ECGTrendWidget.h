/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/7
 **/


#pragma once
#include "TrendWidget.h"
#include "ECGDefine.h"

class QLabel;
class QTimer;
class ECGTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 刷新HR和ST的值。
    void setHRValue(int16_t hr, bool isHR);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 是否发生报警
    void isAlarm(bool isAlarm);

    // 显示参数值
    void showValue(void);

    // 闪烁心跳图标
    void blinkBeatPixmap();

    ECGTrendWidget();
    ~ECGTrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;

protected:
    void showEvent(QShowEvent *e);
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);
    void _timeOut();

private:
    QLabel *_hrBeatIcon;
    QLabel *_hrValue;
    QPixmap beatPixmap;
    QString _hrString;
    bool _isAlarm;

private:
    QTimer *_timer;
};
