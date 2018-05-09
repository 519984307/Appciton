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

class ContinuousPageGeneratorPrivate
{
public:
    ContinuousPageGeneratorPrivate()
        :curPageType(RecordPageGenerator::TitlePage)
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

    QList<RecordWaveSegmentInfo> getPrintWaveInfo();

    RecordPageGenerator::PageType curPageType;
    TrendDataPackage trendData;
};

/**
 * @brief ContinuousPageGeneratorPrivate::getPrintWaveInfo get the continuous print wave info
 * @return a list contain the wave segment info of the waves that need to print
 */
QList<RecordWaveSegmentInfo> ContinuousPageGeneratorPrivate::getPrintWaveInfo()
{
    QList<int> waveID;
    QList<RecordWaveSegmentInfo> infos;

    int printWaveNum = recorderManager.getPrintWaveNum();
    PrintSpeed speed = recorderManager.getPrintSpeed();
    int lengthOfOneSecond = 250; //default for 25mm/s, muliply 10
    switch(speed)
    {
    case PRINT_SPEED_125:
        lengthOfOneSecond = 125;
        break;
    case PRINT_SPEED_500:
        lengthOfOneSecond = 500;
        break;
    default:
        break;
    }

    windowManager.getDisplayedWaveform(waveID);

    //FIXME: Currently, we print the the first @printWaveNum waves in the window manager,
    //       but we should dispay the waveform form the configuration

    for(int i = 0; i < waveID.length() && i < printWaveNum; i++)
    {
        WaveformID id = (WaveformID)waveID.at(i);
        RecordWaveSegmentInfo info;
        info.id = id;
        info.pageNum = 0;
        info.pageWidth = RECORDER_PIXEL_PER_MM * lengthOfOneSecond / 10;
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
        }
            break;
        case WAVE_IBP2:
        {
            info.waveInfo.ibp.pressureName = ibpParam.getEntitle(IBP_INPUT_2);
            IBPScaleInfo scaleInfo = ibpParam.getIBPScale(info.waveInfo.ibp.pressureName);
            info.waveInfo.ibp.high = scaleInfo.high;
            info.waveInfo.ibp.low = scaleInfo.low;
            info.waveInfo.ibp.isAuto = scaleInfo.isAuto;
            break;
        }
        default:
            break;
        }

        infos.append(info);
    }

    //calculate the wave region in the print page
    int waveRegionHeight = (RECORDER_PAGE_HEIGHT - RECORDER_WAVE_UPPER_MARGIN - RECORDER_WAVE_LOWER_MARGIN)
                            / infos.size();
    QList<RecordWaveSegmentInfo>::iterator iter;
    int yOffset = RECORDER_WAVE_UPPER_MARGIN;
    for(iter = infos.begin(); iter != infos.end(); iter++)
    {
        iter->startYOffset = yOffset;
        yOffset += waveRegionHeight;
        iter->endYOffset = yOffset;
    }

    return infos;
}

ContinuousPageGenerator::ContinuousPageGenerator(QObject *parent)
    :RecordPageGenerator(parent), d_ptr(new ContinuousPageGeneratorPrivate())
{
    waveformCache.startRealtimeChannel();
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
        return createTrendPage(d_ptr->trendData, true);
    case WaveScalePage:
        d_ptr->curPageType = WaveSegmentPage;
        return NULL;
    case WaveSegmentPage:
        d_ptr->curPageType = EndPage;
        return NULL;
    case EndPage:
        d_ptr->curPageType = NullPage;
        return NULL;
    default:
        break;
    }
    return NULL;
}

