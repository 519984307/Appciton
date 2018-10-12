/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/10/11
 **/


#include "TrendDataDefine.h"

TrendDataPackage parseTrendSegment(const TrendDataSegment *dataSegment)
{
    TrendDataPackage dataPackage;
    if (dataSegment)
    {
        dataPackage.time = dataSegment->timestamp;
        dataPackage.co2Baro = dataSegment->co2Baro;
        for (int i = 0; i < dataSegment->trendValueNum; i++)
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

const char *convert(ShortTrendDuration index)
{
    const char *trendDuration[] =
    {
        "30 min", "1 h", "2 h", "4 h", "8 h"
    };
    return trendDuration[index];
}
