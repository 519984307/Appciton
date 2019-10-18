/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/2
 **/

#include "EventWaveWidget.h"
#include <QPoint>
#include <QVector>
#include "SystemManager.h"
#include <QResizeEvent>
#include "FontManager.h"
#include "ColorManager.h"
#include "WaveformCache.h"
#include "ConfigManager.h"
#include <QPainter>
#include "LanguageManager.h"

#define INVALID_AXIS_VALUE ((1<<30) - 1)

#define WAVE_TOP_MARGIN             20                                      // 波形顶部边缘距
#define WAVE_REG_HIGH               300                                     // 波形域高度
#define WAVE_NUM                    3                                       // 波形数目
#define WAVE_DATA_REG_HIGH          (WAVE_REG_HIGH / WAVE_NUM -20)          // 单参数波形数据域高度

#define DASH_LENGTH 5

struct WaveRegionBuffer
{
    explicit WaveRegionBuffer(int pointNum)
        : pointBuffer(NULL),
          waveStartPos(0),
          pointNum(pointNum)
    {
        pointBuffer = new QPoint[pointNum];
    }

    WaveRegionBuffer(const WaveRegionBuffer &obj)
        : pointBuffer(new QPoint[pointNum]),
          waveStartPos(obj.waveStartPos),
          pointNum(obj.pointNum)
    {}

    ~WaveRegionBuffer()
    {
        delete [] pointBuffer;
    }

    bool pointIsValue(const QPoint &p)
    {
        /* if the point's x value equal to INT32_MAX,
         * consider it as a invalid point
         */
        return p.y() != INVALID_AXIS_VALUE;
    }

    /**
     * @brief insertPoint insert point into the wave region,
     * &param p point
     * @param frombegin insert point from the begin if true, otherwise, form the end
     */
    void insertPoint(const QPoint &p, bool frombegin)
    {
        if (frombegin)
        {
            // insert point from the begin
            if (waveStartPos == 0)
            {
                waveStartPos = pointNum;
            }
            pointBuffer[--waveStartPos] = p;
        }
        else
        {
            // inset poin form the end
            pointBuffer[waveStartPos++] = p;
            if (waveStartPos == pointNum)
            {
                waveStartPos = 0;
            }
        }
    }

    QPoint *pointBuffer;
    int waveStartPos;
    int pointNum;
};


class EventWaveWidgetPrivate
{
public:
    EventWaveWidgetPrivate()
        : speed(EventWaveWidget::SWEEP_SPEED_125),
          gain(ECG_EVENT_GAIN_X10), pixelWPicth(0),
          pixelHPitch(0),
          displayWaveSeconds(0),
          bufferEmpty(false),
          totalWaveDuration(0),
          currentWaveStartSecond(-1),
          currentWaveMedSecond(0), durationBefore(0),
          durationAfter(0), startX(0), endX(0), waveRagWidth(0)
    {
        // 事件波形宽带按照合适的像素大小,高度按照实际的像素大小;
        pixelWPicth = 0.25;
        pixelHPitch = systemManager.getScreenPixelHPitch();
    }

    ~EventWaveWidgetPrivate()
    {
        qDeleteAll(waveBuffers);
    }

    // calculate the pixel lenght of one second waveform
    int pixLengthOfOneSecondWaveform()
    {
        float lengthMM;
        switch (speed)
        {
        case EventWaveWidget::SWEEP_SPEED_62_5:
            lengthMM = 6.25;
            displayWaveSeconds = 16;
            break;
        case EventWaveWidget::SWEEP_SPEED_125:
            lengthMM = 12.5;
            displayWaveSeconds = 8;
            break;
        case EventWaveWidget::SWEEP_SPEED_250:
            lengthMM = 25;
            displayWaveSeconds = 4;
            break;
        case EventWaveWidget::SWEEP_SPEED_500:
            lengthMM = 50.0;
            displayWaveSeconds = 2;
        default:
            break;
        }

        return lengthMM / pixelWPicth;
    }


    // update the display waveform duration
    void updateDisplayWaveformDuration(int seconds)
    {
        if (displayWaveSeconds == seconds)
        {
            return;
        }

        displayWaveSeconds = seconds;

        reallocateWaveRegionBuffer();
    }

    // reallocate the wave region buffer
    void reallocateWaveRegionBuffer()
    {
        qDeleteAll(waveBuffers);
        return;

        if (waveSegments.isEmpty() || displayWaveSeconds == 0)
        {
            return;
        }

        foreach(WaveformDataSegment *seg, waveSegments)
        {
            waveBuffers.append(new WaveRegionBuffer(seg->sampleRate * displayWaveSeconds));
        }
        bufferEmpty = true;
    }

    // fill data into the buffer
    void fillWaveBuffer(WaveRegionBuffer *buf, WaveformDataSegment *seg, int curStartIndex, int lastStartIndex)
    {
        WaveDataType *waveData = seg->waveData;
        if (bufferEmpty || qAbs(curStartIndex - lastStartIndex) >= buf->pointNum)
        {
            int j = 0;
            for (j = 0; j < buf->pointNum && curStartIndex < seg->waveNum; j++)
            {
                int data = waveData[curStartIndex];
                bool invalid = data & 0x40000000;
                if (invalid)
                {
                    buf->pointBuffer[j] = QPoint(indexToX(j), INVALID_AXIS_VALUE);
                }
                else
                {
                    qint16 value = data & 0xFFFF;
                    buf->pointBuffer[j] = QPoint(indexToX(j),
                                                 valueToY(value));
                }
                curStartIndex++;
            }

            // fill the left space of the buffer with invalid value
            while (j < buf->pointNum)
            {
                buf->pointBuffer[j] = QPoint(indexToX(j), INVALID_AXIS_VALUE);
                j++;
            }

            buf->waveStartPos = 0;
        }
        else
        {
            if (lastStartIndex < curStartIndex)
            {
                // insert points to the wave region from the end
                int insertPointsNum = curStartIndex - lastStartIndex;
                int newDataIndex = curStartIndex + buf->pointNum - insertPointsNum;

                while (insertPointsNum)
                {
                    int index = buf->pointNum -  insertPointsNum;
                    if (newDataIndex >= seg->waveNum)
                    {
                        // no data to fill, fill invalid
                        buf->insertPoint(QPoint(indexToX(index), INVALID_AXIS_VALUE), false);
                        insertPointsNum--;
                        continue;
                    }

                    int data = waveData[newDataIndex++];
                    bool invalid = data & 0x40000000;
                    if (invalid)
                    {
                        buf->insertPoint(QPoint(indexToX(index), INVALID_AXIS_VALUE), false);
                    }
                    else
                    {
                        qint16 value = data & 0xFFFF;
                        buf->insertPoint(QPoint(indexToX(index), valueToY(value)), false);
                    }
                    insertPointsNum--;
                }
            }
            else if (lastStartIndex > curStartIndex)
            {
                // insert points to the wave region from the begin
                int insertPointsNum = lastStartIndex - curStartIndex;
                int newDataIndex = lastStartIndex - 1;
                while (insertPointsNum)
                {
                    int index = insertPointsNum - 1;
                    if (newDataIndex < 0)
                    {
                        // no data to fill, fill invalid
                        buf->insertPoint(QPoint(indexToX(index), INVALID_AXIS_VALUE), true);
                        insertPointsNum--;
                        continue;
                    }
                    int data = waveData[newDataIndex--];
                    bool invalid = data & 0x40000000;
                    if (invalid)
                    {
                        buf->insertPoint(QPoint(indexToX(index), INVALID_AXIS_VALUE), true);
                    }
                    else
                    {
                        qint16 value = data & 0xFFFF;
                        buf->insertPoint(QPoint(indexToX(index), valueToY(value)), false);
                    }
                    insertPointsNum--;
                }
            }
        }
    }

    int indexToX(int index) const
    {
        // TODO
        return index;
    }

    int valueToY(int height) const
    {
        // TODO
        return height;
    }

    QVector<WaveRegionBuffer *> waveBuffers;
    QVector<WaveformDataSegment *> waveSegments;
    EventWaveWidget::SweepSpeed speed;
    ECGEventGain gain;
    float pixelWPicth;      // horizontal gap between two pixel, in unit of mm
    float pixelHPitch;      // veritical gap between two pixel, in unit of mm
    int displayWaveSeconds; // seconds of waveform to display
    bool bufferEmpty;        // buffer is empty, need to fill data
    int totalWaveDuration;    // the duration of the wave segments
    int currentWaveStartSecond;
    int currentWaveMedSecond;
    int durationBefore;
    int durationAfter;

    float startX;
    float endX;
    float waveRagWidth;
};

EventWaveWidget::EventWaveWidget(QWidget *parent)
    : QWidget(parent),
      d_ptr(new EventWaveWidgetPrivate())
{
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Window, Qt::black);
    this->setPalette(pal);
    setAutoFillBackground(true);

    initXCoordinate();
}

EventWaveWidget::~EventWaveWidget()
{
}

void EventWaveWidget::initXCoordinate()
{
    float oneSecondLength = d_ptr->pixLengthOfOneSecondWaveform();
    d_ptr->waveRagWidth = oneSecondLength * d_ptr->displayWaveSeconds;
    d_ptr->startX = (width() - d_ptr->waveRagWidth) / 3;
    d_ptr->endX = (width() + 2 * d_ptr->waveRagWidth) / 3;
}

int EventWaveWidget::getCurrentWaveMedSecond() const
{
    return d_ptr->currentWaveMedSecond;
}

void EventWaveWidget::setWaveMedSecond(int second)
{
    if (d_ptr->currentWaveMedSecond == second)
    {
        return;
    }
    d_ptr->currentWaveMedSecond = second;
    update();
}

int EventWaveWidget::getCurrentWaveStartSecond() const
{
    return d_ptr->currentWaveStartSecond;
}

void EventWaveWidget::setWaveStartSecond(int second)
{
    if (d_ptr->totalWaveDuration == 0 || second > d_ptr->totalWaveDuration - d_ptr->displayWaveSeconds
            || d_ptr->currentWaveStartSecond == second)
    {
        return;
    }


    d_ptr->currentWaveStartSecond = second;

    update();
}

void EventWaveWidget::setSweepSpeed(EventWaveWidget::SweepSpeed speed)
{
    if (d_ptr->speed == speed)
    {
        return;
    }

    d_ptr->speed = speed;
//    int displayWaveSeconds = width() / d_ptr->pixLengthOfOneSecondWaveform();

//    if(displayWaveSeconds % 2)
//    {
//        //must be a event number
//        displayWaveSeconds -= 1;
//    }
//    d_ptr->updateDisplayWaveformDuration(displayWaveSeconds);
    d_ptr->currentWaveMedSecond = 0;
    initXCoordinate();
    update();
}

EventWaveWidget::SweepSpeed EventWaveWidget::getSweepSpeed()
{
    return d_ptr->speed;
}

void EventWaveWidget::setWaveSegments(const QVector<WaveformDataSegment *> waveSegemnts)
{
    d_ptr->waveSegments = waveSegemnts;
    if (waveSegemnts.count())
    {
        d_ptr->totalWaveDuration =  waveSegemnts.at(0)->waveNum / waveSegemnts.at(0)->sampleRate;
    }
    else
    {
        d_ptr->totalWaveDuration = 0;
    }
    d_ptr->reallocateWaveRegionBuffer();
    update();
}

void EventWaveWidget::setInfoSegments(EventInfoSegment *info)
{
    d_ptr->durationAfter = info->duration_after;
    d_ptr->durationBefore = info->duration_before;
}

void EventWaveWidget::setGain(ECGEventGain gain)
{
    d_ptr->gain = gain;
    update();
}

void EventWaveWidget::paintEvent(QPaintEvent *ev)
{
    int count = d_ptr->waveSegments.count();
    if (!count)
    {
        return;
    }

    QWidget::paintEvent(ev);
    QPainter painter(this);
    painter.setPen(QPen(Qt::gray));
    painter.drawLine(rect().topRight(), rect().bottomRight());
    painter.setPen(QPen(Qt::white));
    int fontSize = fontManager.getFontSize(4);
    QFont font = fontManager.textFont(fontSize);
    painter.setFont(font);

    // 标尺刻度
    int coordinateY = (height() + WAVE_REG_HIGH) / 2;
    painter.drawLine(d_ptr->startX, coordinateY, d_ptr->endX, coordinateY);
    painter.drawLine(d_ptr->endX, coordinateY, d_ptr->endX, coordinateY - 5);
    int endSecond;
    int totalSecond;
    switch (d_ptr->speed)
    {
    case SWEEP_SPEED_62_5:
        totalSecond = 16;
        break;
    case SWEEP_SPEED_125:
        totalSecond = 8;
        break;
    case SWEEP_SPEED_250:
        totalSecond = 4;
        break;
    case SWEEP_SPEED_500:
        totalSecond = 2;
        break;
    default:
        break;
    }
    for (int i = 0; i <= totalSecond; i ++)
    {
        painter.drawLine(d_ptr->startX + d_ptr->waveRagWidth / totalSecond * i, coordinateY,
                         d_ptr->startX + d_ptr->waveRagWidth / totalSecond * i, coordinateY - 5);
    }
    d_ptr->currentWaveStartSecond = d_ptr->currentWaveMedSecond - totalSecond / 2;
    endSecond = d_ptr->currentWaveMedSecond + totalSecond / 2;

    QString scaleStr = QString::number(d_ptr->currentWaveStartSecond) + "s";
    painter.drawText(d_ptr->startX - 10, coordinateY + 20, scaleStr);
    scaleStr = QString::number(d_ptr->currentWaveMedSecond) + "s";
    painter.drawText(d_ptr->startX + d_ptr->waveRagWidth / 2 - 10, coordinateY + 20, scaleStr);
    scaleStr = QString::number(endSecond) + "s";
    painter.drawText(d_ptr->startX + d_ptr->waveRagWidth - 10, coordinateY + 20, scaleStr);

    for (int i = 0; i < count; i ++)
    {
        _drawWave(i, painter);
    }
}

void EventWaveWidget::resizeEvent(QResizeEvent *ev)
{
    QWidget::resizeEvent(ev);
    int waveLengthOneSecond = d_ptr->pixLengthOfOneSecondWaveform();
    int displayWaveSeconds = ev->size().width() / waveLengthOneSecond;

    if (displayWaveSeconds % 2)
    {
        // must be a event number
        displayWaveSeconds -= 1;
    }
    d_ptr->updateDisplayWaveformDuration(displayWaveSeconds);
}

/**
 * @brief drawDottedLine draw a dotted line
 * @param painter paitner
 * @param x1 xPos of point 1
 * @param y1 yPos of point 1
 * @param x2 xPos of point 2
 * @param y2 yPos of point 2
 * @param dashOffset dash pattern offset
 */
static void drawDottedLine(QPainter *painter, qreal x1, qreal y1,
                           qreal x2, qreal y2, qreal dashOffset = 0.0)
{
    painter->save();
    QVector<qreal> darsh;
    darsh << DASH_LENGTH << DASH_LENGTH;
    QPen pen = painter->pen();
    pen.setDashPattern(darsh);
    pen.setDashOffset(dashOffset);
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    QLineF dotLine(x1, y1, x2, y2);
    painter->drawLine(dotLine);
    painter->restore();
}

void EventWaveWidget::_drawWave(int index, QPainter &painter)
{
    WaveformDataSegment *waveData = d_ptr->waveSegments.at(index);
    WaveformDesc waveDesc;
    waveDesc.reset();
    QColor color;
    qreal x1 = 0, y1 = 0, x2 = 0, y2 = 0; // 需要连接的两点。
    waveDesc.startY = index * WAVE_REG_HIGH / WAVE_NUM + WAVE_TOP_MARGIN;
    waveDesc.mediumY = waveDesc.startY + WAVE_DATA_REG_HIGH / 2;
    waveDesc.endY = waveDesc.startY + WAVE_DATA_REG_HIGH;
    waveDesc.waveID = waveData->waveID;
    ParamID paramId = paramInfo.getParamID(waveData->waveID);
    waveformCache.getRange(waveDesc.waveID, waveDesc.waveRangeMin, waveDesc.waveRangeMax);
    if (waveData->sampleRate)
    {
        switch (d_ptr->speed)
        {
        case SWEEP_SPEED_62_5:
            waveDesc.offsetX = static_cast<double>(d_ptr->waveRagWidth / 16 / waveData->sampleRate);
            break;
        case SWEEP_SPEED_125:
            waveDesc.offsetX = static_cast<double>(d_ptr->waveRagWidth / 8 / waveData->sampleRate);
            break;
        case SWEEP_SPEED_250:
            waveDesc.offsetX = static_cast<double>(d_ptr->waveRagWidth / 4 / waveData->sampleRate);
            break;
        case SWEEP_SPEED_500:
            waveDesc.offsetX = static_cast<double>(d_ptr->waveRagWidth / 2 / waveData->sampleRate);
            break;
        default:
            break;
        }
    }
    else
    {
        return;
    }
    color = colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID(waveDesc.waveID)));
    if (color != QColor(0, 0, 0))
    {
        painter.setPen(color);
    }
    else
    {
        painter.setPen(Qt::red);
    }
    painter.setRenderHint(QPainter::Antialiasing, true);

    _drawWaveLabel(painter, waveDesc);

    bool start = true;
    int startIndex = (d_ptr->currentWaveStartSecond + d_ptr->durationBefore) * waveData->sampleRate;
    QPainterPath path;
    bool isFirstVaildPoint = true;
    for (int i = 0 + startIndex; (x2 - d_ptr->startX) < d_ptr->waveRagWidth; i++)
    {
        qint16 wave = waveData->waveData[i];
        double waveValue = _mapWaveValue(waveDesc, wave);
        if (start)
        {
            // the first data
            y1 = waveValue;
            x1 = d_ptr->startX;
            x2 = x1 + waveDesc.offsetX;
            start = false;
        }
        // invaild data
        unsigned short flag = waveData->waveData[i]>>16;
        if (flag & INVALID_WAVE_FALG_BIT)
        {
            y1 = y2 = waveValue;
            int j = i + 1;
            flag = waveData->waveData[j]>>16;
            while (flag & INVALID_WAVE_FALG_BIT && x2 - d_ptr->startX < d_ptr->waveRagWidth)
            {
                flag = waveData->waveData[j]>>16;
                x2 += waveDesc.offsetX;
                j++;
            }
            i = j - 1;
            drawDottedLine(&painter, x1, y1, x2, y2);
            isFirstVaildPoint = true;
        }

        // vaild data
        else
        {
            if (isFirstVaildPoint)
            {
                path.moveTo(x1, y1);
                isFirstVaildPoint = false;
            }
            else
            {
                y2 = waveValue;
                path.lineTo(x2, y2);
            }
        }
        x1 = x2;
        x2 += waveDesc.offsetX;
        y1 = y2;

        if (flag & ECG_INTERNAL_FLAG_BIT && paramId == PARAM_ECG)
        {
            QPen pen = painter.pen();
            painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
            painter.drawLine(x2, waveDesc.mediumY - 10 / d_ptr->pixelHPitch / 2,
                              x2, waveDesc.mediumY + 10 / d_ptr->pixelHPitch / 2);
            painter.setPen(pen);
        }
    }
    if (waveDesc.isECG)
    {
        // 绘画ECG增益
        QPen pen = painter.pen();
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
        QRect rulerRect(d_ptr->startX + 25, waveDesc.mediumY, 100, 50);
        switch (d_ptr->gain)
        {
        case ECG_EVENT_GAIN_X0125:
            painter.drawLine(d_ptr->startX + 20, waveDesc.mediumY - 10 / d_ptr->pixelHPitch / 16,
                             d_ptr->startX + 20, waveDesc.mediumY + 10 / d_ptr->pixelHPitch / 16);
            painter.drawText(rulerRect, "1mV");
            break;
        case ECG_EVENT_GAIN_X025:
            painter.drawLine(d_ptr->startX + 20, waveDesc.mediumY - 10 / d_ptr->pixelHPitch / 8,
                             d_ptr->startX + 20, waveDesc.mediumY + 10 / d_ptr->pixelHPitch / 8);
            painter.drawText(rulerRect, "1mV");
            break;
        case ECG_EVENT_GAIN_X05:
            painter.drawLine(d_ptr->startX + 20, waveDesc.mediumY - 10 / d_ptr->pixelHPitch / 4,
                             d_ptr->startX + 20, waveDesc.mediumY + 10 / d_ptr->pixelHPitch / 4);
            painter.drawText(rulerRect, "1mV");
            break;
        case ECG_EVENT_GAIN_X10:
            painter.drawLine(d_ptr->startX + 20, waveDesc.mediumY - 10 / d_ptr->pixelHPitch / 4,
                             d_ptr->startX + 20, waveDesc.mediumY + 10 / d_ptr->pixelHPitch / 4);
            painter.drawText(rulerRect, "1mV");
            break;
        case ECG_EVENT_GAIN_X20:
            painter.drawLine(d_ptr->startX + 20, waveDesc.mediumY - 10 / d_ptr->pixelHPitch / 4,
                             d_ptr->startX + 20, waveDesc.mediumY + 10 / d_ptr->pixelHPitch / 4);
            painter.drawText(rulerRect, "0.5mV");
            break;
        case ECG_EVENT_GAIN_X40:
            painter.drawLine(d_ptr->startX + 20, waveDesc.mediumY - 10 / d_ptr->pixelHPitch / 4,
                             d_ptr->startX + 20, waveDesc.mediumY + 10 / d_ptr->pixelHPitch / 4);
            painter.drawText(rulerRect, "0.25mV");
            break;
        default:
            break;
        }
    }
    if (!path.isEmpty())
    {
        painter.drawPath(path);
    }
}

void EventWaveWidget::_drawWaveLabel(QPainter &painter, const WaveformDesc &waveDesc)
{
    if (waveDesc.waveID == WAVE_NONE)
    {
        return;
    }

    QString title = trs(paramInfo.getParamWaveformName(waveDesc.waveID));
    QRect rectLabel(0, waveDesc.startY, 100, 100);
    painter.drawText(rectLabel, Qt::AlignCenter, title);
}

double EventWaveWidget::_mapWaveValue(WaveformDesc &waveDesc, int wave)
{
    if (waveDesc.waveID == WAVE_NONE)
    {
        return 0;
    }

    int max = waveDesc.waveRangeMax;
    int min = waveDesc.waveRangeMin;
    double startY = waveDesc.startY;
    double endY = waveDesc.endY;
    double dpos = 0;

    switch (waveDesc.waveID)
    {
    case WAVE_ECG_I:
    case WAVE_ECG_II:
    case WAVE_ECG_III:
    case WAVE_ECG_aVR:
    case WAVE_ECG_aVL:
    case WAVE_ECG_aVF:
    case WAVE_ECG_V1:
    case WAVE_ECG_V2:
    case WAVE_ECG_V3:
    case WAVE_ECG_V4:
    case WAVE_ECG_V5:
    case WAVE_ECG_V6:
    {
        double scaleData = 0;
        switch (d_ptr->gain)
        {
        case ECG_EVENT_GAIN_X0125:
            scaleData = 0.125 * 10 * (1 / d_ptr->pixelHPitch) / 2.0;
            break;
        case ECG_EVENT_GAIN_X025:
            scaleData = 0.25 * 10 * (1 / d_ptr->pixelHPitch) / 2.0;
            break;
        case ECG_EVENT_GAIN_X05:
            scaleData = 0.5 * 10 * (1 / d_ptr->pixelHPitch) / 2.0;
            break;
        case ECG_EVENT_GAIN_X10:
            scaleData = 1 * 10 * (1 / d_ptr->pixelHPitch) / 2.0;
            break;
        case ECG_EVENT_GAIN_X20:
            scaleData = 2 * 10 * (1 / d_ptr->pixelHPitch) / 2.0;
            break;
        case ECG_EVENT_GAIN_X40:
            scaleData = 4 * 10 * (1 / d_ptr->pixelHPitch) / 2.0;
            break;
        default:
            break;
        }
        endY = waveDesc.mediumY + scaleData;
        startY = waveDesc.mediumY - scaleData;
        dpos = (max - wave) * ((endY - startY) / (max - min)) + startY;
        waveDesc.isECG = true;
        break;
    }
    default:
        dpos = (max - wave) * ((endY - startY) / (max - min)) + startY;
        break;
    }

    if (dpos < waveDesc.startY)
    {
        dpos = waveDesc.startY;
    }
    else if (dpos > waveDesc.endY)
    {
        dpos = waveDesc.endY;
    }
    return dpos;
}
