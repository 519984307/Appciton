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
#include "Framework/Utility/Unit.h"
class IBPTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 设置测量实时数据。
    void setData(int16_t sys, int16_t dia, int16_t map);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 设置标名。
    void setEntitle(IBPLabel entitle);

    // IBP显示框设置
    void setShowStacked(int num);

    // 校零标志
    void setZeroFlag(bool isEnabled);

    // 是否报警
    void isAlarm(int id, bool flag);

    // 显示参数值
    void showValue(void);

    /**
     * @brief updateUnit  update IBP trend widget unit
     * @param unit  unit type
     */
    void updateUnit(UnitType unit);

    // 构造与析构。
    IBPTrendWidget(const QString &trendName, const IBPLabel &entitle);
    ~IBPTrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;
public:
    virtual void doRestoreNormalStatus();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private slots:

private:
    IBPLabel _entitle;
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
