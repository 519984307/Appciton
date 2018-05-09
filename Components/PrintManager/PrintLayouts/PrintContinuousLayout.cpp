#include <QPainter>
#include <math.h>
#include "PrintContinuousLayout.h"
#include "WaveformCache.h"
#include "Debug.h"
#include "LanguageManager.h"
#include "PatientManager.h"
#include "PrintManager.h"
#include "WindowManager.h"
#include "ECGParam.h"
#include "CO2Param.h"
#include "RESPParam.h"
#include "NIBPParam.h"
#include "TEMPParam.h"
#include "TrendCache.h"
#include "TimeDate.h"
#include "SystemManager.h"
#include "IThread.h"
#include "PrintExec.h"
#include "IConfig.h"
#include "Alarm.h"
#include "ParamManager.h"
#include "SPO2Param.h"
#include "SPO2Symbol.h"
#include "ECGWaveWidget.h"
#include "SPO2WaveWidget.h"

#define WAVE_START_PIXEL_Y (3 * fontHeight() + PIXELS_PER_MM)         // y轴的波形起始位置。
#define WAVE_PIXEL_Y (PRINT_PAGE_HEIGHT * PIXELS_PER_MM - WAVE_START_PIXEL_Y - 20) // y轴波形区域所占像素值。
#define UPDATE_PARAM_DATA_TIME (10)                      // 参数数据更新时间10s
#define WAVE_CPR_LABEL_WIDTH (60)
#define WAVE_CPR_BAR_WIDTH (3 * PIXELS_PER_MM)          // 3mm

//shock info
unsigned PrintContinuousLayout::_shockTime = 0;
int PrintContinuousLayout::_curSelEngryBk = -1;
int PrintContinuousLayout::_curSelEngry = -1;
int PrintContinuousLayout::_delEngey = -1;
int PrintContinuousLayout::_current = -1;
int PrintContinuousLayout::_ttl = -1;

/**************************************************************************************************
 * 检查界面波形是否有变化。
 *************************************************************************************************/
bool PrintContinuousLayout::_checkWaveform(void)
{
    // check wave and gain is change
    WaveGainList waveList;
    _getWaveForm(waveList);

    if (_waveGainList.size() != waveList.size())
    {
        _waveGainList.clear();
        _waveGainList.append(waveList);
        return true;
    }
    else
    {
        int count = _waveGainList.size();
        for (int i = 0; i < count; ++i)
        {
            if (_waveGainList.at(i) != waveList.at(i))
            {
                _waveGainList.clear();
                _waveGainList.append(waveList);
                return true;
            }
        }
    }

    return false;
}

/**************************************************************************************************
 * 检查病人类型是否改变。
 *************************************************************************************************/
bool PrintContinuousLayout::_checkPatientInfo(void)
{
    bool bret = false;
    PatientInfo info = patientManager.getPatientInfo();
    if (info.type != _patInfo.type)
    {
        bret |= true;
    }

    if (0 != memcmp(info.id, _patInfo.id, sizeof(info.id)))
    {
        bret |= true;
    }

    _patInfo = info;

    return bret;
}

/**************************************************************************************************
 * 检查打印
 *************************************************************************************************/
bool PrintContinuousLayout::_checkPrintSpeed(void)
{
    PrintSpeed speed = printManager.getPrintSpeed();

    if (_printSpeed != speed)
    {
        _printSpeed = speed;
        printManager.enablePrinterSpeed(_printSpeed);
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 检查工作模式
 *************************************************************************************************/
bool PrintContinuousLayout::_checkWorkMode()
{
    UserFaceType type = windowManager.getUFaceType();

    if (_faceType != type)
    {
        _faceType = type;
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 检查是否停止通讯
 *************************************************************************************************/
bool PrintContinuousLayout::_checkAllParamStop()
{
    int count = _waveGainList.size();
    for (int i = 0; (i < count && i < _numOfWaveTrace); ++i)
    {
        if (!_checkParamStop(_waveGainList.at(i).id))
        {
            return false;
        }
    }

    return true;
}

/**************************************************************************************************
 * 检查是否需要画带宽
 *************************************************************************************************/
void PrintContinuousLayout::_checkNeedDrawBandwidth(int wavePos, int bandwidth,
            QPainter &painter, int pageWidth, int offx)
{
    if (wavePos >= MAX_WAVE_TRACE)
    {
        return;
    }

    if (!_ecgBdVec[wavePos].isInit)
    {
        return;
    }

    if (_ecgBdVec[wavePos].bdVec.size() > 1)
    {
        if (_ecgBdVec[wavePos].bdVec.last() != bandwidth)
        {
            _ecgBdVec[wavePos].bdVec.append(bandwidth);
        }

        return;
    }
    else if (_ecgBdVec[wavePos].bdVec.size() == 1)
    {
        if (_ecgBdVec[wavePos].bdVec.last() == bandwidth)
        {
            return;
        }

        if (_ecgBdVec[wavePos].startOffx >= pageWidth)
        {
            _ecgBdVec[wavePos].bdVec.append(bandwidth);
            return;
        }
        else
        {
            _ecgBdVec[wavePos].bdVec.clear();
            _ecgBdVec[wavePos].bdVec.append(bandwidth);
        }
    }
    else
    {
        _ecgBdVec[wavePos].bdVec.append(bandwidth);
    }

    _drawECGBandwidth(wavePos, bandwidth, painter, pageWidth, offx);
}

/**************************************************************************************************
 * 检查是否停止通讯
 *************************************************************************************************/
bool PrintContinuousLayout::_checkParamStop(WaveformID id)
{
    switch (id)
    {
        case WAVE_ECG_I:
        case WAVE_ECG_II:
        case WAVE_ECG_III:
        case WAVE_ECG_aVF:
        case WAVE_ECG_aVL:
        case WAVE_ECG_aVR:
        case WAVE_ECG_V1:
        case WAVE_ECG_V2:
        case WAVE_ECG_V3:
        case WAVE_ECG_V4:
        case WAVE_ECG_V5:
        case WAVE_ECG_V6:
            if (ecgParam.isConnected())
            {
                return false;
            }
            break;

        case WAVE_RESP:
            if (ecgParam.isConnected() && respParam.isEnabled())
            {
                return false;
            }
            break;

        case WAVE_SPO2:
            if (spo2Param.isConnected())
            {
                return false;
            }
            break;

        case WAVE_CO2:
            if (co2Param.isConnected())
            {
                return false;
            }
            break;

        default:
            break;
    }

    return true;
}

/**************************************************************************************************
 * 判断是否生成第一个页面。
 *************************************************************************************************/
bool PrintContinuousLayout::_isNeedFirstPage(void)
{
    // 注意：这里必须每个都要检测一遍。
    bool f0 = _checkWorkMode();
    bool f1 = _checkPatientInfo();
    bool f2 = _checkPrintSpeed();
    bool f3 = _checkWaveform();
    if (f0 || f1 || f2 || f3 || _startContinuousPrint)
    {
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 产生第一个打印页。
 *************************************************************************************************/
PrintPage *PrintContinuousLayout::_produceFirstPage(void)
{
    int ySpace = 2;
    int width = 0;
    int offx = 14;
    int x = offx;
    int y = fontHeight();
    QString patientIDStr = trs("PatientID") + ": " + _patInfo.id;
    int patientIDLen = fontManager.textWidthInPixels(patientIDStr, printFont());

    if (!_startContinuousPrint)
    {
        // stop the printer
        printExec.abort();

        if (patientIDLen <= IMAGE_HEIGHT - 2 * offx)
        {
            width = PIXELS_PER_MM * 20 + 7 * (ySpace + fontHeight()) + fontHeight() / 2;
        }
        else
        {
            width = PIXELS_PER_MM * 20 + 8 * (ySpace + fontHeight()) + fontHeight() / 2;
        }

        y += PIXELS_PER_MM * 20;
    }
    else
    {
        if (patientIDLen <= IMAGE_HEIGHT - 2 * offx)
        {
            width = 7 * (ySpace + fontHeight()) + fontHeight() / 2;
        }
        else
        {
            width = 8 * (ySpace + fontHeight()) + fontHeight() / 2;
        }

        _startContinuousPrint = false;
    }

    PrintPage *page = new PrintPage(width);
    page->setID("Conti0");

    QPainter painter(page);
    painter.setFont(printFont());
    painter.setPen(Qt::white);
    painter.fillRect(page->rect(), Qt::SolidPattern); // 清空所有的数据。

    // 以左下角点为原点进行旋转90度。
    painter.translate(0, page->height() - 1);
    painter.rotate(-90);

    QString str;
    QString text = trs("DeviceID") + ":";
    currentConfig.getStrValue("General|DeviceIdentifier", str);
    text += str;
    painter.drawText(x, y, text);
    y += fontHeight() + ySpace;

    text = trs("SerialNum") + ":";
    machineConfig.getStrValue("SerialNumber", str);
    text += str;
    painter.drawText(x, y, text);
    y += fontHeight() + ySpace;

    text = trs("PatientType") + ":";
    text += trs(PatientSymbol::convert(_patInfo.type));
    painter.drawText(x, y, text);
    y += fontHeight() + ySpace;

    // 病人ID。
    if (patientIDLen <= IMAGE_HEIGHT - 2 * offx)
    {
        painter.drawText(x, y, patientIDStr);
        y += fontHeight() + ySpace;
    }
    else
    {
        painter.drawText(x, y, trs("PatientID") + ": ");
        int leftLen = fontManager.textWidthInPixels(trs("PatientID") + ": ", printFont());
        int alignFlag = Qt::AlignTop | Qt::AlignLeft | Qt::TextWrapAnywhere;
        QRect rect(x + leftLen, y - 2 * fontHeight() / 3 - 1, IMAGE_HEIGHT - 2 * offx - leftLen, 2*fontHeight() + ySpace);
        QRect boundRect;
        painter.drawText(rect, alignFlag, _patInfo.id, &boundRect);
        y += 2*fontHeight() + ySpace;
    }

    //notch filter
    text = trs("SupervisorNotchFilter") + ": ";
    text += ECGSymbol::convert(ecgParam.getNotchFilter());
    painter.drawText(x, y, text);
    y += fontHeight() + ySpace;

    painter.drawText(x, y, PrintSymbol::convert(_printSpeed));
    y += fontHeight() + ySpace;
    return page;
}

/**************************************************************************************************
 * 函数说明：获取显示的波形及其增益。
 *************************************************************************************************/
void PrintContinuousLayout::_getWaveForm(WaveGainList &waveList)
{
    QList<int> waveID;
    _numOfWaveTrace = printManager.getPrintWaveNum();

    // get cur display waveform
    if (UFACE_MONITOR_12LEAD != _faceType)
    {
        windowManager.getDisplayedWaveform(waveID);
    }
    else
    {
        int lead = ECG_LEAD_I;
        currentConfig.getNumValue("ECG12L|RealtimePrintTopLead", lead);
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)lead));
        currentConfig.getNumValue("ECG12L|RealtimePrintMiddleLead", lead);
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)lead));
        currentConfig.getNumValue("ECG12L|RealtimePrintBottomLead", lead);
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)lead));
        _numOfWaveTrace = 3;
    }

    int gain = -1;
    int count = waveID.count();

    // 当前可打印的波形数
    waveList.clear();
    for (int i = 0; (i < count && i < _numOfWaveTrace); ++i)
    {
        WaveformID id = (WaveformID)waveID.at(i);
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
                if (UFACE_MONITOR_12LEAD != windowManager.getUFaceType())
                {
                    gain = (int)ecgParam.getGain(ecgParam.waveIDToLeadID(id));
                }
                else
                {
                    //gain = (int)ecgParam.getGain(ecgParam.getCalcLead());
                    gain = (int)ecgParam.get12LGain(ecgParam.waveIDToLeadID(id));
                }
                break;

            case WAVE_RESP:
                gain = (int)respParam.getZoom();
                break;

            case WAVE_SPO2:
                gain = (int)spo2Param.getGain();
                break;

            case WAVE_CO2:
                gain = (int)co2Param.getDisplayZoom();
                break;

            default:
                break;
        }

        if (-1 != gain)
        {
            waveList.append(WaveGain(id, gain));
            gain = -1;
        }
    }
}

/**************************************************************************************************
 * 获取所化波形的信息。
 *************************************************************************************************/
void PrintContinuousLayout::_getWaveFormCommonDesc()
{
    _waveformDescList.clear();

    WaveformDesc waveDesc;
    int count = _waveGainList.size();
    QList<WaveformID> waveList;
    waveList.clear();
    for (int i = 0; i < count; ++i)
    {
        struct WaveGain waveGain = _waveGainList.at(i);
        waveDesc.reset();
        waveDesc.waveID = waveGain.id;
        waveDesc.waveRate = waveformCache.getSampleRate(waveGain.id);
        waveformCache.getRange(waveGain.id, waveDesc.waveRangeMin, waveDesc.waveRangeMax);
        waveformCache.getBaseline(waveGain.id, waveDesc.waveBaseline);
        if (waveGain.id <= WAVE_ECG_V6)
        {
            if (waveGain.id < WAVE_ECG_V6)
            {
                bool is12L = (_faceType == UFACE_MONITOR_12LEAD) ? true : false;
                waveDesc.waveTitle = ECGSymbol::convert(ecgParam.waveIDToLeadID(waveGain.id),
                    ecgParam.getLeadConvention(), is12L);
            }
        }
        else
        {
            waveformCache.getTitle(waveGain.id, waveDesc.waveTitle);
        }

        // 打印机速度/数据速度,x轴的偏移量。
        waveDesc.offsetX = (double)PrintSymbol::convertToPixel(_printSpeed) / waveDesc.waveRate;

        // cacl position
        waveDesc.startY = WAVE_START_PIXEL_Y + i * WAVE_PIXEL_Y / count;
        waveDesc.endY = waveDesc.startY + WAVE_PIXEL_Y / count;
        waveDesc.mediumY = (waveDesc.endY - waveDesc.startY) / 2 + waveDesc.startY;
        if (!waveList.isEmpty() && -1 != waveList.indexOf(waveDesc.waveID))
        {
            waveDesc.isSameWave = true;
        }

        // get pre 6 sec wave data
        //_getWaveData(waveDesc, 6, true, true, false);
        waveDesc.waveBuf.resize(waveDesc.waveRate * 6);
        waveformCache.readStorageChannel(waveDesc.waveID, waveDesc.waveBuf.data(), 6, false, true);

        // insert list
        _waveformDescList.append(waveDesc);
        waveList.append(waveDesc.waveID);
    }
}

/**************************************************************************************************
 * 获取波形数据。
 *************************************************************************************************/
void PrintContinuousLayout::_getWaveData(struct WaveformDesc &waveDesc, int sec)
{
    if (WAVE_NONE == waveDesc.waveID || sec <= 0)
    {
        return ;
    }

    //波形相同前面波形收集数据时，就增加了数据，这里不再获取
    if (waveDesc.isSameWave)
    {
        return;
    }

    int dataLen = waveDesc.waveRate * sec;
    WaveDataType buf[dataLen];

    int size = waveformCache.readRealtimeChannel(waveDesc.waveID, buf, sec);
    int index = 0;
    QVector<WaveDataType> bufVec;
    bufVec.clear();
    for (int i = index; i < size; ++i)
    {
        waveDesc.waveBuf.append(buf[i]);
        bufVec.append(buf[i]);
    }

    int count = _waveGainList.size();
    for (int i = 0; (i < count && i < _numOfWaveTrace); ++i)
    {
        WaveformDescList::iterator it = _waveformDescList.begin() + i;
        if (it->waveID == waveDesc.waveID && it->isSameWave)
        {
            WaveformDesc desc = *it;
            desc.waveBuf << bufVec;
            *it = desc;
        }
    }
}

/**************************************************************************************************
 * 清除已经绘画了的波形数据。
 *************************************************************************************************/
void PrintContinuousLayout::_clearWaveData()
{
    int count = _waveGainList.size();
    for (int i = 0; (i < count && i < _numOfWaveTrace); ++i)
    {
        WaveformDescList::iterator it = _waveformDescList.begin() + i;
        WaveformDesc waveformDesc = *it;
        if (waveformDesc.actualDrawWaveDataNum > waveformDesc.waveBuf.size())
        {
            return;
        }

        if (waveformDesc.actualDrawWaveDataNum > 0)
        {
            QVector<WaveDataType>::iterator dataIt = waveformDesc.waveBuf.begin();
            waveformDesc.waveBuf.erase(dataIt, dataIt + waveformDesc.actualDrawWaveDataNum);
            waveformDesc.actualDrawWaveDataNum = 0;
            *it = waveformDesc;
        }
    }
}

/**************************************************************************************************
 * 函数说明：连续打印时需要打印的参数信息。
 *************************************************************************************************/
void PrintContinuousLayout::_getDrawText(void)
{
    QString text;

    // 时间。
    QString time;
    QString date;
    timeDate.getTime(_time, time, true);
    timeDate.getDate(_time, date, true);

    // 获取指定时间点的Trend数据
    TrendCacheData trendData;
    TrendAlarmStatus alarmStatus;
    trendCache.getTrendAlarmStatus(_time, alarmStatus);

    date += " ";
    date += time;
    date += " ";
    _firstLineDrawStr << date;

    UnitType unit;
    TrendDataType data;
    QString dataStr;
    QString t1,t2;
    UnitType defUnitType;
    int mul = 0;

    QList<ParamID> idList;
    paramManager.getParams(idList);
    ParamID paramID = PARAM_NONE;
    bool isAllParamValid = true;//记录是否所有参数都有效， 决定NIBP的位置
    QString nibpValue, nibpTime;

    for (int i = 0; i < SUB_PARAM_NR; i++)
    {
        paramID = paramInfo.getParamID((SubParamID)i);
        if (-1 == idList.indexOf(paramID))
        {
            isAllParamValid &= false;
            continue;
        }

        if (SUB_PARAM_NIBP_DIA == i || SUB_PARAM_NIBP_MAP == i)
        {
            continue;
        }

        data = trendData.values.value((SubParamID)i, InvData());
        if (data == InvData() || data == UnknownData())
        {
            isAllParamValid &= false;
            continue;
        }

        defUnitType = paramInfo.getUnitOfSubParam((SubParamID)i);
        unit = paramManager.getSubParamUnit(paramID, (SubParamID)i);
        mul = paramInfo.getMultiOfSubParam((SubParamID)i);
        //心率/脉率时，如果血样模块功能没有开启的话只显示心率
        if (SUB_PARAM_HR_PR == i)
        {
            int _SPO2_enable(0);
            machineConfig.getNumValue("SPO2Enable", _SPO2_enable);
            if (1 == _SPO2_enable)
            {
                text = trs(paramInfo.getSubParamName((SubParamID)i));
            }
            else
            {
                text = trs(paramInfo.getSubParamName((SubDupParamID)i));
            }
        }
//        //呼吸率/CO2率时，如果CO2模块功能没有开启的话只显示心率
//        else if(SUB_PARAM_RR_BR == i)
//        {
//            int _CO2_enable(0);
//            machineConfig.getNumValue("CO2Enable", _CO2_enable);
//            if (1 == _CO2_enable)
//            {
//                text = trs(paramInfo.getSubParamName((SubParamID)i));
//            }
//            else
//            {
//                text = trs(paramInfo.getSubParamName((SubDupParamID)i));
//            }
//        }
        else
        {
            text = trs(paramInfo.getSubParamName((SubParamID)i));
        }

        text += "=";

        if (SUB_PARAM_NIBP_SYS != i)
        {
            if (unit == defUnitType)
            {
                if (1 == mul)
                {
                    dataStr = QString::number(data);
                }
                else
                {
                    if (i == SUB_PARAM_T1 || i == SUB_PARAM_T2)
                    {
                        if (data > 500)
                        {
                            dataStr = "> 50";
                        }
                        else if (data < 0)
                        {
                            dataStr = "< 0";
                        }
                        else
                        {
                            dataStr.sprintf("%.1f", data * 1.0 / mul);
                        }
                    }
                    else
                    {
                        dataStr.sprintf("%.1f", data * 1.0 / mul);
                    }
                }
            }
            else
            {
                switch (i)
                {
                    case SUB_PARAM_ETCO2:
                    case SUB_PARAM_FICO2:
                        dataStr = Unit::convert(unit, defUnitType, data * 1.0 / mul, co2Param.getBaro());
                        break;
                    case SUB_PARAM_T1:
                    case SUB_PARAM_T2:
                        if (data > 500)
                        {
                            dataStr = "> " + Unit::convert(unit, defUnitType, 50);
                        }
                        else if (data < 0)
                        {
                            dataStr = "< " + Unit::convert(unit, defUnitType, 0);
                        }
                        else
                        {
                            dataStr = Unit::convert(unit, defUnitType, data * 1.0 / mul);
                        }
                        break;
                    case SUB_PARAM_TD:
                        t1 = Unit::convert(unit, defUnitType, trendData.values[SUB_PARAM_T1] * 1.0 / mul);
                        t2 = Unit::convert(unit, defUnitType, trendData.values[SUB_PARAM_T2] * 1.0 / mul);
                        dataStr.sprintf("%.1f", fabs(t1.toDouble() - t2.toDouble()));
                        break;
                    default:
                        dataStr.sprintf("%d", data);
                        break;
                }
            }

            text += dataStr;

            if (alarmStatus.alarms.value((SubParamID)i, false))
            {
                text += "*";
            }

            text += Unit::localeSymbol(unit) + "  ";
            _secLineDrawStr << text;
        }
        else
        {
            text = paramInfo.getParamName(PARAM_NIBP);
            text += "=";

            if (unit == defUnitType)
            {
                dataStr = QString::number(data);
                if (alarmStatus.alarms[SUB_PARAM_NIBP_SYS])
                {
                    dataStr += "*";
                }
                dataStr += "/";

                dataStr += QString::number(trendData.values[SUB_PARAM_NIBP_DIA]);
                if (alarmStatus.alarms[SUB_PARAM_NIBP_DIA])
                {
                    dataStr += "*";
                }

                dataStr += " (";
                dataStr += QString::number(trendData.values[SUB_PARAM_NIBP_MAP]);
                if (alarmStatus.alarms[SUB_PARAM_NIBP_MAP])
                {
                    dataStr += "*";
                }
                dataStr += ") ";
            }
            else
            {
                dataStr = Unit::convert(unit, defUnitType, trendData.values[SUB_PARAM_NIBP_SYS]);
                if (alarmStatus.alarms[SUB_PARAM_NIBP_SYS])
                {
                    dataStr += "*";
                }
                dataStr += "/";

                dataStr += Unit::convert(unit, defUnitType, trendData.values[SUB_PARAM_NIBP_DIA]);
                if (alarmStatus.alarms[SUB_PARAM_NIBP_DIA])
                {
                    dataStr += "*";
                }
                dataStr += " (";

                dataStr += Unit::convert(unit, defUnitType, trendData.values[SUB_PARAM_NIBP_MAP]);
                if (alarmStatus.alarms[SUB_PARAM_NIBP_MAP])
                {
                    dataStr += "*";
                }
                dataStr += ") ";
            }
            text += dataStr;
            text += Unit::localeSymbol(unit) + "  ";

            nibpTime += "@ ";
            timeDate.getTime(trendData.lastNibpMeasureTime , time, false);
            nibpTime += time + "  ";

            nibpValue = text;
            _secLineDrawStr << text;
            _secLineDrawStr << nibpTime;
        }
    }

    // 如果所有参数有效，NIBP放在第一行
    if (isAllParamValid && _printSpeed == PRINT_SPEED_250)
    {
        _secLineDrawStr.removeOne(nibpValue);
        _secLineDrawStr.removeOne(nibpTime);

        _firstLineDrawStr << "";
        _firstLineDrawStr << nibpValue;
        _firstLineDrawStr << nibpTime;
    }
}

/**************************************************************************************************
 * 函数说明：计算打印页页宽。
 * 函数说明：获取上方参赛信息。
 *************************************************************************************************/
int PrintContinuousLayout::_caclPageWidth(QStringList &text)
{
    int actualWidth = PrintSymbol::convertToPixel(_printSpeed) * UPDATE_PARAM_DATA_TIME;
    if (actualWidth == _totalPageWidth)
    {
        _time += UPDATE_PARAM_DATA_TIME;
        _getDrawText();
        _totalPageWidth = 0;
        _updateCo2Ruler = true;
    }

    if (!_firstLineDrawStr.isEmpty())
    {
        text << _firstLineDrawStr.takeFirst();
    }

    if (!_secLineDrawStr.isEmpty())
    {
        //时间在第一行，其余在第二行，开始位置从第一行开始，为了使绘画一致，给前面的行补空字符
        if (0 == text.count())
        {
            text << "";
        }
        text << _secLineDrawStr.takeFirst();
    }

    if (!_thirdLineDrawStr.isEmpty())
    {
        if (0 == text.count())
        {
            text << "" << "";
        }
        else if (1 == text.count())
        {
            text << "";
        }
        text << _thirdLineDrawStr.takeFirst();
    }
    else if (!text.isEmpty())
    {
        if (1 == text.count())
        {
            text << "" << "";
        }
        else if (2 == text.count())
        {
            text << "";
        }
    }

    // 根据显示参数，计算页宽
    int pageWidth = 0;
    int tmpWidth = 0;
    if (!text.isEmpty())
    {
        for (int i = 0; i < text.count(); ++i)
        {
            if (text.at(i).isEmpty())
            {
                continue;
            }

            tmpWidth = fontManager.textWidthInPixels(text.at(i), printFont());
            if (pageWidth < tmpWidth)
            {
                pageWidth = tmpWidth;
            }
        }
    }
    else
    {
        // 没有需要绘制的参数，取固定长度
        pageWidth = PrintSymbol::convertToPixel(_printSpeed);
        if (pageWidth + _totalPageWidth > actualWidth)
        {
            pageWidth = actualWidth - _totalPageWidth;
        }
        else if (actualWidth - pageWidth - _totalPageWidth < pageWidth)
        {
            //避免出现的页宽度太小
            pageWidth = actualWidth - _totalPageWidth;
        }
    }

    _totalPageWidth += pageWidth;

    return pageWidth;
}

/**************************************************************************************************
 * 函数说明：画文本信息。
 * 函数参数：
 *         painter:绘画位置。
 *         text:需要画入的文本信息。
 *         x:x轴坐标。
 *         y:y轴坐标。
 *************************************************************************************************/
void PrintContinuousLayout::_drawText(QPainter &painter, QStringList &text, qreal x, qreal y)
{
    if (text.isEmpty())
    {
        return;
    }

    int count = text.count();
    for (int i = 0; i < count; ++i)
    {
        QPointF p(x, y);
        painter.drawText(p, text.at(i));
        y += fontHeight();
    }
}

/**************************************************************************************************
 * 函数说明：画文本信息。
 * 函数参数：
 *         painter:绘画位置。
 *         text:需要画入的文本信息。
 *         x:x轴坐标。
 *         y:y轴坐标。
 *************************************************************************************************/
void PrintContinuousLayout::_drawText(QPainter &painter, QString text, qreal x, qreal y)
{
    if (text.isEmpty())
    {
        return;
    }

    QPointF p(x, y);
    painter.drawText(p, text);
}

/**************************************************************************************************
 * 函数说明：检查波形数据是否足够。
 * 函数参数：
 *         wavePos:当前波形的位置。
 *         id：波形id。
 *         pageWidth：页大小。
 * 函数返回值：
 *         数据状态：false：数据不够或者发生错误
 *************************************************************************************************/
bool PrintContinuousLayout::_checkWaveNum(int wavePos, WaveformID id, int pageWidth)
{
    if (wavePos >= _waveformDescList.size())
    {
        return false;
    }

    WaveformDesc waveformDesc = _waveformDescList.at(wavePos);
    if (waveformDesc.waveID != id)
    {
        return false;
    }

    // 需要取出的波形数据个数。
    int waveNum = pageWidth / waveformDesc.offsetX + 1;
    if (waveNum <= 0)
    {
        return false;
    }

    int tmpsize = waveformDesc.waveBuf.size();
    int count = 0;
    while (waveNum > waveformDesc.waveBuf.size())
    {
        if (printManager.isAbort())
        {
            debug("\033[31m printer is abort!\033[m");
            return false;
        }

        if (_checkParamStop(_waveGainList.at(wavePos).id))
        {
            waveNum = waveformDesc.waveBuf.size();
            break;
        }

        if (++count >= 100) //500ms
        {
            if (tmpsize == waveformDesc.waveBuf.size())
            {
                waveNum = waveformDesc.waveBuf.size();
                break;
            }
        }

        _getWaveData(waveformDesc, 1);

        IThread::msleep(5);
    }

    waveformDesc.waveDataNum = waveNum;

    WaveformDescList::iterator it = _waveformDescList.begin();
    *(it + wavePos) = waveformDesc;
    return true;
}

/**************************************************************************************************
 * 函数说明：画虚线。
 * 函数参数：
 *         painter:波形需要画入的位置。
 *         x：需要画线的x坐标位置。
 *         x2:当前x坐标位置。
 *         y1:y1坐标的位置。
 *         y2:y2坐标的位置。
 *************************************************************************************************/
void PrintContinuousLayout::_drawDottedLine(QPainter &painter, qreal x1, qreal y1,
                                            qreal x2, qreal y2)
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

/**************************************************************************************************
 * 映射波形值
 * value:波形实际值
 * max:波形最大值
 * min:波形最小值
 *************************************************************************************************/
double PrintContinuousLayout::_mapWaveValue(const WaveGain &waveGain, const WaveformDesc &waveDesc,  int waveValue)

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
            switch (waveGain.gain)
            {
                case ECG_GAIN_X0125:
                    scaleData = 0.125 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                case ECG_GAIN_X025:
                    scaleData = 0.25 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                case ECG_GAIN_X05:
                    scaleData = 0.5 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                case ECG_GAIN_X10:
                    scaleData = 1.0 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                case ECG_GAIN_X20:
                    scaleData = 2.0 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                case ECG_GAIN_X40:
                    scaleData = 4.0 * 10 * PIXELS_PER_MM / 2.0;
                    break;

                default:
                    break;
            }
            endY = waveDesc.mediumY + scaleData;
            startY = waveDesc.mediumY - scaleData;
            dpos = (max - waveValue) * ((endY - startY) / (max - min)) + startY;
            break;
        }

        case WAVE_RESP:
        {
            // 将枚举类型的放大倍数转换为实际倍数值。
            double respZoom = waveGain.gain - 1;
            switch (waveGain.gain)
            {
                case RESP_ZOOM_X025:
                    respZoom = 0.25;
                    break;
                case RESP_ZOOM_X050:
                    respZoom = 0.5;
                    break;
                default:
                    break;
            }

            short wave = (waveValue - waveDesc.waveBaseline) * respZoom + waveDesc.waveBaseline;
            dpos = (max - wave) * ((endY - startY) / (max - min))  + startY;
            break;
        }

        case WAVE_SPO2:
            SPO2WaveWidget::getGainToValue((SPO2Gain)waveGain.gain, min, max);
            dpos = ((max - waveValue) * ((endY - startY) / (max - min)) + startY);
            break;

        case WAVE_CO2:
        {
            if (waveGain.gain == CO2_DISPLAY_ZOOM_4)
            {
                max = (max * 4 + 19) / 20;
            }
            else if (waveGain.gain == CO2_DISPLAY_ZOOM_8)
            {
                max = (max * 8 + 19) / 20;
            }
            else if (waveGain.gain == CO2_DISPLAY_ZOOM_12)
            {
                max = (max * 12 + 19) / 20;
            }

            dpos = ((max - waveValue) * ((endY - startY) / (max - min)) + startY);
            break;
        }

        default:
            return 0;
    }

    // 截顶处理
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

/**************************************************************************************************
 * 函数说明：画波形标签。
 * 函数参数：
 *         painter:波形需要画入的位置。
 *         id:波形ID。
 *         waveDesc:波形信息。
 *************************************************************************************************/
void PrintContinuousLayout::_drawWaveLabel(QPainter &painter, const WaveformDesc &waveDesc,
                                           const WaveGain &waveGain, int pos)
{
    if (waveDesc.waveID == WAVE_NONE)
    {
        return;
    }

    int fontH = fontHeight();

    int startGainY = (waveDesc.mediumY - waveDesc.startY - 2 * fontH) / 2;
    startGainY = waveDesc.startY + startGainY  + 2 * fontH;
    if (startGainY > waveDesc.mediumY)
    {
        startGainY = waveDesc.mediumY;
    }

    QString txt;
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
            if (_isNeedDrawFirstPage)
            {
                // 波形标识。
                _drawText(painter, waveDesc.waveTitle, 0, waveDesc.startY + fontH);

                //如果是相同的波形则不绘制带宽和增益
                if (!waveDesc.isSameWave)
                {
                    if (waveDesc.waveID == _waveformDescList.at(0).waveID)
                    {
                        //record bandwidth attr and this trace need draw bandwidth
                        _ecgBdVec[pos].isInit = true;
                        _ecgBdVec[pos].startOffx = 0;
                        _ecgBdVec[pos].yPoint = waveDesc.mediumY;
                    }
                }
                // ECG增益。
                txt = ECGSymbol::convert((ECGGain)waveGain.gain);
                _drawText(painter, txt, 0, startGainY);
            }
            break;
        case WAVE_RESP:
            if (_isNeedDrawFirstPage)
            {
                // 画波形标识。
                _drawText(painter, waveDesc.waveTitle, 0, waveDesc.startY + fontH);
                // 画RESP放大倍数。
                txt = RESPSymbol::convert((RESPZoom)waveGain.gain);
                _drawText(painter, txt, 0, startGainY);
            }
            break;
        case WAVE_SPO2:
            if (_isNeedDrawFirstPage)
            {
                _drawText(painter, waveDesc.waveTitle, 0, waveDesc.startY + fontH);

                // spo2 gain str
                _drawText(painter, SPO2Symbol::convert((SPO2Gain)waveGain.gain), 0, startGainY);
            }
            break;
        case WAVE_CO2:
        {
            if (_updateCo2Ruler || _isNeedDrawFirstPage)
            {
                UnitType co2Unit = co2Param.getUnit();
                UnitType defaultType = paramInfo.getUnitOfSubParam(SUB_PARAM_ETCO2);
                CO2DisplayZoom zoom = (CO2DisplayZoom)waveGain.gain;
                if (_isNeedDrawFirstPage)
                {
                    // 波形标识。
                    _drawText(painter, waveDesc.waveTitle, 0, waveDesc.startY + fontH);
                }

                // 标尺值。
                double up = 8.0, mid = 4.0, low = 0;

                switch(zoom)
                {
                case CO2_DISPLAY_ZOOM_4:
                    up = 4.0;
                    mid = 2.0;
                    low = 0;
                    break;
                case CO2_DISPLAY_ZOOM_8:
                    up = 8.0;
                    mid = 4.0;
                    low = 0;
                    break;

                case CO2_DISPLAY_ZOOM_12:
                    up = 12.0;
                    mid = 6.0;
                    low = 0;
                    break;

                case CO2_DISPLAY_ZOOM_20:
                    up = 20.0;
                    mid = 10.0;
                    low = 0;
                    break;

                default:
                    break;
                }

                int offx = fontManager.textWidthInPixels(waveDesc.waveTitle, painter.font());
                int upVal = 0;
                if (UNIT_MMHG == co2Unit)
                {
                    // 上标尺。
                    upVal = Unit::convert(co2Unit, defaultType, up).toFloat();
                    upVal = (upVal + 5) / 10 * 10;
                    txt = QString::number(upVal);
                    txt += " ";
                    txt += trs(Unit::getSymbol(co2Unit));
                    _drawText(painter, txt, offx + 8, waveDesc.startY + fontH);

                    // 中标尺
                    txt = QString::number(upVal / 2);
                    _drawText(painter, txt, offx + 8, waveDesc.mediumY + fontH / 2);

                    // 下标尺
                    txt = Unit::convert(co2Unit, defaultType, low);
                    _drawText(painter, txt, offx + 8, waveDesc.endY);
                }
                else
                {
                    // 上标尺。
                    float tempVal = Unit::convert(co2Unit, defaultType, up).toFloat();
                    txt.sprintf("%.1f", (float)(int)(tempVal + 0.5));
                    txt += " ";
                    txt += trs(Unit::getSymbol(co2Unit));
                    _drawText(painter, txt, offx + 8, waveDesc.startY + fontH);

                    // 中标尺
                    tempVal = Unit::convert(co2Unit, defaultType, mid).toFloat();
                    txt.sprintf("%.1f", (float)(int)(tempVal + 0.5));
                    _drawText(painter, txt, offx + 8, waveDesc.mediumY + fontH / 2);

                    // 下标尺
                    tempVal = Unit::convert(co2Unit, defaultType, low).toFloat();
                    txt.sprintf("%.1f", (float)(int)(tempVal + 0.5));
                    _drawText(painter, txt, offx + 8, waveDesc.endY);
                }

                _updateCo2Ruler = false;
            }

            QPen oldPen = painter.pen();
            QPen pen(Qt::white);
//            pen.setWidth(1);
//            pen.setStyle(Qt::DotLine);
            pen.setStyle(Qt::DashLine);
            QVector<qreal> darsh(2, 4);
            pen.setDashPattern(darsh);
            painter.setPen(pen);

            // 上标尺
            painter.drawLine(0, waveDesc.startY, _totalPageWidth, waveDesc.startY);
            // 中标尺
            painter.drawLine(0, waveDesc.mediumY, _totalPageWidth, waveDesc.mediumY);
            // 下标尺
            painter.drawLine(0, waveDesc.endY, _totalPageWidth, waveDesc.endY);

            painter.setPen(oldPen);

            break;
        }

        default:
            break;
    }
}

/**************************************************************************************************
 * 函数说明：画波形及波形信息。
 * 函数参数：
 *         index：波形索引。
 *         painter:波形需要画入的位置。
 *         drawWaveInformation:波形信息数据。
 *         pageWidth:页宽。
 *************************************************************************************************/
void PrintContinuousLayout::_drawWave(int index, QPainter &painter, int pageWidth)
{
    qreal x1 = 0,y1 = 0,x2 = 0,y2 = 0;  // 需要连接的两点。
    bool start = true;

    if (index > _waveGainList.size() || index > _waveformDescList.size() || index > MAX_WAVE_TRACE)
    {
        return;
    }

    QPen oldPen = painter.pen();

    WaveGain waveGain = _waveGainList.at(index);
    WaveformDesc waveDesc = _waveformDescList.at(index);

    _drawWaveLabel(painter, waveDesc, waveGain, index);

    QImage drawImage;
    drawImage = *_leadoffImage;

    if (0 != _drawLeadFaultLen[index])
    {
        if (drawImage.rect().width() - _drawLeadFaultLen[index] < pageWidth)
        {
            painter.drawImage(0, waveDesc.mediumY - drawImage.rect().height(),
            drawImage, _drawLeadFaultLen[index], 0);
            _drawLeadFaultLen[index] = 0;
        }
        else
        {
            painter.drawImage(0, waveDesc.mediumY - drawImage.rect().height(),
            drawImage, _drawLeadFaultLen[index], 0, pageWidth, drawImage.rect().height());
            _drawLeadFaultLen[index] += pageWidth;
        }
    }

    if (0 == waveDesc.waveDataNum)
    {
        return;
    }

    static int flag[MAX_WAVE_TRACE];
    static bool needThrowDown[MAX_WAVE_TRACE];
    if (_isNeedDrawFirstPage)
    {
        flag[index] = 0;
        needThrowDown[index] = false;
    }

    int i = 0;
    int bandwidth = 0;
    double offx = 0;
    for (; i < waveDesc.waveDataNum; ++i)
    {
        flag[index] |= waveDesc.waveBuf[i] >> 16;
        offx = waveDesc.offsetX;
        if (waveGain.id >= WAVE_ECG_I && waveGain.id < WAVE_ECG_V6)
        {
            bandwidth = (waveDesc.waveBuf[i] >> 20) & 0x07;
            _checkNeedDrawBandwidth(index, bandwidth, painter, pageWidth, x2);
        }

        if (flag[index] & ECG_WAVE_RATE_250_BIT)
        {
            if (needThrowDown[index])
            {
                ++waveDesc.actualDrawWaveDataNum;
                needThrowDown[index] = false;
                x2 += offx;
                if (x2 > pageWidth - 1)
                {
                    break;
                }

                continue;
            }

            needThrowDown[index] = true;
        }
        else
        {
            needThrowDown[index] = false;
        }

        // invalid
        if (flag[index] & INVALID_WAVE_FALG_BIT)
        {
            if (start)
            {
                x1 = 0;
                x2 = waveDesc.offsetPage;
                start = false;
            }

            ++waveDesc.actualDrawWaveDataNum;
            short wave = waveDesc.waveBuf[i] & 0xFFFF;
            double waveData = _mapWaveValue(waveGain, waveDesc, wave);
            y1 = y2 = waveData;

//            if (flag[index] & (ECG_EXTERNAL_SOLD_FLAG_BIT | ECG_EXTERNAL_DOT_FLAG_BIT))//epace
            if (flag[index] & ECG_EXTERNAL_SOLD_FLAG_BIT)//epace
            {
                QPoint start(x2, waveDesc.mediumY);
                QPoint end(x2, waveDesc.endY - 10 - fontHeight());
                QRect r(0, waveDesc.startY, pageWidth, waveDesc.endY - waveDesc.startY);
                ECGWaveWidget::drawEPaceMark(painter, start, end, r, flag[index]);
            }

            flag[index] = 0;
            int j = i + 1;

            // 判断后续是否有更多的无效数据
            while (j < waveDesc.waveDataNum)
            {
                flag[index] |= waveDesc.waveBuf[j] >> 16;
                if (!(flag[index] & INVALID_WAVE_FALG_BIT))
                {
                    break;
                }

                if (waveGain.id >= WAVE_ECG_I && waveGain.id < WAVE_ECG_V6)
                {
                    bandwidth = (waveDesc.waveBuf[i] >> 20) & 0x07;
                    _checkNeedDrawBandwidth(index, bandwidth, painter, pageWidth, x2);
                }

                if (flag[index] & ECG_WAVE_RATE_250_BIT)
                {
                    if (needThrowDown[index])
                    {
                        ++waveDesc.actualDrawWaveDataNum;
                        needThrowDown[index] = false;
                        if (x2 + offx > pageWidth - 1)
                        {
                            break;
                        }

                        j++;
                        x2 += offx;
                        _drawLeadFaultStart[index] += offx;
                        continue;
                    }

                    needThrowDown[index] = true;
                }
                else
                {
                    needThrowDown[index] = false;
                }

                if (x2 + offx > pageWidth - 1)
                {
                    break;
                }

                ++waveDesc.actualDrawWaveDataNum;

                x2 += offx;

                /*if (flag & ECG_INTERNAL_FLAG_BIT)//ipace
                {
                    QPoint start(x2, waveDesc.startY + 10);
                    QPoint end(x2, waveDesc.endY - 10);
                    ECGWaveWidget::drawIPaceMark(painter, start, end);
                }*/

//                if (flag[index] & (ECG_EXTERNAL_SOLD_FLAG_BIT | ECG_EXTERNAL_DOT_FLAG_BIT))//epace
                if (flag[index] & ECG_EXTERNAL_SOLD_FLAG_BIT)//epace
                {
                    QPoint start(x2, waveDesc.mediumY);
                    QPoint end(x2, waveDesc.endY - 10 - fontHeight());
                    QRect r(0, waveDesc.startY, pageWidth, waveDesc.endY - waveDesc.startY);
                    ECGWaveWidget::drawEPaceMark(painter, start, end, r, flag[index]);
                }

                if (ECG_DISPLAY_12_LEAD_FULL != ecgParam.getDisplayMode())
                {
                    if (waveGain.id >= WAVE_ECG_I && waveGain.id <= WAVE_ECG_V6)
                    {
                        _drawLeadFaultStart[index] += offx;
                        if (_drawLeadFaultStart[index] >= 40.0 * PIXELS_PER_MM)
                        {
                            if ((pageWidth - x2) > drawImage.rect().width())
                            {
                                painter.drawImage(x2, waveDesc.mediumY - drawImage.rect().height(),
                                drawImage, 0, 0);
                            }
                            else
                            {
                                painter.drawImage(x2, waveDesc.mediumY - drawImage.rect().height(),
                                drawImage, 0, 0, pageWidth - x2 + 1, drawImage.rect().height());
                                _drawLeadFaultLen[index] = pageWidth - x2 + 1;
                            }
                            _drawLeadFaultStart[index] = -drawImage.rect().width();
                        }
                    }
                }

                flag[index] = 0;
                j++;
            }

            i = j - 1;

            //CO2不绘制虚线波形，因为有标尺线，虚线波形和标尺线重叠不好看
            if (WAVE_CO2 != waveDesc.waveID)
            {
                _drawDottedLine(painter, x1, waveData, x2, waveData);
            }

            x1 = x2;
            x2 += offx;
            y1 = y2;
            if (x2 > pageWidth - 1)
            {
                if (WAVE_CO2 != waveDesc.waveID)
                {
                    QLineF line(x1, y1, pageWidth, y1);
                    if (painter.pen().style() != Qt::SolidLine)
                    {
                        painter.setPen(Qt::SolidLine);
                    }

                    painter.drawLine(line);
                }

                painter.setPen(oldPen);

                waveDesc.positionY = y1;
                waveDesc.offsetPage = x2 - pageWidth + 1;

                break;
            }
        }
        else
        {
            short wave = waveDesc.waveBuf[i] & 0xFFFF;
            if (waveGain.id == WAVE_ECG_aVR && _faceType == UFACE_MONITOR_12LEAD &&
                    ecgParam.get12LDisplayFormat() == DISPLAY_12LEAD_CABRELA)
            {
                wave = -wave;
            }

            double waveData = _mapWaveValue(waveGain, waveDesc, wave);

            if (start)
            {
                if (!_isNeedDrawFirstPage)
                {
                    y1 = waveDesc.positionY;
                }
                else
                {
                    y1 = waveData;
                }

                x1 = 0;
                x2 = waveDesc.offsetPage;
                start = false;
            }
            else
            {
                if (waveDesc.waveBuf[i - 1]>>16 & INVALID_WAVE_FALG_BIT)
                {
                    y1 = waveData;
                }
            }

            // 画线。
            y2 = waveData;
            QLineF line(x1, y1, x2, y2);
            painter.drawLine(line);

            // 数据有效，无效计数清0
            _drawLeadFaultStart[index] = 0;

            //在12L界面中实时打印所有的波形都需要添加标记，其他界面只有计算导联才要添加标记
            if ((waveDesc.waveID == ecgParam.leadToWaveID(ecgParam.getCalcLead()))
                    || (UFACE_MONITOR_12LEAD == windowManager.getUFaceType()))
            {
                if (flag[index] & ECG_INTERNAL_FLAG_BIT)//ipace
                {
                    QPoint start(x2, waveDesc.startY + 10);
                    QPoint end(x2, waveDesc.endY - 10);
                    ECGWaveWidget::drawIPaceMark(painter, start, end);
                }

//                if (flag[index] & (ECG_EXTERNAL_DOT_FLAG_BIT | ECG_EXTERNAL_SOLD_FLAG_BIT))//epace
                if (flag[index] & ECG_EXTERNAL_SOLD_FLAG_BIT)//epace
                {
                    QPoint start(x2, y2);
                    QPoint end(x2, waveDesc.endY - 10 - fontHeight());
                    QRect r(0, waveDesc.startY, pageWidth, waveDesc.endY - waveDesc.startY);
                    ECGWaveWidget::drawEPaceMark(painter, start, end, r, flag[index]);
                }

//                if (flag[index] & ECG_RWAVE_MARK_FLAG_BIT)//R mark
//                {
//                    QPoint start(x2, y2);
//                    QRect r(0, waveDesc.startY, pageWidth, waveDesc.endY - waveDesc.startY);
//                    ECGWaveWidget::drawRMark(painter, start, r);
//                }
            }

            ++waveDesc.actualDrawWaveDataNum;
            flag[index] = 0;

            x1 = x2;
            x2 += offx;
            y1 = y2;
            if (x2 > pageWidth - 1)
            {
                break;
            }
        }
    }

    if (x2 > pageWidth - 1)
    {
        bool drawLine = false;
        QLineF line;
        y2 = y1;
        if (i + 1 < waveDesc.waveDataNum)
        {
            if (!(flag[index] & INVALID_WAVE_FALG_BIT))
            {
                short wave = waveDesc.waveBuf[i + 1] & 0xFFFF;
                if (waveGain.id == WAVE_ECG_aVR && _faceType == UFACE_MONITOR_12LEAD &&
                        ecgParam.get12LDisplayFormat() == DISPLAY_12LEAD_CABRELA)
                {
                    wave = -wave;
                }

                //当x1在页宽范围内，x2不在页宽范围内时，通过计算两点连线的斜率来判断x坐标为页宽时点的y坐标，并进行连线
                qreal tmpValue = _mapWaveValue(waveGain, waveDesc, wave);
                qreal offsety = abs(y1 * 1000 - tmpValue * 1000) / 1000.0 * (pageWidth * 1.0 - 1.0 - x1) / offx;
                y2 = (y1 > tmpValue) ? (y1 - offsety) : (y1 + offsety);
                line.setLine(x1, y1, pageWidth - 1, y2);
                drawLine = true;
            }
        }
        else
        {
            drawLine = true;
            line.setLine(x1, y1, pageWidth - 1, y2);
        }

        if (drawLine)
        {
            if (painter.pen().style() != Qt::SolidLine)
            {
                painter.setPen(Qt::SolidLine);
            }

            painter.drawLine(line);
            painter.setPen(oldPen);
            waveDesc.offsetPage = x2 - pageWidth + 1;
        }
    }
    else
    {
        waveDesc.offsetPage = 0;
    }
    waveDesc.positionY = y2;

    WaveformDescList::iterator waveDescIter = _waveformDescList.begin() + index;
    *waveDescIter = waveDesc;

    return;
}

/**************************************************************************************************
 * 函数说明：绘制放电图片。
 *************************************************************************************************/
void PrintContinuousLayout::_drawShockImage()
{
    if (NULL != _shockInfoImage)
    {
        return;
    }

    QString text;
    text.clear();

    if (_isNeedDrawFirstPage)
    {
        if (_shockTime >= _time)
        {
            text = QString::number(_curSelEngryBk);
            text += " J ";
            text += trs("SEL.");
            text += "  ";

        }
    }
    else if (-1 != _curSelEngry)
    {
        text = QString::number(_curSelEngry);
        text += " J ";
        text += trs("SEL.");
        text += "  ";
        _curSelEngry = -1;
    }

    if (text.isEmpty())
    {
        return;
    }

    _drawShockFlag = false;
    _drawShockInfoWidth = 0;
    text += trs("PatCurrent");
    text += "= ";
    text += QString().sprintf("%.1f", _current / 10.0);
    text += "A";
    text += "  ";

    text += QString::number(_delEngey);
    text += " J ";
    text += trs("DEL.");
    text += "  ";

    text += trs("DefibImped");
    text += "= ";
    text += QString().sprintf("%d", _ttl / 10);
    text += "  ";

    int len = fontManager.textWidthInPixels(text, printFont());
    int fontH = fontManager.textHeightInPixels(printFont());
    _shockInfoImage = new QImage(len, fontH, QImage::Format_MonoLSB);
    QPainter painter(_shockInfoImage);
    painter.setPen(Qt::white);
    painter.setFont(printFont());
    _shockInfoImage->fill(painter.brush().color());
    painter.drawText(_shockInfoImage->rect(), text);
}

/**************************************************************************************************
 * 函数说明：绘制ecg带宽。
 *************************************************************************************************/
void PrintContinuousLayout::_drawECGBandwidth(int wavePos, int bandwidth,
            QPainter &painter, int pageWidth, int offx)
{
    if (wavePos >= MAX_WAVE_TRACE)
    {
        return;
    }

    if (!_ecgBdVec[wavePos].isInit)
    {
        return;
    }

    if (bandwidth >= ECG_BANDWIDTH_NR)
    {
        return;
    }

    if (NULL == _bandwidthImage[bandwidth])
    {
        return;
    }

    if (_ecgBdVec[wavePos].startOffx < offx)
    {
        _ecgBdVec[wavePos].startOffx = offx;
    }

    QRect imageR = _bandwidthImage[bandwidth]->rect();
    int needDrawLen = imageR.width() - _ecgBdVec[wavePos].drawLen;
    if (needDrawLen + _ecgBdVec[wavePos].startOffx > pageWidth)
    {
        painter.drawImage(_ecgBdVec[wavePos].startOffx, _ecgBdVec[wavePos].yPoint,
            *_bandwidthImage[bandwidth], _ecgBdVec[wavePos].drawLen, 0,
            pageWidth - _ecgBdVec[wavePos].startOffx);
        _ecgBdVec[wavePos].drawLen += pageWidth - _ecgBdVec[wavePos].startOffx;
        _ecgBdVec[wavePos].startOffx = pageWidth;
        _ecgBdVec[wavePos].bdVec.append(bandwidth);
    }
    else
    {
        painter.drawImage(_ecgBdVec[wavePos].startOffx, _ecgBdVec[wavePos].yPoint,
            *_bandwidthImage[bandwidth], _ecgBdVec[wavePos].drawLen, 0);
        _ecgBdVec[wavePos].startOffx = needDrawLen;
        _ecgBdVec[wavePos].drawLen = 0;
    }
}

/**************************************************************************************************
 * 函数说明：预处理绘制ecg带宽，即绘制上一次没有画完的带宽。
 *************************************************************************************************/
void PrintContinuousLayout::_preHandleECGBandwidth(QPainter &painter, int pageWidth)
{
    for (int i = 0; i < MAX_WAVE_TRACE; ++i)
    {
        if (!_ecgBdVec[i].isInit || _ecgBdVec[i].bdVec.isEmpty())
        {
            continue;
        }

        while (!_ecgBdVec[i].bdVec.isEmpty())
        {
            int bandwidth = _ecgBdVec[i].bdVec.first();
            if (bandwidth >= ECG_BANDWIDTH_NR)
            {
                _ecgBdVec[i].bdVec.remove(0);
                continue;
            }

            if (NULL == _bandwidthImage[bandwidth])
            {
                _ecgBdVec[i].bdVec.remove(0);
                continue;
            }

            if (0 == _ecgBdVec[i].drawLen)
            {
                if (_ecgBdVec[i].bdVec.size() > 1)
                {
                    _ecgBdVec[i].bdVec.remove(0);
                    continue;
                }

                break;
            }

            QRect imageR = _bandwidthImage[bandwidth]->rect();
            int needDrawLen = imageR.width() - _ecgBdVec[i].drawLen;
            if (needDrawLen > pageWidth)
            {
                painter.drawImage(0, _ecgBdVec[i].yPoint, *_bandwidthImage[bandwidth],
                    _ecgBdVec[i].drawLen, 0, pageWidth);
                _ecgBdVec[i].drawLen += pageWidth;
                _ecgBdVec[i].startOffx = pageWidth;
                break;
            }
            else
            {
                painter.drawImage(0, _ecgBdVec[i].yPoint,
                    *_bandwidthImage[bandwidth], _ecgBdVec[i].drawLen, 0);
                _ecgBdVec[i].startOffx = needDrawLen + PIXELS_PER_MM;
                _ecgBdVec[i].drawLen = 0;
                if (_ecgBdVec[i].bdVec.size() > 1)
                {
                    _ecgBdVec[i].bdVec.remove(0);
                    continue;
                }

                break;
            }
        }
    }
}

/**************************************************************************************************
 * 开始布局。
 *************************************************************************************************/
void PrintContinuousLayout::start(void)
{
    _getWaveForm(_waveGainList);
    _pageNum = 0;
    _patInfo = patientManager.getPatientInfo();
    _printSpeed = printManager.getPrintSpeed();
    _faceType = windowManager.getUFaceType();
    _startContinuousPrint = true;
    _drawShockFlag = false;
    _drawShockInfoWidth = 0;

    _curSelEngry = -1;
    printManager.enablePrinterSpeed(_printSpeed);
    printManager.addPrinterActiveSnapshot();
}

/**************************************************************************************************
 * 布局中止。
 *************************************************************************************************/
void PrintContinuousLayout::abort(void)
{
    _secLineDrawStr.clear();
    _thirdLineDrawStr.clear();
    _waveGainList.clear();
    _waveformDescList.clear();

    if (NULL != _curpage)
    {
        delete _curpage;
        _curpage = NULL;
    }

    if (NULL != _shockInfoImage)
    {
        delete _shockInfoImage;
        _shockInfoImage = NULL;
    }
    //stop the realtime channel cache
    waveformCache.stopRealtimeChannel();
}

/**************************************************************************************************
 * 执行布局输出page对象，连续打印不需要传递进来的PrintContent。
 *************************************************************************************************/
PrintPage *PrintContinuousLayout::exec(PrintContent *)
{
    _curpage = NULL;
    if (printManager.isAbort())
    {
        return NULL;
    }

    if (_isNeedFirstPage())
    {
        _firstLineDrawStr.clear();
        _secLineDrawStr.clear();
        _thirdLineDrawStr.clear();
        _isNeedDrawFirstPage = true;
        _updateCo2Ruler = true;
        _time = timeManager.getCurTime() - 6;
        _totalPageWidth = 0;
        _lastDrawCPRBarWidth = 0;
        _lastDrawCPRLineWidth = 0;
        _lastCPRWaveMapValue = 0;

        for (int i = 0; i < MAX_WAVE_TRACE; ++i)
        {
            _drawLeadFaultStart[i] = 0;
            _drawLeadFaultLen[i] = 0;
            _ecgBdVec[i].reset();
        }

        if (NULL != _shockInfoImage)
        {
            delete _shockInfoImage;
            _shockInfoImage = NULL;
        }

        // 获取打印波形公共信息
        _getDrawText();
        waveformCache.stopRealtimeChannel();
        _getWaveFormCommonDesc();

        return _produceFirstPage();
    }

    if (_checkAllParamStop())
    {
        return NULL;
    }

    // 放电信息
//    _drawShockImage();

    // 计算该次PageWidth的大小。
    QStringList drawStrList;
    drawStrList.clear();
    int pageWidth = _caclPageWidth(drawStrList);

    // 计算页宽错误
    if (pageWidth <= 0)
    {
        debug("\033[31mcacl page width error\033[m");
        return NULL;
    }

    _curpage = new PrintPage(pageWidth);
    if (NULL == _curpage)
    {
        debug("\033[31m real-time print new space fail! \033[m");
        return NULL;
    }

    char str[32] = {'\0'};
    sprintf(str, "Contii%03d", _pageNum++ & 0xffff);
    _curpage->setID(str);

    QPainter painter(_curpage);
    painter.setFont(printFont());
    painter.setPen(Qt::white);
    painter.fillRect(_curpage->rect(), Qt::SolidPattern); // 清空所有的数据。

    // 画上方参数信息。
    _drawText(painter, drawStrList, 0, fontHeight());

    // 画工作模式。
//    _drawWorkMode(painter, _isNeedDrawFirstPage, 0, 3 * PIXELS_PER_MM +
//           WAVE_START_PIXEL_Y + fontHeight());
    _preHandleECGBandwidth(painter, pageWidth);

    int count = _waveGainList.size();
    for (int i = 0; (i < count && i < _numOfWaveTrace); ++i)
    {
        if (_checkWaveNum(i, _waveGainList.at(i).id, pageWidth))
        {
            // 画波形。
            _drawWave(i, painter, pageWidth);
        }
        else
        {
            debug("\033[31m real-time print check data failed \033[m");
            waveformCache.stopRealtimeChannel();
            return NULL;
        }
    }

    // 绘制放电信息
    if (_drawShockFlag && (NULL != _shockInfoImage))
    {
        int drawImageW = _curpage->width() - _drawShockInfoOff;
        if (3 == drawStrList.count())//pacer模式前6s需要绘画pacer信息
        {
            if (!drawStrList.at(2).isEmpty())
            {
                drawImageW = 0;
            }
        }

        if (0 != drawImageW)
        {
            painter.drawImage(_drawShockInfoOff, fontHeight() * 2 + PIXELS_PER_MM,
                *_shockInfoImage, _drawShockInfoWidth, 0);
            _drawShockInfoWidth += drawImageW;
            if (_drawShockInfoWidth >= _shockInfoImage->width())
            {
                delete _shockInfoImage;
                _shockInfoImage = NULL;

               _drawShockFlag = false;
            }

            _drawShockInfoOff = 0;
        }
    }

    _clearWaveData();

    _isNeedDrawFirstPage = false;
    return _curpage;
}

/**************************************************************************************************
 * 设置放电信息。
 *************************************************************************************************/
void PrintContinuousLayout::setShockInfo(unsigned t, int sel, int del, int current, int ttl)
{
    _shockTime = t;
    _curSelEngryBk = sel;
    _curSelEngry = sel;
    _delEngey = del;
    _current = current;
    _ttl = ttl;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PrintContinuousLayout::PrintContinuousLayout() : PrintLayoutIFace(PRINT_LAYOUT_ID_CONTINUOUS)
{
    _shockInfoImage = NULL;
    _curpage = NULL;
    _leadoffImage = NULL;
    _leadFaultImage = NULL;
    _faceType = UFACE_MONITOR_UNKNOW;

    QFont font = printFont();
    int drawLen = fontManager.textWidthInPixels(trs("LeadOff"), font);
    int fontH = fontManager.textHeightInPixels(font);
    QPainter painter;
    _leadoffImage = new QImage(drawLen + 2, fontH + PIXELS_PER_MM, QImage::Format_MonoLSB);
    painter.begin(_leadoffImage);
    painter.setPen(Qt::white);
    painter.setFont(font);
    _leadoffImage->fill(painter.brush().color());
    painter.drawText(_leadoffImage->rect(), trs("LeadOff"));

    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::white);
    painter.setPen(pen);
    painter.drawLine(0, fontH + 1, drawLen, fontH + 1);
    painter.end();

    drawLen = fontManager.textWidthInPixels(trs("LeadFault"), font);
    _leadFaultImage = new QImage(drawLen + 2, fontH + PIXELS_PER_MM, QImage::Format_MonoLSB);
    painter.begin(_leadFaultImage);
    painter.setPen(Qt::white);
    painter.setFont(font);
    _leadFaultImage->fill(painter.brush().color());
    painter.drawText(_leadFaultImage->rect(), trs("LeadFault"));

    painter.setPen(pen);
    painter.drawLine(0, fontH + 1, drawLen, fontH + 1);
    painter.end();

    for (int i = 0; i < ECG_BANDWIDTH_NR; ++i)
    {
        _bandwidthImage[i] = NULL;
    }

    int band = 0;
    currentConfig.getNumValue("ECG|ChestLeadsECGBandwidth", band);
    drawLen = fontManager.textWidthInPixels(ECGSymbol::convert((ECGBandwidth)band), font);
    _bandwidthImage[band] = new QImage(drawLen + 2, fontH, QImage::Format_MonoLSB);
    painter.begin(_bandwidthImage[band]);
    painter.setPen(Qt::white);
    painter.setFont(font);
    _bandwidthImage[band]->fill(painter.brush().color());
    painter.drawText(_bandwidthImage[band]->rect(), ECGSymbol::convert((ECGBandwidth)band));
    painter.end();

    band = ecgParam.getDiagBandwidth();
    drawLen = fontManager.textWidthInPixels(ECGSymbol::convert((ECGBandwidth)band), font);
    _bandwidthImage[band] = new QImage(drawLen + 2, fontH, QImage::Format_MonoLSB);
    painter.begin(_bandwidthImage[band]);
    painter.setPen(Qt::white);
    painter.setFont(font);
    _bandwidthImage[band]->fill(painter.brush().color());
    painter.drawText(_bandwidthImage[band]->rect(), ECGSymbol::convert((ECGBandwidth)band));
    painter.end();

    currentConfig.getNumValue("ECG12L|ECG12LeadBandwidth", band);
    band += ECG_BANDWIDTH_0525_40HZ;
    if (band != (int)ecgParam.getDiagBandwidth())
    {
        drawLen = fontManager.textWidthInPixels(ECGSymbol::convert((ECGBandwidth)band), font);
        _bandwidthImage[band] = new QImage(drawLen + 2, fontH, QImage::Format_MonoLSB);
        painter.begin(_bandwidthImage[band]);
        painter.setPen(Qt::white);
        painter.setFont(font);
        _bandwidthImage[band]->fill(painter.brush().color());
        painter.drawText(_bandwidthImage[band]->rect(), ECGSymbol::convert((ECGBandwidth)band));
        painter.end();
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PrintContinuousLayout::~PrintContinuousLayout()
{
    _firstLineDrawStr.clear();
    _secLineDrawStr.clear();
    _thirdLineDrawStr.clear();
    _waveGainList.clear();
    _waveformDescList.clear();

    if (NULL != _leadoffImage)
    {
        delete _leadoffImage;
        _leadoffImage = NULL;
    }

    if (NULL != _leadFaultImage)
    {
        delete _leadFaultImage;
        _leadFaultImage = NULL;
    }

    if (NULL != _shockInfoImage)
    {
        delete _shockInfoImage;
        _shockInfoImage = NULL;
    }

    for (int i = 0; i <ECG_BANDWIDTH_NR; ++i)
    {
        if (NULL != _bandwidthImage[i])
        {
            delete _bandwidthImage[i];
            _bandwidthImage[i] = NULL;
        }
    }
}
