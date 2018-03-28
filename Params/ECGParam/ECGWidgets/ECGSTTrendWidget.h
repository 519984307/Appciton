#pragma once
#include "TrendWidget.h"
#include "ECGDefine.h"

class QLabel;
class ECGSTTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 刷新HR和ST的值。
    void setSTValue(ECGST lead, short st);

    // 是否发生报警
    void isAlarm(int id, bool flag);

    // 显示参数值
    void showValue(void);

    ECGSTTrendWidget();
    ~ECGSTTrendWidget();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_stLabel[ECG_ST_NR];
    QLabel *_stValue[ECG_ST_NR];

    QString _stString[ECG_ST_NR];
    bool _isAlarm[ECG_ST_NR];
};
