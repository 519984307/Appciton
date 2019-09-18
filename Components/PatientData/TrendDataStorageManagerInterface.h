/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/

#pragma once
#include <qglobal.h>

class TrendDataStorageManagerInterface
{
public:
    virtual ~TrendDataStorageManagerInterface(){}

    static TrendDataStorageManagerInterface *registerTrendDataStorageManager(TrendDataStorageManagerInterface *);

    static TrendDataStorageManagerInterface *getTrendDataStorageManager(void);

    enum TrendDataFlag {
        CollectStatusNone = 0,
        CollectStatusPeriod = (1<<1),       /* Collect in time period*/
        CollectStatusAlarm = (1<<2),        /* Collect when alarm happen */
        CollectStatusPrint = (1<<3),        /* Collect when perform realtime print */
        CollectStatusFreeze = (1<<4),       /* Colloect when freeze */
        CollectStatusCOResult = (1<<5),     /* Collect when get CO result */
        CollectStatusNIBP = (1<<6),         /* Collect when get NIBP result */

        HasAlarm = (1<<7),              /* has param alarm */
        HRSourceIsSpo2 = (1<<8),              /* SPO2 if set, otherwise, ECG */
        BRSourceIsResp = (1<<9),              /* resp if set, otherwise, CO2 */

        NIBPAlarm = (1 << 10),               /* nibp alarm flag trend data */
    };

    Q_DECLARE_FLAGS(TrendDataFlags, TrendDataFlag)

    /**
     * @brief storeData store data, need to know what cause the data to storage
     * @param t
     * @param dataStatus
     */
    virtual void storeData(unsigned t, TrendDataFlags dataStatus) = 0;
};
