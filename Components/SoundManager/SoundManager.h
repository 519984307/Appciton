/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/25
 **/

#pragma once
#include <QObject>
#include <QScopedPointer>
#include "Framework/Sound/SoundManagerInterface.h"
#include "AlarmDefine.h"

class SoundManagerPrivate;
class SoundManager : public QObject, public SoundManagerInterface
{
    Q_OBJECT
public:
    ~SoundManager();

    /**
     * @brief getInstance get the sound manager instance
     * @return  the sound manager instance
     */
    static SoundManager &getInstance();

    /**
     * @brief keyPressTone play an key press tone
     */
    virtual void keyPressTone();

    /**
     * @brief errorTone play an error tone
     */
    void errorTone();

    /**
     * @brief pulseTone play the pulse volumn, if the spo2Value is in range of [0, 100],
     *        sound manager will play the smart pulse tone, for any value out of the range,
     *        the sound manager will play a normal pulse tone
     * @param spo2Value spo2 param value
     */
    void pulseTone(short spo2Value = -1);

    /**
     * @brief heartBeatTone play heart beat sound
     */
    void heartBeatTone();

    /**
     * @brief alarmTone  play alarm sound
     */
    void alarmTone();

    /**
     * @brief nibpCompleteTone play nibp complete tone
     */
    void nibpCompleteTone();

    /**
     * @brief updateAlarm update alarm info
     * @param hasAlarm  the alarm type exist or not
     * @param curHighestPriority if alarm exists, need to provide the current highest priority alarm level
     */
    void updateAlarm(bool hasAlarm, AlarmPriority curHighestPriority = ALARM_PRIO_PROMPT);

    /**
     * @brief setVolume set the volumn level of specific sound type
     * @param type sound type
     * @param lev volumn level
     */
    void setVolume(SoundType type, VolumeLevel lev);

    /**
     * @brief getVolume get the volumn of specific sound type
     * @param type sound type
     * @return  the volumn level
     */
    VolumeLevel getVolume(SoundType type);

    /**
     * @brief selfTest perform selftest
     */
    void selfTest();

    /**
     * @brief stopHandlingSound  是否停止处理声音
     * @param enable  true-不允许播放  false-允许播放
     */
    void stopHandlingSound(bool enable);

    /**
     * @brief setNIBPCompleteTone   是否开启NIBP完成音
     * @param status
     */
    void setNIBPCompleteTone(bool status);

public slots:
    /**
     * @brief volumeInit volume initialize slot, call after selftest
     */
    void volumeInit();

private slots:
    /**
     * @brief alarmTimeout handle looping timer timeout event
     */
    void alarmTimeout();

    /**
     * @brief playFinished  callback when finished, no matter play success or not
     */
    void playFinished();

private:
    SoundManager();
    QScopedPointer<SoundManagerPrivate> d_ptr;
    Q_DISABLE_COPY(SoundManager)
};

#define soundManager (SoundManager::getInstance())
