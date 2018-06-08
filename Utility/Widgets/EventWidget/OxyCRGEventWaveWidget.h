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

struct TrendConvertDesc
{
    TrendConvertDesc()
    {
        max = 0;
        min = 0;
        start = 0;
        end = 0;
    }
    int max;        // 数据范围最大值
    int min;        // 数据范围最小值
    int start;      // 坐标开始像素点
    int end;        // 坐标结束像素点
};

enum OxyCRGEventStorageDuration
{
    OxyCRG_EVENT_DURATION_1_3MIN = 0,           // before:1, after:3
    OxyCRG_EVENT_DURATION_2_2MIN,               // before:2, after:2
    OxyCRG_EVENT_DURATION_3_1MIN                // before:3, after:1
};

class OxyCRGEventWaveWidgetPrivate;
class OxyCRGEventWaveWidget : public IWidget
{
    Q_OBJECT
public:
    OxyCRGEventWaveWidget();
    ~OxyCRGEventWaveWidget();

    void setWaveTrendSegments(const QVector<WaveformDataSegment *> waveSegments, const QVector<TrendDataSegment *> trendSegments);
    void setWaveWidgetCompressed(WaveformID id);
    void setWaveWidgetTrend1(bool isRR);

    // optimization
    void loadOxyCRGEventData(const QList<TrendGraphInfo> &trendInfos, const OxyCRGWaveInfo &waveInfo,
                             EventInfoSegment *const eventInfo);

    QMap<SubParamID, QPainterPath> generatorPainterPath(const QList<TrendGraphInfo> &graphInfo, QPainter &painter, int curDataIndex);
    QPainterPath generatorWaveformPath(const OxyCRGWaveInfo &waveInfo, QPainter &painter);
public slots:
    void leftMoveCursor(void);
    void rightMoveCursor(void);
    void leftMoveCoordinate(void);
    void rightMoveCoordinate(void);

protected:
    void paintEvent(QPaintEvent *e);

private:
    void _drawWave(QPainter &painter);
    double _mapWaveValue(WaveformDesc &waveDesc, int wave);
    void _loadTrendData(void);
    void _drawDottedLine(QPainter &painter, qreal x1, qreal y1, qreal x2, qreal y2);

    double _mapValue(TrendConvertDesc desc, int data);

private:
    QScopedPointer<OxyCRGEventWaveWidgetPrivate> d_ptr;

};
