#pragma once
#include "WaveScanMode.h"

class BarGraphScanMode : public WaveScanMode
{
public:
    BarGraphScanMode(WaveWidget *wave);
    //override
    bool match(int mode, bool isCascade);
    //override
    void paintWave(QPainter &painter, const QRect &rect);
    //override
    bool updateRegion(QRegion &region);
    //override
    int dataSize();
    //override
    void preparePaint();
    //override
    void prepareTransformFactor();

private:
    void _drawBaseLine(QPainter &painter, int beginIndex, int endIndex);
    QPoint _newBarPos;
    QRect _barRects[2];
    bool _bufferReset;
};
