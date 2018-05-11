#pragma once
#include "IWidget.h"
#include "EventDataDefine.h"

struct WaveformDesc
{
    WaveformDesc()
    {
        waveRangeMax = 0;
        waveRangeMin = 0;
        startY = 0;
        endY = 0;
        offsetX = 0;
        waveID = WAVE_NONE;
    }
    int waveRangeMax;
    int waveRangeMin;
    double startY;
    double endY;
    double offsetX;
    WaveformID waveID;
};

class OxyCRGEventWaveWidgetPrivate;
class OxyCRGEventWaveWidget : public IWidget
{
    Q_OBJECT
public:
    OxyCRGEventWaveWidget();
    ~OxyCRGEventWaveWidget();

    void setWaveTrendSegments(const QVector<WaveformDataSegment *> waveSegments, const QVector<TrendDataSegment *> trendSegments);

protected:
    void paintEvent(QPaintEvent *e);

private:
    QScopedPointer<OxyCRGEventWaveWidgetPrivate> d_ptr;

};
