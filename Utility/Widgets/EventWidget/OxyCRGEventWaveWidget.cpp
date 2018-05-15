#include "OxyCRGEventWaveWidget.h"
#include "FontManager.h"
#include "WaveformCache.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include <QPainter>

#define WAVEFORM_SCALE_HIGH         30
#define SCALELINE_NUM               5
#define DISPLAY_TIME_RANGE          120

class OxyCRGEventWaveWidgetPrivate
{    
public:
    OxyCRGEventWaveWidgetPrivate()
    {
        startTime = -60;
        currentWaveID = WAVE_CO2;
        cursorPos = 0;
        subParamMap.insert(SUB_PARAM_HR_PR, InvData());
        subParamMap.insert(SUB_PARAM_SPO2, InvData());
        subParamMap.insert(SUB_PARAM_RR_BR, InvData());
        isRR = true;
        hrDataBuf = NULL;
        spo2DataBuf = NULL;
        rrDataBuf = NULL;
    }
    ~OxyCRGEventWaveWidgetPrivate(){}

    QVector<WaveformDataSegment *> waveSegments;
    QVector<TrendDataSegment *> trendSegments;
    QMap<SubParamID, int> subParamMap;

    float startTime;
    float startX;
    float endX;
    float waveRegWidth;             // 波形区域宽度
    WaveformID currentWaveID;       // 当前波形ID
    float offsetSecond;             // 每秒的偏移量
    float cursorPos;                // 游标索引位置
    float singleParamHigh;          // 单个参数行所占高度
    bool isRR;
    int *hrDataBuf;                // 趋势数据缓存
    int *spo2DataBuf;
    int *rrDataBuf;

};

OxyCRGEventWaveWidget::OxyCRGEventWaveWidget()
    :d_ptr(new OxyCRGEventWaveWidgetPrivate())
{
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Window, Qt::black);
    this->setPalette(pal);
    setAutoFillBackground(true);
}

OxyCRGEventWaveWidget::~OxyCRGEventWaveWidget()
{

}

void OxyCRGEventWaveWidget::setWaveTrendSegments(const QVector<WaveformDataSegment *> waveSegments, const QVector<TrendDataSegment *> trendSegments)
{
    d_ptr->waveSegments = waveSegments;
    d_ptr->trendSegments = trendSegments;
    _loadTrendData();
    d_ptr->cursorPos = 0;
    update();
}

void OxyCRGEventWaveWidget::leftMoveCursor()
{
    if (d_ptr->cursorPos <= -DISPLAY_TIME_RANGE / 2)
    {
        return;
    }
    d_ptr->cursorPos --;
    update();
}

void OxyCRGEventWaveWidget::rightMoveCursor()
{
    if (d_ptr->cursorPos >= DISPLAY_TIME_RANGE / 2)
    {
        return;
    }
    d_ptr->cursorPos ++;
    update();
}

void OxyCRGEventWaveWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);


    d_ptr->waveRegWidth = width()/4*3;
    d_ptr->startX = d_ptr->waveRegWidth / 10;
    d_ptr->endX = d_ptr->waveRegWidth - d_ptr->startX;
    d_ptr->offsetSecond = (d_ptr->endX - d_ptr->startX) / 120;
    d_ptr->singleParamHigh = (height() - WAVEFORM_SCALE_HIGH)/3;

    QPainter painter(this);
    painter.setPen(QPen(Qt::gray));
    int fonSize = fontManager.getFontSize(4);
    QFont font = fontManager.textFont(fonSize);
    painter.setFont(font);

    // 网格线
    painter.drawLine(0, d_ptr->singleParamHigh, width(), d_ptr->singleParamHigh);
    painter.drawLine(0, d_ptr->singleParamHigh * 2, width(), d_ptr->singleParamHigh * 2);
    painter.drawLine(0, height() - WAVEFORM_SCALE_HIGH, width(), height() - WAVEFORM_SCALE_HIGH);
    painter.drawLine(width()/4*3, 0, width()/4*3, d_ptr->singleParamHigh * 3);

    // 标尺刻度
    float t;
    QString scaleStr;
    for (int i = 0; i < SCALELINE_NUM; i ++)
    {
        painter.drawLine(d_ptr->startX + (d_ptr->endX - d_ptr->startX) / 4 * i, height() - WAVEFORM_SCALE_HIGH,
                         d_ptr->startX + (d_ptr->endX - d_ptr->startX) / 4 * i, height() - WAVEFORM_SCALE_HIGH - 5);
        t = d_ptr->startTime + i * 30;
        if (qAbs(t / 60) < 1)
        {
            scaleStr = QString::number(t) + "s";
        }
        else
        {
            scaleStr = QString::number(t / 60) + "min";
        }
        painter.drawText(d_ptr->startX + (d_ptr->endX - d_ptr->startX) / 4 * i - 10, height() - WAVEFORM_SCALE_HIGH + 20, scaleStr);\
    }

    // 游标线
    painter.setPen(QPen(Qt::white));
    painter.drawLine(d_ptr->waveRegWidth / 2 + d_ptr->cursorPos * d_ptr->offsetSecond, 0, d_ptr->waveRegWidth / 2 + d_ptr->cursorPos * d_ptr->offsetSecond, height() - WAVEFORM_SCALE_HIGH);

    _drawTrend(painter);
    _drawWave(painter);
}

void OxyCRGEventWaveWidget::_drawTrend(QPainter &painter)
{
    TrendDataSegment *trendSegment = d_ptr->trendSegments.at(DISPLAY_TIME_RANGE + d_ptr->cursorPos);
    TrendValueSegment value;
    for (int i = 0; i < trendSegment->trendValueNum; i ++)
    {
        value = trendSegment->values[i];
        QMap<SubParamID, int>::iterator iterator = d_ptr->subParamMap.find((SubParamID)value.subParamId);
        if(iterator != d_ptr->subParamMap.end())
        {
            d_ptr->subParamMap[(SubParamID)value.subParamId] = value.value;
        }
    }

    float trendHigh = 0;
    QColor color;
    for (QMap<SubParamID, int>::iterator it = d_ptr->subParamMap.begin(); it != d_ptr->subParamMap.end(); it ++)
    {
        qreal x1 = 0, y1 = 0, x2 = 0, y2 = 0;
        WaveformDesc waveDesc;
        QFont font;
        font.setPixelSize(15);
        painter.setFont(font);
        SubParamID subId = it.key();
        color = colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID(subId)));
        if (color != QColor(0,0,0))
        {
            painter.setPen(color);
        }
        else
        {
            painter.setPen(Qt::red);
        }

        if (subId != SUB_PARAM_RR_BR)
        {
            waveDesc.startY = trendHigh + d_ptr->singleParamHigh / 6;
            waveDesc.endY = trendHigh + d_ptr->singleParamHigh / 6 * 5;
            waveDesc.offsetX = (double)d_ptr->waveRegWidth / 4 / 30;
            bool start = true;
            int startIndex = d_ptr->startTime + 120 + 13;
            if (subId == SUB_PARAM_HR_PR)
            {
                waveDesc.waveRangeMax = 160;
                waveDesc.waveRangeMin = 0;
                for (int i = startIndex; (x2 - d_ptr->startX) < (d_ptr->endX - d_ptr->startX); i ++)
                {
                    int trend = d_ptr->hrDataBuf[i];
                    double trendValue = _mapWaveValue(waveDesc, trend);
                    if (start)
                    {
                        y1 = trendValue;
                        x1 = d_ptr->startX;
                        x2 = x1 + waveDesc.offsetX;
                        start = false;
                    }
                    y2 = trendValue;
                    QLineF line(x1, y1, x2, y2);
                    painter.drawLine(line);

                    x1 = x2;
                    x2 += waveDesc.offsetX;
                    y1 = y2;
                }
            }
            else if (subId == SUB_PARAM_SPO2)
            {
                waveDesc.waveRangeMax = 100;
                waveDesc.waveRangeMin = 92;
                for (int i = startIndex; (x2 - d_ptr->startX) < (d_ptr->endX - d_ptr->startX); i ++)
                {
                    int trend = d_ptr->spo2DataBuf[i];
                    double trendValue = _mapWaveValue(waveDesc, trend);
                    if (start)
                    {
                        y1 = trendValue;
                        x1 = d_ptr->startX;
                        x2 = x1 + waveDesc.offsetX;
                        start = false;
                    }
                    y2 = trendValue;
                    QLineF line(x1, y1, x2, y2);
                    painter.drawLine(line);

                    x1 = x2;
                    x2 += waveDesc.offsetX;
                    y1 = y2;
                }
            }
            painter.drawLine(d_ptr->startX / 4, trendHigh + d_ptr->singleParamHigh / 6, d_ptr->startX /4, trendHigh + d_ptr->singleParamHigh / 6 * 5);
            painter.drawLine(d_ptr->startX / 4, trendHigh + d_ptr->singleParamHigh / 6, d_ptr->startX /4 + 5, trendHigh + d_ptr->singleParamHigh / 6);
            painter.drawLine(d_ptr->startX / 4, trendHigh + d_ptr->singleParamHigh / 6 * 5, d_ptr->startX /4 + 5, trendHigh + d_ptr->singleParamHigh / 6 * 5);

            QRect upRulerRect(d_ptr->startX /4 + 7, trendHigh + d_ptr->singleParamHigh / 6 - 10, 50, 30);
            painter.drawText(upRulerRect, Qt::AlignLeft | Qt::AlignTop, QString::number(waveDesc.waveRangeMax));

            QRect downRulerRect(d_ptr->startX /4 + 7, trendHigh + d_ptr->singleParamHigh / 6 * 5 - 10, 50, 30);
            painter.drawText(downRulerRect, Qt::AlignLeft | Qt::AlignTop, QString::number(waveDesc.waveRangeMin));


        }
        else if (d_ptr->isRR)
        {
            waveDesc.startY = d_ptr->singleParamHigh / 6;
            waveDesc.endY = d_ptr->singleParamHigh / 6 * 5;
            waveDesc.offsetX = (double)d_ptr->waveRegWidth / 4 / 30;
            waveDesc.waveRangeMax = 40;
            waveDesc.waveRangeMin = 0;

            painter.drawLine(d_ptr->waveRegWidth - (d_ptr->startX / 4), d_ptr->singleParamHigh / 6, d_ptr->waveRegWidth - (d_ptr->startX / 4), d_ptr->singleParamHigh / 6 * 5);
            painter.drawLine(d_ptr->waveRegWidth - (d_ptr->startX / 4), d_ptr->singleParamHigh / 6, d_ptr->waveRegWidth - (d_ptr->startX / 4) - 5, d_ptr->singleParamHigh / 6);
            painter.drawLine(d_ptr->waveRegWidth - (d_ptr->startX / 4), d_ptr->singleParamHigh / 6 * 5, d_ptr->waveRegWidth - (d_ptr->startX / 4) - 5, d_ptr->singleParamHigh / 6 * 5);

            QRect upRulerRect(d_ptr->waveRegWidth - (d_ptr->startX / 4) - 57, d_ptr->singleParamHigh / 6 - 10, 50, 30);
            painter.drawText(upRulerRect, Qt::AlignRight | Qt::AlignTop, QString::number(waveDesc.waveRangeMax));

            QRect downRulerRect(d_ptr->waveRegWidth - (d_ptr->startX / 4) - 57, d_ptr->singleParamHigh / 6 * 5 - 10, 50, 30);
            painter.drawText(downRulerRect, Qt::AlignRight | Qt::AlignTop, QString::number(waveDesc.waveRangeMin));
        }

        QRect trendRect(d_ptr->waveRegWidth + 5, trendHigh + 5, width()/4 - 10, 30);
        painter.drawText(trendRect, Qt::AlignLeft | Qt::AlignTop, paramInfo.getSubParamName(subId));
        painter.drawText(trendRect, Qt::AlignRight | Qt::AlignTop, Unit::getSymbol(paramInfo.getUnitOfSubParam(subId)));

        font.setPixelSize(60);
        painter.setFont(font);
        QRect valueRect(d_ptr->waveRegWidth + width()/4/3, trendHigh + 35, width()/3, 50);
        int value = it.value();
        if (value == InvData())
        {
            painter.drawText(valueRect,Qt::AlignVCenter | Qt::AlignTop, "-.-");
        }
        else
        {
            painter.drawText(valueRect,Qt::AlignVCenter | Qt::AlignTop, QString::number(it.value()));
        }
        trendHigh += d_ptr->singleParamHigh;
    }
}

void OxyCRGEventWaveWidget::_drawWave(QPainter &painter)
{
    WaveformDataSegment *waveData = NULL;
    if (!d_ptr->waveSegments.count())
    {
        return;
    }
    for (int i = 0; i < d_ptr->waveSegments.count(); i ++)
    {
        waveData = d_ptr->waveSegments.at(i);
        if (waveData->waveID == d_ptr->currentWaveID)
        {
            break;
        }
    }
    WaveformDesc waveDesc;
    QColor color;
    qreal x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    waveDesc.startY = d_ptr->singleParamHigh * 2 + 10;
    waveDesc.endY = (height() - WAVEFORM_SCALE_HIGH) - 10;
    waveDesc.waveID = waveData->waveID;
    waveformCache.getRange(waveDesc.waveID, waveDesc.waveRangeMin, waveDesc.waveRangeMax);
    if (waveData->sampleRate)
    {
        waveDesc.offsetX = (double)d_ptr->waveRegWidth / 4 / 30 / waveData->sampleRate;
    }
    else
    {
        return;
    }
    color = colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID(waveDesc.waveID)));
    if (color != QColor(0,0,0))
    {
        painter.setPen(color);
    }
    else
    {
        painter.setPen(Qt::red);
    }

    bool start = true;
    int startIndex = (d_ptr->startTime + 120) * waveData->sampleRate;
    for (int i = startIndex; (x2 - d_ptr->startX) < (d_ptr->endX - d_ptr->startX); i ++)
    {
        short wave = waveData->waveData[i];
        double waveValue = _mapWaveValue(waveDesc, wave);
        if (start)
        {
            y1 = waveValue;
            x1 = d_ptr->startX;
            x2 = x1 + waveDesc.offsetX;
            start = false;
        }
        y2 = waveValue;
        QLineF line(x1, y1, x2, y2);
        painter.drawLine(line);

        x1 = x2;
        x2 += waveDesc.offsetX;
        y1 = y2;
    }
    painter.setPen(QPen(color, 1,Qt::DotLine));
    painter.drawLine(d_ptr->startX, waveDesc.startY, d_ptr->endX, waveDesc.startY);
    painter.drawLine(d_ptr->startX, waveDesc.endY, d_ptr->endX, waveDesc.endY);
}

double OxyCRGEventWaveWidget::_mapWaveValue(WaveformDesc &waveDesc, int wave)
{
//    if (waveDesc.waveID == WAVE_NONE)
//    {
//        return 0;
//    }

    int max = waveDesc.waveRangeMax;
    int min = waveDesc.waveRangeMin;
    double startY = waveDesc.startY;
    double endY = waveDesc.endY;
    double dpos = 0;

//    switch (waveDesc.waveID)
//    {
//    case WAVE_CO2:
//    case WAVE_RESP:
        dpos = (max - wave) * ((endY - startY) / (max - min)) + startY;
//        break;
//    default:
//        return 0;
//    }

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

void OxyCRGEventWaveWidget::_loadTrendData()
{
    TrendDataSegment *trendSegment;
    d_ptr->hrDataBuf = new int[d_ptr->trendSegments.count()];
    d_ptr->spo2DataBuf = new int[d_ptr->trendSegments.count()];
    d_ptr->rrDataBuf = new int[d_ptr->trendSegments.count()];
    for (int i = 0; i < d_ptr->trendSegments.count(); i ++)
    {
        trendSegment = d_ptr->trendSegments.at(i);
        for (int j = 0; j < trendSegment->trendValueNum; j ++)
        {
            TrendValueSegment valueSegment = trendSegment->values[j];
            switch ((SubParamID)valueSegment.subParamId)
            {
            case SUB_PARAM_HR_PR:
                d_ptr->hrDataBuf[i] = valueSegment.value;
                break;
            case SUB_PARAM_SPO2:
                d_ptr->spo2DataBuf[i] = valueSegment.value;
                break;
            case SUB_PARAM_RR_BR:
                d_ptr->rrDataBuf[i] = valueSegment.value;
                break;
            default:
                break;
            }
        }
    }
}