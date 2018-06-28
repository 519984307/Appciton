////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的级联回顾模式
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_REVIEW_CASCADE_MODE_H
#define WAVE_REVIEW_CASCADE_MODE_H

#include "WaveCascadeMode.h"

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的级联回顾模式
////////////////////////////////////////////////////////////////////////////////
class WaveReviewCascadeMode: public WaveCascadeMode
{
public:
    explicit WaveReviewCascadeMode(WaveWidget *wave);
    ~WaveReviewCascadeMode();
    bool match(int mode, bool isCascade);
    void reviewWave(const QDateTime &time);
    bool updateRegion(QRegion &region);
    int maxUpdateRate();
    void paintWave(QPainter &painter, const QRect &rect);
};

#endif
