#pragma once
#include "TrendWidget.h"
#include "CO2Define.h"
#include <QLabel>
#include <QResizeEvent>
#include "Debug.h"
class CO2BRTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 窗口是否使能
    virtual bool enable();

    // 设置测量值。
    void setBRValue(short v);


    // 是否报警
    void isAlarm(bool flag);

    // 显示
    void showValue();

    // 立刻显示数据。
    void flushValue(void);

    // 构造与析构。
    CO2BRTrendWidget();
    ~CO2BRTrendWidget();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    void _setValue(short v, QString &str);

    QLabel *_brValue;

    QString _brStr;
    bool _brAlarm;
};
