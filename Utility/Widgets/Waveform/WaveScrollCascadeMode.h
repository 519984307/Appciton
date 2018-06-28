////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的级联滚动模式
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_SCROLL_CASCADE_MODE_H
#define WAVE_SCROLL_CASCADE_MODE_H

#include "WaveCascadeMode.h"

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的级联滚动模式
////////////////////////////////////////////////////////////////////////////////
class WaveScrollCascadeMode: public WaveCascadeMode
{
public:
    explicit WaveScrollCascadeMode(WaveWidget *wave);
    ~WaveScrollCascadeMode();
    bool match(int mode, bool isCascade);
    void addData(int value, int flag, bool isUpdated=true);
    bool updateRegion(QRegion &region);
    int maxUpdateRate();
    void paintWave(QPainter &painter, const QRect &rect);
};

#endif
