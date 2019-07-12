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
#include "SPO2Define.h"

class SPO2TrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 设置SPO2的值。
    void setSPO2Value(int16_t spo2);

    void setPlugInSPO2Value(int16_t spo2);

    void setSPO2DeltaValue(int16_t spo2);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 是否报警
    void isAlarmSPO2(bool flag);
    void isAlarmSPO22(bool flag);
    void isAlarmSPO2D(bool flag);

    // 显示参数值
    void showValue(void);

    SPO2TrendWidget();
    ~SPO2TrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;

    /**
     * @brief updateTrendWidget 更新控件
     */
    void updateTrendWidget();

    /**
     * @brief setBarValue 设置棒图值
     * @param value
     */
    void setBarValue(int16_t value);

public:
    virtual void doRestoreNormalStatus();

protected:
    virtual void setTextSize(void);

    /* reimplment */
    void loadConfig();

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_spo2Value1;
    QLabel *_spo2Name2;
    QLabel *_spo2Value2;
    QLabel *_spo2DeltaName;
    QLabel *_spo2DeltaValue;
    SPO2BarWidget *_spo2Bar;
    QString _spo2String1;
    QString _spo2String2;
    QString _spo2StringD;
    bool _isAlarmSPO2;
    bool _isAlarmSPO2_2;
    bool _isAlarmSPO2_D;
};
