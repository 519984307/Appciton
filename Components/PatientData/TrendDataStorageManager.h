#pragma once
#include "StorageManager.h"
#include "TrendDataDefine.h"

class TrendDataStorageManagerPrivate;
class TrendDataStorageManager : public StorageManager
{
public:
    enum TrendDataFlag {
        CollectStatusPeriod = (1<<1),       /* Collect in time period*/
        CollectStatusAlarm = (1<<2),        /* Collect when alarm happen */
        CollectStatusPrint = (1<<3),        /* Collect when perform realtime print */
        CollectStatusFreeze = (1<<4),       /* Colloect when freeze */
        CollectStatusCOResult = (1<<5),     /* Collect when get CO result */

        HasAlarm = (1<<6),              /* has param alarm */
        HRSourceIsSpo2 = (1<<7),              /* SPO2 if set, otherwise, ECG */
        BRSourceIsResp = (1<<8),              /* resp if set, otherwise, CO2 */
    };

    Q_DECLARE_FLAGS(TrendDataFlags, TrendDataFlag)

    static TrendDataStorageManager &getInstance();
    ~TrendDataStorageManager();

    /* run periodly, data will collect when the timestamp can be divided by 5 */
    void periodRun(unsigned t);

    /* store data, need to know what cause the data to storage */
    void storeData(unsigned t, TrendDataFlags dataStatus);

    /* parse the trend data segment */
    TrendDataPackage parseTrendSegment(const TrendDataSegment *dataSegment);

    /* storage function, move the data from memory to flash */
    void run();

private:
    Q_DECLARE_PRIVATE(TrendDataStorageManager)
    TrendDataStorageManager();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TrendDataStorageManager::TrendDataFlags)

#define trendDataStorageManager (TrendDataStorageManager::getInstance())
