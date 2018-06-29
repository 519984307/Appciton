////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件波形模式基类, 定义坐标映射的接口
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_MODE_H
#define WAVE_MODE_H

#include <QRect>

class QDateTime;
class QPainter;
class QRegion;
class WaveWidget;

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件波形模式基类, 定义坐标映射的接口
////////////////////////////////////////////////////////////////////////////////
class WaveMode
{
public:
    explicit WaveMode(WaveWidget *wave)
            : _wave(wave)
    {
    }
    virtual ~WaveMode()
    {
    }
    virtual bool match(int mode, bool isCascade) = 0;
    virtual int maxUpdateRate() = 0;
    virtual int dataSize() = 0;
    virtual int dataSize(float /* minDataRateOfSameSpeedWave */) {return dataSize();}
    virtual bool updateRegion(QRegion &/*region*/)
    {
        return false;
    }
    virtual int yToValue(int y) = 0;
    virtual int valueToY(int value) = 0;
    virtual int xToIndex(int x) = 0;
    virtual int indexToX(int index) = 0;
    virtual void prepareTransformFactor() = 0;
    virtual void valueRangeChanged() = 0;
    virtual void addData(int /*value*/, int /*flag*/, bool /*isUpdated*/)
    {
    }
    virtual void reviewWave(const QDateTime &/*time*/)
    {
    }
    virtual void preparePaint()
    {
    }
    virtual bool supportPartialPaint()
    {
        return true;
    }
    virtual void paintWave(QPainter &painter, const QRect &rect) = 0;
    virtual void enter()
    {
    }
    virtual void exit()
    {
    }

protected:
    WaveWidget *_wave;      // 波形控件对象
};

#endif
