////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的滚动模式
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_SCROLL_MODE_H
#define WAVE_SCROLL_MODE_H

#include "WaveNormalMode.h"

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的滚动模式
////////////////////////////////////////////////////////////////////////////////
class WaveScrollMode: public WaveNormalMode
{
public:
    explicit WaveScrollMode(WaveWidget *wave);
    ~WaveScrollMode();
    bool match(int mode, bool isCascade);
    int maxUpdateRate();
    bool updateRegion(QRegion &region);
    void valueRangeChanged();
    void addData(int value, int flag, bool isUpdated=true);
    void preparePaint();
    bool supportPartialPaint()
    {
        return false;
    }
    void paintWave(QPainter &painter, const QRect &rect);

private:
    bool _isPending;                // 是否等待重绘
};

#endif
