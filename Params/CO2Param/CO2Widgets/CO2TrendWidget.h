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
#include "CO2Define.h"
#include <QLabel>
#include <QResizeEvent>
#include "Debug.h"
class CO2TrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 窗口是否使能
    virtual bool enable();

    // 设置测量值。
    void setEtCO2Value(int16_t v);
    void setFiCO2Value(int16_t v);
    void setawRRValue(int16_t v);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 单位更改。
    void setUNit(UnitType unit);

    // 设置FiCO2显示。
    void setFiCO2Display(CO2FICO2Display disp);

    // 是否报警
    void isAlarm(int id, bool flag);

    // 显示
    void showValue();

    // 立刻显示数据。
    void flushValue(void);

    // 构造与析构。
    CO2TrendWidget();
    ~CO2TrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;
public:
    virtual void doRestoreNormalStatus();

protected:
    virtual void setTextSize(void);

    /* *reimplment */
    void loadConfig();

private slots:
    void _releaseHandle(IWidget *);

private:
    void _setValue(int16_t v, QString &str);

    QLabel *_etCO2Label;
    QLabel *_etco2Value;
    QLabel *_fico2Label;
    QLabel *_fico2Value;
    QLabel *_awRRName;
    QLabel *_awRRValue;

    QString _etco2Str;
    QString _fico2Str;
    QString _awRRStr;
    bool _etco2Alarm;
    bool _fico2Alarm;
};
