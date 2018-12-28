/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/12/14
 **/

#pragma once
#include <QWidget>
#include <QScopedPointer>
#include <EventDataDefine.h>

struct WaveformDesc
{
    void reset(void)
    {
        waveRangeMax = 0;
        waveRangeMin = 0;
        startY = 0;
        mediumY = 0;
        endY = 0;
        offsetX = 0;
        waveID = WAVE_NONE;
        isECG = false;
    }

    int waveRangeMax;                   // 波形数据最大值。
    int waveRangeMin;                   // 波形数据最小值。
    double startY;                         // 不同波形的y坐标起始偏移像素点。
    double mediumY;                        // 不同波形的y坐标中间位置。
    double endY;                           // 不同波形的y坐标的结束偏移像素点。
    double offsetX;                     // 数据的x轴偏移。
    WaveformID waveID;                  // 波形ID
    bool isECG;
};


class EventWaveWidgetPrivate;
class EventWaveWidget : public QWidget
{
    Q_OBJECT
public:
    enum SweepSpeed
    {
        SWEEP_SPEED_62_5,
        SWEEP_SPEED_125,
        SWEEP_SPEED_250,
        SWEEP_SPEED_500
    };

    explicit EventWaveWidget(QWidget *parent = NULL);
    ~EventWaveWidget();

    void initXCoordinate(void);
    int getCurrentWaveMedSecond() const;
    void setWaveMedSecond(int second);

    /* get current wave display start seconds */
    int getCurrentWaveStartSecond() const;

    /* set the wave display start seconds */
    void setWaveStartSecond(int second);

    /* set the wave draw speed */
    void setSweepSpeed(SweepSpeed speed);
    SweepSpeed getSweepSpeed(void);

    /* set the wave segments to display */
    void setWaveSegments(const QVector<WaveformDataSegment *> waveSegemnts);
    void setInfoSegments(EventInfoSegment *info);

    void setGain(ECGEventGain);

protected:
    void paintEvent(QPaintEvent *ev);
    void resizeEvent(QResizeEvent *ev);

private:
    void _drawWave(int index, QPainter &painter);
    void _drawWaveLabel(QPainter &painter, const WaveformDesc &waveDesc);
    double _mapWaveValue(WaveformDesc &waveDesc, int wave);

private:
    QScopedPointer<EventWaveWidgetPrivate> d_ptr;
};
