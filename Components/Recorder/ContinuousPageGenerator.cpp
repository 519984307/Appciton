#include "ContinuousPageGenerator.h"
#include "LanguageManager.h"
#include "PatientManager.h"
#include "TrendCache.h"
#include "TimeDate.h"
#include "SystemManager.h"
#include "ParamInfo.h"
#include "WaveformCache.h"

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

    void getPrintWaveInfo();

    RecordPageGenerator::PageType curPageType;
    TrendDataPackage trendData;
};

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
