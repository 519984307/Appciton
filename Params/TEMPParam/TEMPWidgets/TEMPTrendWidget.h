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
#include <QLabel>

class TEMPTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 设置体温的值。
    void setTEMPValue(int16_t t1, int16_t t2, int16_t td);

    // 设置体温单位
    void setUNit(UnitType u);

    // 是否报警
    void isAlarm(int id, bool flag);

    // 显示新值。
    void showValue(void);

    TEMPTrendWidget();
    ~TEMPTrendWidget();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    // 温度组
    enum tempGrp
    {
        TEMP_GRP_T1 = 0,
        TEMP_GRP_T2,
        TEMP_GRP_TD,
        TEMP_GRP_NR
    };

    void _alarmIndicate(bool isAlarms, tempGrp grp);

    QLabel *_t1Value;
    QLabel *_t2Value;
    QLabel *_tdValue;
    QLabel *_t1Name;
    QLabel *_t2Name;
    QLabel *_tdName;

    QString _t1Str;
    QString _t2Str;
    QString _tdStr;
    bool _t1Alarm;
    bool _t2Alarm;
    bool _tdAlarm;
};
