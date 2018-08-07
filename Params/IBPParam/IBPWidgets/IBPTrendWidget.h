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
#include "IBPDefine.h"
#include <QLabel>
#include <QResizeEvent>
#include <QStackedWidget>
#include "Debug.h"
class IBPTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 设置测量实时数据。
    void setData(int16_t sys, int16_t dia, int16_t map);

    // 设置标名。
    void setEntitle(IBPPressureName entitle);

    // IBP显示框设置
    void setShowStacked(int num);

    // 校零标志
    void setZeroFlag(bool isEnabled);

    // 是否报警
    void isAlarm(int id, bool flag);

    // 显示参数值
    void showValue(void);

    // 构造与析构。
    IBPTrendWidget(const QString &trendName, const IBPPressureName &entitle);
    ~IBPTrendWidget();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private slots:

private:
    IBPPressureName _entitle;
    QLabel *_zeroWarn;          // 校零提示；

    QLabel *_ibpValue;          // 动脉压
    QLabel *_sysValue;
    QLabel *_diaValue;
    QLabel *_mapValue;
    QLabel *_veinValue;         // 静脉压

    QString _sysString;
    QString _diaString;
    QString _mapString;
    QString _veinString;

    QStackedWidget *_stackedwidget;

    bool _isZero;                // 校零标志
    bool _sysAlarm;
    bool _diaAlarm;
    bool _mapAlarm;

    static const int _margin = 1;
};
