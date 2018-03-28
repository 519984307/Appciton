#pragma once
#include "IWidget.h"


class QLabel;
class CPRBarWidget;
class CPRWidget : public IWidget
{
    Q_OBJECT

public:
    void updataWaveform(short waveform[], int len);
    void updateCompressionRate(short rate);

    CPRWidget();
    ~CPRWidget();

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void timerEvent(QTimerEvent *event);

private slots:
    void _releaseHandle(IWidget *);

private:
    CPRBarWidget *_barWidget;
    QLabel *_rateLabel;
    QLabel *_rateValue;
};
