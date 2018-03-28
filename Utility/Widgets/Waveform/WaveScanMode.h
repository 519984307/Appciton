////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的扫描模式
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_SCAN_MODE_H
#define WAVE_SCAN_MODE_H

#include "WaveNormalMode.h"

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的扫描模式
////////////////////////////////////////////////////////////////////////////////
class WaveScanMode: public WaveNormalMode
{
public:
    WaveScanMode(WaveWidget *wave);
    ~WaveScanMode();
    bool match(int mode, bool isCascade);
    int maxUpdateRate();
    bool updateRegion(QRegion &region);
    void prepareTransformFactor();
    void valueRangeChanged();
    void addData(int value, int flag);
    void paintWave(QPainter &painter, const QRect &rect);

private:
    void _drawWave(QPainter &painter, int beginIndex, int endIndex);
    void _drawDotLine(QPainter &painter, int beginIndex, int endIndex);
    static void _drawSparseCurve(WaveScanMode *self, QPainter &painter,
            int beginIndex, int endIndex);
    static void _drawDenseCurve(WaveScanMode *self, QPainter &painter,
            int beginIndex, int endIndex);
    static void _fillSparseCurve(WaveScanMode *self, QPainter &painter,
            int beginIndex, int endIndex);
    static void _fillDenseCurve(WaveScanMode *self, QPainter &painter,
            int beginIndex, int endIndex);

    typedef void (*PaintWaveFunc)(WaveScanMode *, QPainter &, int, int);

    PaintWaveFunc _paintWaveFunc;  // 连续波形绘制函数

protected:
    int _pendingCount;             // 等待绘制的波形坐标点数
    int _scanLineSpace;            // 扫描线空白数据点数
};

#endif
