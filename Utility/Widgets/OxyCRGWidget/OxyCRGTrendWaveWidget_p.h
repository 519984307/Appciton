/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/25
 **/

#pragma once
#include "Framework/Utility/RingBuff.h"
#include <QColor>
#include "BaseDefine.h"
#include <QPainterPath>
#include "OxyCRGDefine.h"
#include "OxyCRGTrendWaveWidget.h"

#define X_SHIFT     (2)
#define Y_SHIFT     (2)
#define WX_SHIFT     (50)
#define MAX_WAVE_DURATION (8)

/**
 * @brief The OxyCRGWaveSegment struct
 * @note this structure is mainly used to record one point or one line
 * @if is one point, minY is equal to maxY, otherwise, it will be a line
 */
struct OxyCRGWaveSegment
{
    OxyCRGWaveSegment() : minY(0), maxY(0) {}
    bool isValid() const
    {
        return minY != 0 && maxY != 0;
    }
    bool isPoint() const
    {
        return minY == maxY;
    }
    short minY;
    short maxY;
};

struct OxyCRGWaveBuffer
{
    explicit OxyCRGWaveBuffer(int segmentCount)
        : segmentCount(segmentCount),
          newestSegmentIndex(0),
          highDensitiy(false)
    {
        if (this->segmentCount == 0)
        {
            /* at least 1 segment */
            this->segmentCount = 1;
        }
        segments = new OxyCRGWaveSegment[this->segmentCount];
    }

    ~OxyCRGWaveBuffer()
    {
        delete [] segments;
    }

    void resize(int segmentCount)
    {
        if (segmentCount == this->segmentCount)
        {
            // the same size, just clear
            clear();
            return;
        }
        delete [] segments;
        if (segmentCount == 0)
        {
            /* at least 1 */
            segmentCount = 1;
        }
        segments = new OxyCRGWaveSegment[segmentCount];
        this->segmentCount = segmentCount;
        newestSegmentIndex = 0;
    }

    void clear()
    {
        memset(segments, 0, segmentCount * sizeof(OxyCRGWaveSegment));
        newestSegmentIndex = 0;
    }

    /**
     * @brief pushPointData push y value to the segment buffer
     * @param y the y value
     * @param newSegmnet mark whether this is a new segment
     */
    void pushPointData(short y, bool newSegmnet)
    {
        if (newSegmnet)
        {
            OxyCRGWaveSegment &oldSeg = segments[newestSegmentIndex];
            // need to clear exist segment if this is a new segment
            newestSegmentIndex = (newestSegmentIndex + 1) % segmentCount;
            OxyCRGWaveSegment &newSeg =  segments[newestSegmentIndex];
            newSeg.maxY = y;
            newSeg.minY = y;

            // fix the gap between line and a point
            if (highDensitiy && newSeg.isValid() && oldSeg.isValid())
            {
                if (oldSeg.maxY < y)
                {
                    newSeg.minY = oldSeg.maxY;
                }
                else if (oldSeg.minY > y)
                {
                    newSeg.maxY = oldSeg.minY;
                }
            }
        }
        else if (!segments[newestSegmentIndex].isValid() || y == 0)
        {
            segments[newestSegmentIndex].maxY = y;
            segments[newestSegmentIndex].minY = y;
        }
        else
        {
            if (segments[newestSegmentIndex].maxY < y)
            {
                segments[newestSegmentIndex].maxY = y;
            }

            if (segments[newestSegmentIndex].minY > y)
            {
                segments[newestSegmentIndex].minY = y;
            }
        }
    }

    int segmentCount;
    int newestSegmentIndex;
    bool highDensitiy;
    OxyCRGWaveSegment *segments;

private:
    OxyCRGWaveBuffer(const OxyCRGWaveBuffer &other);    // stop the cppcheck complaint
};

class OxyCRGTrendWaveWidgetPrivate
{
public:
    explicit OxyCRGTrendWaveWidgetPrivate(OxyCRGTrendWaveWidget * const q_ptr);

    /**
     * @brief getIntervalSeconds  convert interval to seconds
     */
    int getIntervalSeconds(OxyCRGInterval interval);

    /**
     * @brief drawWave draw the wave data
     */
    void drawWave(QPainter *painter, const QRect &waveRegion, const OxyCRGWaveBuffer * buffer, const QColor &waveColor);

    /**
     * @brief updateWaveDrawingContext udpate the wave draw context, should be called when the interval,
     *        the wavedatarate or waveRegion is changed.
     */
    virtual void updateWaveDrawingContext();

    /**
     * @brief reload the wave buffer base on the data buff
     *
     */
    void reloadWaveBuffer();

    virtual ~OxyCRGTrendWaveWidgetPrivate();

    OxyCRGTrendWaveWidget * q_ptr;
    RingBuff<short> *dataBuf;    // 波形数据缓存
    OxyCRGWaveBuffer *waveBuffer;  // wave buffer

    QString name;                // 波形名称
    int rulerHigh;               // 标尺高值
    int rulerLow;                // 标尺低值

    QColor waveColor;            // 波形颜色

    int waveDataRate;            // 波形速率

    bool drawRuler;              // mark whether need to draw ruler

    QPainterPath backgroundRulerPath;  // 背景标尺
    QRect waveRegion;
    OxyCRGInterval interval;
    float pointGap;              // gap between two data point
    float pointGapSumFraction;   // the point gap sum 's fraction part

private:
    OxyCRGTrendWaveWidgetPrivate(const OxyCRGTrendWaveWidgetPrivate&other);  // make the cppckeck stop complain
};


