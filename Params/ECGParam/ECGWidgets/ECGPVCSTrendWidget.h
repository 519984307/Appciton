#pragma once
#include "TrendWidget.h"
#include "ECGDefine.h"

class QLabel;
class ECGPVCSTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 刷新PVCS的值。
    void setPVCSValue(short pvcs);

    // 是否发生报警
    void isAlarm(bool isAlarm);

    // 显示参数值
    void showValue(void);

    ECGPVCSTrendWidget();
    ~ECGPVCSTrendWidget();

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_pvcsValue;
    QString _pvcsString;
    bool _isAlarm;
};
