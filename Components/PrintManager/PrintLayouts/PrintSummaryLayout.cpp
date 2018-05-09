#include <QPainter>
#include "LanguageManager.h"
#include "PrintSummaryLayout.h"
#include "PrintPage.h"
#include "TimeDate.h"
#include "ParamInfo.h"
#include "WaveformCache.h"
#include "CO2Define.h"
#include "SPO2Define.h"
#include "RESPDefine.h"
#include "ECGParam.h"
#include "ParamManager.h"
#include "UnitManager.h"
#include "ECGWaveWidget.h"
#include "PrintManager.h"
#include "QFontMetrics"
#include "CO2Param.h"
#include "SPO2Symbol.h"
#include "RESPSymbol.h"
#include "NIBPDefine.h"
#include "NIBPSymbol.h"
#include "Alarm.h"
#include "PatientDefine.h"
#include "SystemManager.h"
#include "SummaryStorageManager.h"

/**************************************************************************************************
 * 构造
 *************************************************************************************************/
PrintSummaryLayout::PrintSummaryLayout()
{
    _contentStr.clear();
    _curItem = 0;
}

/**************************************************************************************************
 * 析构
 *************************************************************************************************/
PrintSummaryLayout::~PrintSummaryLayout()
{
    _contentStr.clear();
}

/**************************************************************************************************
 * 打印页初始化
 *************************************************************************************************/
void PrintSummaryLayout::init()
{
    _contentStr.clear();
    _isStartPageInit = false;
    _isOtherPageInit = false;
    _isStartPageOver = false;
    _curItem = 0;
}

/**************************************************************************************************
 * 开始打印页初始化
 *************************************************************************************************/
void PrintSummaryLayout::_startInit(const SummaryHead &head)
{
    if (_isStartPageInit)
    {
        return;
    }

    QString text;
    _contentStr.clear();
    _contentStr << trs("SummaryReport");

   if (head.lastItemTime != 0)
   {
       timeDate.getDateTime(head.lastItemTime, text, true, true);
   }
   else
   {
       text = QString();
   }
   _contentStr << (trs("LastItemTime") + ": " + text);

   _contentStr << (trs("TotalItem") + ": " + QString::number(head.totalItems));
   _contentStr << (trs("DeviceID") + ": " + QString::fromAscii(head.deviceID));
   _contentStr << (trs("SerialNum") + ": " + QString::fromAscii(head.serialNum));
   _contentStr << (trs("PatName") + ": " + QString::fromAscii(head.patientName));

   _isStartPageInit = true;
   _curItem = 0;
}

/**************************************************************************************************
 * 构建打印页
 *************************************************************************************************/
PrintPage *PrintSummaryLayout::buildPage(const unsigned char *data, int len, const QFont &font)
{
    if (NULL == data)
    {
        return NULL;
    }

    if (!_isOtherPageInit)
    {
        SummaryItemDesc desc;
        if ((unsigned)len <= sizeof(SummaryItemDesc))
        {
            return NULL;
        }

        memcpy(&desc, data, sizeof(SummaryItemDesc));
        switch (desc.type)
        {
            case SUMMARY_ECG_RHYTHM:
            {
                SummaryECGRhythm *snapshot = (SummaryECGRhythm*)data;
                if (len != sizeof(SummaryECGRhythm))
                {
                    return NULL;
                }

                _getValue(snapshot->itemDesc, snapshot->commonInfo);
                break;
            }
            case SUMMARY_DIAG_ECG:
            {
                SummaryDiagnosticECG *snapshot = (SummaryDiagnosticECG*)data;
                if(len != sizeof(SummaryDiagnosticECG))
                {
                    return NULL;
                }
                _getValue(snapshot->itemDesc, snapshot->commonInfo);
                break;
            }
            case SUMMARY_ECG_FOREGROUND_ANALYSIS:
            {
                SummaryForegroundECGAnalysis *snapshot = (SummaryForegroundECGAnalysis*)data;
                if (len != sizeof(SummaryForegroundECGAnalysis))
                {
                    return NULL;
                }

                _getValue(snapshot->itemDesc, snapshot->commonInfo);

                QString resultStr = trs("AnalysisResult") + ": ";
                switch((FgAnalysisResult)snapshot->analysisResult)
                {
                case FG_ANALYSIS_HALTED:
                    resultStr += trs("AnalysisHalted");
                    break;
                case FG_ANALYSIS_NOISY_ECG:
                    resultStr += trs("NoisyEcg");
                    break;
                case FG_ANALYSIS_NO_SHOCK_ADVISED:
                    resultStr += trs("NoShockAdvised");
                    break;
                case FG_ANALYSIS_SHOCK_ADVISED:
                    resultStr += trs("ShockAdvised");
                    break;
                default:
                    break;
                }
                _contentStr << resultStr;
                break;
            }
            case SUMMARY_SHOCK_DELIVERY:
            {
                SummaryShockDelivery *snapshot = (SummaryShockDelivery*)data;
                if (len != sizeof(SummaryShockDelivery))
                {
                    return NULL;
                }

                _getValue(snapshot->itemDesc, snapshot->commonInfo);

                _contentStr << trs("ShockCount") + ": " + QString::number(snapshot->shockCount);
                _contentStr << trs("Sel") + ": " + QString::number(snapshot->selectEngry) + " J";
                _contentStr << trs("Del") + ": " + QString::number(snapshot->deliveryEngry) + " J";
                _contentStr << trs("TTI") + ": " + QString().sprintf("%d", snapshot->patientImdepedance / 10) + QString::fromUtf8(" Ω");
                _contentStr << trs("Current") + ": " + QString::number(snapshot->patientCurrent / 10.0, 'f', 1) + " A";
                break;
            }
            case SUMMARY_CHECK_PATIENT_ALARM:
            {
                SummaryCheckPatient *snapshot = (SummaryCheckPatient*)data;
                if (len != sizeof(SummaryCheckPatient))
                {
                    return NULL;
                }

                _getValue(snapshot->itemDesc, snapshot->commonInfo);
                break;
            }
            case SUMMARY_PACER_STARTUP:
            {
                SummaryPacerStart *snapshot = (SummaryPacerStart*)data;
                if (len != sizeof(SummaryPacerStart))
                {
                    return NULL;
                }

                _getValue(snapshot->itemDesc, snapshot->commonInfo);
                break;
            }
            case SUMMARY_PHY_ALARM:
            {
                SummaryPhyAlarm *snapshot = (SummaryPhyAlarm*)data;
                if (len != sizeof(SummaryPhyAlarm))
                {
                    return NULL;
                }

                _getValue(snapshot->itemDesc, snapshot->commonInfo);

                QString text = trs("CurAlarmParamID") + ":";
                text += " " + trs(summaryStorageManager.getPhyAlarmMessage((ParamID)snapshot->curAlarmInfo.paramid,
                                                           snapshot->curAlarmInfo.alarmType,
                                                           snapshot->curAlarmInfo.isOneshot));

                _contentStr << text;

                int count = 0;
                for (int i = 0; i < snapshot->curActiveAlarmNum; i++)
                {
                    count ++;
                    text = trs("ActiveAlarmParamID") + ": " +
                            trs(summaryStorageManager.getPhyAlarmMessage((ParamID)snapshot->curActiveAlarmInfos[i].paramid,
                                                        snapshot->curActiveAlarmInfos[i].alarmType,
                                                        snapshot->curActiveAlarmInfos[i].isOneshot));
                    _contentStr << text;
                }

                //没有active报警
                if (0 == count)
                {
                    text = trs("ActiveAlarmParamID") + ": ";
                    text += trs("None");
                    _contentStr << text;
                }

                break;
            }
            case SUMMARY_PRINTER_ACTION:
            {
                SummaryPrinterAction *snapshot = (SummaryPrinterAction*)data;
                if (len != sizeof(SummaryPrinterAction))
                {
                    return NULL;
                }

                _getValue(snapshot->itemDesc, snapshot->commonInfo);
                break;
            }
            case SUMMARY_PRINTER_ACTION_2_TRACE:
            {
                SummaryPrinterAction2Trace *snapshot = (SummaryPrinterAction2Trace*)data;
                if(len != sizeof(SummaryPrinterAction2Trace))
                {
                    return NULL;
                }
                _getValue(snapshot->itemDesc, snapshot->commonInfo);
                break;
            }
            case SUMMARY_PRINTER_ACTION_3_TRACE:
            {
                SummaryPrinterAction3Trace *snapshot = (SummaryPrinterAction3Trace*)data;
                if(len != sizeof(SummaryPrinterAction3Trace))
                {
                    return NULL;
                }
                _getValue(snapshot->itemDesc, snapshot->commonInfo);
                break;
            }
            case SUMMARY_PRINTER_ACTION_4_TRACE:
            {
                SummaryPrinterAction4Trace *snapshot = (SummaryPrinterAction4Trace*)data;
                if(len != sizeof(SummaryPrinterAction4Trace))
                {
                    return NULL;
                }
                _getValue(snapshot->itemDesc, snapshot->commonInfo);
                break;
            }
            case SUMMARY_CODE_MAKER:
            {
                SummaryCodeMarker *snapshot = (SummaryCodeMarker*)data;
                if (len != sizeof(SummaryCodeMarker))
                {
                    return NULL;
                }

                _getCodeMarkerValue(snapshot);
                break;
            }
            case SUMMARY_NIBP:
            {
                SummaryNIBP *snapshot = (SummaryNIBP*)data;
                if(len != sizeof(SummaryNIBP))
                {
                    return NULL;
                }
                _getNIBPValue(snapshot);
            }
                break;
            default:
                return NULL;
        }

        _isOtherPageInit = true;

        _curItem = 0;
    }

    if (_curItem >= _contentStr.count())
    {
        return NULL;
    }

    return _drawPage(font);
}

/**************************************************************************************************
 * 构建开始打印页
 *************************************************************************************************/
PrintPage *PrintSummaryLayout::buildStartPage(const unsigned char *data, int len, const QFont &font)
{
    if (NULL == data)
    {
        return NULL;
    }

    if ((unsigned)len < sizeof(SummaryHead))
    {
        return NULL;
    }

    SummaryHead head;
    memcpy(&head, data, sizeof(SummaryHead));

    _startInit(head);
    int totalItem = _contentStr.count();

    if (_curItem == totalItem)
    {
        _isStartPageOver = true;
        return NULL;
    }

    if (_isStartPageOver)
    {
        return NULL;
    }

    int fontHigh = fontManager.textHeightInPixels(font);
    int w = _calPageWidth(font);
    if (w <= fontHigh * 2)
    {
        return NULL;
    }

    PrintPage *page = new PrintPage(w);
    page->setPrintSpeed(printManager.getPrintSpeed());
    static int index = 0;
    page->setID("SummaryStart" + QString::number(index++));

    QRect r = page->rect();
    QPainter painter(page);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.fillRect(r, Qt::SolidPattern); // 清空所有的数据。

    // 下面开始绘制内容。
    int yoffset = fontHigh;
    int count = 0;
    int gap = (fontHigh + fontHigh / 3);
    int pageNum = IMAGE_HEIGHT / gap;

    for (int i = _curItem; i < totalItem; ++i)
    {
        QString text = _contentStr.at(i);
        painter.drawText(fontHigh, yoffset, text);
        yoffset += gap;

        ++count;
        if (count >= pageNum)
        {
            break;
        }
    }

    _curItem += count;

    if (_curItem >= totalItem)
    {
        _isStartPageOver = true;
    }

    page->setDrawHeight(yoffset - gap);

    return page;
}

/**************************************************************************************************
 * 构建波形打印页
 *************************************************************************************************/
PrintPage *PrintSummaryLayout::buildWavePage(const QList<PrintWaveInfo> &printWaveInfo,
                                             const QFont &font)
{
    int count = printWaveInfo.count();

    if (0 == count)
    {
        return NULL;
    }

    PrintPage *page = new PrintPage(printWaveInfo.first().pageWidth);
    static int index = 0;
    page->setID("SummaryWave" + QString::number(index++));
    QPainter painter(page);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.fillRect(page->rect(), Qt::SolidPattern); // 清空所有的数据。
    qreal x1 = 0,y1 = 0,x2 = 0,y2 = 0;  // 需要连接的两点。
    static qreal lastWave[4];           //记录图片的最后一个像素的Y坐标, 4 trace at most
    int fontH = fontManager.textHeightInPixels(font);

    for (int j = 0; j < count; ++j)
    {
        PrintWaveInfo info = printWaveInfo.at(j);

        x1 = 0;
        y1 = 0;
        if(info.offsetX < 1 || info.pageNum == 1)
        {
            x2 =  0;
        }
        else
        {
            x2 = info.offsetX;
        }
        y2 = 0;

        _drawWaveLabel(painter, info, j);

        if (NULL != info.callBack)
        {
            info.callBack(painter, info);
        }

        // 数据映射。
        double max = 0;
        double min = 0;
        short wave = 0;
        double waveData = 0.0;
        for (int i = 0; i < info.waveDataNum; i++)
        {
            // 判断数据是否有效。
            // 数据无效（数据无效时，画虚线）。
            if (info.waveBuff[i] & 0x40000000)
            {
                if(lastWave[j] != info.medialY && (i > 0 || info.pageNum >1))
                {
                     x2 = x1 + info.offsetX;
#if 0   //avoid draw solid line when wave data change from valid to invalid
                     QLineF line(x1, (qreal)lastWave[j], x2, (qreal)info.medialY);
                     painter.setPen(QPen(Qt::white, Qt::SolidLine));
                     painter.drawLine(line);
#endif
                }
                else
                {
                    int k = i + 1;

                    // 判断后续是否有更多的无效数据
                    if (k < info.waveDataNum)
                    {
                        while (info.waveBuff[k] & 0x40000000)
                        {
                            x2 += info.offsetX;

                            if (info.waveBuff[k] & 0xA0000)//epace
                            {
                                QPoint start(x2, info.medialY);
                                QPoint end(x2, info.endY - 10 - fontH);
                                QRect r(0, info.startY, page->rect().width(), info.endY - info.startY);
                                ECGWaveWidget::drawEPaceMark(painter, start, end, r,
                                                             (info.waveBuff[k] >> 16));
                            }
                            ++k;

                            if ((k >= info.waveDataNum) || (x2 >= info.pageWidth))
                            {
                                break;
                            }
                        }

                        i = k - 1;
                    }

                    //CO2由于会画标尺现，为了避免标尺线和波形虚线重叠造成虚实结合的线这里CO2不绘制虚线
                    if (WAVE_CO2 != info.id)
                    {
                        QLineF dotLine(x1, (qreal)info.medialY, x2, (qreal)info.medialY);
                        QVector<qreal> darsh;
                        darsh << 5 << 5;
                        QPen pen(Qt::white);
                        pen.setDashPattern(darsh);
                        QPen oldPen = painter.pen();
                        painter.setPen(pen);
                        painter.drawLine(dotLine);
                        painter.setPen(oldPen);
                    }
                }



                // 防止页与页之间不连续
                lastWave[j] = info.medialY;

                //y2 set to zero, then it's value will assign to y1, we can avoid draw solid line
                //when wave data change from invalid to valid
                y2 = 0;

            }
            else
            {
                max = 0;
                min = 0;
                _getWaveRange(info, max, min);

                wave = (short)info.waveBuff[i];
                waveData = _mapWaveValue(info, wave, max, min);

                // 画线。
                if (i == 0 && info.pageNum > 1)
                {
                    y1 = lastWave[j];
                }
                else
                {
                    y1 = (y1 == 0) ? waveData : y1;
                }
                y2 = waveData;

                painter.setPen(QPen(Qt::white, Qt::SolidLine));
                QLineF line(x1, y1, x2, y2);
                painter.drawLine(line);
                // 防止页与页之间不连续
                lastWave[j] = y2;

                //在12L界面中实时打印所有的波形都需要添加标记，其他界面只有计算导联才要添加标记
                if (info.id == ecgParam.leadToWaveID(ecgParam.getCalcLead()))
                {
                    if (info.waveBuff[i] & 0x10000)//ipace
                    {
                        QPoint start(x2, 10);
                        QPoint end(x2, info.endY - 10);
                        ECGWaveWidget::drawIPaceMark(painter, start, end);
                    }

                    if (info.waveBuff[i] & 0xA0000)//epace
                    {
                        QPoint start(x2, y2);
                        QPoint end(x2, info.endY - 10 - fontH);
                        QRect r(0, info.startY, page->rect().width(), info.endY - info.startY);
                        ECGWaveWidget::drawEPaceMark(painter, start, end, r, (info.waveBuff[i] >> 16));
                    }

                    if (info.waveBuff[i] & 0x40000)//R mark
                    {
                        QPoint start(x2, y2);
                        QRect r(0, info.startY, page->rect().width(), info.endY - info.startY);
                        ECGWaveWidget::drawRMark(painter, start, r);
                    }
                }
            }
            x1 = x2;
            x2 += info.offsetX;
            y1 = y2;

            if(page->getDrawHeight() < info.endY)
            {
                page->setDrawHeight(info.endY);
            }

            if(x2 > info.pageWidth - 1 && info.offsetX > 1)
            {
                //当x1在页宽范围内，x2不在页宽范围内时，通过计算两点连线的斜率来判断x坐标为页宽时点的y坐标，并进行连线
                x2 = info.pageWidth - 1;
                if (i + 1 < info.waveDataNum)
                {
                    wave = (short)info.waveBuff[i + 1];
                    qreal tmpValue = _mapWaveValue(info, wave, max, min);
                    qreal offsety = abs(y1 * 1000 - tmpValue * 1000) / 1000.0 * (info.pageWidth * 1.0 - 1.0 - x1) / info.offsetX;
                    y2 = (y1 > tmpValue) ? (y1 - offsety) : (y1 + offsety);

                    lastWave[j] = y2;
                }

                break;
            }
        }

        QLineF line(x1, y1, info.pageWidth, y2);
        painter.drawLine(line);
    }
    return page;
}

/**************************************************************************************************
 * 获取波形范围
 *************************************************************************************************/
void PrintSummaryLayout::_getWaveRange(const PrintWaveInfo &info, double &max, double &min)
{
    int maxValue = 0;
    int minValue = 0;
    waveformCache.getRange(info.id, minValue, maxValue);

    switch (info.id)
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
        case WAVE_RESP:
        case WAVE_SPO2:
            max = maxValue;
            min = minValue;
            break;
        case WAVE_CO2:
        {
            max = maxValue;
            if (info.co2Zoom == CO2_DISPLAY_ZOOM_4)
            {
                max = (maxValue * 4 + 19) / 20;
            }
            else if (info.co2Zoom == CO2_DISPLAY_ZOOM_8)
            {
                max = (maxValue * 8 + 19) / 20;
            }
            else if (info.co2Zoom == CO2_DISPLAY_ZOOM_12)
            {
                max = (maxValue * 12 + 19) / 20;
            }
            else
            {
                max = maxValue;
            }

            min = minValue;
            break;
        }
        default:
            break;
    }
}

/**************************************************************************************************
 * 映射波形值
 * value:波形实际值
 * max:波形最大值
 * min:波形最小值
 *************************************************************************************************/
double PrintSummaryLayout::_mapWaveValue(const PrintWaveInfo &info, short value,
                                         double max, double min)
{
    double waveData = 0;
    switch (info.id)
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
            double scaleData = 0.0;
            switch (info.ecgGain)
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
            double endY = info.medialY + scaleData;
            double startY = info.medialY - scaleData;
            waveData = info.medialY - value * ((endY - startY) / (max - min));
            break;
        }
        case WAVE_RESP:
        {
            int waveBaseline = 0;
            waveformCache.getBaseline(info.id, waveBaseline);
            // 将枚举类型的放大倍数转换为实际倍数值。
            double respZoom = info.respGain - 1;
            switch (info.respGain)
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

            short wave = (value - waveBaseline) * respZoom + waveBaseline;
            waveData = ((max - wave) * ((info.endY - info.startY) / (max - min)) + info.startY);
            break;
        }
        case WAVE_SPO2:
        {
            double spo2Zoom = 1.0;
            int baseline;
            waveformCache.getBaseline(info.id, baseline);
            switch(info.spo2Gain)
            {
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

            value = (value - baseline) * spo2Zoom + baseline;
            waveData = (max - value) * ((info.endY - info.startY) / (max - min)) + info.startY;
            break;

        }
        case WAVE_CO2:
        {
            waveData = (max - value) * ((info.endY - info.startY) / (max - min)) + info.startY;
            break;
        }
        default:
            return 0;
    }

    // 截顶处理
    waveData = (waveData < info.startY) ? info.startY : waveData;
    waveData = (waveData > info.endY) ? info.endY : waveData;
    return waveData;
}

/***************************************************************************************************
 * helper function to check whether sub param can print
 **************************************************************************************************/
static bool canPrintSubParamHelper(SubParamID subParamid, short moduleConfig)
{
    switch(subParamid)
    {
    case SUB_PARAM_RR_BR:
        return moduleConfig & (CONFIG_CO2 | CONFIG_RESP);
    case SUB_PARAM_HR_PR:
        return true;
    case SUB_PARAM_ETCO2:
    case SUB_PARAM_FICO2:
        return moduleConfig & CONFIG_CO2;
    case SUB_PARAM_NIBP_DIA:
    case SUB_PARAM_NIBP_MAP:
    case SUB_PARAM_NIBP_SYS:
        return moduleConfig & CONFIG_NIBP;
    case SUB_PARAM_SPO2:
        return moduleConfig & CONFIG_SPO2;
    case SUB_PARAM_T1:
    case SUB_PARAM_T2:
    case SUB_PARAM_TD:
        return moduleConfig & CONFIG_TEMP;
    default:
        return false;
    }
}

/***************************************************************************************************
 * helper function to get sub param name
 **************************************************************************************************/
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

/**************************************************************************************************
 * 获取公共信息
 *************************************************************************************************/
void PrintSummaryLayout::_getValue(const SummaryItemDesc &desc, const SummaryCommonInfo &info)
{
    QString text;
    QString t1,t2,td;
    _contentStr.clear();

    _contentStr << trs(getSummaryName((SummaryType)desc.type));

    timeDate.getDateTime(desc.time, text, true, true);
    _contentStr << (trs("PrintEventTime") + ": " + text);

    _contentStr << (trs("PatName") + ": " + QString::fromAscii(info.patientName));
    _contentStr << (trs("DeviceID") + ": " + QString::fromAscii(info.deviceID));
    _contentStr << (trs("SerialNum") + ": " + QString::fromAscii(info.serialNum));

    UnitType defaultUnit;
    UnitType unit;
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        if ((i != SUB_PARAM_NIBP_SYS) &&
            (i != SUB_PARAM_NIBP_DIA) &&
            (i != SUB_PARAM_NIBP_MAP))
        {
            defaultUnit = paramInfo.getUnitOfSubParam((SubParamID)i);
            unit = paramManager.getSubParamUnit(paramInfo.getParamID((SubParamID)i), (SubParamID)i);

            if(!canPrintSubParamHelper((SubParamID)i, info.moduleConfig))
            {
                continue;
            }

            text = getSubParamNameHelper((SubParamID)i, info.moduleConfig);

            text += " = ";
            if (info.paramValue[i].value == InvData())
            {
                text += InvStr();
            }
            else
            {
                if (unit == defaultUnit)
                {
                    if (1 == info.paramValue[i].mul)
                    {
                        text += QString::number(info.paramValue[i].value);
                    }
                    else
                    {
                        QString fnum;
                        fnum.sprintf("%.1f", info.paramValue[i].value *
                                     1.0 / info.paramValue[i].mul);

                        if (i == SUB_PARAM_T1 || i == SUB_PARAM_T2)
                        {
                            if (info.paramValue[i].value > 500)
                            {
                                text += "> " + Unit::convert(unit, defaultUnit, 50);
                            }
                            else if (info.paramValue[i].value < 0)
                            {
                                text += "< " + Unit::convert(unit, defaultUnit, 0);
                            }
                            else
                            {
                                text += fnum;
                            }
                        }
                        else
                        {

                            text += fnum;
                        }
                    }
                }
                else
                {
                    if (i == SUB_PARAM_T1 || i == SUB_PARAM_T2)
                    {
                        if (info.paramValue[i].value > 500)
                        {
                            text += "> 50";
                        }
                        else if (info.paramValue[i].value < 0)
                        {
                            text += "< 0";
                        }
                        else
                        {
                            text += Unit::convert(unit, defaultUnit,
                                                  info.paramValue[i].value * 1.0 / info.paramValue[i].mul
                                                  , info.co2bro);
                        }
                    }
                    else if (i == SUB_PARAM_TD)
                    {
                        t1 = Unit::convert(unit, defaultUnit,
                                           info.paramValue[SUB_PARAM_T1].value * 1.0 / info.paramValue[SUB_PARAM_T1].mul);
                        t2 = Unit::convert(unit, defaultUnit,
                                           info.paramValue[SUB_PARAM_T2].value * 1.0 / info.paramValue[SUB_PARAM_T2].mul);
                        text += td.sprintf("%.1f", fabs(t1.toDouble() - t2.toDouble()));
                    }
                    else
                    {
                        text += Unit::convert(unit, defaultUnit,
                                              info.paramValue[i].value * 1.0 / info.paramValue[i].mul
                                              , info.co2bro);
                    }
                }
            }
            text += " " + Unit::localeSymbol(unit);
            _contentStr << text;
        }
    }

    if(info.moduleConfig & CONFIG_NIBP)
    {
        defaultUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
        unit = paramManager.getSubParamUnit(PARAM_NIBP, SUB_PARAM_NIBP_SYS);
        text = paramInfo.getParamName(PARAM_NIBP);
        text += " = ";
        if (info.paramValue[SUB_PARAM_NIBP_SYS].value == InvData())
        {
            text += InvStr();
            text += "/";
            text += InvStr();
            text += " (";
            text += InvStr();
            text += ")";
        }
        else
        {
            int mul = info.paramValue[SUB_PARAM_NIBP_SYS].mul;
            if (unit == defaultUnit)
            {
                text += QString::number(info.paramValue[SUB_PARAM_NIBP_SYS].value);
                text += "/";
                text += QString::number(info.paramValue[SUB_PARAM_NIBP_DIA].value);
                text += " (";
                text += QString::number(info.paramValue[SUB_PARAM_NIBP_MAP].value);
                text += ")";
            }
            else
            {
                text += Unit::convert(unit, defaultUnit, info.paramValue[SUB_PARAM_NIBP_SYS].value
                                      * 1.0 / mul);
                text += "/";
                text += Unit::convert(unit, defaultUnit, info.paramValue[SUB_PARAM_NIBP_DIA].value
                                      * 1.0 / mul);
                text += " (";

                text += Unit::convert(unit, defaultUnit, info.paramValue[SUB_PARAM_NIBP_MAP].value
                                      * 1.0 / mul);
                text += ")";
            }
        }
        text += " " + Unit::localeSymbol(unit);
        _contentStr << text;

        text = trs("PrintEventTime") + ":";
        if (info.lastMeasureTime)
        {
            timeDate.getDateTime(info.lastMeasureTime, text, true, true);
            _contentStr << (trs("PrintEventTime") + ": " + text);
        }
        else
        {
            _contentStr<<text;
        }
    }

    text = trs("NotchFilter") + ": ";
    switch((ECGNotchFilter)info.ECGNotchFilter)
    {
    case ECG_NOTCH_50HZ:
        text += "50 Hz";
        break;
    case ECG_NOTCH_60HZ:
        text += "60 Hz";
        break;
    case ECG_NOTCH_50_AND_60HZ:
        text += "50&60 Hz";
        break;
    case ECG_NOTCH_OFF1:
    default:
        text += "OFF";
        break;
    }
    _contentStr << text;

    text = trs("PatientType") + ": ";
    switch((PatientType)info.patientType)
    {
    case PATIENT_TYPE_ADULT:
        text += trs("Adult");
        break;
    case PATIENT_TYPE_PED:
        text += trs("Pediatric");
        break;
    case PATIENT_TYPE_NEO:
        text += trs("Neonate");
        break;
    default:
        break;
    }
    _contentStr << text;



    _contentStr << (trs("ChartSpeed") + ":" + QString(" %1 mm/s").arg(printManager.getPrintSpeed() == PRINT_SPEED_500 ? 50 : 25));
}

/**************************************************************************************************
 * 获取code marker信息
 *************************************************************************************************/
void PrintSummaryLayout::_getCodeMarkerValue(SummaryCodeMarker *data)
{
    if (NULL == data)
    {
        return;
    }

    QString text;
    QString t1,t2,td;
    _contentStr.clear();

    _contentStr << trs(getSummaryName((SummaryType)data->itemDesc.type));

    _contentStr << (trs("SelectCodeName") + ": " + QString(trs(data->selectCodeName)));

    timeDate.getDateTime(data->itemDesc.time, text, true, true);
    _contentStr << (trs("PrintEventTime") + ": " + text);

    _contentStr << (trs("PatName") + ": " + QString::fromAscii(data->patientName));
    _contentStr << (trs("DeviceID") + ": " + QString::fromAscii(data->deviceID));
    _contentStr << (trs("SerialNum") + ": " + QString::fromAscii(data->serialNum));

    if(data->sync)
    {
        _contentStr << trs("Sync");
    }

    UnitType unit;
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        unit = paramInfo.getUnitOfSubParam((SubParamID)i);
        if ((i != SUB_PARAM_NIBP_SYS) &&
            (i != SUB_PARAM_NIBP_DIA) &&
            (i != SUB_PARAM_NIBP_MAP))
        {
            if(!canPrintSubParamHelper((SubParamID)i, data->moduleConfig))
            {
                continue;
            }

            text = getSubParamNameHelper((SubParamID)i, data->moduleConfig);

            text += " = ";
            if (data->paramValue[i].value == UnknownData())
            {
                text += UnknownStr();
            }
            else if (data->paramValue[i].value == InvData())
            {
                text += InvStr();
            }
            else
            {
                if (unit == data->paramValue[i].type)
                {
                    if (1 == data->paramValue[i].mul)
                    {
                        text += QString::number(data->paramValue[i].value);
                    }
                    else
                    {
                        QString fnum;
                        fnum.sprintf("%.1f", data->paramValue[i].value *
                            1.0 / data->paramValue[i].mul);
                        text += fnum;
                    }
                }
                else
                {
                    if (1 == data->paramValue[i].mul)
                    {
                        text += Unit::convert(data->paramValue[i].type, unit,
                                      data->paramValue[i].value, data->co2bro);
                    }
                    else
                    {
                        if (i == SUB_PARAM_TD)
                        {
                            t1 = Unit::convert(data->paramValue[SUB_PARAM_T1].type, unit,
                                               data->paramValue[SUB_PARAM_T1].value * 1.0 / data->paramValue[SUB_PARAM_T1].mul);
                            t2 = Unit::convert(data->paramValue[SUB_PARAM_T2].type, unit,
                                               data->paramValue[SUB_PARAM_T2].value * 1.0 / data->paramValue[SUB_PARAM_T2].mul);
                            text += td.sprintf("%.1f", fabs(t1.toDouble() - t2.toDouble()));
                        }
                        else
                        {
                            text += Unit::convert(data->paramValue[i].type, unit,
                                          data->paramValue[i].value * 1.0 / data->paramValue[i].mul,
                                          data->co2bro);
                        }

                    }
                }
            }
            text +=  " " + Unit::localeSymbol(data->paramValue[i].type);
            _contentStr << text;
        }
    }

    if(data->moduleConfig & CONFIG_NIBP)
    {
        text = paramInfo.getParamName(PARAM_NIBP);
        text += " = ";
        if (data->paramValue[SUB_PARAM_NIBP_SYS].value == InvData())
        {
            text += InvStr();
            text += "/";
            text += InvStr();
            text += "(";
            text += InvStr();
            text += ")";
        }
        else
        {
            unit = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
            UnitType curUnit = data->paramValue[SUB_PARAM_NIBP_SYS].type;
            int mul = data->paramValue[SUB_PARAM_NIBP_SYS].mul;
            if (unit == curUnit)
            {
                text += QString::number(data->paramValue[SUB_PARAM_NIBP_SYS].value);
                text += "/";
                text += QString::number(data->paramValue[SUB_PARAM_NIBP_DIA].value);
                text += "(";
                text += QString::number(data->paramValue[SUB_PARAM_NIBP_MAP].value);
                text += ")";
            }
            else
            {
                text += Unit::convert(curUnit, unit, data->paramValue[SUB_PARAM_NIBP_SYS].value
                                      * 1.0 / mul);
                text += "/";

                text += Unit::convert(curUnit, unit, data->paramValue[SUB_PARAM_NIBP_DIA].value
                                      * 1.0 / mul);
                text += "(";

                text += Unit::convert(curUnit, unit, data->paramValue[SUB_PARAM_NIBP_MAP].value
                                      * 1.0 / mul);
                text += ")";
            }
        }
        text += " " + Unit::localeSymbol(data->paramValue[SUB_PARAM_NIBP_SYS].type);
        _contentStr << text;

        text = trs("PrintEventTime") + ":";
        if (data->lastMeasureCompleteTime)
        {
            timeDate.getDateTime(data->lastMeasureCompleteTime, text, true, true);
            _contentStr << (trs("PrintEventTime") + ": " + text);
        }
        else
        {
            _contentStr<<text;
        }
    }
}

/***************************************************************************************************
 * get NIBP info
 **************************************************************************************************/
void PrintSummaryLayout::_getNIBPValue(SummaryNIBP *data)
{
    if (NULL == data)
    {
        return;
    }
    QString text;
    QString t1,t2,td;
    _contentStr.clear();

    _contentStr << trs(getSummaryName((SummaryType)data->itemDesc.type));

    timeDate.getDateTime(data->itemDesc.time, text, true, true);
    _contentStr << (trs("PrintEventTime") + ": " + text);

    //if failed, print additional fail reason
    if ((NIBPOneShotType)data->nibpErrorCode != NIBP_ONESHOT_NONE)
    {
        text = trs("Failure");
        text += ": " + trs(NIBPSymbol::convert((NIBPOneShotType) data->nibpErrorCode));
        _contentStr << text;
    }

    _contentStr << (trs("PatName") + ": " + QString::fromAscii(data->patientName));
    _contentStr << (trs("DeviceID") + ": " + QString::fromAscii(data->deviceID));
    _contentStr << (trs("SerialNum") + ": " + QString::fromAscii(data->serialNum));

    UnitType unit;
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        unit = paramInfo.getUnitOfSubParam((SubParamID)i);
        if ((i != SUB_PARAM_NIBP_SYS) &&
            (i != SUB_PARAM_NIBP_DIA) &&
            (i != SUB_PARAM_NIBP_MAP))
        {
            if(!canPrintSubParamHelper((SubParamID)i, data->moduleConfig))
            {
                continue;
            }

            text = getSubParamNameHelper((SubParamID)i, data->moduleConfig);

            text += " = ";
            if (data->paramValue[i].value == UnknownData())
            {
                text = UnknownStr();
            }
            else if (data->paramValue[i].value == InvData())
            {
                text += InvStr();
            }
            else
            {
                if (unit == data->paramValue[i].type)
                {
                    if (1 == data->paramValue[i].mul)
                    {
                        text += QString::number(data->paramValue[i].value);
                    }
                    else
                    {
                        QString fnum;
                        fnum.sprintf("%.1f", data->paramValue[i].value *
                            1.0 / data->paramValue[i].mul);
                        text += fnum;
                    }
                }
                else
                {
                    if (1 == data->paramValue[i].mul)
                    {
                        text += Unit::convert(data->paramValue[i].type, unit,
                                      data->paramValue[i].value, data->co2bro);
                    }
                    else
                    {
                        if (i == SUB_PARAM_TD)
                        {
                            t1 = Unit::convert(data->paramValue[SUB_PARAM_T1].type, unit,
                                               data->paramValue[SUB_PARAM_T1].value * 1.0 / data->paramValue[SUB_PARAM_T1].mul);
                            t2 = Unit::convert(data->paramValue[SUB_PARAM_T2].type, unit,
                                               data->paramValue[SUB_PARAM_T2].value * 1.0 / data->paramValue[SUB_PARAM_T2].mul);
                            text += td.sprintf("%.1f", fabs(t1.toDouble() - t2.toDouble()));
                        }
                        else
                        {
                            text += Unit::convert(data->paramValue[i].type, unit,
                                          data->paramValue[i].value * 1.0 / data->paramValue[i].mul,
                                          data->co2bro);
                        }

                    }
                }
            }
            text +=  " " + Unit::localeSymbol(data->paramValue[i].type);
            _contentStr << text;
        }
    }

    text = paramInfo.getParamName(PARAM_NIBP);
    text += " = ";
    if (data->paramValue[SUB_PARAM_NIBP_SYS].value == InvData())
    {
        text += InvStr();
        text += "/";
        text += InvStr();
        text += "(";
        text += InvStr();
        text += ")";
    }
    else
    {
        unit = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
        UnitType curUnit = data->paramValue[SUB_PARAM_NIBP_SYS].type;
        int mul = data->paramValue[SUB_PARAM_NIBP_SYS].mul;
        if (unit == curUnit)
        {
            text += QString::number(data->paramValue[SUB_PARAM_NIBP_SYS].value);
            text += "/";
            text += QString::number(data->paramValue[SUB_PARAM_NIBP_DIA].value);
            text += "(";
            text += QString::number(data->paramValue[SUB_PARAM_NIBP_MAP].value);
            text += ")";
        }
        else
        {
            text += Unit::convert(curUnit, unit, data->paramValue[SUB_PARAM_NIBP_SYS].value
                    * 1.0 / mul);
            text += "/";

            text += Unit::convert(curUnit, unit, data->paramValue[SUB_PARAM_NIBP_DIA].value
                    * 1.0 / mul);
            text += "(";

            text += Unit::convert(curUnit, unit, data->paramValue[SUB_PARAM_NIBP_MAP].value
                    * 1.0 / mul);
            text += ")";
        }
    }
    text += " " + Unit::localeSymbol(data->paramValue[SUB_PARAM_NIBP_SYS].type);
    _contentStr << text;

    text = trs("PatientType") + ": ";
    switch((PatientType)data->patientType)
    {
    case PATIENT_TYPE_ADULT:
        text += trs("Adult");
        break;
    case PATIENT_TYPE_PED:
        text += trs("Pediatric");
        break;
    case PATIENT_TYPE_NEO:
        text += trs("Neonate");
        break;
    default:
        break;
    }
    _contentStr << text;
}

/**************************************************************************************************
 * 计算打印页宽度
 *************************************************************************************************/
int PrintSummaryLayout::_calPageWidth(const QFont &font)
{
    int fontH = fontManager.textHeightInPixels(font);
    int gap = fontH + fontH / 3;
    int curWidth = 0;
    int width = 0;
    int high = fontH;
    int totalItem = _contentStr.count();
    QFontMetrics fm(font);

    QString text;
    for (int i = _curItem; i < totalItem; ++i)
    {
        text = _contentStr.at(i);
        curWidth = fm.width(text);
        if (width < curWidth)
        {
            width = curWidth;
        }

        high += gap;
        // 超过一列
        if (high + fontH >= IMAGE_HEIGHT)
        {
            break;
        }
    }

    // 返回宽度加上左右边距
    return width + 4 * fontH;
}

/***************************************************************************************************
 * drawFragmentText : mainly use to draw text on the fragment wave pages.
 *           @painter: painter
 *           @startX : start x position
 *           @startY : start y position
 *           @fragmentWidth : each fragment width
 *           @currentID : current fragment ID , id start from 1
 *           @text: text to print
 **************************************************************************************************/
static void drawFragmentText(QPainter &painter, int startX, int startY, int fragmentWidth, int currentID, QString &text)
{
    QFontMetrics fm(painter.font());
    int printTextWidth = fm.width(text);
    int currentPrintWidth = (currentID - 1) * fragmentWidth;
    if(printTextWidth > currentPrintWidth)
    {
        painter.drawText(startX - currentPrintWidth, startY, text);
    }
}

/**************************************************************************************************
 * 画波形标签
 *************************************************************************************************/
void PrintSummaryLayout::_drawWaveLabel(QPainter &painter, const PrintWaveInfo &info, int index)
{
    int x = 0;
    int y = info.startY;
    int fontH = fontManager.textHeightInPixels(painter.font());

    switch (info.id)
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
            if(index == 0)
            {
                // 导联名称, 增益, Bandwidth
                QString caption = QString("%1    %2    %3").arg(ECGSymbol::convert((ECGLead)info.id, ecgParam.getLeadConvention(), false))
                        .arg(ECGSymbol::convert((ECGGain)info.ecgGain))
                        .arg(ECGSymbol::convert((ECGBandwidth)info.ecgBandwidth));
                drawFragmentText(painter, x, y, info.pageWidth, info.pageNum, caption);

                //draw the start time of the wave strip
                QString text;
                timeDate.getDateTime(info.time - 6, text, true, true);
                drawFragmentText(painter, x, y + fontH, info.pageWidth, info.pageNum, text);


                // 波形开始和10s处的标记
                int rate = waveformCache.getSampleRate(info.id);
                int timeLen = info.waveDataNum / rate;
                int lastTime = timeLen * (info.pageNum - 1);
                int curTime = timeLen * info.pageNum;
                //draw time of the event
                if ((curTime > 6) && (lastTime <= 6))
                {
                    x = (info.pageWidth / timeLen) * (6 - lastTime);
                    timeDate.getTime(info.time, text, true);
                    painter.drawText(x, y + fontH, text);
                    if(info.summaryType == SUMMARY_CHECK_PATIENT_ALARM)
                    {
                        painter.drawText(x, y, trs(getSummaryName((SummaryType)info.summaryType)));
                    }
                    else if(info.summaryType == SUMMARY_DIAG_ECG)
                    {
                        //draw diagnostic bandwidth
                        painter.drawText(x, y, ECGSymbol::convert((ECGBandwidth)info.diagECGBandWidth));
                    }
                }

                //draw time of 10s intervals from the start of the wave strip
                if ((curTime > 10) && (lastTime <= 10))
                {
                    x = (info.pageWidth / timeLen) * (10 - lastTime);
                    timeDate.getTime(info.time + 4, text, true);
                    painter.drawText(x, y + fontH, text);
                }
            }
            else
            {
                if(info.drawAllEcgGain)
                {
                    QString caption;
                    // 导联名称, 增益
                    caption = QString("%1    %2").arg(ECGSymbol::convert((ECGLead)info.id, ecgParam.getLeadConvention(), false))
                            .arg(ECGSymbol::convert((ECGGain)(info.ecgGain)));
                    drawFragmentText(painter, x, y, info.pageWidth, info.pageNum, caption);
                }
                else if(1 == info.pageNum)
                {
                    QString text = QString("%1").arg(ECGSymbol::convert((ECGLead)info.id, ecgParam.getLeadConvention(), false));
                    painter.drawText(x, y + fontH, text);
                }
            }
            break;
        }
        case WAVE_RESP:
            if (1 == info.pageNum)
            {
                QString text = QString("%1    %2").arg(paramInfo.getParamName(PARAM_RESP)).arg(RESPSymbol::convert((RESPZoom)info.respGain));
                painter.drawText(x, y + fontH, text);
            }
            break;
        case WAVE_SPO2:
            if (1 == info.pageNum)
            {
                QString text = QString("%1    %2").arg("Pleth").arg(SPO2Symbol::convert((SPO2Gain)info.spo2Gain));
                painter.drawText(x, y + fontH, text);
            }
            break;
        case WAVE_CO2:
        {
            QPen pen = painter.pen();
            Qt::PenStyle style = pen.style();
            pen.setStyle(Qt::DashLine);
            QVector<qreal> darsh(2, 4);
            pen.setDashPattern(darsh);
            painter.setPen(pen);
            QLineF line1, line2, line3;
            int y1 = info.startY;
            int y2 = 0;
            int y3 = info.endY;
            float max = 0;
            float min = 0;

            y2 = (y3 - y1) / 2 + y1;
            if (info.co2Zoom == CO2_DISPLAY_ZOOM_4)
            {
                max = 4.0;
            }
            else if (info.co2Zoom == CO2_DISPLAY_ZOOM_8)
            {
                max = 8.0;
            }
            else if (info.co2Zoom == CO2_DISPLAY_ZOOM_12)
            {
                max = 12.0;
            }
            else
            {
                max = 20.0;
            }

            QString text = QString::fromAscii(paramInfo.getParamName(PARAM_CO2));
            if (1 == info.pageNum)
            {
                painter.drawText(x, y + fontH, text);

                int textlen = fontManager.textWidthInPixels(text, painter.font()) + 10;
                line1 = QLineF(x + textlen, y1, x + info.pageWidth, y1);
                line2 = QLineF(x + textlen, y2, x + info.pageWidth, y2);
                line3 = QLineF(x + textlen, y3, x + info.pageWidth, y3);

                UnitType  unit = co2Param.getUnit();
                QString  tt;
                QString midStr;
                if (unit == UNIT_KPA)
                {
                    float tempVal = Unit::convert(UNIT_KPA, UNIT_PERCENT, max).toFloat();
                    text.sprintf("%.1f", (float)(int)(tempVal + 0.5));
                    text += " " + Unit::localeSymbol(unit);
                    tempVal = Unit::convert(UNIT_KPA, UNIT_PERCENT, min).toFloat();
                    tt.sprintf("%.1f", (float)(int)(tempVal + 0.5));
                    tempVal = Unit::convert(UNIT_KPA, UNIT_PERCENT, (max + min) / 2).toFloat();
                    midStr.sprintf("%.1f", (float)(int)(tempVal + 0.5));
                }
                else if(unit == UNIT_MMHG)
                {
                    int tempVal = Unit::convert(UNIT_MMHG, UNIT_PERCENT, max).toInt();
                    tempVal = (tempVal + 5) / 10 * 10;
                    text = QString::number(tempVal);
                    text += " " + Unit::localeSymbol(unit);
                    tt = Unit::convert(UNIT_MMHG, UNIT_PERCENT, min);
                    midStr = QString::number(tempVal / 2);
                }
                else
                {
                    text.sprintf("%.1f", max);
                    text += " " + Unit::localeSymbol(unit);
                    tt.sprintf("%.1f", min);
                    midStr.sprintf("%.1f", (max + min) / 2);
                }
                painter.drawText(x + textlen, y1 + painter.pen().width() + fontH, text);

                painter.drawText(x + textlen, y2 + painter.pen().width() + fontH, midStr);

                painter.drawText(x + textlen, y3 - painter.pen().width(), tt);
            }
            else
            {
                line1 = QLineF(x, y1, x + info.pageWidth, y1);
                line2 = QLineF(x, y2, x + info.pageWidth, y2);
                line3 = QLineF(x, y3, x + info.pageWidth, y3);
            }

            painter.drawLine(line1);
            painter.drawLine(line2);
            painter.drawLine(line3);
            pen.setStyle(style);
            painter.setPen(pen);
            break;
        }
        default:
            break;
    }
}

/**************************************************************************************************
 * 构建打印页
 *************************************************************************************************/
PrintPage *PrintSummaryLayout::_drawPage(const QFont &font)
{
    int fontH = fontManager.textHeightInPixels(font);
    int minWidth = fontManager.textWidthInPixels(trs("SummaryReportComplete"), font);
    int w = _calPageWidth(font);
    if (w <= fontH * 2)
    {
        return NULL;
    }
    else if(w < minWidth)
    {
        //need enough space to print complete info
        w = minWidth;
    }

    int gap = fontH + fontH / 3;

    PrintPage *page = new PrintPage(w);
    if(_curItem == 0)
    {
        page->setPrintSpeed(printManager.getPrintSpeed());
    }
    static int index = 0;
    page->setID("Summary" + QString::number(index++));

    QRect r = page->rect();
    QPainter painter(page);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.fillRect(r, Qt::SolidPattern); // 清空所有的数据。

    int yoffset = fontH;
    int count = 0;
    int totalItem = _contentStr.count();
    for (int i = _curItem; i < totalItem; ++i)
    {
        QString text = _contentStr.at(i);
        painter.drawText(fontH, yoffset, text);
        yoffset += gap;

        ++count;
        if (yoffset + fontH >= IMAGE_HEIGHT)
        {
            break;
        }
    }

    _curItem += count;
    page->setDrawHeight(yoffset - gap);

    return page;
}
