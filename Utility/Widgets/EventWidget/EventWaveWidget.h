#pragma once
#include <QWidget>
#include <QScopedPointer>
#include <EventDataDefine.h>

struct WaveformDesc
{
    void reset(void)
    {
        startY = 0;
        mediumY = 0;
        endY = 0;
        waveID = WAVE_NONE;
        waveBuf.clear();
    }

    int startY;                         // 不同波形的y坐标起始偏移像素点。
    int mediumY;                        // 不同波形的y坐标中间位置。
    int endY;                           // 不同波形的y坐标的结束偏移像素点。
    WaveformID waveID;                  // 波形ID
    QVector<WaveDataType> waveBuf;      //波形数据
};


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
    void _drawWave(int index, QPainter &painter);
    void _drawWaveLabel(QPainter &painter, const WaveformDesc &waveDesc);

private:
    QScopedPointer<EventWaveWidgetPrivate> d_ptr;
};
