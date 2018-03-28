#pragma once
#include "TrendWidget.h"
#include "ECGDefine.h"

class QLabel;
class QTimer;
class ECGTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 刷新HR和ST的值。
    void setHRValue(short hr, bool isHR);

    // 是否发生报警
    void isAlarm(bool isAlarm);

    // 显示参数值
    void showValue(void);

    // 闪烁心跳图标
    void blinkBeatPixmap();

    //设置计算导联字串
    void setTrendWidgetCalcName(ECGLead calLead);

    ECGTrendWidget();
    ~ECGTrendWidget();

#ifdef CONFIG_UNIT_TEST
    friend class TestECGParam;
#endif
protected:
    void showEvent(QShowEvent *e);
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);
    void _timeOut();

private:
    QLabel *_hrBeatIcon;
    QLabel *_hrValue;
    QPixmap beatPixmap;
    QString _hrString;
    bool _isAlarm;

private:
    QTimer *_timer;
};
