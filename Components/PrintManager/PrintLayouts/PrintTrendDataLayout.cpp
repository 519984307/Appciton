#include "PrintTrendDataLayout.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "FontManager.h"
#include "TimeManager.h"
#include "TimeDate.h"
#include "CircleFile.h"
#include "TimeSymbol.h"
#include "DataStorageDirManager.h"
#include "ParamManager.h"
#include "TEMPParam.h"
#include "PrintManager.h"
#include <math.h>
#include <QFontMetrics>
#include "SystemManager.h"

/**************************************************************************************************
 * 构造
 *************************************************************************************************/
PrintTrendDataLayout::PrintTrendDataLayout() : PrintLayoutIFace(PRINT_LAYOUT_ID_TREND_REPORT)
{
    _index = -1;
    _totalData = 0;
    _filePath.clear();
    _paramList.clear();
    _file = NULL;
    _printStartPage = true;
}

/**************************************************************************************************
 * 析构
 *************************************************************************************************/
PrintTrendDataLayout::~PrintTrendDataLayout()
{
    if (NULL != _file)
    {
        delete _file;
    }

    _filePath.clear();
    _paramList.clear();
}

/**************************************************************************************************
 * 开始布局
 *************************************************************************************************/
void PrintTrendDataLayout::start()
{
//    trendDataWidget.getPrintPath(_filePath);
    if (0 == _filePath.count())
    {
        return;
    }

    _fileNum = 0;
//    _interval = trendDataWidget.getInterval();
//    _type = (RescueDataPrintType)trendDataWidget.getPrintIndex();

    QString date;
    timeDate.getDate(0, date, true);
    _timeWidth = fontManager.textWidthInPixels(date, printFont());
    _nibpParamWidth = 11 * fontManager.textWidthInPixels("M", printFont());
    _otherParamWidth = 6 * fontManager.textWidthInPixels("M", printFont());

    printManager.enablePrinterSpeed(printManager.getPrintSpeed());
}

/**************************************************************************************************
 * 终止
 *************************************************************************************************/
void PrintTrendDataLayout::abort()
{
    _paramList.clear();
    _filePath.clear();

    if (NULL != _file)
    {
        delete _file;
        _file = NULL;
    }

    _fileNum = 0;
    _index = -1;
}

/**************************************************************************************************
 * 构建打印页
 *************************************************************************************************/
PrintPage *PrintTrendDataLayout::exec(PrintContent */*content*/)
{
    switch (_type)
    {
        case PRINT_SELECT_RESCUE:
        case PRINT_RANGE_RESCUE:
            return _rescuePage();
            break;
        default:
            return NULL;
    }
}

/**************************************************************************************************
 * 数据初始化
 *************************************************************************************************/
bool PrintTrendDataLayout::_dataInit()
{
    if (_index < 0)
    {
        while (_fileNum < _filePath.count())
        {
            QString fileStr = DATA_STORE_PATH + _filePath.at(_fileNum++)
                    + PARAM_DATA_FILE_NAME;

            if (NULL != _file)
            {
                _file->reload(fileStr, QIODevice::ReadOnly);
            }
            else
            {
                _file = StorageManager::open(fileStr, QIODevice::ReadOnly);
            }

            if(!_file->isValid())
            {
                qdebug("Invalid trend file, skip..\n");
                continue;
            }

            _file->readAdditionalData((char*)&_descInfo, sizeof(ParamDataDescription));

            //contruct paramlist
            _paramList.clear();
            _paramList.append(SUB_PARAM_HR_PR);  //always exist hr
            if (_descInfo.moduleConfig & (CONFIG_CO2 | CONFIG_RESP))
            {
                _paramList.append(SUB_PARAM_RR_BR);
            }
            if(_descInfo.moduleConfig & CONFIG_NIBP)
            {
                _paramList.append(SUB_PARAM_NIBP_SYS); //only add sys for nibp
            }
            if(_descInfo.moduleConfig & CONFIG_SPO2)
            {
                _paramList.append(SUB_PARAM_SPO2);
            }
            if(_descInfo.moduleConfig & CONFIG_TEMP)
            {
                _paramList.append(SUB_PARAM_T1);
                _paramList.append(SUB_PARAM_T2);
                _paramList.append(SUB_PARAM_TD);
            }
            if(_descInfo.moduleConfig & CONFIG_CO2)
            {
                _paramList.append(SUB_PARAM_ETCO2);
                _paramList.append(SUB_PARAM_FICO2);
            }
            paramInfo.sortSubParamId(_paramList);

            _totalData = _file->getBlockNR();

            if (_totalData > 0)
            {
                _printStartPage = true;
                _index = _totalData - 1;
                return true;
            }
            else if(_totalData == 0)
            {
                _printStartPage = true;
                _index = 0;
                return true;
            }
        }

        if (_index < 0)
        {
            //no more incident found
            return false;
        }
    }

    return true;
}

/**************************************************************************************************
 * 画数据头
 *************************************************************************************************/
int PrintTrendDataLayout::_drawRescuePageTitle(PrintPage *page, unsigned int timestamp)
{
    if (NULL == page)
    {
        return 0;
    }

    QPainter painter(page);
    painter.setFont(printFont());
    painter.setPen(Qt::white);

    int total = _paramList.count();
    if (0 == total)
    {
        return 0;
    }

    int xoffset = fontHeight();
    int yoffset = fontHeight();
    QString text = trs("Time") + ":";
    painter.drawText(xoffset, yoffset, text);
    timeDate.getDate(timestamp, text, true);
    painter.drawText(xoffset, yoffset + fontHeight(), text);
    QString unitStr;

    xoffset += _timeWidth + fontHeight();
    for (int i = 0; i < total; ++i)
    {
        SubParamID id = _paramList.at(i);
        // NIBP和RR特殊处理，因为他们的数据时合并显示的。
        text = trs(paramInfo.getSubParamName(id, true));
        unitStr = Unit::localeSymbol(paramManager.getSubParamUnit(paramInfo.getParamID(id), id));
        if (id == SUB_PARAM_NIBP_SYS)
        {
            text = trs(paramInfo.getParamName(PARAM_NIBP));
        }
        else if (id == SUB_PARAM_HR_PR)
        {
            if(!(_descInfo.moduleConfig & CONFIG_SPO2))
            {
                text = trs(paramInfo.getSubParamName(SUB_DUP_PARAM_HR));
            }
        }
        else if( id == SUB_PARAM_RR_BR)
        {
            if ((_descInfo.moduleConfig & CONFIG_CO2) && (_descInfo.moduleConfig & CONFIG_RESP))
            {
                text = trs(paramInfo.getSubParamName(SUB_PARAM_RR_BR, true));
            }
            else if ((_descInfo.moduleConfig & CONFIG_CO2))
            {
                text = trs(paramInfo.getSubParamName(SUB_DUP_PARAM_BR));
            }
            else
            {
                text = trs(paramInfo.getSubParamName(SUB_DUP_PARAM_RR));
            }
        }

        painter.drawText(xoffset, yoffset, text);
        painter.drawText(xoffset, yoffset + fontHeight(), QString("(%1)").arg(unitStr));

        if (id != SUB_PARAM_NIBP_SYS)
        {
            xoffset += _otherParamWidth + fontHeight();
        }
        else
        {
            xoffset += _nibpParamWidth + fontHeight();
        }
    }

    return (2 * fontHeight());
}

/**************************************************************************************************
 * 画数据头
 * high:本次绘画的y坐标
 *************************************************************************************************/
int PrintTrendDataLayout::_drawRescuePageData(PrintPage *page, ParamDataStorageManager::ParamBuf &buf, int high)
{
    if (NULL == page)
    {
        return 0;
    }

    QPainter painter(page);
    painter.setFont(printFont());
    painter.setPen(Qt::white);

    int total = _paramList.count();
    if (0 == total)
    {
        return 0;
    }

    int fontH = fontHeight();
    QString text;
    QString t1,t2;
    timeDate.getTime(buf.item.t, text, true);
    painter.drawText(fontH, high, text);
    bool alarmFlag = false;

    int xoffset = fontH + _timeWidth + fontH;
    for (int i = 0; i < total; ++i)
    {
        alarmFlag = false;
        text.clear();
        SubParamID id = _paramList.at(i);
        UnitType defaultType = paramInfo.getUnitOfSubParam(id);
        UnitType unit = paramManager.getSubParamUnit(paramInfo.getParamID(id), id);
        int mul = paramInfo.getMultiOfSubParam(id);
        if (SUB_PARAM_NIBP_SYS == id)
        {
            if (!buf.item.isNibpShowMeasureValue)
            {
                text.sprintf("%s/%s (%s)", InvStr(), InvStr(), InvStr());
            }
            else
            {
                QString sys, dia,map;
                if (defaultType == unit)
                {
                    sys.sprintf("%d", (int)(buf.paramItem[SUB_PARAM_NIBP_SYS].value * 1.0 / mul));
                    dia.sprintf("%d", (int)(buf.paramItem[SUB_PARAM_NIBP_DIA].value * 1.0 / mul));
                    map.sprintf("%d", (int)(buf.paramItem[SUB_PARAM_NIBP_MAP].value * 1.0 / mul));
                }
                else
                {
                    sys = Unit::convert(unit, defaultType,
                        buf.paramItem[SUB_PARAM_NIBP_SYS].value * 1.0 / mul);
                    dia = Unit::convert(unit, defaultType,
                        buf.paramItem[SUB_PARAM_NIBP_DIA].value * 1.0 / mul);
                    map = Unit::convert(unit, defaultType,
                        buf.paramItem[SUB_PARAM_NIBP_MAP].value * 1.0 / mul);
                }

                text = sys;
                if (0 != buf.paramItem[SUB_PARAM_NIBP_SYS].alarmFlag)
                {
                    text += "*/";
                }
                else
                {
                    text += "/";
                }

                text += dia;
                if (0 != buf.paramItem[SUB_PARAM_NIBP_DIA].alarmFlag)
                {
                    text += "*";
                }

                text += " (";
                text += map;
                if (0 != buf.paramItem[SUB_PARAM_NIBP_MAP].alarmFlag)
                {
                    text += "*";
                }
                text += ")";
            }
        }
        else
        {
            if (buf.paramItem[id].value == UnknownData())
            {
                text = UnknownStr();
            }
            else if (buf.paramItem[id].value == InvData())
            {
                text = InvStr();
            }
            else
            {
                if (unit == defaultType)
                {
                    if (1 == mul)
                    {
                        text.sprintf("%d", buf.paramItem[id].value);
                    }
                    else
                    {
                        if (id == SUB_PARAM_T1 || id == SUB_PARAM_T2)
                        {
                            if (buf.paramItem[id].value > 500)
                            {
                                text = "> 50";
                            }
                            else if (buf.paramItem[id].value < 0)
                            {
                                text = "< 0";
                            }
                            else
                            {
                                text.sprintf("%.1f", buf.paramItem[id].value * 1.0 / mul);
                            }
                        }
                        else
                        {
                            text.sprintf("%.1f", buf.paramItem[id].value * 1.0 / mul);
                        }
                    }
                }
                else
                {
                    if (id == SUB_PARAM_T1 || id == SUB_PARAM_T2)
                    {
                        if (buf.paramItem[id].value > 500)
                        {
                            text = "> " + Unit::convert(unit, defaultType, 50);
                        }
                        else if (buf.paramItem[id].value < 0)
                        {
                            text = "< " + Unit::convert(unit, defaultType, 0);
                        }
                        else
                        {
                            text = Unit::convert(unit, defaultType,
                                                 buf.paramItem[id].value * 1.0 / mul, buf.item.co2Baro);
                        }
                    }
                    else if (id == SUB_PARAM_TD)
                    {
                        t1 = Unit::convert(unit, defaultType,buf.paramItem[SUB_PARAM_T1].value * 1.0 / mul);
                        t2 = Unit::convert(unit, defaultType,buf.paramItem[SUB_PARAM_T2].value * 1.0 / mul);
                        text.sprintf("%.1f", fabs(t1.toDouble() - t2.toDouble()));
                    }
                    else
                    {
                        text = Unit::convert(unit, defaultType,
                                             buf.paramItem[id].value * 1.0 / mul, buf.item.co2Baro);
                    }
                }
                alarmFlag = buf.paramItem[id].alarmFlag;
            }

            if (0 != alarmFlag)
            {
                text += "*";
            }
        }

        painter.drawText(xoffset, high, text);
        if (id != SUB_PARAM_NIBP_SYS)
        {
            xoffset += _otherParamWidth + fontH;
        }
        else
        {
            xoffset += _nibpParamWidth + fontH;
        }
    }

    return fontH;
}

/**************************************************************************************************
 * 画完成信息
 * high:本次绘画的y坐标
 *************************************************************************************************/
void PrintTrendDataLayout::_drawCompleteInfo(PrintPage *page, int high)
{
    if (NULL == page)
    {
        return;
    }

    QRect r = page->rect();
    QPainter painter(page);
    painter.setFont(printFont());
    painter.setPen(Qt::white);

    QString text = trs("TrendReportComplete");
    QRect rect(QPoint(0, high), QSize(r.width(), r.height() - high));
    QRect boundingRect;
    painter.drawText(rect, Qt::AlignTop | Qt::AlignHCenter, text, &boundingRect);
}

/**************************************************************************************************
 * 构建数据页
 *************************************************************************************************/
PrintPage *PrintTrendDataLayout::_rescuePage()
{
    if (!_dataInit())
    {
        return NULL;
    }

    if(NULL == _file)
    {
        return NULL;
    }

    //打印开始页
    if (_printStartPage)
    {
        _printStartPage = false;
        return _startPage(_descInfo);
    }

    int fontH = fontHeight();
    int maxWidth = 0;
    if (_paramList.indexOf(SUB_PARAM_NIBP_SYS) >= 0)
    {
        maxWidth = fontH * (_paramList.count() + 2) + _timeWidth + (_paramList.count() - 1) * _otherParamWidth +
                _nibpParamWidth;
    }
    else
    {
        maxWidth = fontH * (_paramList.count() + 2) + _timeWidth + _paramList.count() * _otherParamWidth;
    }
    PrintPage *page = new PrintPage(maxWidth);
    static int id = 0;
    page->setID("TrendData" + QString::number(id++));
    int high = page->rect().height() - fontH;
    int py = fontH;

    //画数据头
    int titleHigh = 2*fontH; //title height, place holder, draw later
    high -= titleHigh;
    py += titleHigh;

    ParamDataStorageManager::ParamBuf itemBuf;
    bool isValid = false;

    int index = 0;
    static unsigned startTime30sAlign = 0;
    unsigned firstTrendDataTime = 0;

    while (high >= fontH && _index >= 0)
    {
        // 寻找下一个有效间隔数据，报警和NIBP测量数据不计算间隔
        while(_index >= 0)
        {
            index = _index;
            --_index;
            isValid = _file->readBlockData(index, (char *)&itemBuf, sizeof(itemBuf));

            if (!isValid)
            {
                break;
            }

            //alarm and nibp, always display
            if (itemBuf.item.isAlarm || itemBuf.item.isNibpShowMeasureValue)
            {
                //the first item might be alarm or nibp complete item
                if(startTime30sAlign == 0 && (itemBuf.item.t % 30) == 0)
                {
                    startTime30sAlign = 0;
                }
                break;
            }

            //update 30s align start time
            if (startTime30sAlign == 0)
            {
                startTime30sAlign = itemBuf.item.t;
                break;
            }

            if(0 == (qAbs(startTime30sAlign - itemBuf.item.t) % (_interval * 30)))
            {
                break;
            }

            //last trend data, but not match the interval
            if (index == 0)
            {
                isValid = false;
            }
        }

        if (!isValid)
        {
            _index = -1;
            break;
        }

        //_file->readBlockData(index, (char *)&itemBuf, sizeof(itemBuf));
        if(firstTrendDataTime == 0)
        {
            firstTrendDataTime = itemBuf.item.t;
        }

        int dataHigh = _drawRescuePageData(page, itemBuf, py);
        high -= dataHigh;
        py += dataHigh;
    }


    if(firstTrendDataTime == 0)
    {
        //this case happen when no trend data in the incident,
        //set the value to the incident start time
        firstTrendDataTime = _descInfo.startTime;
    }

    //draw title
    _drawRescuePageTitle(page, firstTrendDataTime);

    //last one might be unprintable, do some check to avoid start a new page
    if (_index <= _interval)
    {
        index = _index;
        while(index >= 0)
        {
            isValid = _file->readBlockData(index, (char *)&itemBuf, sizeof(itemBuf));
            if(!isValid)
            {
                index = -1;
                break;
            }
            else if (itemBuf.item.isAlarm || itemBuf.item.isNibpShowMeasureValue)
            {
                break;
            }
            else if( 0 == qAbs(startTime30sAlign - itemBuf.item.t) % (_interval * 30))
            {
                break;
            }
            index --;
        }

        //left items will not print, print is finished
        if(index < 0)
        {
            _index = -1;
        }
    }

    if(_index < 0)
    {
        startTime30sAlign = 0;
        _drawCompleteInfo(page, py - fontH);
    }

    return page;
}

/**************************************************************************************************
 * 构建开始页
 *************************************************************************************************/
PrintPage *PrintTrendDataLayout::_startPage(const ParamDataDescription &info)
{
    //偏移
    int fontH = fontHeight();

    QStringList strList;
    strList.clear();

    QString text = trs("TrendDataReport");
    strList << text;

    timeDate.getDateTime(info.startTime, text, true, true);
    strList << (trs("RescueStartTime") + ": " + text);

    strList << (trs("DeviceID") + ": " + QString::fromAscii(info.deviceID));
    strList << (trs("SerialNum") + ": " + QString::fromAscii(info.serialNum));

    text = trs("IncidentIDNum") + ": ";
    text += QString::fromAscii(info.fdFileName);
    strList << text;

    strList << (trs("PatName") + ": " + QString::fromAscii(info.patientName));
    strList << (trs("PatID") + ": " + QString::fromAscii(info.PatientID));

    QFontMetrics fm(printFont());
    int width = 0;
    foreach(QString str, strList)
    {
        int curWidth = fm.width(str);
        if(curWidth > width)
        {
            width = curWidth;
        }
    }

    width += 2 * fontH;

    PrintPage *startPage = new PrintPage(width);
    startPage->setPrintSpeed(printManager.getPrintSpeed());
    startPage->setID("TreadStart" + QString::number(_fileNum - 1));

    QRect r = startPage->rect();
    QPainter painter(startPage);
    painter.setFont(printFont());
    painter.setPen(Qt::white);
    painter.fillRect(r, Qt::SolidPattern); // 清空所有的数据。

    int count = strList.count();
    int xoffset = fontHeight();
    int yoffset = fontHeight();
    int gap = fontH + fontH / 3;
    for (int i = 0; i < count; ++i)
    {
        painter.drawText(xoffset, yoffset, strList.at(i));
        yoffset += gap;
    }

    strList.clear();
    return startPage;
}
