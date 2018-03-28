#pragma once
#include <QWidget>
#include <QScopedPointer>
#include <EventDataDefine.h>

class EventWaveWidgetPrivate;
class EventWaveWidget : public QWidget
{
    Q_OBJECT
public:
    enum SweepSpeed
    {
        SWEEP_SPEED_125,
        SWEEP_SPEED_250,
    };

    EventWaveWidget(QWidget *parent = NULL);
    ~EventWaveWidget();

    /* get current wave display start seconds */
    int getCurrentWaveStartSecond() const;

    /* set the wave display start seconds */
    void setWaveStartSecond(int second);

    /* set the wave draw speed */
    void setSweepSpeed(SweepSpeed speed);

    /* set the wave segments to display */
    void setWaveSegments(const QVector<WaveformDataSegment *> waveSegemnts);

protected:
    void paintEvent(QPaintEvent *ev);
    void resizeEvent(QResizeEvent *ev);

private:
    QScopedPointer<EventWaveWidgetPrivate> d_ptr;
};
