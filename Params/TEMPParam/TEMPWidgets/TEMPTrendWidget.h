#pragma once
#include "TrendWidget.h"
#include <QLabel>

class TEMPTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 设置体温的值。
    void setTEMPValue(short t1, short t2, short td);

    // 设置体温单位
    void setUNit(UnitType u);

    // 是否报警
    void isAlarm(int id, bool flag);

    // 显示新值。
    void showValue(void);

    TEMPTrendWidget();
    ~TEMPTrendWidget();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    void _alarmIndicate(bool isAlarm);

    short _showWhich;     // 显示的值。
    QLabel *_tValue;

    QString _t1Str;
    QString _t2Str;
    QString _tdStr;
    bool _t1Alarm;
    bool _t2Alarm;
    bool _tdAlarm;
};
