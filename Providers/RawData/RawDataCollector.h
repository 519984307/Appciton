/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/11/15
 **/

#pragma once

class RawDataCollectorPrivate;
class RawDataCollector
{
public:
    enum CollectDataType
    {
        ECG_DATA,
        SPO2_DATA,
        NIBP_DATA,
        TEMP_DATA,
        DATA_TYPE_NR,
    };

    static RawDataCollector &getInstance();

    ~RawDataCollector();

    /**
     * @brief collectData collect data
     * @param type the data type
     * @param data pointer to the raw data
     * @param len the data length
     */
    void collectData(CollectDataType type, const unsigned char *data, int len);

    /**
     * @brief setCollectStatus enable or disable the collection of specific type
     * @param type the type
     * @param enable
     */
    void setCollectStatus(CollectDataType type, bool enable);

    // 循环运行
    void run();

private:
    RawDataCollector();
    RawDataCollector(const RawDataCollector &other);    // stop the cppcheck complain

    RawDataCollectorPrivate * const d_ptr;
};

#define rawDataCollector (RawDataCollector::getInstance())
