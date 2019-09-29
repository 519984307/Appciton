/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/25
 **/

#include "SoundManager.h"
#include "WavFile.h"
#include "WavPlayer.h"
#include <QMap>
#include <QDebug>
#include <linux/soundcard.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <QTimer>
#include "IConfig.h"
#include "ConfigManager.h"
#include <QThread>
#include "NightModeManager.h"
#include "PatientManager.h"

#define SOUND_DIR "/usr/local/nPM/sound/"
#define PULSE_TONE_DIR "/usr/local/nPM/sound/pulse_tone/"
#define MUTE_CTRL_DEV "/dev/pmos_misc"
#define PMOS_MUTE_CMD _IOW('I', 0, int)
#define PMOS_MUTE_STATUS _IOR('I', 1, int)

enum PulseToneType
{
    NORMAL_PULSE_TONE,
    SMART_PULSE_TONE_80,
    SMART_PULSE_TONE_85,
    SMART_PULSE_TONE_90,
    SMART_PULSE_TONE_91,
    SMART_PULSE_TONE_92,
    SMART_PULSE_TONE_93,
    SMART_PULSE_TONE_94,
    SMART_PULSE_TONE_95,
    SMART_PULSE_TONE_96,
    SMART_PULSE_TONE_97,
    SMART_PULSE_TONE_98,
    SMART_PULSE_TONE_99,
    SMART_PULSE_TONE_100,
};

class SoundManagerPrivate
{
public:
    explicit SoundManagerPrivate(SoundManager *const q_ptr)
        : q_ptr(q_ptr), curSoundType(SoundManager::SOUND_TYPE_NONE), player(NULL),
          curVolume(SoundManager::VOLUME_LEV_0), muteCtrlFd(-1), almTimer(NULL),
          curAlarmPriority(ALARM_PRIO_PROMPT), pendingWavFile(NULL)
        , pendingVolume(SoundManager::VOLUME_LEV_0)
        , stopHandlingSound(false)
    {
        for (int i = SoundManager::SOUND_TYPE_NONE; i < SoundManager::SOUND_TYPE_NR; i++)
        {
            // set default volumn to VOLUMN_LEV_3
            soundVolumes[i] = SoundManager::VOLUME_LEV_3;
        }

        int fd = open(MUTE_CTRL_DEV, O_RDONLY);
        if (fd < 0)
        {
            perror("Open mute control device faild");
        }
        else
        {
            muteCtrlFd = fd;
        }


        almTimer = new QTimer(q_ptr);
        QObject::connect(almTimer, SIGNAL(timeout()), q_ptr, SLOT(alarmTimeout()));

        // initialize the alarm interval for the 3 alarm levels
        alarmInterval[0] = 25000;
        alarmInterval[1] = 15000;
        alarmInterval[2] = 10000;

        playerThread = new QThread(q_ptr);
        player = new WavPlayer();
        player->moveToThread(playerThread);

        QObject::connect(playerThread, SIGNAL(finished()), player, SLOT(deleteLater()));
        QObject::connect(player, SIGNAL(finished()), q_ptr, SLOT(playFinished()));
    }

    ~SoundManagerPrivate()
    {
    }


    /**
     * @brief key contruct the key  base on the sound's sound type and specify type,
     *        used to find the QSound object in the @soundObjects Map
     * @param soundType the sound type
     * @param specType the specific type of the sound type
     * @return
     */
    static int key(SoundManager::SoundType soundType, unsigned short specType = 0)
    {
        return ((unsigned short)soundType << 16) | specType;
    }

    /**
     * @brief getWavFilePath get a wav fiel base on the sound type and specType type
     * @param soundType  the sound type
     * @param specType the specific type of the sound type
     * @return
     */
    static const char *getWavFilePath(SoundManager::SoundType soundType, unsigned short specType = 0)
    {
        switch (soundType)
        {
        case SoundManager::SOUND_TYPE_NOTIFICATION:
            return SOUND_DIR"key.wav";
        case SoundManager::SOUND_TYPE_ERROR:
            return SOUND_DIR"error.wav";
        case SoundManager::SOUND_TYPE_PULSE:
            switch ((PulseToneType)specType)
            {
            case SMART_PULSE_TONE_80:
                return PULSE_TONE_DIR"pulse_80.wav";
            case SMART_PULSE_TONE_85:
                return PULSE_TONE_DIR"pulse_85.wav";
            case SMART_PULSE_TONE_90:
                return PULSE_TONE_DIR"pulse_90.wav";
            case SMART_PULSE_TONE_91:
                return PULSE_TONE_DIR"pulse_91.wav";
            case SMART_PULSE_TONE_92:
                return PULSE_TONE_DIR"pulse_92.wav";
            case SMART_PULSE_TONE_93:
                return PULSE_TONE_DIR"pulse_93.wav";
            case SMART_PULSE_TONE_94:
                return PULSE_TONE_DIR"pulse_94.wav";
            case SMART_PULSE_TONE_95:
                return PULSE_TONE_DIR"pulse_95.wav";
            case SMART_PULSE_TONE_96:
                return PULSE_TONE_DIR"pulse_96.wav";
            case SMART_PULSE_TONE_97:
                return PULSE_TONE_DIR"pulse_97.wav";
            case SMART_PULSE_TONE_98:
                return PULSE_TONE_DIR"pulse_98.wav";
            case SMART_PULSE_TONE_99:
                return PULSE_TONE_DIR"pulse_99.wav";
            case SMART_PULSE_TONE_100:
                return PULSE_TONE_DIR"pulse_100.wav";
            case NORMAL_PULSE_TONE:
                return PULSE_TONE_DIR"pulse_100.wav";
            default:
                break;
            }
            break;
        case SoundManager::SOUND_TYPE_HEARTBEAT:
            return SOUND_DIR"heart_beep.wav";
        case SoundManager::SOUND_TYPE_NIBP_COMPLETE:
            return SOUND_DIR"lo_lev.wav";
        case SoundManager::SOUND_TYPE_ALARM:
            switch ((AlarmPriority)specType)
            {
            case ALARM_PRIO_LOW:
                return SOUND_DIR"lo_lev.wav";
            case ALARM_PRIO_MED:
                return SOUND_DIR"med_lev.wav";
            case ALARM_PRIO_HIGH:
                return SOUND_DIR"hi_lev.wav";
            default:
                break;
            }
            break;
        default:
            break;
        }

        return "";
    }

    /**
     * @brief setVolume set the play volume
     * @param volume  volume level
     */
    void setVolume(SoundManager::VolumeLevel volumeLev)
    {
        int v = 0;
        switch (volumeLev)
        {
        case SoundManager::VOLUME_LEV_1:
            v = 70;
            break;
        case SoundManager::VOLUME_LEV_2:
            v = 78;
            break;
        case SoundManager::VOLUME_LEV_3:
            v = 80;
            break;
        case SoundManager::VOLUME_LEV_4:
            v = 88;
            break;
        case SoundManager::VOLUME_LEV_5:
            v = 100;
            break;
        default:
            break;
        }

        QMetaObject::invokeMethod(player, "setVolume", Q_ARG(int, v));
    }

    /**
     * @brief playSound play sound
     * @param soundType sound type
     * @param specType specify wav file type
     */
    void playSound(SoundManager::SoundType soundType, unsigned short specType = 0, bool isForceUpdate = false)
    {
        WavFile *wav = wavFiles.value(key(soundType, specType));
        if (wav == NULL)
        {
            wav = new WavFile();
            wav->load(getWavFilePath(soundType, specType));
            wavFiles.insert(key(soundType, specType), wav);
        }

        if (soundVolumes[soundType] == SoundManager::VOLUME_LEV_0)
        {
            // already muted, don't play
            return;
        }

        if (stopHandlingSound == true)
        {
            return;
        }

        if (player->isPlaying())
        {
            if (curSoundType > soundType && !isForceUpdate)
            {
                // or has high priority sound playing, don't play
                return;
            }
            else
            {
                // stop the playing sound
                QMetaObject::invokeMethod(player, "stop");

                // store info for play after receive player finished signal
                pendingWavFile = wav;
                pendingVolume = soundVolumes[soundType];
                curSoundType = soundType;
                return;
            }
        }

        // set volume of the sound type
        if (curVolume != soundVolumes[soundType])
        {
            curVolume = soundVolumes[soundType];
            setVolume(curVolume);
        }

        QMetaObject::invokeMethod(player, "play", Q_ARG(WavFile *, wav));
        curSoundType = soundType;
    }


    /**
     * @brief setMute mute the audio output
     * @param mute mute or not
     */
    void setMute(bool mute)
    {
        if (muteCtrlFd != -1)
        {
            if (ioctl(muteCtrlFd, PMOS_MUTE_CMD, mute) < 0)
            {
                perror("Set mute status failed");
            }
        }
    }

    /**
     * @brief isMute get the curren mute status
     * @return
     */
    bool isMute()
    {
        if (muteCtrlFd != -1)
        {
            long value = 0;
            if (ioctl(muteCtrlFd, PMOS_MUTE_STATUS, &value) < 0)
            {
                perror("get mute status failed");
                return  false;
            }
            else
            {
                return true;
            }
        }
        return true;
    }

    SoundManager *const q_ptr;
    SoundManager::SoundType curSoundType;
    WavPlayer *player;
    QThread *playerThread;
    SoundManager::VolumeLevel curVolume;
    int muteCtrlFd;
    QTimer *almTimer;
    int alarmInterval[3];
    AlarmPriority curAlarmPriority;
    WavFile *pendingWavFile;
    SoundManager::VolumeLevel pendingVolume;
    bool stopHandlingSound;

    QMap<int, WavFile *> wavFiles;
    SoundManager::VolumeLevel soundVolumes[SoundManager::SOUND_TYPE_NR];

private:
    // disable copy construct
    SoundManagerPrivate(const SoundManagerPrivate&);
};

SoundManager::~SoundManager()
{
    d_ptr->playerThread->quit();
    d_ptr->playerThread->wait();
}

SoundManager::SoundManager()
    : QObject(),
      d_ptr(new SoundManagerPrivate(this))
{
    d_ptr->playerThread->start();
    qRegisterMetaType<WavFile *>("WavFile*");
    connect(&patientManager, SIGNAL(signalPatientType(PatientType)), this, SLOT(volumeInit()));
}

SoundManager &SoundManager::getInstance()
{
    static SoundManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new SoundManager();
        SoundManagerInterface *old = registerSoundManager(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

void SoundManager::keyPressTone()
{
    d_ptr->playSound(SOUND_TYPE_NOTIFICATION);
}

void SoundManager::errorTone()
{
    d_ptr->playSound(SOUND_TYPE_ERROR);
}

void SoundManager::pulseTone(short spo2Value)
{
    PulseToneType type = NORMAL_PULSE_TONE;
    if (spo2Value >= 0 && spo2Value <= 84)
    {
        type = SMART_PULSE_TONE_80;
    }
    else if (spo2Value >= 85 && spo2Value <= 87)
    {
        type = SMART_PULSE_TONE_85;
    }
    else if (spo2Value >= 88 && spo2Value <= 89)
    {
        type = SMART_PULSE_TONE_90;
    }
    else if (spo2Value >= 90 && spo2Value <= 100)
    {
        type = (PulseToneType)(SMART_PULSE_TONE_90 + (spo2Value -  90));
    }

    d_ptr->playSound(SOUND_TYPE_PULSE, type);
}

void SoundManager::heartBeatTone()
{
    d_ptr->playSound(SOUND_TYPE_HEARTBEAT);
}

void SoundManager::alarmTone()
{
    d_ptr->playSound(SOUND_TYPE_ALARM);
}

void SoundManager::nibpCompleteTone()
{
    d_ptr->playSound(SOUND_TYPE_NIBP_COMPLETE);
}

void SoundManager::updateAlarm(bool hasAlarm, AlarmPriority curHighestPriority)
{
    if (!d_ptr->almTimer)
    {
        return;
    }

    if (!hasAlarm)
    {
        // no alarm, stop the alarm timer
        if (d_ptr->almTimer->isActive())
        {
            d_ptr->almTimer->stop();
        }
        d_ptr->curAlarmPriority = ALARM_PRIO_PROMPT;
        return;
    }

    d_ptr->curAlarmPriority = curHighestPriority;

    if (curHighestPriority == ALARM_PRIO_PROMPT)
    {
        return;
    }

    if (!d_ptr->almTimer->isActive())
    {
        d_ptr->almTimer->start(d_ptr->alarmInterval[curHighestPriority]);
        d_ptr->playSound(SOUND_TYPE_ALARM, curHighestPriority);
    }
    else
    {
        if (d_ptr->almTimer->interval() != d_ptr->alarmInterval[curHighestPriority])
        {
            d_ptr->almTimer->stop();
            d_ptr->almTimer->start(d_ptr->alarmInterval[curHighestPriority]);
            d_ptr->playSound(SOUND_TYPE_ALARM, curHighestPriority, true);
        }
    }
}

void SoundManager::setVolume(SoundManager::SoundType type, SoundManager::VolumeLevel lev)
{
    int nibpCompleteToneStatus = true;
    systemConfig.getNumValue("PrimaryCfg|NIBP|CompleteTone", nibpCompleteToneStatus);
    if (type == SOUND_TYPE_NOTIFICATION && nibpCompleteToneStatus)
    {
        d_ptr->soundVolumes[SOUND_TYPE_NIBP_COMPLETE] = lev;
    }
    d_ptr->soundVolumes[type] = lev;
}

SoundManager::VolumeLevel SoundManager::getVolume(SoundManager::SoundType type)
{
    return d_ptr->soundVolumes[type];
}

void SoundManager::selfTest()
{
    d_ptr->setMute(false);
    d_ptr->playSound(SOUND_TYPE_ALARM, ALARM_PRIO_LOW);

    QTimer::singleShot(1000, this, SLOT(volumeInit()));
}

void SoundManager::stopHandlingSound(bool enable)
{
    d_ptr->stopHandlingSound = enable;
    if (enable == true && d_ptr->player->isPlaying())
    {
        QMetaObject::invokeMethod(d_ptr->player, "stop");
    }
}

void SoundManager::setNIBPCompleteTone(bool status)
{
    if (status)
    {
        setVolume(SOUND_TYPE_NIBP_COMPLETE, d_ptr->soundVolumes[SOUND_TYPE_NOTIFICATION]);
    }
    else
    {
        setVolume(SOUND_TYPE_NIBP_COMPLETE, VOLUME_LEV_0);
    }
}

void SoundManager::alarmTimeout()
{
    if (!d_ptr->almTimer)
    {
        return;
    }

    // 如果当前报警时间间隔和当前最高报警等级不一致，则刷新高级间隔
    if (d_ptr->almTimer->interval() != d_ptr->alarmInterval[d_ptr->curAlarmPriority])
    {
        d_ptr->almTimer->setInterval(d_ptr->alarmInterval[d_ptr->curAlarmPriority]);
    }
    d_ptr->playSound(SOUND_TYPE_ALARM, d_ptr->curAlarmPriority);
}

void SoundManager::volumeInit()
{
    int alarmVolume = VOLUME_LEV_3;
    int keyVolume = VOLUME_LEV_3;
    int qrsVolume = VOLUME_LEV_3;
    int nibpVolumeFlag = 0;

    if (nightModeManager.nightMode())
    {
        systemConfig.getNumValue("NightMode|AlarmVolume", alarmVolume);
        systemConfig.getNumValue("NightMode|KeyPressVolume", keyVolume);
        systemConfig.getNumValue("NightMode|HeartBeatVolume", qrsVolume);
    }
    else
    {
        systemConfig.getNumValue("Alarms|DefaultAlarmVolume", alarmVolume);
        systemConfig.getNumValue("General|KeyPressVolume", keyVolume);
        currentConfig.getNumValue("ECG|QRSVolume", qrsVolume);
    }

    systemConfig.getNumValue("PrimaryCfg|NIBP|CompleteTone", nibpVolumeFlag);
    d_ptr->soundVolumes[SOUND_TYPE_ALARM] = static_cast<VolumeLevel>(alarmVolume);
    d_ptr->soundVolumes[SOUND_TYPE_NOTIFICATION] = static_cast<VolumeLevel>(keyVolume);
    d_ptr->soundVolumes[SOUND_TYPE_HEARTBEAT] = static_cast<VolumeLevel>(qrsVolume);
    d_ptr->soundVolumes[SOUND_TYPE_PULSE] = static_cast<VolumeLevel>(qrsVolume);
    if (nibpVolumeFlag)
    {
        // nibp完成音属于提示音，要和按键提示音保持同样音量
        d_ptr->soundVolumes[SOUND_TYPE_NIBP_COMPLETE] = static_cast<VolumeLevel>(keyVolume);
    }
    else
    {
        d_ptr->soundVolumes[SOUND_TYPE_NIBP_COMPLETE] = VOLUME_LEV_0;
    }
}

void SoundManager::playFinished()
{
    // has another wave file to play
    if (d_ptr->pendingWavFile)
    {
        // set volume of the sound type
        if (d_ptr->curVolume != d_ptr->pendingVolume)
        {
            d_ptr->curVolume = d_ptr->pendingVolume;
            d_ptr->setVolume(d_ptr->curVolume);
        }

        QMetaObject::invokeMethod(d_ptr->player, "play", Q_ARG(WavFile *, d_ptr->pendingWavFile));
        d_ptr->pendingWavFile = NULL;
    }
}
