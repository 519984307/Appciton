#include "OxyCRGEventWaveWidget.h"
#include "FontManager.h"
#include "WaveformCache.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "ConfigManager.h"
#include <QPainter>

#define WAVEFORM_SCALE_HIGH         30
#define SCALELINE_NUM               5
#define DISPLAY_TIME_RANGE          120
#define isEqual(a, b) (qAbs((a)-(b)) < 0.000001)

class OxyCRGEventWaveWidgetPrivate
{    
public:
    OxyCRGEventWaveWidgetPrivate()
    {
        startTime = -60;
        durationBefore = -120;
        currentWaveID = WAVE_RESP;
        cursorPos = 0;
        trendDataNum = 0;
        subParamMap.insert(SUB_PARAM_HR_PR, InvData());
        subParamMap.insert(SUB_PARAM_SPO2, InvData());
        subParamMap.insert(SUB_PARAM_RR_BR, InvData());
        isRR = false;
        hrDataBuf = NULL;
        spo2DataBuf = NULL;
        rrDataBuf = NULL;
        reset();
    }
    ~OxyCRGEventWaveWidgetPrivate(){}

    void reset()
    {
        if (hrDataBuf)
        {
            qFree(hrDataBuf);
            hrDataBuf = NULL;
        }
        if (spo2DataBuf)
        {
            qFree(spo2DataBuf);
            hrDataBuf = NULL;
        }
        if (rrDataBuf)
        {
            qFree(rrDataBuf);
            rrDataBuf = NULL;
        }
        int durationType;
        if (currentConfig.getNumValue("OxyCRG|EventStorageSetup", durationType))
        {
            if ((OxyCRGEventStorageDuration)durationType == OxyCRG_EVENT_DURATION_1_3MIN)
            {
                startTime = 0;
                durationBefore = -60;
            }
            else if ((OxyCRGEventStorageDuration)durationType == OxyCRG_EVENT_DURATION_3_1MIN)
            {
                startTime = -120;
                durationBefore = -180;
            }
        }
    }

    QVector<WaveformDataSegment *> waveSegments;
    QVector<TrendDataSegment *> trendSegments;
    int trendDataNum;
    QMap<SubParamID, int> subParamMap;

    int startTime;
    int durationBefore;
    float startX;
    float endX;
    float waveRegWidth;             // 波形区域宽度
    WaveformID currentWaveID;       // 当前波形ID
    float offsetSecond;             // 每秒的偏移量
    int cursorPos;                // 游标索引位置
    float singleParamHigh;          // 单个参数行所占高度
    bool isRR;
    int *hrDataBuf;                // 趋势数据缓存
    int *spo2DataBuf;
    int *rrDataBuf;

    // optimization
    QList<TrendGraphInfo> trendInfos;
    OxyCRGWaveInfo waveInfo;
    EventInfoSegment *eventInfo;
    TrendConvertDesc timeDesc;
    unsigned cursorTime;

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

void OxyCRGEventWaveWidget::setWaveWidgetCompressed(WaveformID id)
{
    d_ptr->currentWaveID = id;
    update();
}

void OxyCRGEventWaveWidget::setWaveWidgetTrend1(bool isRR)
{
    d_ptr->isRR = isRR;
    update();
}

void OxyCRGEventWaveWidget::loadOxyCRGEventData(const QList<TrendGraphInfo> &trendInfos, const OxyCRGWaveInfo &waveInfo, EventInfoSegment *const eventInfo)
{
    d_ptr->trendInfos = trendInfos;
    d_ptr->waveInfo = waveInfo;
    d_ptr->eventInfo = eventInfo;
    d_ptr->cursorTime = eventInfo->timestamp;
    update();
}

QMap<SubParamID, QPainterPath> OxyCRGEventWaveWidget::generatorPainterPath(const QList<TrendGraphInfo> &graphInfo, QPainter &painter, int curDataIndex)
{
    QMap<SubParamID, QPainterPath> paths;
    float trendHigh = 0;
    for (int i = 0; i < graphInfo.count(); i ++)
    {        
        TrendGraphInfo trendInfo = graphInfo.at(i);

        // 趋势数据
        QFont font;
        font.setPixelSize(20);
        painter.setFont(font);
        QColor color = colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID(trendInfo.subParamID)));
        if (color != QColor(0,0,0))
        {
            painter.setPen(color);
        }
        else
        {
            if (trendInfo.subParamID == SUB_PARAM_RR_BR)
            {
                painter.setPen(Qt::green);
            }
            else
            {
                painter.setPen(Qt::red);
            }
        }
        TrendDataType trendData;
        if (curDataIndex == InvData())
        {
            trendData = InvData();
        }
        else
        {
            trendData = trendInfo.trendData.at(curDataIndex).data;
        }
        QRect trendRect(d_ptr->waveRegWidth + 5, trendHigh + 5, width()/4 - 10, 30);
        painter.drawText(trendRect, Qt::AlignLeft | Qt::AlignTop, paramInfo.getSubParamName(trendInfo.subParamID));
        painter.drawText(trendRect, Qt::AlignRight | Qt::AlignTop, Unit::getSymbol(paramInfo.getUnitOfSubParam(trendInfo.subParamID)));

        font.setPixelSize(60);
        painter.setFont(font);
        QRect valueRect(d_ptr->waveRegWidth + width()/4/3, trendHigh + 35, width()/3, 50);
        if (trendData == InvData())
        {
            painter.drawText(valueRect,Qt::AlignVCenter | Qt::AlignTop, "-.-");
        }
        else
        {
            painter.drawText(valueRect,Qt::AlignVCenter | Qt::AlignTop, QString::number(trendData));
        }
        trendHigh += d_ptr->singleParamHigh;
        if (!d_ptr->isRR && trendInfo.subParamID == SUB_PARAM_RR_BR)
        {
            continue;
        }
        QPainterPath path;

        // 波形
        QPointF lastPoint;
        bool lastPointInvalid = true;
        TrendConvertDesc waveDesc;
        switch(trendInfo.subParamID)
        {
        case SUB_PARAM_HR_PR:
            waveDesc.start = d_ptr->singleParamHigh / 6;
            waveDesc.end = waveDesc.start + d_ptr->singleParamHigh / 3 * 2;
            waveDesc.max = 160;
            waveDesc.min = 0;
            break;
        case SUB_PARAM_SPO2:
            waveDesc.start = d_ptr->singleParamHigh / 6 + d_ptr->singleParamHigh;
            waveDesc.end = waveDesc.start + d_ptr->singleParamHigh / 3 * 2;
            waveDesc.max = 100;
            waveDesc.min = 85;
            break;
        case SUB_PARAM_RR_BR:
            waveDesc.start = d_ptr->singleParamHigh / 6;
            waveDesc.end = waveDesc.start + d_ptr->singleParamHigh / 3 * 2;
            waveDesc.max = 40;
            waveDesc.min = 0;
            break;
        default:
            break;
        }
        QVector<TrendGraphData>::ConstIterator iter = trendInfo.trendData.constBegin();
        for (; iter != trendInfo.trendData.constEnd(); iter ++)
        {
            if (iter->timestamp > (unsigned)d_ptr->timeDesc.min || iter->timestamp < (unsigned)d_ptr->timeDesc.max)
            {
                continue;
            }
            if (iter->data == InvData())
            {
                if (!lastPointInvalid)
                {
                    path.lineTo(lastPoint);
                    lastPointInvalid = false;
                }
                continue;
            }

            qreal x = _mapValue(d_ptr->timeDesc, iter->timestamp);
            qreal y = _mapValue(waveDesc, iter->data);

            if (lastPointInvalid)
            {
                path.moveTo(x, y);
                lastPointInvalid = false;
            }
            else
            {
                if (!isEqual(lastPoint.y(), y))
                {
                    path.lineTo(lastPoint);
                    path.lineTo(x,y);
                }
            }
            lastPoint.rx() = x;
            lastPoint.ry() = y;
        }

        if (!lastPointInvalid)
        {
            path.lineTo(lastPoint);
        }

        // 标尺
        int fontSize = fontManager.getFontSize(4);
        font = fontManager.textFont(fontSize);
        if (trendInfo.subParamID != SUB_PARAM_RR_BR)
        {
            path.moveTo(d_ptr->timeDesc.start / 4 + 5, waveDesc.start);
            path.lineTo(d_ptr->timeDesc.start / 4, waveDesc.start);
            path.lineTo(d_ptr->timeDesc.start / 4, waveDesc.end);
            path.lineTo(d_ptr->timeDesc.start / 4 + 5, waveDesc.end);

            path.addText(d_ptr->timeDesc.start /4 + 7, waveDesc.start, font, QString::number(waveDesc.max));
            path.addText(d_ptr->timeDesc.start /4 + 7, waveDesc.end, font, QString::number(waveDesc.min));
        }
        else
        {
            path.moveTo(d_ptr->waveRegWidth - (d_ptr->timeDesc.start / 4) - 5, waveDesc.start);
            path.lineTo(d_ptr->waveRegWidth - (d_ptr->timeDesc.start / 4), waveDesc.start);
            path.lineTo(d_ptr->waveRegWidth - (d_ptr->timeDesc.start / 4), waveDesc.end);
            path.lineTo(d_ptr->waveRegWidth - (d_ptr->timeDesc.start / 4) - 5, waveDesc.end);

            path.addText(d_ptr->waveRegWidth - (d_ptr->timeDesc.start / 4) - 30, waveDesc.start, font, QString::number(waveDesc.max));
            path.addText(d_ptr->waveRegWidth - (d_ptr->timeDesc.start / 4) - 30, waveDesc.end, font, QString::number(waveDesc.min));
        }
        paths.insert(trendInfo.subParamID, path);

    }
    return paths;
}

QPainterPath OxyCRGEventWaveWidget::generatorWaveformPath(const OxyCRGWaveInfo &waveInfo, QPainter &painter)
{
    QPainterPath wavePath;
    WaveformDesc waveDesc;
    qreal x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    waveDesc.startY = d_ptr->singleParamHigh * 2 + d_ptr->singleParamHigh / 6;
    waveDesc.endY = waveDesc.startY + d_ptr->singleParamHigh / 3 * 2;
    waveDesc.waveID = waveInfo.id;
    waveformCache.getRange(waveDesc.waveID, waveDesc.waveRangeMin, waveDesc.waveRangeMax);
    if (waveInfo.sampleRate)
    {
        waveDesc.offsetX = (double)d_ptr->waveRegWidth / 4 / 30 / waveInfo.sampleRate;
    }
    else
    {
        return wavePath;
    }

    bool start = true;
    int startIndex = (d_ptr->startTime - d_ptr->durationBefore) * waveInfo.sampleRate;
    for (int i = startIndex; (x2 - d_ptr->timeDesc.start) < (d_ptr->timeDesc.end - d_ptr->timeDesc.start); i ++)
    {
        short wave = waveInfo.waveData.at(i);
        double waveValue = _mapWaveValue(waveDesc, wave);
        if (start)
        {
            y1 = waveValue;
            x1 = d_ptr->timeDesc.start;
            x2 = x1 + waveDesc.offsetX;
            start = false;
        }
        y2 = waveValue;
        if (y1 != InvData() && y2 != InvData())
        {
            wavePath.moveTo(x1, y1);
            wavePath.lineTo(x2, y2);
        }
        else if (y1 != InvData())
        {
            wavePath.moveTo(x1, y1);
        }
        else if (y2 != InvData())
        {
            wavePath.moveTo(x2, y2);
        }

        x1 = x2;
        x2 += waveDesc.offsetX;
        y1 = y2;
    }

    QFont font;
    font.setPixelSize(20);
    wavePath.addText(d_ptr->timeDesc.start / 4, (waveDesc.startY + waveDesc.endY) / 2, font, paramInfo.getParamName(paramInfo.getParamID(waveDesc.waveID)));

    font.setPixelSize(15);
    painter.setFont(font);
    QColor color = colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID(d_ptr->waveInfo.id)));
    if (waveDesc.waveID == WAVE_CO2)
    {
        painter.setPen(QPen(color, 2, Qt::DotLine));
        painter.drawLine(d_ptr->timeDesc.start, waveDesc.startY, d_ptr->timeDesc.end, waveDesc.startY);
        painter.drawLine(d_ptr->timeDesc.start, waveDesc.endY, d_ptr->timeDesc.end, waveDesc.endY);

        QRect upRulerRect(d_ptr->timeDesc.start - 50, waveDesc.startY - 10, 50, 30);
        painter.drawText(upRulerRect, Qt::AlignRight | Qt::AlignTop, QString::number(waveDesc.waveRangeMax));

        QRect downRulerRect(d_ptr->timeDesc.start - 50, waveDesc.endY - 10, 50, 30);
        painter.drawText(downRulerRect, Qt::AlignRight | Qt::AlignTop, QString::number(waveDesc.waveRangeMin));
    }
    painter.setPen(QPen(color, 2, Qt::SolidLine));
    return wavePath;

}

void OxyCRGEventWaveWidget::leftMoveCursor()
{
//    if (d_ptr->cursorPos <= -DISPLAY_TIME_RANGE / 2)
//    {
//        return;
//    }
//    d_ptr->cursorPos --;
    if (d_ptr->cursorTime <= (unsigned)d_ptr->timeDesc.max)
    {
        return;
    }
    d_ptr->cursorTime --;
    update();
}

void OxyCRGEventWaveWidget::rightMoveCursor()
{
//    if (d_ptr->cursorPos >= DISPLAY_TIME_RANGE / 2)
//    {
//        return;
//    }
//    d_ptr->cursorPos ++;
    if (d_ptr->cursorTime >= (unsigned)d_ptr->timeDesc.min)
    {
        return;
    }
    d_ptr->cursorTime ++;
    update();
}

void OxyCRGEventWaveWidget::leftMoveCoordinate()
{
    if (d_ptr->startTime <= d_ptr->durationBefore)
    {
        return;
    }
    d_ptr->startTime -= 30;
    d_ptr->cursorTime -= 30;
    update();
}

void OxyCRGEventWaveWidget::rightMoveCoordinate()
{
    if (d_ptr->startTime >= d_ptr->durationBefore + 120)
    {
        return;
    }
    d_ptr->startTime += 30;
    d_ptr->cursorTime += 30;
    update();
}

void OxyCRGEventWaveWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);

    d_ptr->waveRegWidth = width()/4*3;
    d_ptr->timeDesc.start = d_ptr->waveRegWidth / 10;
    d_ptr->timeDesc.end = d_ptr->waveRegWidth - d_ptr->timeDesc.start;
    d_ptr->singleParamHigh = (height() - WAVEFORM_SCALE_HIGH) / 3;

    QPainter painter(this);
    painter.setPen(QPen(Qt::gray));
    int fontSize = fontManager.getFontSize(4);
    QFont font = fontManager.textFont(fontSize);
    painter.setFont(font);

    // 网络线
    painter.drawLine(0, d_ptr->singleParamHigh, width(), d_ptr->singleParamHigh);
    painter.drawLine(0, d_ptr->singleParamHigh * 2, width(), d_ptr->singleParamHigh * 2);
    painter.drawLine(0, height() - WAVEFORM_SCALE_HIGH, width(), height() - WAVEFORM_SCALE_HIGH);
    painter.drawLine(width()/4*3, 0, width()/4*3, d_ptr->singleParamHigh * 3);

    // 横坐标标尺刻度
    float t;
    QString scaleStr;
    double singleWidth = (double)((d_ptr->timeDesc.end - d_ptr->timeDesc.start) / 4.0);
    for (int i = 0; i < SCALELINE_NUM; i ++)
    {
        double scalePos = d_ptr->timeDesc.start + singleWidth * i;
        painter.drawLine(scalePos, height() - WAVEFORM_SCALE_HIGH,
                         scalePos, height() - WAVEFORM_SCALE_HIGH - 5);
        t = d_ptr->startTime + i * 30;
        if (qAbs(t / 60) < 1)
        {
            scaleStr = QString::number(t) + "s";
        }
        else
        {
            scaleStr = QString::number(t / 60) + "min";
        }
        painter.drawText(d_ptr->timeDesc.start + (d_ptr->timeDesc.end - d_ptr->timeDesc.start) / 4 * i - 10, height() - WAVEFORM_SCALE_HIGH + 20, scaleStr);

    }

    // 游标线
    painter.setPen(QPen(Qt::white));
    d_ptr->timeDesc.max = d_ptr->eventInfo->timestamp + d_ptr->startTime;
    d_ptr->timeDesc.min = d_ptr->timeDesc.max + 120;
    double cursorPos = _mapValue(d_ptr->timeDesc, d_ptr->cursorTime);
    painter.drawLine(cursorPos, 0, cursorPos, height() - WAVEFORM_SCALE_HIGH);


    // 趋势波形图
    int lowPos = 0;
    int highPos = d_ptr->trendInfos.at(0).trendData.count() - 1;
    int dataIndex = InvData();
    while(lowPos <= highPos)
    {
        int midPos = (lowPos + highPos)/2;
        unsigned t = d_ptr->trendInfos.at(0).trendData.at(midPos).timestamp;
        if (d_ptr->cursorTime < t)
        {
            highPos = midPos - 1;
        }
        else if (d_ptr->cursorTime > t)
        {
            lowPos = midPos + 1;
        }
        else
        {
            dataIndex = midPos;
            break;
        }
    }

    painter.save();
    QMap<SubParamID, QPainterPath> paths = generatorPainterPath(d_ptr->trendInfos, painter, dataIndex);
    QMap<SubParamID, QPainterPath>::ConstIterator iter;
    QColor color;
    for (iter = paths.constBegin(); iter != paths.constEnd(); iter ++)
    {
        color = colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID(iter.key())));
        if (color != QColor(0,0,0))
        {
            painter.setPen(color);
        }
        else
        {
            if (iter.key() == SUB_PARAM_RR_BR)
            {
                painter.setPen(Qt::green);
            }
            else
            {
                painter.setPen(Qt::red);
            }
        }
        painter.save();
        painter.drawPath(iter.value());
        painter.restore();
    }
    painter.restore();

    //波形图
    color = colorManager.getColor(paramInfo.getParamName(paramInfo.getParamID(d_ptr->waveInfo.id)));
    painter.setPen(color);
    QPainterPath wavePath = generatorWaveformPath(d_ptr->waveInfo, painter);
//    painter.setPen(QPen(Qt::SolidLine));
    painter.save();
    painter.drawPath(wavePath);
    painter.restore();

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
    waveDesc.startY = d_ptr->singleParamHigh * 2 + d_ptr->singleParamHigh / 6;
    waveDesc.endY = waveDesc.startY + d_ptr->singleParamHigh / 3 * 2;
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
    int startIndex = (d_ptr->startTime - d_ptr->durationBefore) * waveData->sampleRate;
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
        if (y1 != InvData() && y2 != InvData())
        {
            QLineF line(x1, y1, x2, y2);
            painter.drawLine(line);
        }
        else if (y1 != InvData())
        {
            painter.drawPoint(x1, y1);
        }
        else if (y2 != InvData())
        {
            painter.drawPoint(x2, y2);
        }

        x1 = x2;
        x2 += waveDesc.offsetX;
        y1 = y2;

    }
    QFont font;
    font.setPixelSize(20);
    painter.setFont(font);
    QRect waveName(0,waveDesc.startY, d_ptr->startX, waveDesc.endY - waveDesc.startY );
    painter.drawText(waveName, Qt::AlignVCenter | Qt::AlignHCenter, paramInfo.getParamName(paramInfo.getParamID(waveDesc.waveID)));

    font.setPixelSize(15);
    painter.setFont(font);
    painter.setPen(QPen(color, 1,Qt::DotLine));
    if (waveDesc.waveID == WAVE_CO2)
    {
        painter.drawLine(d_ptr->startX, waveDesc.startY, d_ptr->endX, waveDesc.startY);
        painter.drawLine(d_ptr->startX, waveDesc.endY, d_ptr->endX, waveDesc.endY);

        QRect upRulerRect(d_ptr->startX - 50, waveDesc.startY - 10, 50, 30);
        painter.drawText(upRulerRect, Qt::AlignRight | Qt::AlignTop, QString::number(waveDesc.waveRangeMax));

        QRect downRulerRect(d_ptr->startX - 50, waveDesc.endY - 10, 50, 30);
        painter.drawText(downRulerRect, Qt::AlignRight | Qt::AlignTop, QString::number(waveDesc.waveRangeMin));
    }

}

double OxyCRGEventWaveWidget::_mapWaveValue(WaveformDesc &waveDesc, int wave)
{
    if (wave == InvData())
    {
        return InvData();
    }

    int max = waveDesc.waveRangeMax;
    int min = waveDesc.waveRangeMin;
    double startY = waveDesc.startY;
    double endY = waveDesc.endY;
    double dpos = 0;

    dpos = (max - wave) * ((endY - startY) / (max - min)) + startY;

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
    d_ptr->reset();
    TrendDataSegment *trendSegment;
    d_ptr->trendDataNum = d_ptr->trendSegments.count();
    d_ptr->hrDataBuf = new int[d_ptr->trendDataNum];
    d_ptr->spo2DataBuf = new int[d_ptr->trendDataNum];
    d_ptr->rrDataBuf = new int[d_ptr->trendDataNum];
    for (int i = 0; i < d_ptr->trendSegments.count(); i ++)
    {
        d_ptr->hrDataBuf[i] = InvData();
        d_ptr->spo2DataBuf[i] = InvData();
        d_ptr->rrDataBuf[i] = InvData();
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

void OxyCRGEventWaveWidget::_drawDottedLine(QPainter &painter, qreal x1, qreal y1, qreal x2, qreal y2)
{
    QPen oldPen = painter.pen();

    QVector<qreal> darsh;
    darsh << 5 << 5;
    QPen pen(Qt::white);
    pen.setDashPattern(darsh);
    painter.setPen(pen);
    QLineF dotLine(x1, y1, x2, y2);
    painter.drawLine(dotLine);

    painter.setPen(oldPen);
}

double OxyCRGEventWaveWidget::_mapValue(TrendConvertDesc desc, int data)
{
    if (data == InvData())
    {
        return InvData();
    }

    double dpos = 0;
    dpos = (desc.max - data) * (desc.end - desc.start) / (desc.max - desc.min) + desc.start;

    if (dpos < desc.start)
    {
        dpos = desc.start;
    }
    else if (dpos > desc.end)
    {
        dpos = desc.end;
    }

    return dpos;
}
