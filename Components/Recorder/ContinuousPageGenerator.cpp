#include "ContinuousPageGenerator.h"
#include "LanguageManager.h"
#include "PatientManager.h"
#include "TrendCache.h"
#include "TimeDate.h"
#include "SystemManager.h"
#include "ParamInfo.h"
#include "WaveformCache.h"
#include "RecorderManager.h"
#include "WindowManager.h"
#include "ECGParam.h"
#include "RESPParam.h"
#include "SPO2Param.h"
#include "CO2Param.h"
#include "AGParam.h"
#include "IBPParam.h"
#include "ParamManager.h"
#include "Utility.h"

class ContinuousPageGeneratorPrivate
{
public:
    ContinuousPageGeneratorPrivate(ContinuousPageGenerator * const q_ptr)
        : q_ptr(q_ptr),
          curPageType(RecordPageGenerator::TitlePage),
          curDrawWaveSegment(0),
          totalDrawWaveSegment(-1)
    {
        TrendCacheData data;
        TrendAlarmStatus almStatus;
        unsigned t = timeDate.time();
        trendCache.getTendData(t, data);
        trendCache.getTrendAlarmStatus(t, almStatus);
        bool alarm = false;
        foreach (bool st, almStatus.alarms) {
            if(st)
            {
                alarm = true;
                break;
            }
        }
        trendData.subparamValue = data.values;
        trendData.subparamAlarm = almStatus.alarms;
        trendData.co2Baro = data.co2baro;
        trendData.time = t;
        trendData.alarmFlag = alarm;
    }

    QList<RecordWaveSegmentInfo> getPrintWaveInfos();

    void fetchWaveData();

    bool isParamStop(WaveformID id);

    ContinuousPageGenerator *q_ptr;
    RecordPageGenerator::PageType curPageType;
    TrendDataPackage trendData;
    QList<RecordWaveSegmentInfo> waveInfos;
    int curDrawWaveSegment;
    int totalDrawWaveSegment;   //total wave segments that need to draw, -1 means unlimit
};

/**
 * @brief ContinuousPageGeneratorPrivate::getPrintWaveInfo get the continuous print wave info
 * @return a list contain the wave segment info of the waves that need to print
 */
QList<RecordWaveSegmentInfo> ContinuousPageGeneratorPrivate::getPrintWaveInfos()
{
    QList<int> waveID;
    QList<RecordWaveSegmentInfo> infos;

    int printWaveNum = recorderManager.getPrintWaveNum();

    windowManager.getDisplayedWaveform(waveID);

    //FIXME: Currently, we print the the first @printWaveNum waves in the window manager,
    //       but we should dispay the waveform form the configuration

    for(int i = 0; i < waveID.length() && i < printWaveNum; i++)
    {
        WaveformID id = (WaveformID)waveID.at(i);
        QString caption = paramInfo.getParamName(paramInfo.getParamID(id));
        int captionLength = 0;
        RecordWaveSegmentInfo info;
        info.id = id;
        info.sampleRate = waveformCache.getSampleRate(id);
        waveformCache.getRange(id, info.minWaveValue, info.maxWaveValue);
        waveformCache.getBaseline(id, info.waveBaseLine);
        switch(id)
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
            caption = QString("%1   %2").arg(ECGSymbol::convert(ecgParam.waveIDToLeadID(id),
                                                                ecgParam.getLeadConvention()))
                    .arg(ECGSymbol::convert(ecgParam.getFilterMode()));
            info.waveInfo.ecg.in12LeadMode = windowManager.getUFaceType() == UFACE_MONITOR_12LEAD;
            info.waveInfo.ecg._12LeadDisplayFormat = ecgParam.get12LDisplayFormat();
            captionLength = fontManager.textWidthInPixels(caption, q_ptr->font());
            break;
        case WAVE_RESP:
            info.waveInfo.resp.zoom = respParam.getZoom();
            break;
        case WAVE_SPO2:
            info.waveInfo.spo2.gain = spo2Param.getGain();
            break;
        case WAVE_CO2:
            info.waveInfo.co2.zoom = co2Param.getDisplayZoom();
            break;
        case WAVE_N2O:
        case WAVE_AA1:
        case WAVE_AA2:
        case WAVE_O2:
            info.waveInfo.ag.zoom = agParam.getDisplayZoom();
            break;
        case WAVE_IBP1:
        {
            info.waveInfo.ibp.pressureName = ibpParam.getEntitle(IBP_INPUT_1);
            IBPScaleInfo scaleInfo = ibpParam.getIBPScale(info.waveInfo.ibp.pressureName);
            info.waveInfo.ibp.high = scaleInfo.high;
            info.waveInfo.ibp.low = scaleInfo.low;
            info.waveInfo.ibp.isAuto = scaleInfo.isAuto;
            info.waveInfo.ibp.unit = paramManager.getSubParamUnit(PARAM_IBP, SUB_PARAM_ART_SYS);
        }
            break;
        case WAVE_IBP2:
        {
            info.waveInfo.ibp.pressureName = ibpParam.getEntitle(IBP_INPUT_2);
            IBPScaleInfo scaleInfo = ibpParam.getIBPScale(info.waveInfo.ibp.pressureName);
            info.waveInfo.ibp.high = scaleInfo.high;
            info.waveInfo.ibp.low = scaleInfo.low;
            info.waveInfo.ibp.isAuto = scaleInfo.isAuto;
            info.waveInfo.ibp.unit = paramManager.getSubParamUnit(PARAM_IBP, SUB_PARAM_ART_SYS);
            break;
        }
        default:
            break;
        }

        captionLength = fontManager.textWidthInPixels(caption, q_ptr->font());
        info.drawCtx.captionPixLength = captionLength;
        Util::strlcpy(info.drawCtx.caption, qPrintable(caption), sizeof(info.drawCtx.caption));
        info.drawCtx.curPageFirstXpos = 0.0;
        info.drawCtx.prevSegmentLastYpos = 0.0;
        info.drawCtx.dashOffset = 0.0;
        info.drawCtx.lastWaveFlags = 0;
        infos.append(info);
    }

    //calculate the wave region in the print page
    int waveRegionHeight = (RECORDER_PAGE_HEIGHT - RECORDER_WAVE_UPPER_MARGIN - RECORDER_WAVE_LOWER_MARGIN)
                            / infos.size();

    // wave heights when has 3 waves
    int waveHeights[] = {120, 120, 80};

    QList<RecordWaveSegmentInfo>::iterator iter;
    int yOffset = RECORDER_WAVE_UPPER_MARGIN;
    int j=0;
    for(iter = infos.begin(); iter != infos.end(); iter++)
    {
        iter->startYOffset = yOffset;
        if(infos.size() == 3)
        {
            yOffset += waveHeights[j++];
        }
        else
        {
            yOffset += waveRegionHeight;
        }
        iter->endYOffset = yOffset;
        iter->middleYOffset = (iter->startYOffset + iter->endYOffset) / 2;
    }

    return infos;
}

/**
 * @brief ContinuousPageGeneratorPrivate::fetchWaveData
 *          get the wave data from the realtime channel in the wavecache.
 *          we should fetch 1 second wave data form the realtimeChannel,
 *          if wavecache don't have enough data, we fill invalid data.
 */
void ContinuousPageGeneratorPrivate::fetchWaveData()
{
    QList<RecordWaveSegmentInfo>::iterator iter;
    for(iter = waveInfos.begin(); iter < waveInfos.end(); iter++)
    {
        if(iter->secondWaveBuff.size() != iter->sampleRate)
        {
            iter->secondWaveBuff.resize(iter->sampleRate);
        }

        int curSize = 0;
        int retryCount = 0;
        int lastReadSize = 0;
        while(curSize < iter->sampleRate)
        {
            if(recorderManager.isAbort())
            {
                //param stop
                break;
            }

            if(isParamStop(iter->id))
            {
                //param stop
                break;
            }

            curSize += waveformCache.readRealtimeChannel(iter->id,
                                                         iter->sampleRate - curSize,
                                                         iter->secondWaveBuff.data() + curSize);

            if(++retryCount >= 1000) //1000 ms has passed and haven't finished reading
            {
                if(curSize == lastReadSize)
                {
                    break;
                }
            }

            lastReadSize = curSize;
            Util::waitInEventLoop(5);
        }

        if(curSize < iter->sampleRate)
        {
            //no enough data, fill with invalid
            qFill(iter->secondWaveBuff.data() + curSize, iter->secondWaveBuff.data()+iter->sampleRate, (WaveDataType) INVALID_WAVE_FALG_BIT);
        }
    }
}

/**
 * @brief ContinuousPageGeneratorPrivate::isParamStop check whether the param is stopped
 * @param id waveform id
 * @return true if stopped, otherwise, return false;
 */
bool ContinuousPageGeneratorPrivate::isParamStop(WaveformID id)
{
    return paramManager.isParamStopped(paramInfo.getParamID(id));
}

ContinuousPageGenerator::ContinuousPageGenerator(QObject *parent)
    :RecordPageGenerator(parent), d_ptr(new ContinuousPageGeneratorPrivate(this))
{
    waveformCache.startRealtimeChannel();
    d_ptr->waveInfos = d_ptr->getPrintWaveInfos();
}

ContinuousPageGenerator::~ContinuousPageGenerator()
{
    waveformCache.stopRealtimeChannel();
}

int ContinuousPageGenerator::type() const
{
    return Type;
}

RecordPage *ContinuousPageGenerator::createPage()
{
    switch(d_ptr->curPageType)
    {
    case TitlePage:
        d_ptr->curPageType = TrendPage;
        return createTitlePage(QString(trs("RealtimeSegmentWaveRecording")), patientManager.getPatientInfo(), 0);
    case TrendPage:
        d_ptr->curPageType = WaveScalePage;
        return createTrendPage(d_ptr->trendData, false);
    case WaveScalePage:
        d_ptr->curPageType = WaveSegmentPage;
        return createWaveScalePage(d_ptr->waveInfos, recorderManager.getPrintSpeed());
    case WaveSegmentPage:
    if(!recorderManager.isAbort())
    {
        RecordPage *page;
        d_ptr->fetchWaveData();

        page = createWaveSegments(d_ptr->waveInfos, d_ptr->curDrawWaveSegment++, recorderManager.getPrintSpeed());

        if(d_ptr->totalDrawWaveSegment > 0 && d_ptr->curDrawWaveSegment >= d_ptr->totalDrawWaveSegment)
        {
            d_ptr->curPageType = EndPage;
        }
        return page;
    }
    //fall through
    case EndPage:
        d_ptr->curPageType = NullPage;
        return createEndPage();
    default:
        break;
    }
    return NULL;
}

