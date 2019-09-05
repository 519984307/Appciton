/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/28
 **/

#include "RecordPageGenerator.h"
#include <QTimerEvent>
#include <QDateTime>
#include <qmath.h>
#include <QPainterPath>
#include <QPainter>
#include <QStringList>
#include "FontManager.h"
#include "LanguageManager.h"
#include "ParamInfo.h"
#include "SystemManager.h"
#include "ParamManager.h"
#include "RESPSymbol.h"
#include "WaveformCache.h"
#include "RESPParam.h"
#include "SPO2Param.h"
#include "IBPParam.h"
#include "AGParam.h"
#include "ECGParam.h"
#include "CO2Param.h"
#include "Utility.h"
#include "LayoutManager.h"
#include <QDebug>
#include "RecorderManager.h"
#include "TimeDate.h"
#include "TimeManager.h"
#include "AlarmConfig.h"
#include "UnitManager.h"
#include "PatientManager.h"
#include "EventDataDefine.h"
#include "TrendDataStorageManager.h"

#define DEFAULT_PAGE_WIDTH 200
#define PEN_WIDTH 2
#define DASH_LENGTH 5
RecordPageGenerator::RecordPageGenerator(QObject *parent)
    : QObject(parent), _requestStop(false), _generate(true), _trigger(false),
      _timerInterval(0)
{
}

RecordPageGenerator::~RecordPageGenerator()
{
    _timer.stop();
}

void RecordPageGenerator::setTrigger(bool flag)
{
    _trigger = flag;
}

bool RecordPageGenerator::isTrigger() const
{
    return _trigger;
}

RecordPageGenerator::PrintPriority RecordPageGenerator::getPriority() const
{
    if (_trigger)
    {
        return PriorityTrigger;
    }

    return PriorityReview;
}

int RecordPageGenerator::type() const
{
    return Type;
}

void RecordPageGenerator::start(int interval)
{
    _timerInterval = interval;
    _timer.start(_timerInterval, this);
    onStartGenerate();
}

void RecordPageGenerator::stop()
{
    _requestStop = true;
}

void RecordPageGenerator::pageControl(bool pause)
{
    _generate = !pause;

    if (_generate)
    {
        _timer.start(_timerInterval, this);
    }
    qDebug() << "Page Generator " << (pause ? "pause" : "restart");
}

QFont RecordPageGenerator::font()
{
    return fontManager.recordFont(24);
}

void RecordPageGenerator::setPrintTime(PrintTime timeSec)
{
   Q_UNUSED(timeSec)
}

RecordPage *RecordPageGenerator::createPage()
{
    RecordPage *p = new RecordPage(DEFAULT_PAGE_WIDTH);
#if 1
    // draw a sin wave
    qreal yOffset = p->height() / 2;
    qreal xOffset = 2 * 3.1415926 / DEFAULT_PAGE_WIDTH;
    QPainter painter(p);
    QPen pen(Qt::white, 2);
    painter.setPen(pen);
    static QPointF lastP(0, yOffset);
    QPointF p2;

    for (int i = 0; i < p->width(); i++)
    {
        p2.setX(i);
        p2.setY(yOffset - qSin(i * xOffset) * (p->height() / 2 - 20));
        painter.drawLine(lastP, p2);
        lastP = p2;
    }

    lastP.setX(0);
    lastP.setY(lastP.y() - 1);


#endif
    return p;
}

RecordPage *RecordPageGenerator::createTitlePage(const QString &title, const PatientInfo &patInfo, unsigned timestamp)
{
    QStringList infos;
    infos.append(QString("%1: %2").arg(trs("Name")).arg(patInfo.name));
    infos.append(QString("%1: %2").arg(trs("PatientSex")).arg(trs(PatientSymbol::convert(patInfo.sex))));
    infos.append(QString("%1: %2").arg(trs("PatientType")).arg(trs(PatientSymbol::convert(patInfo.type))));
    infos.append(QString("%1: %2").arg(trs("Blood")).arg(PatientSymbol::convert(patInfo.blood)));
    QString str;
    QString fotmat;
    timeDate.getDateFormat(fotmat, true);
    str = QString("%1: %2").arg(trs("BornDate")).arg(patInfo.bornDate.toString(fotmat));
    infos.append(str);

    str = QString("%1: ").arg(trs("Weight"));
    if (patInfo.weight)
    {
        float weight = patInfo.weight;
        QString weightStr = Unit::convert(patientManager.getWeightUnit(), UNIT_KG, weight);
        str += QString("%1 %2").arg(weightStr).arg(trs(Unit::getSymbol(patientManager.getWeightUnit())));
    }
    infos.append(str);

    str = QString("%1: ").arg(trs("Height"));
    if (patInfo.height)
    {
        float height = patInfo.height;
        QString heightStr = Unit::convert(patientManager.getHeightUnit(), UNIT_CM, height);
        str += QString("%1 %2").arg(heightStr).arg(trs(Unit::getSymbol(patientManager.getHeightUnit())));
    }
    infos.append(str);

    infos.append(QString("%1: %2").arg(trs("ID")).arg(patInfo.id));
    infos.append(QString("%1: %2").arg(trs("PatientPacemarker")).arg(trs(PatientSymbol::convert(static_cast<PatientPacer>(patInfo.pacer)))));

    // calculate the info text width
    int textWidth = 0;
    int w = 0;
    QFont font = fontManager.recordFont(24);
    foreach(QString infoStr, infos)
    {
        w = fontManager.textWidthInPixels(infoStr, font);
        if (w > textWidth)
        {
            textWidth = w;
        }
    }

    // title width
    w = fontManager.textWidthInPixels(title, font);
    if (w > textWidth)
    {
        textWidth =  w;
    }

    unsigned t = timeDate.time();
    if (timestamp)
    {
        t = timestamp;
    }

    QString timeDateStr;
    bool showSecond = timeManager.isShowSecond();
    timeDate.getDateTime(t, timeDateStr, true, showSecond);
    QString timeStr = QString("%1: %2").arg(trs("PrintTime")).arg(timeDateStr);

    // record time width
    w = fontManager.textWidthInPixels(timeStr, font);
    if (w > textWidth)
    {
        textWidth =  w;
    }

    int pageWidth = textWidth + font.pixelSize() * 3;
    int fontH = fontManager.textHeightInPixels(font);

    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    painter.setPen(Qt::white);
    painter.setFont(font);

    // we assume the page can hold all the rows
    QRect textRect(font.pixelSize(), 0, textWidth, fontH);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, title);

    // left one empty row
    textRect.translate(0, fontH / 2);

    foreach(QString infoStr, infos)
    {
        textRect.translate(0, fontH);
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, infoStr);
    }

    // recording time in the bottom
    textRect.setTop(page->height() - fontH - fontH / 2);
    textRect.setBottom(page->height() - fontH / 2);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, timeStr);

    return page;
}

/**
 * @brief contructNormalTrendStringItem contruct the trend string for params other than IBP or NIBP
 * @param subParamId sub param id
 * @param data the trend value
 * @param almFlag the alarm flag
 * @param unit current display unit
 * @param defaultUnit default unit
 * @param co2Baro
 * @return trend string
 */
static QString contructNormalTrendStringItem(SubParamID subParamId, TrendDataType data, bool almFlag,
        UnitType unit, UnitType defaultUnit, short co2Bro)
{
    // name
    QString trendString =  trs(paramInfo.getSubParamName(subParamId));
    trendString += "\t";

    // value
    int mul = paramInfo.getMultiOfSubParam(subParamId);
    if (almFlag)
    {
        trendString += "*";
    }
    if (data == InvData())
    {
        trendString += InvStr();
    }
    else
    {
        if (1 == mul)
        {
            trendString += Unit::convert(unit, defaultUnit, data, co2Bro);
        }
        else
        {
            trendString += Unit::convert(unit, defaultUnit, data * 1.0 / mul, co2Bro);
        }
    }
    trendString += "\t";

    // unit
    trendString += trs(Unit::getSymbol(unit));

    return trendString;
}

/**
 * @brief contructPressTrendStringItem contruct the press trend string for the IBP or NIBP
 * @param subParamId
 * @param data press value array, 1 or 3 value
 * @param almFlag the alarm flag array, 1 or 3 value
 * @param unit
 * @param defaultUnit
 * @return
 */
static QString contructPressTrendStringItem(SubParamID subParamId, TrendDataType *data, bool *almFlag,
        UnitType unit, UnitType defaultUnit)
{
    QString trendString;
    Q_ASSERT(data != NULL);
    // name
    if (subParamId >= SUB_PARAM_ART_SYS && subParamId <= SUB_PARAM_AUXP2_SYS)
    {
        trendString = paramInfo.getIBPPressName(subParamId);
    }
    else if (subParamId >= SUB_PARAM_NIBP_SYS && subParamId <= SUB_PARAM_NIBP_PR)
    {
        trendString = paramInfo.getParamName(PARAM_NIBP);
    }
    else
    {
        // should not get here
        return trendString;
    }
    trendString += "\t";


    // get value

    TrendDataType sys = InvData();
    TrendDataType dia = InvData();
    TrendDataType map = InvData();
    // have 3 press value by default
    int valueNum = 1;

    switch (subParamId)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_NIBP_SYS:
        valueNum = 3;
        sys = data[0];
        dia = data[1];
        map = data[2];
        break;
    default:
        map = data[0];
        break;
    }

    QString valueStr;
    if (valueNum == 1)
    {
        if (almFlag[0])
        {
            valueStr = "(*%1)";
        }
        else
        {
            valueStr = "(%1)";
        }
        if (map == InvData())
        {
            valueStr = valueStr.arg(InvData());
        }
        else
        {
            int mul = paramInfo.getMultiOfSubParam(subParamId);
            if (mul == 1)
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, map));
            }
            else
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, map * 1.0 / mul));
            }
        }
    }
    else
    {
        if (almFlag[0])
        {
            valueStr = "*%1";
        }
        else
        {
            valueStr = "%1";
        }

        if (almFlag[1])
        {
            valueStr += "/*%2";
        }
        else
        {
            valueStr += "/%2";
        }

        if (almFlag[2])
        {
            valueStr += "(*%3)";
        }
        else
        {
            valueStr += "(%3)";
        }

        if (sys == InvData())
        {
            valueStr = valueStr.arg(InvStr()).arg(InvStr()).arg(InvStr());
        }
        else
        {
            int mul = paramInfo.getMultiOfSubParam(subParamId);
            if (mul == 1)
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, sys))
                           .arg(Unit::convert(unit, defaultUnit, dia))
                           .arg(Unit::convert(unit, defaultUnit, map));
            }
            else
            {
                valueStr = valueStr.arg(Unit::convert(unit, defaultUnit, sys * 1.0 / mul))
                           .arg(Unit::convert(unit, defaultUnit, dia * 1.0 / mul))
                           .arg(Unit::convert(unit, defaultUnit, map * 1.0 / mul));
            }
        }
    }

    trendString += valueStr;
    trendString += "\t";

    // unit
    trendString += trs(Unit::getSymbol(unit));

    return trendString;
}

/**
 * @brief The TrendStringSegmentInfo struct
 *        use to handle the trend string info
 */
struct TrendStringSegmentInfo
{
    QString nameSegment;
    QString valueSegment;
    QString unitSegment;
    int nameSegmentWidth;
    int valueSegmentWidth;
    int unitSegmentWidth;
};

/**
 * @brief converToStringSegmets convert the trend string into segments
 * @param trendStringList input trend string list
 * @param strSegInfoList output string segments info list
 * @param font string display font
 */
static void converToStringSegmets(const QStringList &trendStringList, QList<TrendStringSegmentInfo> &strSegInfoList,
                                  const QFont &font)
{
    strSegInfoList.clear();
    foreach(QString str, trendStringList)
    {
        TrendStringSegmentInfo segInfo;
        segInfo.nameSegment = str.section('\t', 0, 0);
        segInfo.valueSegment = str.section('\t', 1, 1);
        segInfo.unitSegment = str.section('\t', 2, 2);
        segInfo.nameSegmentWidth = fontManager.textWidthInPixels(segInfo.nameSegment, font) + font.pixelSize();
        segInfo.valueSegmentWidth = fontManager.textWidthInPixels(segInfo.valueSegment, font) + font.pixelSize();
        segInfo.unitSegmentWidth = fontManager.textWidthInPixels(segInfo.unitSegment, font) + font.pixelSize();
        strSegInfoList.append(segInfo);
    }
}

RecordPage *RecordPageGenerator::createTrendPage(const TrendDataPackage &trendData, bool showEventTime,
        const QString &timeStringCaption, const QString &trendPageTitle, const QString &extraInfo)
{
    QStringList trendStringList = getTrendStringList(trendData);
    if (extraInfo != QString())
    {
        trendStringList.append(extraInfo);
    }

    QString timeStr;
    if (showEventTime)
    {
        QString timeDateStr;
        timeDate.getDateTime(trendData.time, timeDateStr, true, true);
        if (timeStringCaption.isEmpty())
        {
            timeStr = QString("%1: %2").arg(trs("EventTime")).arg(timeDateStr);
        }
        else
        {
            timeStr = QString("%1: %2").arg(timeStringCaption).arg(timeDateStr);
        }
    }

    QFont font = fontManager.recordFont(24);

    int fontH = fontManager.textHeightInPixels(font);

    int avaliableTextHeight = RECORDER_PAGE_HEIGHT -  2 * fontH - fontH / 2;


    // if the contains too many lines, need to seperate into several group
    QVector<int> segmentWidths; // record the maximum widht or the group segments
    int avaliableLine = avaliableTextHeight / fontH;
    if (!trendPageTitle.isEmpty())
    {
        // 触发打印加上标题所以少显示一行
        avaliableLine--;
    }
    QList<TrendStringSegmentInfo> strSegInfoList;
    converToStringSegmets(trendStringList, strSegInfoList, font);
    int index = -1;
    int n = 0;
    foreach(TrendStringSegmentInfo segInfo, strSegInfoList)
    {
        if (n % avaliableLine == 0)
        {
            // each line contain 3 segments
            segmentWidths.append(0);
            segmentWidths.append(0);
            segmentWidths.append(0);
            index++;
        }
        n++;
        if (segInfo.nameSegmentWidth > segmentWidths[index * 3])
        {
            segmentWidths[index * 3] = segInfo.nameSegmentWidth;
        }
        if (segInfo.valueSegmentWidth > segmentWidths[index * 3 + 1])
        {
            segmentWidths[index * 3 + 1] = segInfo.valueSegmentWidth;
        }
        if (segInfo.unitSegmentWidth > segmentWidths[index * 3 + 2])
        {
            segmentWidths[index * 3 + 2] = segInfo.unitSegmentWidth;
        }
    }

    // calculate the page width of all the group line
    int pageWidth = 0;
    foreach(int maxW, segmentWidths)
    {
        pageWidth += maxW;
    }

    // check time string width, time string is drawn at the bottom
    if (showEventTime && pageWidth < fontManager.textWidthInPixels(timeStr, font))
    {
        pageWidth = fontManager.textWidthInPixels(timeStr, font);
    }

    // add the gap between group
    pageWidth += (segmentWidths.size() / 3) * font.pixelSize() + font.pixelSize() * 2;

    // Do the drawing stuff
    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    painter.setPen(Qt::white);
    painter.setFont(font);

    QTextOption textOption;
    textOption.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    int xoffset = font.pixelSize();
    int startYoffset = fontH;
    if (!trendPageTitle.isEmpty())
    {
        QRect rect(xoffset, startYoffset, page->width(), fontH);
        painter.drawText(rect, trendPageTitle, textOption);
        startYoffset += fontH;
    }

    for (int i = 0; i < segmentWidths.size(); i += 3)
    {
        QRectF nameRect(xoffset, startYoffset, segmentWidths[i], fontH);
        xoffset += segmentWidths[i];
        QRectF valueRect(xoffset, startYoffset, segmentWidths[i + 1], fontH);
        xoffset += segmentWidths[i + 1];
        QRectF unitRect(xoffset, startYoffset, segmentWidths[i + 2], fontH);
        xoffset += segmentWidths[i + 2];
        TrendStringSegmentInfo seg;
        for (int lines = 0; lines < avaliableLine; lines++)
        {
            if (strSegInfoList.isEmpty())
            {
                break;
            }
            seg = strSegInfoList.takeFirst();

            painter.drawText(nameRect, seg.nameSegment, textOption);
            painter.drawText(valueRect, seg.valueSegment, textOption);
            painter.drawText(unitRect, seg.unitSegment, textOption);

            nameRect.translate(0, fontH);
            valueRect.translate(0, fontH);
            unitRect.translate(0, fontH);
        }
        xoffset += font.pixelSize();
    }


    if (showEventTime)
    {
        QRectF r;
        r.setLeft(font.pixelSize());
        r.setTop(page->height() - fontH - fontH / 2);
        r.setBottom(page->height() - fontH / 2);
        r.setHeight(fontH);
        r.setWidth(pageWidth);

        painter.drawText(r, timeStr, textOption);
    }

    return page;
}

static bool isPressSubParam(SubParamID SubParamID)
{
    switch (SubParamID)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_AUXP2_DIA:
    case SUB_PARAM_AUXP2_MAP:
    case SUB_PARAM_NIBP_SYS:
    case SUB_PARAM_NIBP_DIA:
    case SUB_PARAM_NIBP_MAP:
    case SUB_PARAM_NIBP_PR:
        return true;
    default:
        break;
    }
    return false;
}

QStringList RecordPageGenerator::getTrendStringList(const TrendDataPackage &trendData)
{
    QStringList strList;
    ParamID paramid;
    SubParamID subparamID;
    for (int i = 0; i < SUB_PARAM_NR; i++)
    {
        subparamID = (SubParamID)i;
        if (!trendData.subparamValue.contains(subparamID))
        {
            continue;
        }

        paramid = paramInfo.getParamID(subparamID);
        if (!isPressSubParam(subparamID))
        {
            if (subparamID == SUB_PARAM_TD)
            {
                QString t1Str = Unit::convert(paramManager.getSubParamUnit(paramid, subparamID),
                                              paramInfo.getUnitOfSubParam(subparamID),
                                              trendData.subparamValue[SUB_PARAM_T1] / 10.0,
                                              trendData.co2Baro);
                QString t2Str = Unit::convert(paramManager.getSubParamUnit(paramid, subparamID),
                                              paramInfo.getUnitOfSubParam(subparamID),
                                              trendData.subparamValue[SUB_PARAM_T2] / 10.0,
                                              trendData.co2Baro);

                TrendDataType td;
                if (trendData.subparamValue[SUB_PARAM_T1] == InvData()
                        || trendData.subparamValue[SUB_PARAM_T2] == InvData())
                {
                    // 有一个无效数据，计算的温度差则无效
                    td = InvData();
                }
                else
                {
                    td = fabs(t1Str.toDouble() * 10 - t2Str.toDouble() * 10);
                }

                strList.append(contructNormalTrendStringItem(subparamID,
                               td,
                               trendData.subparamAlarm[subparamID],
                               paramManager.getSubParamUnit(paramid, subparamID),
                               paramManager.getSubParamUnit(paramid, subparamID),
                               trendData.co2Baro));
            }
            else
            {
                if (subparamID == SUB_PARAM_ETCO2 || subparamID == SUB_PARAM_FICO2)
                {
                    // co2没有连外接模块时,不打印CO2趋势数据
                    if (!co2Param.isConnected())
                    {
                        continue;
                    }
                }
                strList.append(contructNormalTrendStringItem(subparamID,
                               trendData.subparamValue[subparamID],
                               trendData.subparamAlarm[subparamID],
                               paramManager.getSubParamUnit(paramid, subparamID),
                               paramInfo.getUnitOfSubParam(subparamID),
                               trendData.co2Baro));
            }
        }
        else
        {
            TrendDataType data[3] = {InvData()};
            bool alarms[3] = {false};
            bool handle = true;
            switch (subparamID)
            {
            case SUB_PARAM_NIBP_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_NIBP_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_NIBP_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_NIBP_MAP, InvData());
                alarms[0] = trendData.subparamAlarm.value(SUB_PARAM_NIBP_SYS, false);
                alarms[1] = trendData.subparamAlarm.value(SUB_PARAM_NIBP_DIA, false);
                alarms[2] = trendData.subparamAlarm.value(SUB_PARAM_NIBP_MAP, false);
                break;
            case SUB_PARAM_ART_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_ART_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_ART_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_ART_MAP, InvData());
                alarms[0] = trendData.subparamAlarm.value(SUB_PARAM_ART_SYS, false);
                alarms[1] = trendData.subparamAlarm.value(SUB_PARAM_ART_DIA, false);
                alarms[2] = trendData.subparamAlarm.value(SUB_PARAM_ART_MAP, false);
                break;
            case SUB_PARAM_PA_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_PA_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_PA_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_PA_MAP, InvData());
                alarms[0] = trendData.subparamAlarm.value(SUB_PARAM_PA_SYS, false);
                alarms[1] = trendData.subparamAlarm.value(SUB_PARAM_PA_DIA, false);
                alarms[2] = trendData.subparamAlarm.value(SUB_PARAM_PA_MAP, false);
                break;
            case SUB_PARAM_CVP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_CVP_MAP, InvData());
                alarms[0] = trendData.subparamAlarm.value(SUB_PARAM_CVP_MAP, false);
                break;
            case SUB_PARAM_LAP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_LAP_MAP, InvData());
                alarms[0] = trendData.subparamAlarm.value(SUB_PARAM_LAP_MAP, false);
                break;
            case SUB_PARAM_RAP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_RAP_MAP, InvData());
                alarms[0] = trendData.subparamAlarm.value(SUB_PARAM_RAP_MAP, false);
                break;
            case SUB_PARAM_ICP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_ICP_MAP, InvData());
                alarms[0] = trendData.subparamAlarm.value(SUB_PARAM_ICP_MAP, false);
                break;
            case SUB_PARAM_AUXP1_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_AUXP1_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_AUXP1_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_AUXP1_MAP, InvData());
                alarms[0] = trendData.subparamAlarm.value(SUB_PARAM_AUXP1_SYS, false);
                alarms[1] = trendData.subparamAlarm.value(SUB_PARAM_AUXP1_DIA, false);
                alarms[2] = trendData.subparamAlarm.value(SUB_PARAM_AUXP1_MAP, false);
                break;
            case SUB_PARAM_AUXP2_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_AUXP2_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_AUXP2_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_AUXP2_MAP, InvData());
                alarms[0] = trendData.subparamAlarm.value(SUB_PARAM_AUXP2_SYS, false);
                alarms[1] = trendData.subparamAlarm.value(SUB_PARAM_AUXP2_DIA, false);
                alarms[2] = trendData.subparamAlarm.value(SUB_PARAM_AUXP2_MAP, false);
                break;
            default:
                handle = false;
                break;
            }

            if (!handle)
            {
                continue;
            }
            strList.append(contructPressTrendStringItem(subparamID,
                           data,
                           alarms,
                           paramManager.getSubParamUnit(paramid, subparamID),
                           paramInfo.getUnitOfSubParam(subparamID)));
        }
    }
    return strList;
}

/**
 * @brief drawECGGain draw the ecg ruler on the page
 * @param page print page
 * @param painter painter of the page
 * @param waveInfo wave info
 * @param waveNum total wave num in the page
 */
static void drawECGGain(RecordPage *page, QPainter *painter, const RecordWaveSegmentInfo &waveInfo, int waveNum)
{
    int rulerHeight = 10 * RECORDER_PIXEL_PER_MM; // height for 1.0 ECG gain
    QString str = "1 mV";
    ECGGain gain = waveInfo.waveInfo.ecg.gain;
    if (gain == ECG_GAIN_AUTO)
    {
        gain = ecgParam.getECGAutoGain(ecgParam.waveIDToLeadID(waveInfo.id));
    }
    switch (gain)
    {
    case ECG_GAIN_X0125:
        rulerHeight /= 8;
        break;
    case ECG_GAIN_X025:
        rulerHeight /= 4;
        break;
    case ECG_GAIN_X05:
        rulerHeight /= 2;
        break;
    case ECG_GAIN_X20:
        str = "0.5 mV";
        break;
    case ECG_GAIN_X40:
        str = "0.25 mV";
        break;
    case ECG_GAIN_X10:
    case ECG_GAIN_AUTO:
        break;
    default:
        break;
    }

    int pageWidth = page->width();
    // draw at the bottom
    int yBottom = waveInfo.endYOffset;

    if (waveNum == 1)
    {
        // only one wave, draw at the baseline
        yBottom = waveInfo.startYOffset + (waveInfo.endYOffset - waveInfo.startYOffset) / 2;
    }

    QPainterPath path;
    path.moveTo(0, yBottom);
    path.lineTo(pageWidth / 3, yBottom);
    path.lineTo(pageWidth / 3, yBottom - rulerHeight);
    path.lineTo(pageWidth * 2 / 3, yBottom - rulerHeight);
    path.lineTo(pageWidth * 2 / 3, yBottom);
    path.lineTo(pageWidth, yBottom);
    painter->save();
    QPen p(Qt::white, PEN_WIDTH);
    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
    int fontH = fontManager.textHeightInPixels(painter->font());
    QRect rect(0, yBottom - rulerHeight - fontH, pageWidth, fontH);
    painter->setPen(Qt::white);
    painter->drawText(rect, Qt::AlignCenter,  str);
    painter->restore();
}

#define RULER_TICK_LEN 8
// draw a gas style ruler
static void drawGasRuler(RecordPage *page, QPainter *painter, const RecordWaveSegmentInfo &waveInfo,
                             const QString &high, const QString &low, const UnitType unit)
{
    QPainterPath path;
    path.moveTo(RULER_TICK_LEN, waveInfo.startYOffset);
    path.lineTo(0, waveInfo.startYOffset);
    path.lineTo(0, waveInfo.endYOffset);
    path.lineTo(RULER_TICK_LEN, waveInfo.endYOffset);

    painter->save();

    QPen p(Qt::white, PEN_WIDTH);
    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);

    painter->setPen(Qt::white);

    int fontH = fontManager.textHeightInPixels(painter->font());
    QRect rect(RULER_TICK_LEN, waveInfo.startYOffset, page->width() - RULER_TICK_LEN, fontH);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, high);
    rect.translate(0, waveInfo.endYOffset - waveInfo.startYOffset - fontH);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, low);

    rect.setY((waveInfo.endYOffset -  waveInfo.startYOffset - fontH) / 2 + waveInfo.startYOffset);
    rect.setHeight(fontH);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, trs(Unit::getSymbol(unit)));

    painter->restore();
}

// draw the IBP ruler on the page
static void drawIBPRuler(RecordPage *page, QPainter *painter, const RecordWaveSegmentInfo &waveInfo)
{
    QPainterPath path;
    path.moveTo(RULER_TICK_LEN, waveInfo.startYOffset);
    path.lineTo(0, waveInfo.startYOffset);
    path.lineTo(0, waveInfo.endYOffset);
    path.lineTo(RULER_TICK_LEN, waveInfo.endYOffset);

    painter->save();

    QPen p(Qt::white, PEN_WIDTH);
    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);

    painter->setPen(Qt::white);

    int fontH = fontManager.textHeightInPixels(painter->font());
    QRect rect(RULER_TICK_LEN, waveInfo.startYOffset, page->width() - RULER_TICK_LEN, fontH);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, QString::number(waveInfo.waveInfo.ibp.high));
    rect.translate(0, waveInfo.endYOffset - waveInfo.startYOffset - fontH);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, QString::number(waveInfo.waveInfo.ibp.low));

    rect.setY((waveInfo.endYOffset -  waveInfo.startYOffset - fontH) / 2 + waveInfo.startYOffset);
    rect.setHeight(fontH);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, trs(Unit::getSymbol(waveInfo.waveInfo.ibp.unit)));

    painter->restore();
}

// draw the RESP Zoom on the page
static void drawRespZoom(RecordPage *page, QPainter *painter, const RecordWaveSegmentInfo &waveInfo)
{
    QString zoom = RESPSymbol::convert(waveInfo.waveInfo.resp.zoom);

    painter->save();

    int fontH = fontManager.textHeightInPixels(painter->font());
    QRect rect(0, waveInfo.startYOffset, page->width(), fontH);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, zoom);

    painter->restore();
}

RecordPage *RecordPageGenerator::createWaveScalePage(const QList<RecordWaveSegmentInfo> &waveInfos, PrintSpeed speed)
{
    if (waveInfos.count() == 0)
    {
        return NULL;
    }
    int pageWidth = 25 * RECORDER_PIXEL_PER_MM;

    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    QPen pen(Qt::white, PEN_WIDTH);
    painter.setPen(pen);
    QFont font = fontManager.recordFont(24);
    painter.setFont(font);

    int fontH = fontManager.textHeightInPixels(font);

    // draw the print speed
    QRect rect(fontH * 2, fontH / 2, page->width(), fontH);
    painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, PrintSymbol::convert(speed));

    int waveNum = waveInfos.size();
    QList<RecordWaveSegmentInfo>::ConstIterator iter;
    for (iter = waveInfos.constBegin(); iter != waveInfos.constEnd(); iter++)
    {
        switch (iter->id)
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
            drawECGGain(page, &painter, *iter, waveNum);
            break;
        case WAVE_RESP:
            drawRespZoom(page, &painter, *iter);
            break;
        case WAVE_SPO2:
            break;
        case WAVE_CO2:
        {
            int high;           // 高限值
            CO2DisplayZoom zoom = iter->waveInfo.co2.zoom;
            switch (zoom)
            {
            case CO2_DISPLAY_ZOOM_4:
                high = 4;
                break;
            case CO2_DISPLAY_ZOOM_8:
                high = 8;
                break;
            case CO2_DISPLAY_ZOOM_12:
                high = 12;
                break;
            case CO2_DISPLAY_ZOOM_20:
            default:
                high = 20;
                break;
            }
            UnitType unit = co2Param.getUnit();
            QString highString;
            if (unit == UNIT_MMHG)
            {
                high = Unit::convert(UNIT_MMHG, UNIT_PERCENT, high).toInt();
                high = (high + 5) / 10 * 10;            // 取整
            }
            highString = QString::number(high, 'g', 3);
            drawGasRuler(page, &painter, *iter, highString, "0", unit);
        }
        break;
        case WAVE_N2O:
        case WAVE_AA1:
        case WAVE_AA2:
        case WAVE_O2:
        {
            int high;
            AGDisplayZoom zoom = iter->waveInfo.ag.zoom;
            switch (zoom)
            {
            case AG_DISPLAY_ZOOM_4:
                high = 4;
                break;
            case AG_DISPLAY_ZOOM_8:
                high = 8;
                break;
            case AG_DISPLAY_ZOOM_15:
            default:
                high = 15;
                break;
            }
            drawGasRuler(page, &painter, *iter, QString::number(high, 'g', 2), "0", UNIT_PERCENT);
        }
        break;

        case WAVE_ART:
        case WAVE_PA:
        case WAVE_CVP:
        case WAVE_LAP:
        case WAVE_RAP:
        case WAVE_ICP:
        case WAVE_AUXP1:
        case WAVE_AUXP2:
            drawIBPRuler(page, &painter, *iter);
            break;

        default:
            break;
        }
    }

    return page;
}


/**
 * @brief drawCaption draw the wave's caption
 * @param page the record page
 * @param patiner painter
 * @param waveInfo wave segment info;
 * @param segIndex the wave segment index
 */
static void drawCaption(RecordPage *page, QPainter *painter, const RecordWaveSegmentInfo &waveInfo, int segIndex)
{
    if (page->width() * (segIndex) >= waveInfo.drawCtx.captionPixLength)
    {
        // the caption has heen completely drawn
        return;
    }

    int fontH = fontManager.textHeightInPixels(painter->font());
    QRect rect(0, waveInfo.startYOffset, waveInfo.drawCtx.captionPixLength, fontH);
    painter->save();
    painter->setPen(Qt::white);
    painter->translate(-segIndex * page->width(), 0);
    QString caption = waveInfo.drawCtx.caption;
    QString mode = trs(caption.section(" ", 3));
    caption = caption.section(" ", 0, 2);
    caption = caption + mode;
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, caption);
    painter->restore();
}

/**
 * @brief mapWaveValue map the wave value
 * @param waveInfo the wave segment info
 * @param wave input wave value
 * @return  output wave value
 */
static qreal mapWaveValue(const RecordWaveSegmentInfo &waveInfo, short wave)
{
    qreal waveData = 0.0;

    qreal startY = waveInfo.startYOffset;
    qreal endY = waveInfo.endYOffset;

    switch (waveInfo.id)
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
        double scale = 0;
        ECGGain gain = waveInfo.waveInfo.ecg.gain;
        if (gain == ECG_GAIN_AUTO)
        {
            gain = ecgParam.getECGAutoGain(ecgParam.waveIDToLeadID(waveInfo.id));
        }
        switch (gain)
        {
        case ECG_GAIN_X0125:
            scale = 0.125 * 10 * RECORDER_PIXEL_PER_MM / 2.0;
            break;

        case ECG_GAIN_X025:
            scale = 0.25 * 10 * RECORDER_PIXEL_PER_MM / 2.0;
            break;

        case ECG_GAIN_X05:
            scale = 0.5 * 10 * RECORDER_PIXEL_PER_MM / 2.0;
            break;

        case ECG_GAIN_X10:
            scale = 1.0 * 10 * RECORDER_PIXEL_PER_MM / 2.0;
            break;

        case ECG_GAIN_X20:
            scale = 2.0 * 10 * RECORDER_PIXEL_PER_MM / 2.0;
            break;

        case ECG_GAIN_X40:
            scale = 4.0 * 10 * RECORDER_PIXEL_PER_MM / 2.0;
            break;

        default:
            break;
        }
        endY = waveInfo.middleYOffset + scale;
        startY = waveInfo.middleYOffset - scale;
        waveData = (waveInfo.maxWaveValue - wave) * ((endY - startY) / (waveInfo.maxWaveValue - waveInfo.minWaveValue)) +
                   startY;
        break;
    }

    case WAVE_RESP:
    {
        qreal respZoom = 1.0;
        switch (waveInfo.waveInfo.resp.zoom)
        {
        case RESP_ZOOM_X025:
            respZoom = 0.25;
            break;
        case RESP_ZOOM_X050:
            respZoom = 0.5;
            break;
        case RESP_ZOOM_X100:
            respZoom = 1.0;
            break;
        case RESP_ZOOM_X200:
            respZoom = 2.0;
            break;
        case RESP_ZOOM_X300:
            respZoom = 3.0;
            break;
        case RESP_ZOOM_X400:
            respZoom = 4.0;
            break;
        case RESP_ZOOM_X500:
            respZoom = 5.0;
            break;
        default:
            break;
        }

        short zoomWave = (wave - waveInfo.waveBaseLine) * respZoom + waveInfo.waveBaseLine;
        waveData = (waveInfo.maxWaveValue -  zoomWave) * ((endY -  startY) / (waveInfo.maxWaveValue -  waveInfo.minWaveValue)) +
                   startY;
        break;
    }

    case WAVE_SPO2:
    {
        qreal spo2Zoom = 1.0;

        short zoomWave = (wave - waveInfo.waveBaseLine) * spo2Zoom + waveInfo.waveBaseLine;
        waveData = (waveInfo.maxWaveValue -  zoomWave) * ((endY -  startY) / (waveInfo.maxWaveValue -  waveInfo.minWaveValue)) +
                   startY;
        break;
    }

    case WAVE_CO2:
    {
        int max = waveInfo.maxWaveValue;
        switch (waveInfo.waveInfo.co2.zoom)
        {
        case CO2_DISPLAY_ZOOM_4:
            max = max * 4 / 20;
            break;
        case CO2_DISPLAY_ZOOM_8:
            max  = max * 8 / 20;
            break;
        case CO2_DISPLAY_ZOOM_12:
            max = max * 12 / 20;
            break;
        default:
            break;
        }

        waveData = (max - wave) * (endY -  startY) / (max -  waveInfo.minWaveValue) + startY;
        break;
    }

    case WAVE_N2O:
    case WAVE_AA1:
    case WAVE_AA2:
    case WAVE_O2:
    {
        int max = waveInfo.maxWaveValue;
        switch (waveInfo.waveInfo.ag.zoom)
        {
        case AG_DISPLAY_ZOOM_4:
            max = max * 4 / 15;
            break;
        case AG_DISPLAY_ZOOM_8:
            max = max * 8 / 15;
            break;
        default:
            break;
        }
        waveData = (max - wave) * (endY -  startY) / (max -  waveInfo.minWaveValue) + startY;
        break;
    }

    case WAVE_ART:
    case WAVE_PA:
    case WAVE_CVP:
    case WAVE_LAP:
    case WAVE_RAP:
    case WAVE_ICP:
    case WAVE_AUXP1:
    case WAVE_AUXP2:
    {
        int max = waveInfo.waveInfo.ibp.high * 10 + 1000;
        int min = waveInfo.waveInfo.ibp.low * 10 + 1000;
        waveData = (max - wave) * (endY -  startY) / (max -  min) + startY;
        break;
    }

    default:
        break;
    }

    // cut off
    if (waveData < waveInfo.startYOffset)
    {
        waveData = waveInfo.startYOffset;
    }
    else if (waveData > waveInfo.endYOffset)
    {
        waveData = waveInfo.endYOffset;
    }

    return waveData;
}

/**
 * @brief mapOxyCRGWaveValue
 * @param waveInfo
 * @param wave
 * @return
 */
static qreal mapOxyCRGWaveValue(const OxyCRGWaveInfo &waveInfo, qreal waveHeight, short wave)
{
    qreal waveData = 0.0;

    if (waveInfo.id == WAVE_RESP)
    {
        qreal respZoom = 1.0;
        switch (waveInfo.waveInfo.resp.zoom)
        {
        case RESP_ZOOM_X100:
            respZoom = 1.0;
            break;
        case RESP_ZOOM_X200:
            respZoom = 2.0;
            break;
        case RESP_ZOOM_X300:
            respZoom = 3.0;
            break;
        case RESP_ZOOM_X400:
            respZoom = 4.0;
            break;
        case RESP_ZOOM_X500:
            respZoom = 5.0;
            break;
        default:
            break;
        }

        short zoomWave = (wave - waveInfo.waveBaseLine) * respZoom + waveInfo.waveBaseLine;
        waveData = zoomWave * (waveHeight / (waveInfo.maxWaveValue -  waveInfo.minWaveValue));
    }
    else if (waveInfo.id == WAVE_CO2)
    {
        int max = waveInfo.maxWaveValue;
        switch (waveInfo.waveInfo.co2.zoom)
        {
        case CO2_DISPLAY_ZOOM_4:
            max = max * 4 / 20;
            break;
        case CO2_DISPLAY_ZOOM_8:
            max  = max * 8 / 20;
            break;
        case CO2_DISPLAY_ZOOM_12:
            max = max * 12 / 20;
            break;
        default:
            break;
        }

        waveData = (wave - waveInfo.minWaveValue) * waveHeight / (max -  waveInfo.minWaveValue);
    }

    if (waveData > waveHeight)
    {
        waveData = waveHeight;
    }
    else if (waveData < 0)
    {
        waveData = 0;
    }

    return -waveData;
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
    QPen pen(Qt::white, PEN_WIDTH);
    pen.setDashPattern(darsh);
    pen.setDashOffset(dashOffset);
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    QLineF dotLine(x1, y1, x2, y2);
    painter->drawLine(dotLine);
    painter->restore();
}



/**
 * @brief drawWaveSegment draw wave segment
 * @param page  the record page
 * @param painter paitner
 * @param waveInfo the wave segment info
 * @param segmentIndex segment index
 */
static void drawWaveSegment(RecordPage *page, QPainter *painter, RecordWaveSegmentInfo &waveInfo, int segmentIndex)
{
    qreal x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    int pageWidth = page->width();
    qreal offsetX = pageWidth * 1.0 / waveInfo.sampleRate;
    int i;

    int wavebuffSize = waveInfo.secondWaveBuff.size();

    float pixelPitch = systemManager.getScreenPixelHPitch();
    ParamID paramId = paramInfo.getParamID(waveInfo.id);
    for (i = 0; i < waveInfo.sampleRate && i < wavebuffSize; i++)
    {
        unsigned short flag = waveInfo.secondWaveBuff[i] >> 16;
        // invalid data
        if (flag & INVALID_WAVE_FALG_BIT)
        {
            if (i == 0)
            {
                // start point
                x1 = 0;
                x2 = waveInfo.drawCtx.curPageFirstXpos;
            }
            short wave = waveInfo.secondWaveBuff[i] & 0xFFFF;
            double waveData = mapWaveValue(waveInfo, wave);
            y1 = y2 = waveData;

            int j = i + 1;
            while (j < waveInfo.sampleRate && j < wavebuffSize)
            {
                flag = waveInfo.secondWaveBuff[j] >> 16;
                if (!(flag & INVALID_WAVE_FALG_BIT))
                {
                    break;
                }

                if (x2 + offsetX > pageWidth - 1)
                {
                    break;
                }
                x2 += offsetX;
                j++;
            }

            i = j - 1;

            if (x2 > pageWidth - 1)
            {
                drawDottedLine(painter, x1, waveData, pageWidth, waveData);

                qreal tmp = (pageWidth - x1) / DASH_LENGTH;
                waveInfo.drawCtx.dashOffset = tmp - static_cast<int>(tmp);
            }
            else
            {
                drawDottedLine(painter, x1, waveData, x2, waveData, waveInfo.drawCtx.dashOffset);
                waveInfo.drawCtx.dashOffset = 0.0;
            }

            x1 = x2;
            x2 += offsetX;
            y1 = y2;

            waveInfo.drawCtx.lastWaveFlags = flag;

            if (x2 > pageWidth - 1)
            {
                waveInfo.drawCtx.prevSegmentLastYpos = y1;
                waveInfo.drawCtx.curPageFirstXpos = x2 - pageWidth + 1;
                return;
            }
        }
        else // valid wave data
        {
            short wave = waveInfo.secondWaveBuff[i] & 0xFFFF;
            if (waveInfo.id == WAVE_ECG_aVR && waveInfo.waveInfo.ecg.in12LeadMode
                    && waveInfo.waveInfo.ecg._12LeadDisplayFormat == DISPLAY_12LEAD_CABRELA)
            {
                wave = - wave;
            }

            double waveData = mapWaveValue(waveInfo, wave);

            if (i == 0)
            {
                if (segmentIndex == 0)
                {
                    y1 = waveData;
                    waveInfo.drawCtx.dashOffset = 0.0;
                }
                else
                {
                    y1 = waveInfo.drawCtx.prevSegmentLastYpos;
                }
                x1 = 0;
                x2 = waveInfo.drawCtx.curPageFirstXpos;
            }
            else
            {
                if ((waveInfo.secondWaveBuff[i - 1] >> 16) & INVALID_WAVE_FALG_BIT)
                {
                    y1 = waveData;
                }
            }

            y2 = waveData;
            QLineF line(x1, y1, x2, y2);
            painter->drawLine(line);

            if (flag & ECG_INTERNAL_FLAG_BIT && paramId == PARAM_ECG)
            {
                QPen pen = painter->pen();
                painter->setPen(QPen(Qt::white, 2, Qt::DashLine));
                painter->drawLine(x2, waveInfo.middleYOffset - 10 / pixelPitch / 2,
                                  x2, waveInfo.middleYOffset + 10 / pixelPitch / 2);
                painter->setPen(pen);
            }

            x1 = x2;
            x2 += offsetX;
            y1 = y2;

            waveInfo.drawCtx.lastWaveFlags = flag;

            if (x2 > pageWidth - 1)
            {
                break;
            }
        }
    }

    waveInfo.drawCtx.curPageFirstXpos = 0;
    if (x2 > pageWidth - 1)
    {
        bool drawLine = false;
        QLineF line;
        y2 = y1;
        if (i + 1 < waveInfo.sampleRate)
        {
            if (!(waveInfo.drawCtx.lastWaveFlags & INVALID_WAVE_FALG_BIT))
            {
                short wave = waveInfo.secondWaveBuff[i + 1] & 0xFFFF;
                if (waveInfo.id == WAVE_ECG_aVR && waveInfo.waveInfo.ecg.in12LeadMode
                        && waveInfo.waveInfo.ecg._12LeadDisplayFormat == DISPLAY_12LEAD_CABRELA)
                {
                    wave = -wave;
                }

                // 当x1在页宽范围内，x2不在页宽范围内时，通过计算两点连线的斜率来判断x坐标为页宽时点的y坐标，并进行连线
                qreal tmpValue = mapWaveValue(waveInfo, wave);
                qreal deltaY = (tmpValue * 1000 - y1 * 1000) / 1000.0 * (pageWidth * 1.0 - 1.0 - x1) / offsetX;
                y2 = y1 + deltaY;
                line.setLine(x1, y1, pageWidth - 1, y2);
                drawLine = true;
            }
        }
        else
        {
            if (!(waveInfo.drawCtx.lastWaveFlags & INVALID_WAVE_FALG_BIT))
            {
                drawLine = true;
                line.setLine(x1, y1, pageWidth - 1, y2);
            }
        }

        if (drawLine)
        {
            painter->drawLine(line);
            waveInfo.drawCtx.curPageFirstXpos = x2 - pageWidth + 1;
        }
    }
    else
    {
        if (!(waveInfo.drawCtx.lastWaveFlags & INVALID_WAVE_FALG_BIT) && wavebuffSize == waveInfo.sampleRate)
        {
            // draw to the edge
            y2 = y1;
            QLineF line(x1, y1, pageWidth -  1, y2);
            painter->drawLine(line);
        }
    }
    waveInfo.drawCtx.prevSegmentLastYpos = y2;
}

RecordPage *RecordPageGenerator::createWaveSegments(QList<RecordWaveSegmentInfo> &waveInfos, int segmentIndex,
        PrintSpeed speed)
{
    int pageWidth = 25 * RECORDER_PIXEL_PER_MM;
    switch (speed)
    {
    case PRINT_SPEED_125:
        pageWidth = 125 * RECORDER_PIXEL_PER_MM / 10;
        break;
    case PRINT_SPEED_500:
        pageWidth = 50 * RECORDER_PIXEL_PER_MM;
        break;
    default:
        break;
    }

    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    QPen pen(Qt::white, PEN_WIDTH);
    painter.setPen(pen);
    QFont font = fontManager.recordFont(24);
    painter.setFont(font);

    QList<RecordWaveSegmentInfo>::iterator iter = waveInfos.begin();
    for (; iter != waveInfos.end(); ++iter)
    {
        drawCaption(page, &painter, *iter, segmentIndex);
        drawWaveSegment(page, &painter, *iter, segmentIndex);
    }
    return page;
}

void RecordPageGenerator::drawGraphAxis(QPainter *painter, const GraphAxisInfo &axisInfo)
{
    painter->save();
    painter->translate(axisInfo.origin);
    QPen pen(Qt::white, 1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    QVector<QLineF> lines;

    // y axis
    lines.append(QLineF(0, 0, 0, -axisInfo.height));
    qreal sectionHeight = axisInfo.ySectionHeight;
    for (int i = 0; i < axisInfo.ySectionNum && sectionHeight <= axisInfo.height; i++)
    {
        lines.append(QLineF(-axisInfo.tickLength / 2, -sectionHeight, axisInfo.tickLength / 2, -sectionHeight));
        sectionHeight += axisInfo.ySectionHeight;
    }

    // x axis
    lines.append(QLineF(0, 0, axisInfo.width, 0));
    qreal sectionWidth = axisInfo.xSectionWidth;
    for (int i = 0; i < axisInfo.xSectionNum && sectionWidth <= axisInfo.width; i++)
    {
        lines.append(QLine(sectionWidth, -axisInfo.tickLength / 2, sectionWidth, axisInfo.tickLength / 2));
        sectionWidth += axisInfo.xSectionWidth;
    }

    painter->drawLines(lines);

    // draw arrow
    if (axisInfo.drawArrow)
    {
        QPainterPath upArrow;
        upArrow.moveTo(0, -axisInfo.height);
        upArrow.lineTo(-axisInfo.tickLength * 0.80, -axisInfo.height + axisInfo.tickLength * 1.5);
        upArrow.lineTo(axisInfo.tickLength * 0.80, -axisInfo.height + axisInfo.tickLength * 1.5);
        upArrow.lineTo(0, -axisInfo.height);
        painter->fillPath(upArrow, Qt::white);
        QPainterPath rightArrow;
        rightArrow.moveTo(axisInfo.width, 0);
        rightArrow.lineTo(axisInfo.width - axisInfo.tickLength * 1.5, axisInfo.tickLength * 0.80);
        rightArrow.lineTo(axisInfo.width - axisInfo.tickLength * 1.5, -axisInfo.tickLength * 0.80);
        rightArrow.lineTo(axisInfo.width, 0);
        painter->fillPath(rightArrow, Qt::white);
    }

    // draw labels
    QFont font = fontManager.recordFont(18);
    painter->setFont(font);
    qreal fontH = fontManager.textHeightInPixels(font);
    qreal pixSize = font.pixelSize();

    sectionHeight = 0;
    for (int i = 0; i < axisInfo.yLabels.size() && sectionHeight <= axisInfo.height; i++)
    {
        if (!axisInfo.yLabels.at(i).isEmpty())
        {
            QRectF rect;
            rect.setLeft(-axisInfo.marginLeft);
            rect.setRight(-pixSize);
            rect.setTop(- sectionHeight - fontH / 2);
            rect.setHeight(fontH);
            painter->drawText(rect, Qt::AlignVCenter | Qt::AlignRight, axisInfo.yLabels[i]);
        }

        sectionHeight += axisInfo.ySectionHeight;
    }

    sectionWidth = 0;
    for (int i = 0; i < axisInfo.xLabels.size() && sectionWidth <= axisInfo.width; i++)
    {
        if (!axisInfo.xLabels.at(i).isEmpty())
        {
            QRectF rect;
            rect.setTop(axisInfo.tickLength);
            rect.setHeight(fontH);
            rect.setLeft(sectionWidth - axisInfo.xSectionWidth / 2);
            rect.setRight(sectionWidth + axisInfo.xSectionWidth / 2);
            painter->drawText(rect, Qt::AlignHCenter | Qt::AlignTop, axisInfo.xLabels[i]);
        }
        sectionWidth += axisInfo.xSectionWidth;
    }

    // draw caption
    int  captionHeigth = -108;
    if (axisInfo.yLabels.size() != 0)
    {
        captionHeigth = -(sectionHeight - axisInfo.ySectionHeight) - fontH / 2;
    }
    QRectF captionRect;
    captionRect.setLeft(pixSize + pixSize / 2);
    captionRect.setWidth(axisInfo.xSectionWidth * 2); // should be enough
    captionRect.setTop(captionHeigth);
    captionRect.setHeight(fontH);

    painter->drawText(captionRect, Qt::AlignLeft | Qt::AlignVCenter, axisInfo.caption);


    painter->restore();
}


#define isEqual(a, b) (qAbs((a)-(b)) < 0.000001)

static inline qreal timestampToX(unsigned t, const GraphAxisInfo &axisInfo, const TrendGraphInfo &graphInfo)
{
    return (t - graphInfo.startTime) * 1.0 * axisInfo.validWidth / (graphInfo.endTime - graphInfo.startTime);
}

static inline qreal mapTrendYValue(TrendDataType val, const GraphAxisInfo &axisInfo, const TrendGraphInfo &graphInfo)
{
    qreal validHeight = axisInfo.validHeight;
    qreal mapH = (val * graphInfo.scale.scale - graphInfo.scale.min) * 1.0 * validHeight /
            (graphInfo.scale.max -  graphInfo.scale.min);
    if (mapH > validHeight)
    {
        mapH = validHeight;
    }
    else if (mapH < 0.0)
    {
        mapH = 0.0;
    }
    return -mapH;
}

#define TICK_LENGTH         RECORDER_PIXEL_PER_MM
QPainterPath getPrintV2Path(const GraphAxisInfo &axisInfo, const TrendGraphInfo &graphInfo, int index)
{
    QPainterPath path;
    bool lastPointInvalid = true;
    QPointF lastPoint;

    QVector<TrendGraphDataV2>::ConstIterator iter = graphInfo.trendDataV2.constBegin();
    for (; iter != graphInfo.trendDataV2.constEnd(); iter++)
    {
        TrendDataType data = iter->data[index];
        if (data == InvData())
        {
            if (!lastPointInvalid)
            {
                path.lineTo(lastPoint);
                lastPointInvalid = true;
            }
            continue;
        }

        qreal x = timestampToX(iter->timestamp, axisInfo, graphInfo);
        ParamID paramId = paramInfo.getParamID(graphInfo.subParamID);
        UnitType type = paramManager.getSubParamUnit(paramId, graphInfo.subParamID);
        int v = 0;
        if (paramId == PARAM_CO2)
        {
            v = Unit::convert(type, UNIT_PERCENT, data / 10.0, co2Param.getBaro()).toDouble();
        }
        else if (paramId == PARAM_TEMP)
        {
            QString vStr = Unit::convert(type, UNIT_TC, data / 10.0);
            v = vStr.toDouble();
        }
        else
        {
            v = data / 10;
        }
        qreal value = mapTrendYValue(v, axisInfo, graphInfo);

        if (lastPointInvalid)
        {
            path.moveTo(x, value);
            lastPointInvalid = false;
        }
        else
        {
            if (!isEqual(lastPoint.y(), value))
            {
                path.lineTo(lastPoint);
                path.lineTo(x, value);
            }
        }

        lastPoint.rx() = x;
        lastPoint.ry() = value;
    }

    if (!lastPointInvalid)
    {
        path.lineTo(lastPoint);
    }
    return path;
}

QPainterPath getPrintV3Path(const GraphAxisInfo &axisInfo, const TrendGraphInfo &graphInfo, int index)
{
    QPainterPath path;
    bool lastPointInvalid = true;
    QPointF lastPoint;

    QVector<TrendGraphDataV3>::ConstIterator iter = graphInfo.trendDataV3.constBegin();
    for (; iter != graphInfo.trendDataV3.constEnd(); iter++)
    {
        TrendDataType data = iter->data[index];
        if (data == InvData())
        {
            if (!lastPointInvalid)
            {
                path.lineTo(lastPoint);
                lastPointInvalid = true;
            }
            continue;
        }

        qreal x = timestampToX(iter->timestamp, axisInfo, graphInfo);
        qreal value = mapTrendYValue(data, axisInfo, graphInfo);

        if (lastPointInvalid)
        {
            path.moveTo(x, value);
            lastPointInvalid = false;
        }
        else
        {
            if (!isEqual(lastPoint.y(), value))
            {
                path.lineTo(lastPoint);
                path.lineTo(x, value);
            }
        }

        lastPoint.rx() = x;
        lastPoint.ry() = value;
    }

    if (!lastPointInvalid)
    {
        path.lineTo(lastPoint);
    }
    return path;
}

QList<QPainterPath> generatorPainterPath(const GraphAxisInfo &axisInfo, const TrendGraphInfo &graphInfo)
{
    QList<QPainterPath> paths;

    switch (graphInfo.subParamID)
    {
    case SUB_PARAM_NIBP_SYS:
    {
        QPainterPath path;

        QVector<TrendGraphDataV3>::ConstIterator iter = graphInfo.trendDataV3.constBegin();
        for (; iter != graphInfo.trendDataV3.constEnd(); iter++)
        {
            if (iter->data[0] == InvData() || !(iter->status & TrendDataStorageManager::CollectStatusNIBP))
            {
                continue;
            }

            qreal x = timestampToX(iter->timestamp, axisInfo, graphInfo);
            qreal sys = mapTrendYValue(iter->data[0], axisInfo, graphInfo);
            qreal dia = mapTrendYValue(iter->data[1], axisInfo, graphInfo);
            qreal map = mapTrendYValue(iter->data[2], axisInfo, graphInfo);

            // draw nibp symbol
            if (sys == -axisInfo.validHeight)
            {
                path.moveTo(x - TICK_LENGTH / 2, sys + 0.866 * TICK_LENGTH);
                path.lineTo(x, sys);
                path.lineTo(x + TICK_LENGTH / 2, sys + 0.866 * TICK_LENGTH);
            }
            else
            {
                path.moveTo(x - TICK_LENGTH / 2, sys - 0.866 * TICK_LENGTH);
                path.lineTo(x, sys);
                path.lineTo(x + TICK_LENGTH / 2, sys - 0.866 * TICK_LENGTH);
            }

            if (dia == 0)
            {
                path.moveTo(x - TICK_LENGTH / 2, dia - 0.866 * TICK_LENGTH);
                path.lineTo(x, dia);
                path.lineTo(x + TICK_LENGTH / 2, dia - 0.866 * TICK_LENGTH);
            }
            else
            {
                path.moveTo(x - TICK_LENGTH / 2, dia + 0.866 * TICK_LENGTH);
                path.lineTo(x, dia);
                path.lineTo(x + TICK_LENGTH / 2, dia + 0.866 * TICK_LENGTH);
            }

            path.moveTo(x, sys);
            path.lineTo(x, dia);

            path.moveTo(x - TICK_LENGTH / 2, map);
            path.lineTo(x + TICK_LENGTH / 2, map);
        }
        paths.append(path);
    }
    break;
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP2_SYS:
    {
        int trendNum = 3;       // IBP 动脉压有3个趋势参数
        for (int i = 0; i < trendNum; i++)
        {
            QPainterPath path = getPrintV3Path(axisInfo, graphInfo, i);
            paths.append(path);
        }
    }
    break;
    case SUB_PARAM_ETCO2:
    case SUB_PARAM_ETN2O:
    case SUB_PARAM_ETAA1:
    case SUB_PARAM_ETAA2:
    case SUB_PARAM_ETO2:
    case SUB_PARAM_T1:
    {
        int trendNum = 2;       // 体温和co2有2个趋势参数
        for (int i = 0; i < trendNum; i++)
        {
            QPainterPath path = getPrintV2Path(axisInfo, graphInfo, i);
            paths.append(path);
        }
    }
    break;
    default:
    {
        QPainterPath path;

        QPointF lastPoint;
        bool lastPointInvalid = true;
        QVector<TrendGraphData>::ConstIterator iter =  graphInfo.trendData.constBegin();
        for (; iter != graphInfo.trendData.constEnd(); iter++)
        {
            if (iter->data == InvData())
            {
                if (!lastPointInvalid)
                {
                    path.lineTo(lastPoint);
                    lastPointInvalid = true;
                }
                continue;
            }

            qreal x = timestampToX(iter->timestamp, axisInfo, graphInfo);
            qreal y = mapTrendYValue(iter->data, axisInfo, graphInfo);

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
                    path.lineTo(x, y);
                }
            }

            lastPoint.rx() = x;
            lastPoint.ry() = y;
        }

        if (!lastPointInvalid)
        {
            path.lineTo(lastPoint);
        }

        paths.append(path);
    }
    break;
    }

    return paths;
}

void RecordPageGenerator::drawTrendGraph(QPainter *painter, const GraphAxisInfo &axisInfo,
        const TrendGraphInfo &graphInfo)
{
    painter->save();
    painter->setPen(Qt::white);
    painter->setBrush(Qt::NoBrush);
    QList<QPainterPath> paths = generatorPainterPath(axisInfo, graphInfo);
    QList<QPainterPath>::ConstIterator iter;
    for (iter = paths.constBegin(); iter != paths.constEnd(); iter++)
    {
        painter->save();
        painter->translate(axisInfo.origin);
        painter->drawPath(*iter);
        painter->restore();
    }
    painter->restore();
}

void RecordPageGenerator::drawTrendGraphEventSymbol(QPainter *painter, const GraphAxisInfo &axisInfo, const TrendGraphInfo &graphInfo, const QList<EventInfoSegment> &eventList)
{
    painter->save();
    painter->translate(axisInfo.origin);
    QPen pen(Qt::white, 1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    QFont font = fontManager.recordFont(18);
    painter->setFont(font);
    qreal fontH = fontManager.textHeightInPixels(font);

    int aEventFlagHeight = -300;
    int mEventFlagHeight = aEventFlagHeight - fontH;
    for (int i = 0; i < eventList.count(); ++i)
    {
        QRectF eventRect;
        unsigned eventTime = eventList.at(i).timestamp;
        if (eventTime < graphInfo.startTime || eventTime > graphInfo.endTime)
        {
            continue;
        }
        qreal timeX = timestampToX(eventList.at(i).timestamp, axisInfo, graphInfo);
        if (timeX > axisInfo.width - 10)
        {
            timeX =  axisInfo.width - 10;
        }
        eventRect.setLeft(timeX);
        eventRect.setWidth(axisInfo.xSectionWidth); // should be enough
        eventRect.setHeight(fontH);
        if (eventList.at(i).type == EventPhysiologicalAlarm)
        {
            eventRect.setTop(aEventFlagHeight);
            painter->drawText(eventRect, Qt::AlignLeft | Qt::AlignVCenter, "A");
        }
        else if (eventList.at(i).type != EventOxyCRG)
        {
            eventRect.setTop(mEventFlagHeight);
            painter->drawText(eventRect, Qt::AlignLeft | Qt::AlignVCenter, "M");
        }
    }

    painter->restore();
}

QList<RecordWaveSegmentInfo> RecordPageGenerator::getWaveInfos(const QList<WaveformID> &waves)
{
    QList<RecordWaveSegmentInfo> infos;

    for (int i = 0; i < waves.length(); i++)
    {
        WaveformID id = waves.at(i);
        QString caption = paramInfo.getParamName(paramInfo.getParamID(id));
        int captionLength = 0;
        RecordWaveSegmentInfo info;
        info.id = id;
        info.sampleRate = waveformCache.getSampleRate(id);
        waveformCache.getRange(id, info.minWaveValue, info.maxWaveValue);
        waveformCache.getBaseline(id, info.waveBaseLine);
        switch (id)
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
            info.waveInfo.ecg.gain = ecgParam.getGain(ecgParam.waveIDToLeadID(id));
            caption = QString("%1   %2   %3 %4").arg(ECGSymbol::convert(ecgParam.waveIDToLeadID(id),
                                                                        ecgParam.getLeadConvention()))
                    .arg(trs(ECGSymbol::convert(ecgParam.getFilterMode()))).arg(trs("Notch"))
                    .arg(trs(ECGSymbol::convert(ecgParam.getNotchFilter())));

            info.waveInfo.ecg.in12LeadMode = layoutManager.getUFaceType() == UFACE_MONITOR_ECG_FULLSCREEN;
            info.waveInfo.ecg._12LeadDisplayFormat = ecgParam.get12LDisplayFormat();
            captionLength = fontManager.textWidthInPixels(caption, font());
            break;
        case WAVE_RESP:
            info.waveInfo.resp.zoom = respParam.getZoom();
            break;
        case WAVE_SPO2:
            caption = trs(paramInfo.getParamWaveformName(WAVE_SPO2));
            break;
        case WAVE_CO2:
            info.waveInfo.co2.zoom = co2Param.getDisplayZoom();
            break;
        case WAVE_N2O:
            info.waveInfo.ag.zoom = agParam.getDisplayZoom(AG_TYPE_N2O);
            break;
        case WAVE_AA1:
            info.waveInfo.ag.zoom = agParam.getDisplayZoom(AG_TYPE_AA1);
            break;
        case WAVE_AA2:
            info.waveInfo.ag.zoom = agParam.getDisplayZoom(AG_TYPE_AA2);
            break;
        case WAVE_O2:
            info.waveInfo.ag.zoom = agParam.getDisplayZoom(AG_TYPE_O2);
            break;
        case WAVE_ART:
        case WAVE_PA:
        case WAVE_CVP:
        case WAVE_LAP:
        case WAVE_RAP:
        case WAVE_ICP:
        case WAVE_AUXP1:
        case WAVE_AUXP2:
        {
            info.waveInfo.ibp.pressureName = ibpParam.getPressureName(id);
            caption = IBPSymbol::convert(ibpParam.getPressureName(id));
            IBPScaleInfo scaleInfo = ibpParam.getIBPScale(info.waveInfo.ibp.pressureName);
            info.waveInfo.ibp.high = scaleInfo.high;
            info.waveInfo.ibp.low = scaleInfo.low;
            info.waveInfo.ibp.isAuto = scaleInfo.isAuto;
            info.waveInfo.ibp.unit = paramManager.getSubParamUnit(PARAM_IBP, SUB_PARAM_ART_SYS);
            break;
        }
        default:
            continue;
            break;
        }

        captionLength = fontManager.textWidthInPixels(caption, font());
        info.drawCtx.captionPixLength = captionLength;
        info.drawCtx.caption = caption;
        info.drawCtx.curPageFirstXpos = 0.0;
        info.drawCtx.prevSegmentLastYpos = 0.0;
        info.drawCtx.dashOffset = 0.0;
        info.drawCtx.lastWaveFlags = 0;
        infos.append(info);
    }

    if (infos.size() == 0)
    {
        return infos;
    }


    // calculate the wave region in the print page
    int waveRegionHeight = (RECORDER_PAGE_HEIGHT - RECORDER_WAVE_UPPER_MARGIN - RECORDER_WAVE_LOWER_MARGIN)
                           / infos.size();

    // wave heights when has 3 waves
    int waveHeights[] = {120, 120, 80};

    QList<RecordWaveSegmentInfo>::iterator iter;
    int yOffset = RECORDER_WAVE_UPPER_MARGIN;
    int j = 0;
    for (iter = infos.begin(); iter != infos.end(); ++iter)
    {
        iter->startYOffset = yOffset;
        if (infos.size() == 3)
        {
            yOffset += waveHeights[j++];
        }
        else
        {
            yOffset += waveRegionHeight;
        }

        // 解决打印波形重合问题,两道波形之间增加间隙
        if (iter != infos.end() - 1)
        {
            iter->endYOffset = yOffset - 2;
        }
        else
        {
            iter->endYOffset = yOffset;
        }
        iter->middleYOffset = (iter->startYOffset + iter->endYOffset) / 2;
    }

    return infos;
}

static void drawOxyCRGWaveform(QPainter *painter, const GraphAxisInfo &axisInfo, const OxyCRGWaveInfo &waveInfo)
{
    qreal x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    int width = axisInfo.validWidth;
    int waveNum = waveInfo.waveData.size();

    qreal offsetX = axisInfo.validWidth / waveNum;
    for (int i = 0; i < waveNum; i++)
    {
        unsigned short flag = waveInfo.waveData[i] >> 16;
        // invalid data
        if (flag & INVALID_WAVE_FALG_BIT)
        {
            short wave = waveInfo.waveData[i] && 0xFFFF;
            qreal waveData = mapOxyCRGWaveValue(waveInfo, axisInfo.validHeight, wave);
            y1 = y2 = waveData;

            int j = i + 1;
            while (j < waveNum)
            {
                flag = waveInfo.waveData[j] >> 16;
                if (!(flag & INVALID_WAVE_FALG_BIT))
                {
                    break;
                }

                if (x2 + offsetX > width - 1)
                {
                    break;
                }
                x2 += offsetX;
                j++;
            }
            i = j - 1;

            drawDottedLine(painter, x1, waveData, x2, waveData);

            x1 = x2;
            x2 += offsetX;
            y1 = y2;

            if (x2 > width - 1)
            {
                return;
            }
        }
        else
        {
            short wave = waveInfo.waveData[i] & 0xFFFF;
            qreal waveData = mapOxyCRGWaveValue(waveInfo, axisInfo.validHeight, wave);
            if (i == 0 || ((waveInfo.waveData[i - 1] >> 16) & INVALID_WAVE_FALG_BIT))
            {
                y1 = waveData;
            }

            y2 = waveData;

            QLineF line(x1, y1, x2, y2);
            painter->drawLine(line);
            x1 = x2;
            x2 += offsetX;
            y1 = y2;

            if (x2 > width - 1)
            {
                break;
            }
        }
    }
}

#define AXIS_Y_SECTION_HEIGHT   (3 * RECORDER_PIXEL_PER_MM)
#define AXIS_Y_SECTION_NUM  4
#define AXIS_X_SECTION_WIDTH    (25 * RECORDER_PIXEL_PER_MM)
#define AXIS_X_SECTION_NUM  4
#define GRAPH_SPACING 16
#define GRAPH_START_HEIGHT      (3 * RECORDER_PIXEL_PER_MM + RECORDER_PIXEL_PER_MM / 2)
RecordPage *RecordPageGenerator::createOxyCRGGraph(const QList<TrendGraphInfo> &trendGraphInfo,
        const OxyCRGWaveInfo &waveInfo)
{
    GraphAxisInfo axisInfo;

    axisInfo.ySectionHeight = AXIS_Y_SECTION_HEIGHT;
    axisInfo.ySectionNum = AXIS_Y_SECTION_NUM;
    axisInfo.height = axisInfo.ySectionHeight * axisInfo.ySectionNum;
    axisInfo.validHeight = axisInfo.height;
    axisInfo.marginLeft = AXIS_X_SECTION_WIDTH / 2;
    axisInfo.xSectionWidth = AXIS_X_SECTION_WIDTH;
    axisInfo.xSectionNum = AXIS_X_SECTION_NUM;
    axisInfo.validWidth = axisInfo.xSectionWidth * axisInfo.xSectionNum;
    axisInfo.width = axisInfo.validWidth + axisInfo.xSectionWidth / 2;
    axisInfo.tickLength = RECORDER_PIXEL_PER_MM;
    axisInfo.drawArrow = false;

    RecordPage *page = new RecordPage(axisInfo.marginLeft + axisInfo.width);
    QPainter painter(page);
    painter.setPen(Qt::white);

    int heightOffset = GRAPH_START_HEIGHT;
    // draw  trend graph
    QList<TrendGraphInfo>::ConstIterator iter = trendGraphInfo.constBegin();
    for (; iter != trendGraphInfo.constEnd(); iter++)
    {
        SubParamID subParamID = iter->subParamID;
        QString name;
        if (subParamID == SUB_PARAM_HR_PR)
        {
            name = paramInfo.getSubParamName(SUB_DUP_PARAM_HR);
        }
        else
        {
            name = paramInfo.getSubParamName(subParamID);
        }

        axisInfo.caption = name;
        QStringList yLabels;
        yLabels << QString::number(iter->scale.min)
                << QString() << QString() << QString()
                << QString::number(iter->scale.max);
        axisInfo.yLabels = yLabels;

        axisInfo.origin  = QPointF(axisInfo.marginLeft, heightOffset + axisInfo.height);

        // draw axis
        drawGraphAxis(&painter, axisInfo);

        // draw trendgraph
        drawTrendGraph(&painter, axisInfo, *iter);

        heightOffset += axisInfo.height + GRAPH_SPACING;
    }

    // draw wave
    axisInfo.caption = trs(paramInfo.getParamName(paramInfo.getParamID(waveInfo.id)));
    if (waveInfo.id == WAVE_CO2)
    {
        int high;
        switch (waveInfo.waveInfo.co2.zoom)
        {
        case CO2_DISPLAY_ZOOM_4:
            high = 4;
            break;
        case CO2_DISPLAY_ZOOM_8:
            high = 8;
            break;
        case CO2_DISPLAY_ZOOM_12:
            high = 12;
            break;
        case CO2_DISPLAY_ZOOM_20:
        default:
            high = 20;
        }
        axisInfo.yLabels = QStringList() << QString::number(0) << QString() << QString() << QString()
                           << QString::number(high);
    }
    else
    {
        axisInfo.yLabels.clear();
    }
    axisInfo.xLabels = QStringList() << "-2min" << "-1min" << "0" << "1min" << "2min";
    axisInfo.origin = QPointF(axisInfo.marginLeft, heightOffset + axisInfo.height);

    // draw axis
    drawGraphAxis(&painter, axisInfo);

    // draw the waveform
    painter.translate(axisInfo.origin);
    drawOxyCRGWaveform(&painter, axisInfo, waveInfo);

    return page;
}

RecordPage *RecordPageGenerator::createStringListSegemnt(const QStringList &strList)
{
    QFont font = fontManager.recordFont(24);

    int fontH = fontManager.textHeightInPixels(font);

    int avaliableTextHeight = RECORDER_PAGE_HEIGHT -  fontH;

    int avaliableLine = avaliableTextHeight / fontH;

    int maxStrWidth = fontH;

    for (int i = 0; i < avaliableLine && i < strList.size(); i++)
    {
        int w = fontManager.textWidthInPixels(strList.at(i), font);
        if (w > maxStrWidth)
        {
            maxStrWidth = w;
        }
    }

    RecordPage *page = new RecordPage(maxStrWidth + fontH);

    QPainter painter(page);
    painter.setPen(Qt::white);
    painter.setFont(font);

    QRect rect(fontH, fontH, maxStrWidth, fontH);
    for (int i = 0; i < avaliableLine && i < strList.size(); i++)
    {
        painter.drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, strList.at(i));
        rect.translate(0, fontH);
    }

    return page;
}

RecordPage *RecordPageGenerator::createEndPage()
{
    RecordPage *page = new RecordPage(160);
    return page;
}

void RecordPageGenerator::timerEvent(QTimerEvent *ev)
{
    if (_timer.timerId() == ev->timerId())
    {
        _timer.stop();

        if (_requestStop)
        {
            // we need to check stop condition here because we're unabled to stop if
            // the _generate flag is set to false.
            _requestStop = false;
            emit stopped();
            onStopGenerate();
            return;
        }

        if (!_generate)
        {
            return;
        }

        RecordPage *page = createPage();
        if (page == NULL)
        {
            emit stopped();
            onStopGenerate();
            return;
        }
        else if (_requestStop)
        {
            _requestStop = false;
            delete page;
            emit stopped();
            onStopGenerate();
            return;
        }

        emit generatePage(page);
        qDebug() << "generate page" << page->getID();
        _timer.start(_timerInterval, this);
    }
}
