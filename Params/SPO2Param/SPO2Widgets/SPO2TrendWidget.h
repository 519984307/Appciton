/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once
#include "TrendWidget.h"
#include "SPO2BarWidget.h"
#include <QLabel>

class SPO2TrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 设置SPO2的值。
    void setSPO2Value(int16_t spo2);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 设置棒图的值。
    void setBarValue(int16_t bar);

    // 设置PI值
    void setPIValue(int16_t pi);

    // 显示搜索脉搏的提示信息。
    void setSearchForPulse(bool isSearching);

    // 是否报警
    void isAlarm(bool flag);

    // 显示参数值
    void showValue(void);

    SPO2TrendWidget();
    ~SPO2TrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;

public:
    virtual void doRestoreNormalStatus();

protected:
    virtual void setTextSize(void);

    /* reimplment */
    void loadConfig();

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_spo2Value;
    QLabel *_piName;
    QLabel *_piValue;
    SPO2BarWidget *_spo2Bar;
    QString _spo2String;
    QString _piString;
    bool _isAlarm;
};
