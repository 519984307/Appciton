/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/20
 **/

#pragma once

#include <QObject>
#include "WaveNormalMode.h"

class FreezeWaveReviewMode : public QObject, public WaveNormalMode
{
    Q_OBJECT
public:
    explicit FreezeWaveReviewMode(WaveWidget *wave);
    ~FreezeWaveReviewMode();
    bool match(int mode, bool isCascade);
    void prepareTransformFactor();
    void valueRangeChanged() {}
    int maxUpdateRate()
    {
        return -1;
    }
    void paintWave(QPainter &painter, const QRect &rect);
    void enter();
    void exit();

private slots:
    void _onModelChanged();

private:
    void _loadWaveData();
    void _drawWave(QPainter &painter, int beginIndex, int endIndex);
    void _drawDotLine(QPainter &painter, int beginIndex, int endIndex);
    static void _drawSparseCurve(FreezeWaveReviewMode *self, QPainter &painter,
                                 int beginIndex, int endIndex);
    static void _drawDenseCurve(FreezeWaveReviewMode *self, QPainter &painter,
                                int beginIndex, int endIndex);
    static void _fillSparseCurve(FreezeWaveReviewMode *self, QPainter &painter,
                                 int beginIndex, int endIndex);
    static void _fillDenseCurve(FreezeWaveReviewMode *self, QPainter &painter,
                                int beginIndex, int endIndex);

    typedef void (*PaintWaveFunc)(FreezeWaveReviewMode *, QPainter &, int, int);

    PaintWaveFunc _paintWaveFunc;  // 连续波形绘制函数
};
