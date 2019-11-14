/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/18
 **/

#include "OxyCRGPageGenerator.h"
#include "PatientManager.h"
#include "TrendCache.h"
#include "Framework/TimeDate/TimeDate.h"

class OxyCRGPageGeneratorPrivate
{
public:
    OxyCRGPageGeneratorPrivate()
        : curPageType(RecordPageGenerator::TitlePage)
    {
        TrendCacheData data;
        TrendAlarmStatus almStatus;
        unsigned t = timeDate->time();
        trendCache.getTrendData(t, &data);
        trendCache.getTrendAlarmStatus(t, &almStatus);
        bool alarm = false;
        foreach(bool st, almStatus.alarms)
        {
            if (st)
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

        QMap<SubParamID, TrendDataType>::iterator it = trendData.subparamValue.begin();
        while (it != trendData.subparamValue.end())
        {
            if (it.key() != SUB_PARAM_HR_PR && it.key() != SUB_PARAM_RR_BR
                    && it.key() != SUB_PARAM_SPO2 && it.key() != SUB_PARAM_ETCO2
                    && it.key() != SUB_PARAM_FICO2)
            {
                it = trendData.subparamValue.erase(it);
                continue;
            }
            ++it;
        }
    }

    TrendDataPackage trendData;
    RecordPageGenerator::PageType curPageType;
    QList<TrendGraphInfo> trendInfos;
    OxyCRGWaveInfo oxyCRGWaveInfo;
    QString eventTitle;
};

OxyCRGPageGenerator::OxyCRGPageGenerator(const QList<TrendGraphInfo> &trendInfos,
        const OxyCRGWaveInfo &waveInfo, QString &eventTitle,
        QObject *parent)
    : RecordPageGenerator(parent), d_ptr(new OxyCRGPageGeneratorPrivate)
{
    d_ptr->trendInfos = trendInfos;
    d_ptr->oxyCRGWaveInfo = waveInfo;
    d_ptr->eventTitle = eventTitle;
}

OxyCRGPageGenerator::~OxyCRGPageGenerator()
{
}

int OxyCRGPageGenerator::type() const
{
    return Type;
}

RecordPage *OxyCRGPageGenerator::createPage()
{
    switch (d_ptr->curPageType)
    {
    case TitlePage:
        // BUG: patient info of the event might not be the current session patient
        d_ptr->curPageType = TrendPage;
        return createTitlePage(d_ptr->eventTitle, patientManager.getPatientInfo());
    case TrendPage:
        d_ptr->curPageType = TrendOxyCRGPage;
        return createTrendPage(d_ptr->trendData, true);
    case TrendOxyCRGPage:
        d_ptr->curPageType = EndPage;
        return createOxyCRGGraph(d_ptr->trendInfos, d_ptr->oxyCRGWaveInfo);
    case EndPage:
        d_ptr->curPageType = NullPage;
        return createEndPage();
    default:
        break;
    }
    return NULL;
}
