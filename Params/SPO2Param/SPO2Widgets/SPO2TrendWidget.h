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
    void setSPO2Value(qint16 spo2);

    void setPluginSPO2Value(qint16 spo2);

    void setSPO2DeltaValue(qint16 spo2);

    void setPIValue(qint16 pi);

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
    void setBarValue(qint16 value);

public:
    virtual void doRestoreNormalStatus();

protected:
    virtual void setTextSize(void);

    /* reimplment */
    void loadConfig();

private slots:
    void _releaseHandle(IWidget *);
    void onLayoutChange();

private:
    QLabel *_spo2Value1;
    QLabel *_spo2Name2;
    QLabel *_spo2Value2;
    QLabel *_spo2DeltaName;
    QLabel *_spo2DeltaValue;
    QLabel *_piName;
    QLabel *_piValue;
    SPO2BarWidget *_spo2Bar;
    QString _spo2String1;
    QString _spo2String2;
    QString _spo2StringD;
    QString _piString;
    bool _isAlarmSPO2;
    bool _isAlarmSPO2_2;
    bool _isAlarmSPO2_D;
};
