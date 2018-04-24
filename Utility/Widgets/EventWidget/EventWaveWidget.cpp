#include "EventWaveWidget.h"
#include <QPoint>
#include <QVector>
#include "SystemManager.h"
#include <QResizeEvent>
#include "FontManager.h"
#include <QPainter>

#define INVALID_AXIS_VALUE ((1<<30) - 1)

struct WaveRegionBuffer
{
    WaveRegionBuffer(int pointNum)
        : waveStartPos(0),
          pointNum(pointNum)
    {
        pointBuffer = new QPoint[pointNum];
    }

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
        if(frombegin)
        {
            //insert point from the begin
            if(waveStartPos == 0)
            {
                waveStartPos = pointNum;
            }
            pointBuffer[--waveStartPos] = p;
        }
        else
        {
            //inset poin form the end
            pointBuffer[waveStartPos++] = p;
            if(waveStartPos == pointNum)
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
        :speed(EventWaveWidget::SWEEP_SPEED_250),
          displayWaveSeconds(0),
          bufferEmpty(false),
          totalWaveDuration(0),
          currentWaveStartSecond(0)
    {
        pixelWPicth = systemManager.getScreenPixelWPitch();
        pixelHPitch = systemManager.getScreenPixelHPitch();
    }

    ~EventWaveWidgetPrivate()
    {
        qDeleteAll(waveBuffers);
    }

    //calculate the pixel lenght of one second waveform
    int pixLengthOfOneSecondWaveform()
    {
        float lengthMM;
        switch(speed)
        {
        case EventWaveWidget::SWEEP_SPEED_125:
            lengthMM = 12.5;
            break;

        case EventWaveWidget::SWEEP_SPEED_250:
        default:
            lengthMM = 25.0;
            break;
        }

        return lengthMM / pixelWPicth;
    }


    //update the display waveform duration
    void updateDisplayWaveformDuration(int seconds)
    {
        if(displayWaveSeconds == seconds )
        {
            return;
        }

        displayWaveSeconds = seconds;

        reallocateWaveRegionBuffer();
    }

    //reallocate the wave region buffer
    void reallocateWaveRegionBuffer()
    {
        qDeleteAll(waveBuffers);
        return;

        if(waveSegments.isEmpty() || displayWaveSeconds == 0)
        {
            return;
        }

        foreach(WaveformDataSegment *seg, waveSegments)
        {
            waveBuffers.append(new WaveRegionBuffer(seg->sampleRate * displayWaveSeconds));
        }
        bufferEmpty = true;
    }

    //fill data into the buffer
    void fillWaveBuffer(WaveRegionBuffer *buf, WaveformDataSegment *seg, int curStartIndex, int lastStartIndex)
    {
        WaveDataType *waveData = seg->waveData;
        if(bufferEmpty || qAbs(curStartIndex - lastStartIndex) >= buf->pointNum)
        {
            int j = 0;
            for(j =0; j < buf->pointNum && curStartIndex < seg->waveNum; j++)
            {
                int data = waveData[curStartIndex];
                bool invalid = data & 0x40000000;
                if(invalid)
                {
                    buf->pointBuffer[j] = QPoint(indexToX(j), INVALID_AXIS_VALUE);
                }
                else
                {
                    short value = data &0xFFFF;
                    buf->pointBuffer[j] = QPoint(indexToX(j),
                                                 valueToY(value));

                }
                curStartIndex ++;
            }

            // fill the left space of the buffer with invalid value
            while(j < buf->pointNum)
            {
                buf->pointBuffer[j] = QPoint(indexToX(j), INVALID_AXIS_VALUE);
                j++;
            }

            buf->waveStartPos = 0;
        }
        else
        {
            if(lastStartIndex < curStartIndex)
            {
                //insert points to the wave region from the end
                int insertPointsNum = curStartIndex - lastStartIndex;
                int newDataIndex = curStartIndex + buf->pointNum - insertPointsNum;

                while(insertPointsNum)
                {
                    int index = buf->pointNum -  insertPointsNum;
                    if(newDataIndex >= seg->waveNum)
                    {
                        //no data to fill, fill invalid
                        buf->insertPoint(QPoint(indexToX(index), INVALID_AXIS_VALUE), false);
                        insertPointsNum --;
                        continue;
                    }

                    int data = waveData[newDataIndex++];
                    bool invalid = data & 0x40000000;
                    if(invalid)
                    {
                        buf->insertPoint(QPoint(indexToX(index), INVALID_AXIS_VALUE), false);
                    }
                    else
                    {
                        short value = data &0xFFFF;
                        buf->insertPoint(QPoint(indexToX(index), valueToY(value)), false);
                    }
                    insertPointsNum --;
                }

            }
            else if(lastStartIndex > curStartIndex)
            {
                //insert points to the wave region from the begin
                int insertPointsNum = lastStartIndex - curStartIndex;
                int newDataIndex = lastStartIndex - 1;
                while(insertPointsNum)
                {
                    int index = insertPointsNum - 1;
                    if(newDataIndex < 0)
                    {
                        //no data to fill, fill invalid
                        buf->insertPoint(QPoint(indexToX(index), INVALID_AXIS_VALUE), true);
                        insertPointsNum --;
                        continue;
                    }
                    int data = waveData[newDataIndex--];
                    bool invalid = data & 0x40000000;
                    if(invalid)
                    {
                        buf->insertPoint(QPoint(indexToX(index), INVALID_AXIS_VALUE), true);
                    }
                    else
                    {
                        short value = data &0xFFFF;
                        buf->insertPoint(QPoint(indexToX(index), valueToY(value)), false);
                    }
                    insertPointsNum --;
                }
            }
        }
    }

    int indexToX(int index) const
    {
        //TODO
        return index;
    }

    int valueToY(int height) const
    {
        //TODO
        return height;
    }

    QVector<WaveRegionBuffer *> waveBuffers;
    QVector<WaveformDataSegment *> waveSegments;
    EventWaveWidget::SweepSpeed speed;
    float pixelWPicth;      //horizontal gap between two pixel, in unit of mm
    float pixelHPitch;      //veritical gap between two pixel, in unit of mm
    int displayWaveSeconds; // seconds of waveform to display
    bool bufferEmpty;        // buffer is empty, need to fill data
    int totalWaveDuration;    // the duration of the wave segments
    int currentWaveStartSecond;
};

EventWaveWidget::EventWaveWidget(QWidget *parent)
    :QWidget(parent),
      d_ptr(new EventWaveWidgetPrivate())
{
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Window, Qt::black);
    this->setPalette(pal);
    setAutoFillBackground(true);
}

EventWaveWidget::~EventWaveWidget()
{

}

int EventWaveWidget::getCurrentWaveStartSecond() const
{
    return d_ptr->currentWaveStartSecond;
}

void EventWaveWidget::setWaveStartSecond(int second)
{
    if(d_ptr->totalWaveDuration == 0 || second > d_ptr->totalWaveDuration - d_ptr->displayWaveSeconds
    || d_ptr->currentWaveStartSecond == second)
    {
        return;
    }


    d_ptr->currentWaveStartSecond = second;

    update();
}

void EventWaveWidget::setSweepSpeed(EventWaveWidget::SweepSpeed speed)
{
    if(d_ptr->speed == speed)
    {
        return;
    }

    d_ptr->speed = speed;
    int displayWaveSeconds = width() / d_ptr->pixLengthOfOneSecondWaveform();

    if(displayWaveSeconds % 2)
    {
        //must be a event number
        displayWaveSeconds -= 1;
    }
    d_ptr->updateDisplayWaveformDuration(displayWaveSeconds);
    update();
}

void EventWaveWidget::setWaveSegments(const QVector<WaveformDataSegment *> waveSegemnts)
{
    d_ptr->waveSegments = waveSegemnts;
    if(waveSegemnts.count())
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

void EventWaveWidget::paintEvent(QPaintEvent *ev)
{
    int count = d_ptr->waveSegments.count();
    if (!count)
    {
        return;
    }

    QWidget::paintEvent(ev);
    QPainter painter(this);
    painter.setPen(QPen(Qt::white));
    int fontSize = fontManager.getFontSize(4);
    QFont font = fontManager.textFont(fontSize);
    painter.setFont(font);
//    qreal x1 = 0, y1 = 0, x2 = 0, y2 = 0;

//    for (int j = 0; j < count; ++j)
//    {
//        WaveformDataSegment *waveformData = d_ptr->waveSegments.at(j);

//        double max = 0;
//        double min = 0;
//        short wave = 0;
//        double waveData = 0.0;

//        for (int i = 0; i < waveformData->waveNum; i ++)
//        {

//        }

//    }

}

void EventWaveWidget::resizeEvent(QResizeEvent *ev)
{
    QWidget::resizeEvent(ev);
    int waveLengthOneSecond = d_ptr->pixLengthOfOneSecondWaveform();
    int displayWaveSeconds = ev->size().width() / waveLengthOneSecond;

    if(displayWaveSeconds % 2)
    {
        //must be a event number
        displayWaveSeconds -= 1;
    }
    d_ptr->updateDisplayWaveformDuration(displayWaveSeconds);
}
