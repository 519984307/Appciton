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
#include <QLabel>
#include <QGroupBox>
#include <QStackedWidget>
#include "AlarmConfig.h"
#include "ParamManager.h"
#include <QScopedPointer>

class NIBPTrendWidgetPrivate;
class NIBPTrendWidget: public TrendWidget
{
    Q_OBJECT
#ifdef CONFIG_UNIT_TEST
    friend  class TestNIBPParam;
#endif
public:
    // 设置测量结果的数据。
    void setResults(int16_t sys, int16_t dia, int16_t map, unsigned time);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 恢复测量结果的数据。
    void recoverResults(int16_t &sys, int16_t &dia, int16_t &map, unsigned &time);

    // 保存测量结果的数据。
    void saveResults(int16_t sys, int16_t dia, int16_t map, unsigned time);

    // 设置实时袖带压。
    void setCuffPressure(int p);

    // 设置倒计时。
    void setCountdown(int t);

    // 单位更改。
    void updateUnit(UnitType unit);

    // 是否报警
    void isAlarm(int id, bool flag);

    // 显示血压值
    void showValue();

    // 显示字
    void showText(QString text);

    // NIBP显示框设置
    void setShowStacked(int num);

    //上次测量时间的显示
    void setShowMeasureCount(bool lastMeasureCountflag);

    // 显示STAT模式
    void showModelText(const QString &text);

    NIBPTrendWidget();
    ~NIBPTrendWidget();

    QList<SubParamID> getShortTrendSubParams() const;
public:
    virtual void doRestoreNormalStatus();

    virtual void updatePalette(const QPalette &pal);

    /* reimplment */
    void updateWidgetConfig();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    const QScopedPointer<NIBPTrendWidgetPrivate> d_ptr;
};
