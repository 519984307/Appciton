////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的回顾模式
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_REVIEW_MODE_H
#define WAVE_REVIEW_MODE_H

#include <QObject>
#include <QDateTime>
#include "WaveNormalMode.h"

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的回顾模式
////////////////////////////////////////////////////////////////////////////////
class WaveReviewMode: public QObject, public WaveNormalMode
{
    Q_OBJECT
public:
    explicit WaveReviewMode(WaveWidget *wave);
    ~WaveReviewMode();
    bool match(int mode, bool isCascade);
    void reviewWave(const QDateTime &time);
    void prepareTransformFactor();
    void valueRangeChanged()
    {
    }
    int maxUpdateRate();
    void paintWave(QPainter &painter, const QRect &rect);
    void enter();
    void exit();

private slots:
    void _onModelChanged();

private:
    void _loadWaveData();

    static void _drawSparseCurve(WaveReviewMode *self, QPainter &painter,
            int beginIndex, int endIndex);
    static void _drawDenseCurve(WaveReviewMode *self, QPainter &painter,
            int beginIndex, int endIndex);
    static void _fillSparseCurve(WaveReviewMode *self, QPainter &painter,
            int beginIndex, int endIndex);
    static void _fillDenseCurve(WaveReviewMode *self, QPainter &painter,
            int beginIndex, int endIndex);

    typedef void (*PaintWaveFunc)(WaveReviewMode *, QPainter &, int, int);

    PaintWaveFunc _paintWaveFunc;  // 连续波形绘制函数
    QDateTime _reviewTime;         // 回顾时刻
    bool _needLoadWaveData;        // 是否需要重新加载波形数据
};

#endif
