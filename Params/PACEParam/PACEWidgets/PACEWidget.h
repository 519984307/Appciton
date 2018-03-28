#pragma once
#include "IWidget.h"

class QLabel;
class PACEWidget : public IWidget
{
    Q_OBJECT

public:
    void pulseDelivered(void);

    PACEWidget();
    ~PACEWidget();

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void timerEvent(QTimerEvent *event);

private slots:
    void _releaseHandle(IWidget *);

private:
    static const int _barHeight = 22;

    QLabel *_rateLabel;
    QLabel *_rateValue;
    QLabel *_rateUnit;

    QLabel *_currentLabel;
    QLabel *_currentValue;
    QLabel *_currentUnit;

    QLabel *_modeLabel;
    QLabel *_modeValue;

    int _pulseDeliver;
    QRect _flushArea;
};
