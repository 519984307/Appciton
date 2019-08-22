/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/25
 **/

#pragma once
#include "StorageManager.h"
#include "TrendDataDefine.h"
#include "TrendDataStorageManagerInterface.h"

#define SHORT_TREND_DATA_NUM  512   // the short trend data number for each time interval

class TrendDataStorageManagerPrivate;
class TrendDataStorageManager : public StorageManager, public TrendDataStorageManagerInterface
{
    Q_OBJECT
public:
    static TrendDataStorageManager &getInstance();
    ~TrendDataStorageManager();

    /* run periodly, data will collect when the timestamp can be divided by 5 */
    void periodRun(unsigned t);

    /**
     * @brief stopPeriodRun 暂停存储数据到缓冲
     */
    void stopPeriodRun();
    void restartPeriodRun();

    /* store data, need to know what cause the data to storage */
    void storeData(unsigned t, TrendDataFlags dataStatus);

    /* storage function, move the data from memory to flash */
    void run();

    /**
     * @brief registerShortTrend register a short trend storage in the memory
     * @param subParamID the sub param id
     */
    void registerShortTrend(SubParamID subParamID);

    /**
     * @brief unRegisterShortTrend unRegister a short trend in the memory
     * @param SubParamID the sub param id
     */
    void unRegisterShortTrend(SubParamID subParamID);

    /**
     * @brief getShortTrendData get a short period of trend data, the trend data is store in the memory,
     *        so it would be very fast to fetch.
     * @param subParamID the subparam id
     * @param dataBuf the buffer to store the trend data
     * @param count the number of trend data should be fetched. the value should be less than SHORT_TREND_DATA_NUM
     * @param interval the trend data interval
     * @return the number of read trend data
     */
    int getShortTrendData(SubParamID subParam, TrendDataType *dataBuf, int count, ShortTrendInterval interval) const;

    /**
     * @brief getLatestShortTrendData get the latest short trend data
     * @param subParam sub param id
     * @param interval the intreval
     * @return the trend data
     */
    TrendDataType getLatestShortTrendData(SubParamID subParam, ShortTrendInterval interval);

protected:
    /*reimplement*/
    void newPatientHandle();

signals:
    void newTrendDataArrived(ShortTrendInterval interval);
    void newNibpDataReceived();

private:
    Q_DECLARE_PRIVATE(TrendDataStorageManager)
    TrendDataStorageManager();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TrendDataStorageManager::TrendDataFlags)

#define trendDataStorageManager (TrendDataStorageManager::getInstance())
