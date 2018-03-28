#pragma once
#include "TrendWidget.h"
#include <QLabel>

class RESPTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 窗口是否使能
    bool enable();

    // 刷新BR和RR的值。
    void setRRValue(short rr, bool isRR);

    // 是否报警
    void isAlarm(bool flag);

    // 显示
    void showValue(void);

    RESPTrendWidget();
    ~RESPTrendWidget();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_rrValue;
    bool _isAlarm;
    QString _rrString;
};
