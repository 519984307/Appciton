#include "ContinuousPageGenerator.h"
#include "LanguageManager.h"
#include "PatientManager.h"
#include "TrendCache.h"
#include "TimeDate.h"
#include "SystemManager.h"
#include "ParamInfo.h"

class ContinuousPageGeneratorPrivate
{
public:
    enum GenerateState {
        Title,
        Trend,
        Wave,
    };

    ContinuousPageGeneratorPrivate()
        :state(Title)
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

    int state;
    TrendDataPackage trendData;
};

ContinuousPageGenerator::ContinuousPageGenerator(QObject *parent)
    :RecordPageGenerator(parent), d_ptr(new ContinuousPageGeneratorPrivate())
{

}

ContinuousPageGenerator::~ContinuousPageGenerator()
{

}

int ContinuousPageGenerator::type() const
{
    return Type;
}

RecordPage *ContinuousPageGenerator::createPage()
{
    switch(d_ptr->state)
    {
    case ContinuousPageGeneratorPrivate::Title:
        d_ptr->state += 1;
        return createTitlePage(QString(trs("RealtimeSegmentWaveRecording")), patientManager.getPatientInfo(), 0);
    case ContinuousPageGeneratorPrivate::Trend:
        d_ptr->state += 1;
        return createTrendPage(d_ptr->trendData, false);
    default:
        break;
    }
    return NULL;
}
