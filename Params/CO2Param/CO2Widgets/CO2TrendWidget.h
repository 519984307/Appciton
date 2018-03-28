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
    void setEtCO2Value(short v);
    void setFiCO2Value(short v);

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

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    void _setValue(short v, QString &str);

    QLabel *_etco2Value;
    QLabel *_fico2Label;
    QLabel *_fico2Value;

    QString _etco2Str;
    QString _fico2Str;
    bool _etco2Alarm;
    bool _fico2Alarm;
};
