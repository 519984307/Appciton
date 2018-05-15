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

#define DEFAULT_PAGE_WIDTH 200
RecordPageGenerator::RecordPageGenerator(QObject *parent)
    :QObject(parent), _requestStop(false), _generate(true), _timerID(-1)
{
}

RecordPageGenerator::~RecordPageGenerator()
{
}

int RecordPageGenerator::type() const
{
    return Type;
}

void RecordPageGenerator::start(int interval)
{
    _timerID = startTimer(interval);
}

void RecordPageGenerator::stop()
{
    _requestStop = true;
}

void RecordPageGenerator::pageControl(bool pause)
{
    _generate = !pause;
}

QFont RecordPageGenerator::font() const
{
    return fontManager.recordFont(24);
}

RecordPage *RecordPageGenerator::createPage()
{
    RecordPage *p = new RecordPage(DEFAULT_PAGE_WIDTH);
#if 1
    //draw a sin wave
    qreal yOffset = p->height() / 2;
    qreal xOffset = 2 * 3.1415926 / DEFAULT_PAGE_WIDTH;
    QPainter painter(p);
    painter.setPen(Qt::white);
    static QPointF lastP(0, yOffset);
    QPointF p2;

    for(int i = 0; i< p->width(); i++)
    {
        p2.setX(i);
        p2.setY(yOffset - qSin(i * xOffset) * (p->height() / 2 - 2));
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
    infos.append(QString("%1: %2").arg(trs("Gender")).arg(PatientSymbol::convert(patInfo.sex)));
    infos.append(QString("%1: %2").arg(trs("PatientType")).arg(PatientSymbol::convert(patInfo.type)));
    infos.append(QString("%1: %2").arg(trs("Blood")).arg(PatientSymbol::convert(patInfo.blood)));
    QString str;
    str = QString("%1: ").arg(trs("Age"));
    if(patInfo.age > 0)
    {
        str += QString::number(patInfo.age);
    }
    infos.append(str);

    str = QString("%1: ").arg(trs("Weight"));
    if(patInfo.weight)
    {
        str += QString("%1 %2").arg(QString::number(patInfo.weight)).arg(PatientSymbol::convert(patInfo.weightUnit));
    }
    infos.append(str);

    str = QString("%1: ").arg(trs("Height"));
    if(patInfo.height)
    {
        str += QString::number(patInfo.height);
    }
    infos.append(str);

    infos.append(QString("%1: ").arg(trs("ID")).arg(patInfo.id));

    //calculate the info text width
    int textWidth = 0;
    int w = 0;
    QFont font = fontManager.recordFont(24);
    foreach (QString infoStr, infos) {
        w = fontManager.textWidthInPixels(infoStr, font);
        if(w > textWidth)
        {
            textWidth = w;
        }
    }

    //title width
    w = fontManager.textWidthInPixels(title, font);
    if(w > textWidth)
    {
        textWidth =  w;
    }

    QDateTime dt = QDateTime::currentDateTime();
    if(timestamp)
    {
        dt = QDateTime::fromTime_t(timestamp);
    }

    QString timeStr = QString("%1: %2").arg(trs("RecordTime")).arg(dt.toString("yyyy-MM-dd HH:mm:ss"));

    //record time width
    w = fontManager.textWidthInPixels(timeStr, font);
    if(w > textWidth)
    {
        textWidth =  w;
    }

    int pageWidth = textWidth + font.pixelSize() * 3;
    int fontH = fontManager.textHeightInPixels(font);

    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    painter.setPen(Qt::white);
    painter.setFont(font);

    //we assume the page can hold all the rows
    QRect textRect(font.pixelSize(), fontH, textWidth, fontH);
    painter.drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, title);

    //left one empty row
    textRect.translate(0, fontH/2);

    foreach (QString infoStr, infos) {
        textRect.translate(0, fontH);
        painter.drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, infoStr);
    }

    //recording time in the bottom
    textRect.setTop(page->height() - fontH - fontH/2);
    textRect.setBottom(page->height() - fontH / 2);
    painter.drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, timeStr);

    return page;
}


/**
 * @brief getSubParamNameHelper get the proper sub param name base on the module config
 * @param subParamId    sub param id
 * @param moduleConfig  the module config
 * @return sub param name
 */
static QString getSubParamNameHelper(SubParamID subParamId, short moduleConfig)
{
    if(subParamId == SUB_PARAM_HR_PR && !(moduleConfig & CONFIG_SPO2))
    {
        return trs(paramInfo.getSubParamName(SUB_DUP_PARAM_HR));
    }
    else if (subParamId == SUB_PARAM_RR_BR)
    {
        if ((moduleConfig & CONFIG_CO2) && (moduleConfig & CONFIG_RESP))
        {
            return trs(paramInfo.getSubParamName(SUB_PARAM_RR_BR, true));
        }
        else if ((moduleConfig & CONFIG_CO2))
        {
            return trs(paramInfo.getSubParamName(SUB_DUP_PARAM_BR));
        }
        else
        {
            return trs(paramInfo.getSubParamName(SUB_DUP_PARAM_RR));
        }
    }
    else
    {
        return trs(paramInfo.getSubParamName(subParamId));
    }
}

/**
 * @brief contructNormalTrendStringItem contruct the trend string for params other than IBP or NIBP
 * @param subParamId sub param id
 * @param data the trend value
 * @param unit current display unit
 * @param defaultUnit default unit
 * @param co2Baro
 * @return trend string
 */
static QString contructNormalTrendStringItem(SubParamID subParamId, TrendDataType data,
                                        UnitType unit, UnitType defaultUnit, short co2Bro)
{
    //name
    QString trendString =  getSubParamNameHelper(subParamId, systemManager.getModuleConfig());
    trendString += "\t";

    //value
    int mul = paramInfo.getMultiOfSubParam(subParamId);
    if(data == InvData())
    {
        trendString += InvStr();
    }
    else
    {
        if(1 == mul)
        {
            trendString += Unit::convert(unit, defaultUnit, data, co2Bro);
        }
        else
        {
            trendString += Unit::convert(unit, defaultUnit, data * 1.0 / mul, co2Bro);
        }
    }
    trendString += "\t";

    //unit
    trendString += Unit::localeSymbol(unit);

    return trendString;
}

/**
 * @brief contructPressTrendStringItem contruct the press trend string for the IBP or NIBP
 * @param subParamId
 * @param data
 * @param unit
 * @param defaultUnit
 * @return
 */
static QString contructPressTrendStringItem(SubParamID subParamId, TrendDataType *data,
                                            UnitType unit, UnitType defaultUnit)
{
    QString trendString;
    Q_ASSERT(data != NULL);
    //name
    if (subParamId >= SUB_PARAM_ART_SYS && subParamId <= SUB_PARAM_AUXP2_SYS)
    {
        trendString = paramInfo.getIBPPressName(subParamId);
    }
    else if(subParamId >= SUB_PARAM_NIBP_SYS && subParamId <= SUB_PARAM_NIBP_MAP)
    {
        trendString = paramInfo.getParamName(PARAM_NIBP);
    }
    else
    {
        //should not get here
        return trendString;
    }
    trendString += "\t";


    //get value

    TrendDataType sys = InvData();
    TrendDataType dia = InvData();
    TrendDataType map = InvData();
    //have 3 press value by default
    int valueNum = 1;

    switch(subParamId)
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
    if(valueNum == 1)
    {
        valueStr="(%1)";
        if(map == InvData())
        {
            valueStr = valueStr.arg(InvData());
        }
        else
        {
            int mul = paramInfo.getMultiOfSubParam(subParamId);
            if(mul == 1)
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
        valueStr= "%1/%2(%3)";
        if(sys == InvData())
        {
            valueStr = valueStr.arg(InvStr()).arg(InvStr()).arg(InvStr());
        }
        else
        {
            int mul = paramInfo.getMultiOfSubParam(subParamId);
            if(mul == 1)
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


    //unit

    trendString += Unit::localeSymbol(unit);

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
static void converToStringSegmets(const QStringList &trendStringList, QList<TrendStringSegmentInfo>& strSegInfoList, const QFont &font)
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
        segInfo.unitSegmentWidth = fontManager.textWidthInPixels(segInfo.unitSegment, font)+ font.pixelSize();
        strSegInfoList.append(segInfo);
    }
}

RecordPage *RecordPageGenerator::createTrendPage(const TrendDataPackage &trendData, bool showEventTime)
{
    QStringList trendStringList = getTrendStringList(trendData);

    QString timeStr;
    if(showEventTime)
    {
        QDateTime dt = QDateTime::fromTime_t(trendData.time);
        timeStr = QString("%1: %2").arg(trs("EventTime")).arg(dt.toString("yyyy-MM-dd HH:mm:ss"));
    }

    QFont font = fontManager.recordFont(24);

    int fontH = fontManager.textHeightInPixels(font);

    int avaliableTextHeight = RECORDER_PAGE_HEIGHT -  2 * fontH - fontH / 2;


    // if the contains too many lines, need to seperate into several group
    QVector<int> segmentWidths; //record the maximum widht or the group segments
    int avaliableLine = avaliableTextHeight / fontH;
    QList<TrendStringSegmentInfo> strSegInfoList;
    converToStringSegmets(trendStringList, strSegInfoList, font);
    int index = -1;
    int n = 0;
    foreach (TrendStringSegmentInfo segInfo, strSegInfoList) {
        if(n % avaliableLine == 0)
        {
            //each line contain 3 segments
            segmentWidths.append(0);
            segmentWidths.append(0);
            segmentWidths.append(0);
            index++;
        }
        n++;
        if(segInfo.nameSegmentWidth > segmentWidths[index * 3])
        {
            segmentWidths[index * 3] = segInfo.nameSegmentWidth;
        }
        if(segInfo.valueSegmentWidth> segmentWidths[index * 3 + 1])
        {
            segmentWidths[index * 3 + 1] = segInfo.valueSegmentWidth;
        }
        if(segInfo.unitSegmentWidth > segmentWidths[index * 3 + 2])
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
    if(showEventTime && pageWidth < fontManager.textWidthInPixels(timeStr))
    {
        pageWidth = fontManager.textWidthInPixels(timeStr);
    }

    // add the gap between group
    pageWidth += (segmentWidths.size() / 3) * font.pixelSize() + font.pixelSize() * 2;

    // Do the drawing stuff
    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    painter.setPen(Qt::white);
    painter.setFont(font);

    QTextOption textOption;
    textOption.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

    int xoffset = font.pixelSize();
    for(int i = 0; i<segmentWidths.size(); i+=3)
    {
        QRectF nameRect(xoffset, fontH, segmentWidths[i], fontH);
        xoffset += segmentWidths[i];
        QRectF valueRect(xoffset, fontH, segmentWidths[i+1], fontH);
        xoffset += segmentWidths[i+1];
        QRectF unitRect(xoffset, fontH, segmentWidths[i+2], fontH);
        xoffset += segmentWidths[i+2];
        TrendStringSegmentInfo seg;
        for(int lines = 0; lines < avaliableLine; lines++)
        {
            if(strSegInfoList.isEmpty())
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


    if(showEventTime)
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
    switch (SubParamID) {
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
    for(int i = 0; i < SUB_PARAM_NR; i++)
    {
        subparamID = (SubParamID)i;
        if(!trendData.subparamValue.contains(subparamID))
        {
            continue;
        }

        paramid = paramInfo.getParamID(subparamID);
        if(!isPressSubParam(subparamID))
        {
            strList.append(contructNormalTrendStringItem(subparamID,
                                                         trendData.subparamValue[subparamID],
                                                         paramManager.getSubParamUnit(paramid, subparamID),
                                                        paramInfo.getUnitOfSubParam(subparamID),
                                                        trendData.co2Baro));
        }
        else
        {
            TrendDataType data[3] = {InvData()};
            bool handle = true;
            switch (subparamID) {
            case SUB_PARAM_NIBP_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_NIBP_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_NIBP_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_NIBP_MAP, InvData());
                break;
            case SUB_PARAM_ART_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_ART_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_ART_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_ART_MAP, InvData());
                break;
            case SUB_PARAM_PA_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_PA_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_PA_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_PA_MAP, InvData());
                break;
            case SUB_PARAM_CVP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_CVP_MAP, InvData());
                break;
            case SUB_PARAM_LAP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_LAP_MAP, InvData());
                break;
            case SUB_PARAM_RAP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_RAP_MAP, InvData());
                break;
            case SUB_PARAM_ICP_MAP:
                data[0] = trendData.subparamValue.value(SUB_PARAM_ICP_MAP, InvData());
                break;
            case SUB_PARAM_AUXP1_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_AUXP1_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_AUXP1_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_AUXP1_MAP, InvData());
                break;
            case SUB_PARAM_AUXP2_SYS:
                data[0] = trendData.subparamValue.value(SUB_PARAM_AUXP2_SYS, InvData());
                data[1] = trendData.subparamValue.value(SUB_PARAM_AUXP2_DIA, InvData());
                data[2] = trendData.subparamValue.value(SUB_PARAM_AUXP2_MAP, InvData());
                break;
            default:
                handle = false;
                break;
            }

            if(!handle)
            {
               continue;
            }
            strList.append(contructPressTrendStringItem(subparamID,
                                                        data,
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
 */
static void drawECGGain(RecordPage *page, QPainter *painter, const RecordWaveSegmentInfo &waveInfo)
{
    int rulerHeight = 10 * RECORDER_PIXEL_PER_MM; // height for 1.0 ECG gain
    QString str = "1 mV";
    switch(waveInfo.waveInfo.ecg.gain)
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
    // draw at the baseline
    //int yBottom = waveInfo.startYOffset + (waveInfo.endYOffset - waveInfo.startYOffset) / 2;
    // draw at the bottom
    int yBottom = waveInfo.endYOffset;
    QPainterPath path;
    path.moveTo(0, yBottom);
    path.lineTo(pageWidth / 3, yBottom);
    path.lineTo(pageWidth / 3, yBottom - rulerHeight);
    path.lineTo(pageWidth * 2 / 3, yBottom - rulerHeight);
    path.lineTo(pageWidth * 2 / 3, yBottom);
    path.lineTo(pageWidth, yBottom);
    painter->save();
    QPen p(Qt::white, 1);
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
//draw a percent style ruler
static void drawPercentRuler(RecordPage *page, QPainter *painter, const RecordWaveSegmentInfo &waveInfo,
                             const QString &high, const QString &low)
{
    QPainterPath path;
    path.moveTo(RULER_TICK_LEN, waveInfo.startYOffset);
    path.lineTo(0, waveInfo.startYOffset);
    path.lineTo(0, waveInfo.endYOffset);
    path.lineTo(RULER_TICK_LEN, waveInfo.endYOffset);

    painter->save();

    QPen p(Qt::white, 1);
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
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, "%");

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

    QPen p(Qt::white, 1);
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
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, Unit::localeSymbol(waveInfo.waveInfo.ibp.unit));

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
    Q_ASSERT(waveInfos.size() > 0);
    int pageWidth = 25 * RECORDER_PIXEL_PER_MM;

    RecordPage *page = new RecordPage(pageWidth);
    QPainter painter(page);
    painter.setPen(Qt::white);
    QFont font = fontManager.recordFont(24);
    painter.setFont(font);

    int fontH = fontManager.textHeightInPixels(font);

    //draw the print speed
    QRect rect(0, fontH / 2, page->width(), fontH);
    painter.drawText(rect, Qt::AlignLeft|Qt::AlignVCenter, PrintSymbol::convert(speed));

    QList<RecordWaveSegmentInfo>::ConstIterator iter;
    for(iter = waveInfos.constBegin(); iter != waveInfos.constEnd(); iter++)
    {
        switch(iter->id)
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
            drawECGGain(page, &painter, *iter);
            break;
        case WAVE_RESP:
            drawRespZoom(page, &painter, *iter);
            break;
        case WAVE_SPO2:
            break;
        case WAVE_CO2:
        {
            int high;
            CO2DisplayZoom zoom = iter->waveInfo.co2.zoom;
            switch (zoom) {
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

            drawPercentRuler(page, &painter, *iter, QString().sprintf("%.01f", (double)high), "0");
        }
            break;
        case WAVE_N2O:
        case WAVE_AA1:
        case WAVE_AA2:
        case WAVE_O2:
        {
            int high;
            AGDisplayZoom zoom = iter->waveInfo.ag.zoom;
            switch (zoom) {
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
            drawPercentRuler(page, &painter, *iter, QString().sprintf("%.01f", (double)high), "0");
        }
            break;

        case WAVE_IBP1:
        case WAVE_IBP2:
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
    if(page->width() * (segIndex) >= waveInfo.drawCtx.captionPixLength)
    {
        //the caption has heen completely drawn
        return;
    }

    int fontH = fontManager.textHeightInPixels(painter->font());
    QRect rect(0, waveInfo.startYOffset, waveInfo.drawCtx.captionPixLength, fontH);
    painter->save();
    painter->setPen(Qt::white);
    painter->translate(-segIndex * page->width(), 0);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, waveInfo.drawCtx.caption);
    painter->restore();
}

/**
 * @brief mapWaveValue map the wave value
 * @param waveInfo the wave segment info
 * @param wave input wave value
 * @return  output wave value
 */
static qreal mapWaveValue(const RecordWaveSegmentInfo & waveInfo, short wave)
{
    qreal waveData = 0.0;

    qreal startY = waveInfo.startYOffset;
    qreal endY = waveInfo.endYOffset;

    switch(waveInfo.id)
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
        switch(waveInfo.waveInfo.ecg.gain)
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
        waveData = (waveInfo.maxWaveValue - wave) *((endY - startY) / (waveInfo.maxWaveValue - waveInfo.minWaveValue)) + startY;
        break;
    }

    case WAVE_RESP:
    {
        qreal respZoom = 1.0;
        switch(waveInfo.waveInfo.resp.zoom)
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
        waveData = (waveInfo.maxWaveValue -  zoomWave) * ((endY -  startY) / (waveInfo.maxWaveValue -  waveInfo.minWaveValue)) + startY;
        break;
    }

    case WAVE_SPO2:
    {
        qreal spo2Zoom = 1.0;
        switch (waveInfo.waveInfo.spo2.gain) {
        case SPO2_GAIN_X10:
            spo2Zoom = 1.0;
            break;
        case SPO2_GAIN_X20:
            spo2Zoom = 2.0;
            break;
        case SPO2_GAIN_X40:
            spo2Zoom = 4.0;
            break;
        case SPO2_GAIN_X80:
            spo2Zoom = 8.0;
            break;
        default:
            break;
        }

        short zoomWave = (wave - waveInfo.waveBaseLine) * spo2Zoom + waveInfo.waveBaseLine;
        waveData = (waveInfo.maxWaveValue -  zoomWave) * ((endY -  startY) / (waveInfo.maxWaveValue -  waveInfo.minWaveValue)) + startY;
        break;
    }

    case WAVE_CO2:
    {
        int max = waveInfo.maxWaveValue;
        switch(waveInfo.waveInfo.co2.zoom)
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
        switch(waveInfo.waveInfo.ag.zoom)
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

    case WAVE_IBP1:
    case WAVE_IBP2:
    {
        int max = waveInfo.waveInfo.ibp.high * 10;
        int min = waveInfo.waveInfo.ibp.low * 10;
        waveData = (max - wave) * (endY -  startY) / (max -  min) + startY;
        break;
    }

    default:
        break;
    }

    //cut off
    if(waveData < waveInfo.startYOffset)
    {
        waveData = waveInfo.startYOffset;
    }
    else if(waveData > waveInfo.endYOffset)
    {
        waveData = waveInfo.endYOffset;
    }

    return waveData;
}

/**
 * @brief drawDottedLine draw a dotted line
 * @param painter paitner
 * @param x1 xPos of point 1
 * @param y1 yPos of point 1
 * @param x2 xPos of point 2
 * @param y2 yPos of point 2
 */
static void drawDottedLine(QPainter *painter, qreal x1, qreal y1,
                           qreal x2, qreal y2)
{
    painter->save();
    QVector<qreal> darsh;
    darsh << 5 << 5;
    QPen pen(Qt::white);
    pen.setDashPattern(darsh);
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
    qreal offsetX = pageWidth * 1.0 / waveInfo.waveNum;
    int i;
    for(i = 0; i < waveInfo.waveNum; i++)
    {
        unsigned short flag = waveInfo.waveBuff[i] >> 16;
        //invalid data
        if(flag & INVALID_WAVE_FALG_BIT)
        {
            if(i == 0)
            {
                //start point
                x1 = 0;
                x2 = waveInfo.drawCtx.curPageFirstXpos;
            }
            short wave = waveInfo.waveBuff[i] & 0xFFFF;
            double waveData = mapWaveValue(waveInfo, wave);
            y1 = y2 = waveData;

            int j = i + 1;
            while(j < waveInfo.waveNum)
            {
                flag = waveInfo.waveBuff[j] >> 16;
                if(!(flag & INVALID_WAVE_FALG_BIT))
                {
                    break;
                }

                if(x2 + offsetX > pageWidth - 1)
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

            waveInfo.drawCtx.lastWaveFlags = flag;

            if (x2 > pageWidth - 1)
            {
                QLineF line(x1, y1, pageWidth, y1);
                painter->drawLine(line);
                waveInfo.drawCtx.prevSegmentLastYpos = y1;
                waveInfo.drawCtx.curPageFirstXpos = x2 - pageWidth + 1;
                break;
            }
        }
        else //valid wave data
        {
            short wave = waveInfo.waveBuff[i] & 0xFFFF;
            if(waveInfo.id == WAVE_ECG_aVR && waveInfo.waveInfo.ecg.in12LeadMode
                    && waveInfo.waveInfo.ecg._12LeadDisplayFormat == DISPLAY_12LEAD_CABRELA)
            {
                wave = - wave;
            }

            double waveData = mapWaveValue(waveInfo, wave);

            if(i==0)
            {
                if(segmentIndex == 0)
                {
                    y1 = waveData;
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
                if((waveInfo.waveBuff[i - 1] >> 16) & INVALID_WAVE_FALG_BIT)
                {
                    y1 = waveData;
                }
            }

            y2 = waveData;
            QLineF line(x1, y1, x2, y2);
            painter->drawLine(line);

            x1 = x2;
            x2 += offsetX;
            y1 = y2;

            waveInfo.drawCtx.lastWaveFlags = flag;

            if(x2 > pageWidth - 1)
            {
                break;
            }
        }

    }

    waveInfo.drawCtx.curPageFirstXpos=0;
    if (x2 > pageWidth - 1)
    {
        bool drawLine = false;
        QLineF line;
        y2 = y1;
        if(i + 1 < waveInfo.waveNum)
        {
            if(!(waveInfo.drawCtx.lastWaveFlags & INVALID_WAVE_FALG_BIT))
            {
                short wave = waveInfo.waveBuff[i + 1] & 0xFFFF;
                if(waveInfo.id == WAVE_ECG_aVR && waveInfo.waveInfo.ecg.in12LeadMode
                        && waveInfo.waveInfo.ecg._12LeadDisplayFormat == DISPLAY_12LEAD_CABRELA)
                {
                    wave = -wave;
                }

                //当x1在页宽范围内，x2不在页宽范围内时，通过计算两点连线的斜率来判断x坐标为页宽时点的y坐标，并进行连线
                qreal tmpValue = mapWaveValue(waveInfo, wave);
                qreal deltaY = (tmpValue * 1000 - y1 * 1000) / 1000.0 * (pageWidth * 1.0 - 1.0 - x1) / offsetX;
                y2 = y1 + deltaY;
                line.setLine(x1, y1, pageWidth - 1, y2);
                drawLine = true;
            }
        }
        else
        {
            drawLine = true;
            line.setLine(x1, y1, pageWidth - 1, y2);
        }

        if(drawLine)
        {
            painter->drawLine(line);
            waveInfo.drawCtx.curPageFirstXpos = x2 - pageWidth + 1;
        }
    }
    else
    {
        if(!(waveInfo.drawCtx.lastWaveFlags & INVALID_WAVE_FALG_BIT))
        {
            //draw to the edge
            y2 = y1;
            QLineF line(x1, y1, pageWidth -  1, y2);
            painter->drawLine(line);
        }
    }
    waveInfo.drawCtx.prevSegmentLastYpos = y2;
}

RecordPage *RecordPageGenerator::createWaveSegments(QList<RecordWaveSegmentInfo> &waveInfos, int segmentIndex, PrintSpeed speed)
{
    int pageWidth = 25 * RECORDER_PIXEL_PER_MM;
    switch(speed)
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
    painter.setPen(Qt::white);
    QFont font = fontManager.recordFont(24);
    painter.setFont(font);

    QList<RecordWaveSegmentInfo>::iterator iter = waveInfos.begin();
    for(;iter != waveInfos.end(); iter++)
    {
        drawCaption(page, &painter, *iter, segmentIndex);
        drawWaveSegment(page, &painter, *iter, segmentIndex);
    }
    return page;
}

void RecordPageGenerator::timerEvent(QTimerEvent *ev)
{
    if(_timerID == ev->timerId())
    {
        if(_requestStop)
        {
            killTimer(_timerID);
            _timerID = -1;
            _requestStop = false;
            emit stop();
            return;
        }

        if(!_generate)
        {
            return;
        }

        RecordPage *page = createPage();
        if(page == NULL)
        {
            killTimer(_timerID);
            emit stop();
            return;
        }

        emit generatePage(page);
    }
}
