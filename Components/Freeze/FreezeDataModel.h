/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/2/28
 **/


#pragma once
#include <QObject>
#include "ParamDefine.h"
#include "BaseDefine.h"
#include <QScopedPointer>

class FreezeDataModelPrivate;
class  FreezeDataModel : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief FreezeDataModel  freeDataModel
     * @param t the timestamp this model created
     * @param id wave id
     * @param parent
     */
    FreezeDataModel(unsigned t, WaveformID id,  QObject *parent = 0);
    ~FreezeDataModel();

    /**
     * @brief timestamp get the create timestamp
     */
    unsigned timestamp() const;

    /**
     * @brief totalReveiwTime get the total reveiw time
     * @return  time
     */
    unsigned totalReveiwTime() const;

    /**
     * @brief getReviewStartSecond get the current review time
     * @return
     */
    unsigned getReviewStartSecond() const;

    /**
     * @brief setReviewStartSecond set the review time
     * @param second
     */
    void setReviewStartSecond(int second);

    /**
     * @brief getWaveData fill data to the buffer
     * @note  the newest data is on the end of the buffer, will fill invalid data
     *        if data is no enough
     * @param buff the wave buffer
     * @param size the data size
     * @return
     */
    void getWaveData(WaveDataType *buff, int size);

    /**
     * @brief getWaveformID get waveform id of this model
     * @return the waveform id
     */
    WaveformID getWaveformID() const;

    /**
     * @brief getSampleRate get the wave's sample rate
     * @return sample rate
     */
    int getSampleRate() const;


signals:
    /**
     * @brief dataChanged notify the data has been change, need to
     */
    void dataChanged();

private:
    QScopedPointer<FreezeDataModelPrivate> d_ptr;
};
