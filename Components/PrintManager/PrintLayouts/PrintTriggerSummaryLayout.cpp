#include "PrintTriggerSummaryLayout.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "FontManager.h"
#include "TimeManager.h"
#include "TimeDate.h"
#include "TimeSymbol.h"
#include "DataStorageDirManager.h"
#include "WaveformCache.h"
#include "PrintManager.h"
#include "ECGDefine.h"
#include "Debug.h"
#include <QDateTime>
#include "IThread.h"
#include "SummaryStorageManager.h"
#include "RescueDataDefine.h"

//foreground draw wave callback
extern void _foregroundCallback(QPainter &painter, const PrintWaveInfo &info);

/**************************************************************************************************
 * 构造
 *************************************************************************************************/
PrintTriggerSummaryLayout::PrintTriggerSummaryLayout() :
    PrintLayoutIFace(PRINT_LAYOUT_ID_SUMMARY_TRIGGER_REPORT),
    PrintSummaryLayout()
{

}

/**************************************************************************************************
 * 析构
 *************************************************************************************************/
PrintTriggerSummaryLayout::~PrintTriggerSummaryLayout()
{

}

/**************************************************************************************************
 * 开始布局
 *************************************************************************************************/
void PrintTriggerSummaryLayout::start()
{
    init();
    _timeNum = 0;
    _pageTimeLen = 0;
    _waveInfoList.clear();
    printManager.enablePrinterSpeed(printManager.getPrintSpeed());
}

/**************************************************************************************************
 * 终止
 *************************************************************************************************/
void PrintTriggerSummaryLayout::abort()
{
}

/**************************************************************************************************
 * 构建打印页
 *************************************************************************************************/
PrintPage *PrintTriggerSummaryLayout::exec(PrintContent *content)
{
    if (NULL == content)
    {
        return NULL;
    }

    if(content->extraDataLen != sizeof(long))
    {
        qdebug("Invalid data pointer.\n");
        return NULL;
    }

    _summaryitem = (SummaryItem *)(*((long *)content->extraData));
    if (NULL == _summaryitem || _timeNum == DATA_STORAGE_WAVE_TIME)
    {
        return NULL;
    }

    if ((content->printType != PRINT_TYPE_TRIGGER_CODE_MARKER || content->printType != PRINT_TYPE_TRIGGER_NIBP)
            && _timeNum >= 6)
    {
        SummaryItemDesc *desc =  reinterpret_cast<SummaryItemDesc *> (_summaryitem->getSummaryData());
        SummaryCommonInfo *commonInfo = reinterpret_cast<SummaryCommonInfo *>(_summaryitem->getSummaryData() + sizeof(SummaryItemDesc));
        int rate =  waveformCache.getSampleRate((WaveformID)commonInfo->ECGLeadName);
        while ( _summaryitem->getCacheWaveLength((WaveformID) commonInfo->ECGLeadName) < (_timeNum + _pageTimeLen) * rate)
        {
            IThread::msleep(100);
            int passtime = timeManager.getCurTime() - desc->time;
            if(passtime > _timeNum)
            {
                break;
            }
        }

        if(content->printType == PRINT_TYPE_TRIGGER_PHY_ALARM)
        {
            SummaryPhyAlarm *phyAlm = reinterpret_cast<SummaryPhyAlarm *>(_summaryitem->getSummaryData());

            if (phyAlm->waveID > WAVE_ECG_V6 && phyAlm->waveID < WAVE_NR)
            {
                rate = waveformCache.getSampleRate((WaveformID) phyAlm->waveID);
                int curlen = _summaryitem->getCacheWaveLength((WaveformID) phyAlm->waveID);
                while ( curlen < (_timeNum + _pageTimeLen) * rate)
                {
                    IThread::msleep(100);
                    int passtime = timeManager.getCurTime() - desc->time;
                    curlen = _summaryitem->getCacheWaveLength((WaveformID) phyAlm->waveID);
                    if(passtime > _timeNum)
                    {
                        break;
                    }
                }
            }
        }
    }

    PrintPage *page = buildPage((unsigned char *)_summaryitem->getSummaryData(), _summaryitem->summaryDataLength(), printFont());
    if (NULL != page)
    {
        return page;
    }

    return _drawWave((unsigned char *)_summaryitem->getSummaryData(), _summaryitem->summaryDataLength());
}

/**************************************************************************************************
 * 构建波形打印页
 *************************************************************************************************/
PrintPage *PrintTriggerSummaryLayout::_drawWave(unsigned char *data, int datalen)
{
    if (NULL == data)
    {
        return NULL;
    }

    SummaryItemDesc desc;
    if ((unsigned)datalen <= sizeof(SummaryItemDesc))
    {
        return NULL;
    }
    memcpy(&desc, data, sizeof(SummaryItemDesc));
    SummaryCommonInfo *info = (SummaryCommonInfo*)(data + sizeof(SummaryItemDesc));
    if (NULL == info)
    {
        return NULL;
    }

    if (0 == _timeNum)
    {
        int width = 0;
        PrintSpeed speed= printManager.getPrintSpeed();
        switch (speed)
        {
            case PRINT_SPEED_250:
                _pageTimeLen = 1;
                width = 25 * PIXELS_PER_MM;
                break;
            case PRINT_SPEED_500:
                _pageTimeLen = 1;
                width = 50 * PIXELS_PER_MM;
                break;
            default:
                break;
        }

        int y = fontManager.textHeightInPixels(printFont());
        int waveHeigh = SUMMARY_WAVE_HEIGHT - y;
        switch (desc.type)
        {
            case SUMMARY_ECG_RHYTHM:
            case SUMMARY_DIAG_ECG:
            case SUMMARY_ECG_FOREGROUND_ANALYSIS:
            case SUMMARY_SHOCK_DELIVERY:
            case SUMMARY_CHECK_PATIENT_ALARM:
            case SUMMARY_PACER_STARTUP:
            case SUMMARY_PRINTER_ACTION:
            {
                PrintWaveInfo waveInfo;
                waveInfo.summaryType = desc.type;
                waveInfo.id = WaveformID(info->ECGLeadName);
                waveInfo.calcLead = (ECGLead)info->ECGLeadName;
                waveInfo.co2Zoom = info->co2Zoom;
                waveInfo.spo2Gain = info->spo2Gain;
                waveInfo.respGain = info->respGain;
                waveInfo.ecgGain = info->ECGGain;
                waveInfo.time = desc.time;
                waveInfo.ecgBandwidth = info->ECGBandwidth;
                switch (waveInfo.ecgGain)
                {
                    case ECG_GAIN_X0125:
                    case ECG_GAIN_X025:
                    case ECG_GAIN_X05:
                    case ECG_GAIN_X10:
                    case ECG_GAIN_X15:
                    case ECG_GAIN_X20:
                    case ECG_GAIN_X30:
                    //    waveInfo.startY = y;
                    //    waveInfo.endY = y + SUMMARY_WAVE_HEIGHT / 2;
                    //    waveInfo.medialY = y + SUMMARY_WAVE_HEIGHT / 4;
                    //    break;
                    default:
                        waveInfo.startY = y;
                        waveInfo.endY = y + waveHeigh;
                        waveInfo.medialY = y + waveHeigh / 2;
                        break;
                }
                waveInfo.pageWidth = width;
                waveInfo.waveDataNum = _pageTimeLen * waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)width / waveInfo.waveDataNum;
                if (desc.type == SUMMARY_ECG_FOREGROUND_ANALYSIS)
                {
                    waveInfo.data = (char *) data;
                    waveInfo.callBack = _foregroundCallback;
                }
                else if(desc.type == SUMMARY_DIAG_ECG)
                {
                    SummaryDiagnosticECG *summary = reinterpret_cast<SummaryDiagnosticECG *>(data);
                    waveInfo.diagECGBandWidth = summary->diagBandWidth;
                }
                _waveInfoList.append(waveInfo);
                break;
            }
            case SUMMARY_PHY_ALARM:
            {
                SummaryPhyAlarm *summary = (SummaryPhyAlarm*)data;
                if (datalen != sizeof(SummaryPhyAlarm))
                {
                    return NULL;
                }
                PrintWaveInfo waveInfo;
                waveInfo.id = WaveformID(info->ECGLeadName);
                waveInfo.calcLead = (ECGLead) info->ECGLeadName;
                waveInfo.co2Zoom = info->co2Zoom;
                waveInfo.spo2Gain = info->spo2Gain;
                waveInfo.respGain = info->respGain;
                waveInfo.ecgGain = info->ECGGain;
                waveInfo.ecgBandwidth = info->ECGBandwidth;
                waveInfo.time = desc.time;
                switch (waveInfo.ecgGain)
                {
                    case ECG_GAIN_X0125:
                    case ECG_GAIN_X025:
                    case ECG_GAIN_X05:
                    case ECG_GAIN_X10:
                    case ECG_GAIN_X15:
                    case ECG_GAIN_X20:
                    case ECG_GAIN_X30:
                    default:
                    if (summary->waveID != (char) WAVE_NONE)
                    {
                        waveInfo.startY = y;
                        waveInfo.endY = y + waveHeigh / 2;
                        waveInfo.medialY = y + waveHeigh / 4;
                        break;
                    }
                    else
                    {
                        waveInfo.startY = y;
                        waveInfo.endY = y + waveHeigh;
                        waveInfo.medialY = y + waveHeigh / 2;
                    }
                        break;
                }
                waveInfo.pageWidth = width;
                waveInfo.waveDataNum = _pageTimeLen * waveformCache.getSampleRate(waveInfo.id);
                waveInfo.offsetX = (double)width / waveInfo.waveDataNum;
                _waveInfoList.append(waveInfo);

                if (summary->waveID > WAVE_ECG_V6 && summary->waveID < WAVE_NR)
                {
                    waveInfo.time = desc.time;
                    waveInfo.pageWidth = width;
                    waveInfo.waveDataNum = _pageTimeLen *
                            waveformCache.getSampleRate((WaveformID)summary->waveID);
                    waveInfo.offsetX = (double)width / waveInfo.waveDataNum;
                    waveInfo.startY = y + waveHeigh / 2;
                    waveInfo.endY = y + waveHeigh;
                    waveInfo.medialY = y + waveHeigh * 3 / 4;
                    waveInfo.id = (WaveformID)summary->waveID;
                    _waveInfoList.append(waveInfo);
                }
                break;
            }
            case SUMMARY_CODE_MAKER:
            case SUMMARY_NIBP:
            default:
                return NULL;
        }
    }

    if (_timeNum < DATA_STORAGE_WAVE_TIME)
    {
        WaveList::iterator waveIter = _waveInfoList.begin();
        while (waveIter != _waveInfoList.end())
        {
            PrintWaveInfo &waveinfo = *waveIter;

            waveinfo.pageNum = _timeNum / _pageTimeLen + 1;
            //initialize to invalid data
            qFill(waveinfo.waveBuff,
                  waveinfo.waveBuff + sizeof(waveinfo.waveBuff) / sizeof(waveinfo.waveBuff[0]),
                    0x40000000);

            int len = 0;

            if (waveinfo.id <= WAVE_ECG_V6)
            {
                if (info->waveNum > waveinfo.waveDataNum * (_timeNum + _pageTimeLen))
                {
                    len = waveinfo.waveDataNum;
                }
                else
                {
                    len = info->waveNum - waveinfo.waveDataNum * _timeNum;
                }

                if (len < 0)
                {
                    len = 0;
                }

                if (len > 0)
                {
                    memcpy(waveinfo.waveBuff, &info->ECGWave[_timeNum * waveinfo.waveDataNum],
                            len * 4);
                }

            }
            else
            {
                SummaryPhyAlarm *summary = (SummaryPhyAlarm*)data;

                if (summary->waveNum > waveinfo.waveDataNum * (_timeNum + _pageTimeLen))
                {
                    len = waveinfo.waveDataNum;
                }
                else
                {
                    len = summary->waveNum - waveinfo.waveDataNum * _timeNum;
                }

                if (len < 0)
                {
                    len = 0;
                }

                if (len > 0)
                {
                    memcpy(waveinfo.waveBuff, &summary->waveBuf[_timeNum * waveinfo.waveDataNum],
                            len * 4);
                }

            }

            ++waveIter;
        }

        _timeNum += _pageTimeLen;

        return buildWavePage(_waveInfoList, printFont());
    }
    else
    {
        return NULL;
    }
}
