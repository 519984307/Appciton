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
#include <QObject>

class RawDataCollectorPrivate;
class RawDataCollector : public QObject
{
    Q_OBJECT
public:
    enum CollectDataType
    {
        ECG_DATA,
        SPO2_DATA,
        NIBP_DATA,
        TEMP_DATA,
        CO2_DATA,
        DATA_TYPE_NR,
    };

    static RawDataCollector &getInstance();

    ~RawDataCollector();

    /**
     * @brief collectData collect data
     * @param type the data type
     * @param data pointer to the raw data
     * @param len the data length
     * @param stop stop collection flag
     */
    void collectData(CollectDataType type, const unsigned char *data, int len, bool stop = false);

    /**
     * @brief setCollectStatus enable or disable the collection of specific type
     * @param type the type
     * @param enable
     */
    void setCollectStatus(CollectDataType type, bool enable);

    // 循环运行
    void run();

public slots:
    void startCollectData();

    void stopCollectData();

protected:
    void timerEvent(QTimerEvent *e);

private:
    RawDataCollector();
    RawDataCollector(const RawDataCollector &other);    // stop the cppcheck complain

    RawDataCollectorPrivate * const d_ptr;
};

#define rawDataCollector (RawDataCollector::getInstance())
