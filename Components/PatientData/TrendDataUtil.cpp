#include "TrendDataDefine.h"

TrendDataPackage parseTrendSegment(const TrendDataSegment *dataSegment)
{
    TrendDataPackage dataPackage;
    if(dataSegment)
    {
        dataPackage.time = dataSegment->timestamp;
        dataPackage.co2Baro = dataSegment->co2Baro;
        for(int i = 0; i < dataSegment->trendValueNum; i++)
        {
            dataPackage.subparamValue[(SubParamID)dataSegment->values[i].subParamId] = dataSegment->values[i].value;
            dataPackage.subparamAlarm[(SubParamID)dataSegment->values[i].subParamId] = dataSegment->values[i].alarmFlag;
            if (!dataPackage.alarmFlag && dataSegment->values[i].alarmFlag)
            {
                dataPackage.alarmFlag = dataSegment->values[i].alarmFlag;
            }
        }
    }
    return dataPackage;
}
