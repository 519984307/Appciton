#include <QCoreApplication>
#include "SoundManager.h"
#include "SoundChannel.h"
#include "BaseDefine.h"
#include "IConfig.h"
#include "SystemManager.h"
#include "Debug.h"
#include <sys/ioctl.h>
#include "LanguageManager.h"

#define SOUND_DIR			"/usr/local/nPM/sound"
#define PULSE_TONE_DIR		SOUND_DIR"/pulse_tone"
#define MUTE_CTRL_DEV       "/dev/npm_misc"

// 错误警告音信息
ErrorSoundInfo SoundManager::_errorSoundInfo[] =
{
    {0, SOUND_DIR"/error.wav", -1},
    {1, SOUND_DIR"/nonalarmbeeps.wav", -1},
    {2, SOUND_DIR"/attachpads.wav", -1}
};

// 报警音信息
AlarmSoundInfo SoundManager::_alarmSoundInfo[] =
{
    //报警关闭后的提示音
    {ALARM_PRIO_PROMPT, SOUND_DIR"/alarm_prompt.wav", -1},
    //低级
    {ALARM_PRIO_LOW, SOUND_DIR"/lo_lev.wav", -1},
    //中级
    {ALARM_PRIO_MED, SOUND_DIR"/med_lev.wav", -1},
    //高级
    {ALARM_PRIO_HIGH, SOUND_DIR"/hi_lev.wav", -1},
};

// 心跳音信息
HeartBeatInfo SoundManager::_HeartBeatSoundInfo[] =
{
    //心跳,QRS
    {0, SOUND_DIR"/heart_beep.wav", -1},
    {1, PULSE_TONE_DIR"/pulse_100.wav", -1},
    //脉搏
    {2, NULL, -1},
};

// 脉搏调制音映射表
PulseToneInfo SoundManager::_pulseToneInfo[] =
{
    { 0, 84,   PULSE_TONE_DIR"/pulse_80.wav", -1 },
    { 85, 87,   PULSE_TONE_DIR"/pulse_85.wav", -1 },
    { 88, 90,   PULSE_TONE_DIR"/pulse_90.wav", -1 },
    { 91, 91,   PULSE_TONE_DIR"/pulse_91.wav", -1 },
    { 92, 92,   PULSE_TONE_DIR"/pulse_92.wav", -1 },
    { 93, 93,   PULSE_TONE_DIR"/pulse_93.wav", -1 },
    { 94, 94,   PULSE_TONE_DIR"/pulse_94.wav", -1 },
    { 95, 95,   PULSE_TONE_DIR"/pulse_95.wav", -1 },
    { 96, 96,   PULSE_TONE_DIR"/pulse_96.wav", -1 },
    { 97, 97,   PULSE_TONE_DIR"/pulse_97.wav", -1 },
    { 98, 98,   PULSE_TONE_DIR"/pulse_98.wav", -1 },
    { 99, 99,   PULSE_TONE_DIR"/pulse_99.wav", -1 },
    { 100, 100, PULSE_TONE_DIR"/pulse_100.wav", -1 }
};

//每种类型的声音总数
const int SoundManager::_pulseToneInfoCount = sizeof(SoundManager::_pulseToneInfo)
        / sizeof(SoundManager::_pulseToneInfo[0]);
const int SoundManager::_errorSoundInfoCount = sizeof(SoundManager::_errorSoundInfo)
        / sizeof(SoundManager::_errorSoundInfo[0]);
const int SoundManager::_alarmSoundInfoCount = sizeof(SoundManager::_alarmSoundInfo)
        / sizeof(SoundManager::_alarmSoundInfo[0]);
const int SoundManager::_HeartBeatSoundInfoCount = sizeof(SoundManager::_HeartBeatSoundInfo)
        / sizeof(SoundManager::_HeartBeatSoundInfo[0]);

//alarm period time
int SoundManager::alarmInterval[] = {25000, 15000, 10000};

SoundManager *SoundManager::_selfObj = NULL;

/**************************************************************************************************
 * 功能：根据报警状态播放报警音。
 * 参数：
 *     hasAlarm: 是否有报警。
 *     alarmLevel: 当前最高的报警等级（有报警时)。
 *************************************************************************************************/
void SoundManager::updateAlarm(SoundType type, bool hasAlarm, AlarmPriority priority)
{
    if (!_alarmTimer)
    {
        return;
    }

    if (!hasAlarm)
    {
        if (_alarmTimer->isActive())
        {
            _alarmTimer->stop();
            stop(_curAlarmType);
            _curAlarmType = SOUND_TYPE_MAX;
        }
        return;
    }

    if (priority == ALARM_PRIO_PROMPT)
    {
        return;
    }

    if (type != _curAlarmType)
    {
        _alarmTimer->stop();
        stop(_curAlarmType);
        _curAlarmType = type;
    }

#if 1
    if (!_alarmTimer->isActive() || (_alarmTimer->interval() != alarmInterval[priority]))
    {
        if (_alarmTimer->isActive())
        {
            stop(_curAlarmType);
        }

        _alarmTimer->start(alarmInterval[priority]);
        playSound(_curAlarmType, priority, true);
    }
#else
    switch (priority)
    {
        case ALARM_PRIO_HIGH:
            if (!_alarmTimer->isActive() || (_alarmTimer->interval() != _alarmHighInterval))
            {
                if (_alarmTimer->isActive())
                {
                    stop(_curAlarmType);
                }

                _alarmTimer->start(_alarmHighInterval);
                playSound(_curAlarmType, ALARM_PRIO_HIGH, true);
            }
            break;

        case ALARM_PRIO_MED:
            if (!_alarmTimer->isActive() || (_alarmTimer->interval() != _alarmMedInterval))
            {
                if (_alarmTimer->isActive())
                {
                    stop(_curAlarmType);
                }

                _alarmTimer->start(_alarmMedInterval);
                playSound(_curAlarmType, ALARM_PRIO_MED, true);
            }
            break;

        case ALARM_PRIO_LOW:
            if (!_alarmTimer->isActive() || (_alarmTimer->interval() != _alarmLowInterval))
            {
                if (_alarmTimer->isActive())
                {
                    stop(_curAlarmType);
                }

                _alarmTimer->start(_alarmLowInterval);
                playSound(_curAlarmType, ALARM_PRIO_LOW, true);
            }
            break;

        default:
            break;
    }
#endif
}

/**************************************************************************************************
 * 功能： 根据血氧饱和度计算脉搏音。
 * 参数：
 *      spo2: 血氧饱和度。
 *************************************************************************************************/
void SoundManager::_setPulseTone(short spo2)
{
    if ((spo2 < 0) || (spo2 > 100))
    {
        _pulseToneSoundId = -1;
        return;
    }

    SoundChannel *channel = _channelList.at(SOUND_VOLUME_CHANNEL_HEART_BEAT);
    for (int i = 0; i < _pulseToneInfoCount; i++)
    {
        if ((spo2 >= _pulseToneInfo[i].minSpo2) && (spo2 <= _pulseToneInfo[i].maxSpo2))
        {
            _pulseToneSoundId = _pulseToneInfo[i].soundId;
            if (-1 == _pulseToneSoundId)
            {
                _pulseToneSoundId = channel->addSound(_pulseToneInfo[i].fileName);
                _pulseToneInfo[i].soundId = _pulseToneSoundId;
            }
            return;
        }
    }

    _pulseToneSoundId = -1;
}

/**************************************************************************************************
 * 功能： 设置心率值。
 * 参数：
 *      hr：心率值。
 *************************************************************************************************/
void SoundManager::setHr(short hr)
{
    _isHrValid = (hr != InvData());
}

/**************************************************************************************************
 * 功能： 设置脉率值。
 *************************************************************************************************/
void SoundManager::setPr(short pr)
{
    _isPrValid = (pr != InvData() && pr != UnknownData());
}

/**************************************************************************************************
 * 功能： 播放自检音。
 *************************************************************************************************/
void SoundManager::selfTest(void)
{
    setSoundVolume(SOUND_VOLUME_CHANNEL_ALARM, SOUND_VOL_3);
    _curAlarmType = SOUND_TYPE_TECH_ALARM;
    playSound(SOUND_TYPE_TECH_ALARM, ALARM_PRIO_LOW, true);

    QTimer::singleShot(1000, this, SLOT(_volumeInit()));
}

/**************************************************************************************************
 * 功能： 响应心跳事件, 如果脉搏调制音关闭, 则播放心跳音，否则播放脉搏调制音。
 *************************************************************************************************/
void SoundManager::heartBeat(void)
{
    if (_isHrValid)
    {
        playSound(SOUND_TYPE_HEART_BEAT, 0, false);
    }
}

/**************************************************************************************************
 * 功能： 响应心跳事件, 如果脉搏调制音关闭, 则播放心跳音，否则播放脉搏调制音。
 * 参数:
 *    spo2Value:需要根据SPO2的值选择脉搏音(0-100),当为-1时，表示未启用智能脉搏音
 *************************************************************************************************/
void SoundManager::pluse(short spo2Value)
{
    if (_isHrValid)
    {
        return;
    }

    if (_isPrValid)
    {
        if (_isSmartPulseTone)
        {
            _setPulseTone(spo2Value);
            playSound(SOUND_TYPE_HEART_BEAT, 2, false);
        }
        else
        {
            playSound(SOUND_TYPE_HEART_BEAT, 1, false);
        }
    }
}

/**************************************************************************************************
 * 功能： 智能脉搏音使能标志
 *************************************************************************************************/
void SoundManager::SmartPulse(int sens)
{
    if (sens == 0)
    {
        _isSmartPulseTone = true;
    }
    else
    {
        _isSmartPulseTone = false;
    }
}

/**************************************************************************************************
 * 功能： 播放错误报警音。
 *************************************************************************************************/
void SoundManager::errorTone(int id)
{
    //because preempt attach pads beep too long
    //error warning and non-alarm prompt will preempt attach pads beep
    static int lastID = -1;
    bool preempt = false;
    if (lastID == ERROR_TONE_ATTACH_PADS_BEEP && id != ERROR_TONE_ATTACH_PADS_BEEP)
    {
        preempt = true;
    }

    playSound(SOUND_TYPE_ERROR_WARNING, id, preempt);
    lastID = id;
}

/**************************************************************************************************
 * 功能： 设置报警音量。
 *************************************************************************************************/
void SoundManager::setAlarmVolume(SoundVolume volume)
{
    superRunConfig.setNumValue("Alarm|DefaultAlarmVolume", (int)volume);
    setSoundVolume(SOUND_VOLUME_CHANNEL_ALARM, volume);
}

/**************************************************************************************************
 * 功能： 获取报警音量。
 *************************************************************************************************/
SoundVolume SoundManager::getVolume(SoundVolumeChannel type)
{
    int v = 2;
    switch (type)
    {
        case SOUND_VOLUME_CHANNEL_ALARM:
        {
            superRunConfig.getNumValue("Alarm|DefaultAlarmVolume", v);
            return (SoundVolume)v;
        }

        case SOUND_VOLUME_CHANNEL_ERROR_WARNING:
            return errorVolume;

        default:
            return (SoundVolume)v;
    }
}

/**************************************************************************************************
 * 功能： 设置指定类型声音的音量大小。
 * 参数:
 *     type:声音类型
 *     level:音量等级
 *************************************************************************************************/
bool SoundManager::setSoundVolume(SoundVolumeChannel type, SoundVolume level)
{
    //判断声音类型
    if (type >= SOUND_VOLUME_CHANNEL_NR)
    {
        debug("\033[31m sound type err:%d\033[m", type);
        return false;
    }

    //判断音量等级
    if (level > SOUND_VOL_MAX)
    {
        debug("\033[31m sound level err:%d\033[m", level);
        return false;
    }

    //防止超范围
    unsigned size = _volumeList.size();
    if (type >= size)
    {
        return false;
    }

    //获取指定混音器
    snd_mixer_elem_t *pmixerElem = _volumeList.at(type);

    //设置音量
    if (NULL != pmixerElem)
    {
        _setVolume(pmixerElem, level);
    }

    return true;
}

/**************************************************************************************************
 * 功能： 线程函数。
 *************************************************************************************************/
void SoundManager::run()
{
    if(!_playSemaphore.tryAcquire(1, 1000)) //block 1000 ms at most
    {
        //nothing to play
        return;
    }

    bool playing = true;
    bool mute = _isMute();
    int playChannels = 0;
    int idleChannel = 0;
    SoundChannel *channel = NULL;
    if (!_channelList.empty())
    {
        for (int i = 0; i < SOUND_TYPE_MAX; ++i)
        {
            channel = _getSoundChannel((SoundType)i);
            if (_checkSoundChannelIsPlaying((SoundType)i))
            {
                playChannels += 1;
                if (mute)
                {
                    _setMute(false); //stop mute
                    mute = false;
                }

                channel->onWriteReady(playing);
                if (playing)
                {
                    playing = false;
                }
            }

            if (channel->isIdle())
            {
                idleChannel += 1;
            }
        }
    }

    if(playChannels > 0 || idleChannel != SOUND_TYPE_MAX)
    {
        _playSemaphore.release(); //haven't finished playing, need to release
    }
    else if(!mute) //nothing to play and haven't mute
    {
        _setMute(true);
    }
}

/**************************************************************************************************
 * 停止某种类型的播放。
 *************************************************************************************************/
void SoundManager::stop(SoundType type)
{
    if (type >= SOUND_TYPE_MAX)
    {
        return;
    }

    //获取通道
    SoundChannel *channel = _getSoundChannel(type);
    if (_checkSoundChannelIsPlaying(type))
    {
        //停止
        channel->stop();
    }
}

/**************************************************************************************************
 * 功能： 音量初始化。
 *************************************************************************************************/
void SoundManager::_volumeInit()
{
    // 音量初始化
    setAlarmVolume(getVolume(SOUND_VOLUME_CHANNEL_ALARM));
    setSoundVolume(SOUND_VOLUME_CHANNEL_ERROR_WARNING, getVolume(SOUND_VOLUME_CHANNEL_ERROR_WARNING));
}

/**************************************************************************************************
 * 功能： 重复报警音。
 *************************************************************************************************/
void SoundManager::_repeatAlarmSound()
{
    if (!_alarmTimer)
    {
        return;
    }

    // 根据定时器间隔确定当前报警等级
    if (_alarmTimer->interval() == alarmInterval[ALARM_PRIO_HIGH])
    {
        playSound(_curAlarmType, ALARM_PRIO_HIGH);
    }
    else if (_alarmTimer->interval() == alarmInterval[ALARM_PRIO_MED])
    {
        playSound(_curAlarmType, ALARM_PRIO_MED);
    }
    else if (_alarmTimer->interval() == alarmInterval[ALARM_PRIO_LOW])
    {
        playSound(_curAlarmType, ALARM_PRIO_LOW);
    }
    else
    {
        debug("Unknow alarm timer inverval: %d", _alarmTimer->interval());
    }
}

/**************************************************************************************************
 * 功能： 初始化音量控制。
 *************************************************************************************************/
void SoundManager::_initMixer()
{
#if !defined(Q_WS_QWS)
    return;
#endif

    int ret = snd_mixer_open(&_mixer, 0);
    if (ret < 0)
    {
        debug("%s", snd_strerror(ret));
        _mixer = NULL;
        return;
    }

    ret = snd_mixer_attach(_mixer, "default");
    if (ret < 0)
    {
        debug("%s", snd_strerror(ret));
        snd_mixer_close(_mixer);
        _mixer = NULL;
        return;
    }

    ret = snd_mixer_selem_register(_mixer, NULL, NULL);
    if (ret < 0)
    {
        debug("%s", snd_strerror(ret));
        snd_mixer_close(_mixer);
        _mixer = NULL;
        return;
    }

    ret = snd_mixer_load(_mixer);
    if (ret < 0)
    {
        debug("%s", snd_strerror(ret));
        snd_mixer_close(_mixer);
        _mixer = NULL;
        return;
    }

    VolListIter iter = _volumeList.begin();
    for (snd_mixer_elem_t *elem = snd_mixer_first_elem(_mixer); elem;
            elem = snd_mixer_elem_next(elem))
    {
        if ((snd_mixer_elem_get_type(elem) == SND_MIXER_ELEM_SIMPLE)
                && snd_mixer_selem_is_active(elem) && snd_mixer_selem_has_playback_volume(elem))
        {
            if (!strcmp(snd_mixer_selem_get_name(elem), "Alarm"))
            {
                *(iter + SOUND_VOLUME_CHANNEL_ALARM) = elem;
            }
            else if (!strcmp(snd_mixer_selem_get_name(elem), "Heart Beat"))
            {
                *(iter + SOUND_VOLUME_CHANNEL_HEART_BEAT) = elem;
            }
            else if (!strcmp(snd_mixer_selem_get_name(elem), "Error"))
            {
                *(iter + SOUND_VOLUME_CHANNEL_ERROR_WARNING) = elem;
            }
            else
            {
                continue;
            }
        }
    }

    if (!_volumeList.at(SOUND_VOLUME_CHANNEL_HEART_BEAT))
    {
        debug("Fail to access volume control: 'Heart Beat'");
    }
    if (!_volumeList.at(SOUND_VOLUME_CHANNEL_ERROR_WARNING))
    {
        debug("Fail to access volume control: 'Error'");
    }
    if (!_volumeList.at(SOUND_VOLUME_CHANNEL_ALARM))
    {
        debug("Fail to access volume control: 'Alarm'");
    }
}

/**************************************************************************************************
 * 功能： 。
 *************************************************************************************************/
void SoundManager::_initLoad(void)
{
    for (int i = SOUND_TYPE_NONE + 1; i < SOUND_TYPE_MAX; i++)
    {
        SoundChannel &channel = *_getSoundChannel((SoundType)i);
        int totalID = -1;

        switch (i)
        {
        case SOUND_TYPE_ERROR_WARNING:
            totalID = _errorSoundInfoCount;
            for (int j = 0; j < totalID; j++)
            {
                if (_errorSoundInfo[j].soundId != -1)
                {
                    continue;
                }
                _errorSoundInfo[j].soundId = channel.addSound(_errorSoundInfo[j].fileName);
            }
            break;

        case SOUND_TYPE_ALARM_OFF_PROMPT:
        case SOUND_TYPE_PHY_ALARM:
        case SOUND_TYPE_TECH_ALARM:
            totalID = _alarmSoundInfoCount;
            for (int j = 0; j < totalID; j++)
            {
                if (_alarmSoundInfo[j].soundId != -1)
                {
                    continue;
                }
                _alarmSoundInfo[j].soundId = channel.addSound(_alarmSoundInfo[j].fileName);
            }
            break;

        case SOUND_TYPE_HEART_BEAT:
            totalID = _HeartBeatSoundInfoCount;
            for (int j = 0; j < totalID; j++)
            {
                if (_HeartBeatSoundInfo[j].soundId != -1)
                {
                    continue;
                }
                _HeartBeatSoundInfo[j].soundId = channel.addSound(_HeartBeatSoundInfo[j].fileName);
            }
            break;

        default:
            break;
        }
    }
}

/***************************************************************************************************
 * mute the speaker throung gpio
 **************************************************************************************************/
void SoundManager::_setMute(bool mute)
{
    mute = mute;
#if defined(Q_WS_QWS)
    #define IDM_MUTE_CMD _IOW('I', 0, int)
    if(ioctl(_muteCtrlFd, IDM_MUTE_CMD, mute) < 0)
    {
//        perror("Set mute failed");
    }
#endif
}

/***************************************************************************************************
 * get the mute status
 **************************************************************************************************/
bool SoundManager::_isMute()
{
#define IDM_MUTE_STATUS _IOR('I', 1, int)
    long value = 0;
    if(ioctl(_muteCtrlFd, IDM_MUTE_STATUS, &value) < 0)
    {
//        perror("Get mute status failed");
    }
    return value;
}

/**************************************************************************************************
 * 功能： 声音播放的接口，对其进行统一管理。
 * 参数:
 *      type: 类型。
 *      id：type类型对应的音频标识。
 *      preempt：是否抢占式播放。
 * .....selftest:是否自检
 *************************************************************************************************/
bool SoundManager::playSound(SoundType type, int id, bool preempt)
{
    if (type >= SOUND_TYPE_MAX)
    {
        return false;
    }

    //获取通道
    SoundChannel *tmpChannel = _getSoundChannel(type);
    if (NULL == tmpChannel)
    {
        return false;
    }

    //获取音频ID
    int soundID = _getSoundId(type, tmpChannel, id);
    if (soundID < 0)
    {
        debug("\033[31m get sound id error:type = %d, id = %d\033[m", type, id);
        return false;
    }

    //优先级判断
    bool ret = false;
    ret = _checkSoundPriority(type);
    if (!ret)
    {
        return false;
    }

    //播放
    tmpChannel->play(soundID, preempt);

    _playSemaphore.release();

    return true;
}

/**************************************************************************************************
 * 功能： 设置音量等级。
 * 参数:
 *      level: 音量等级, 范围从0到5
 *************************************************************************************************/
void SoundManager::_setVolume(snd_mixer_elem_t *elem, SoundVolume level)
{
    if (!elem)
    {
        return;
    }

    level = qBound(SOUND_VOL_0, level, SOUND_VOL_MAX);

    int volume = _maxVolume - ((SOUND_VOL_MAX - level) * (_maxVolume - _minVolume) / SOUND_VOL_MAX);
    volume = qBound(_minVolume, volume, _maxVolume);
    volume = (volume == _minVolume) ? 0 : volume;
    int ret = snd_mixer_selem_set_playback_volume_all(elem, volume);
    if (ret < 0)
    {
        debug("%s: %d/[%d, %d]", snd_strerror(ret), volume, _minVolume, _maxVolume);
        return;
    }
}

/**************************************************************************************************
 * 功能： 获取声音类型对应的通道。
 * 参数:
 *      type: 声音类型
 * 返回值:
 *      对应的声音通道。
 *************************************************************************************************/
SoundChannel *SoundManager::_getSoundChannel(SoundType type)
{
    if (type >= SOUND_TYPE_MAX)
    {
        return NULL;
    }

    int channelType = 0;
    switch (type)
    {
        case SOUND_TYPE_ERROR_WARNING:
            channelType = SOUND_VOLUME_CHANNEL_ERROR_WARNING;
            break;

        case SOUND_TYPE_PHY_ALARM:
        case SOUND_TYPE_TECH_ALARM:
        case SOUND_TYPE_ALARM_OFF_PROMPT:
            channelType = SOUND_VOLUME_CHANNEL_ALARM;
            break;

        case SOUND_TYPE_HEART_BEAT:
            channelType = SOUND_VOLUME_CHANNEL_HEART_BEAT;
            break;

        default:
            break;
    }

    return _channelList.at(channelType);
}

/**************************************************************************************************
 * 功能： 获取音频ID。
 * 参数:
 *      type: 声音类型
 *      channel:类型对应的通道
 *      id:该类型对应的声音序号
 * 返回值:
 *      -1,参数错误；其它，对应的音频ID。
 *************************************************************************************************/
int SoundManager::_getSoundId(SoundType type, SoundChannel *channel, int id)
{
    int retID = -1;

    if (type >= SOUND_TYPE_MAX)
    {
        return retID;
    }

    if (NULL == channel)
    {
        return retID;
    }

    const char *wavFileName = NULL;
    switch (type)
    {
        case SOUND_TYPE_ERROR_WARNING:
            if (id >= _errorSoundInfoCount)
            {
                return retID;
            }

            retID = _errorSoundInfo[id].soundId;
            wavFileName = _errorSoundInfo[id].fileName;
            //未添加文件列表中，需要添加
            if (-1 == retID)
            {
                if (NULL != wavFileName)
                {
                    retID = channel->addSound(wavFileName);
                    _errorSoundInfo[id].soundId = retID;
                }
            }
            break;

        case SOUND_TYPE_PHY_ALARM:
        case SOUND_TYPE_TECH_ALARM:
        case SOUND_TYPE_ALARM_OFF_PROMPT:
            if (id + 1 >= _alarmSoundInfoCount)
            {
                return retID;
            }

            retID = _alarmSoundInfo[id + 1].soundId;
            wavFileName = _alarmSoundInfo[id + 1].fileName;
            //未添加文件列表中，需要添加
            if (-1 == retID)
            {
                if (NULL != wavFileName)
                {
                    retID = channel->addSound(wavFileName);
                    _alarmSoundInfo[id + 1].soundId = retID;
                }
            }
            break;

        case SOUND_TYPE_HEART_BEAT:
            if (id >= _HeartBeatSoundInfoCount)
            {
                return retID;
            }

            //脉搏音单独处理
            if (2 == id)
            {
                return _pulseToneSoundId;
            }
            else
            {
                retID = _HeartBeatSoundInfo[id].soundId;
                wavFileName = _HeartBeatSoundInfo[id].fileName;
            }
            //未添加文件列表中，需要添加
            if (-1 == retID)
            {
                if (NULL != wavFileName)
                {
                    retID = channel->addSound(wavFileName);
                    _HeartBeatSoundInfo[id].soundId = retID;
                }
            }
            break;
        default:
            break;
    }

    return retID;
}

/**************************************************************************************************
 * 功能：检测优先级。
 * 返回值:
 * false,优先级低，本次不处理；true，本次处理
 *************************************************************************************************/
bool SoundManager::_checkSoundPriority(SoundType type)
{
    if (type >= SOUND_TYPE_MAX)
    {
        return false;
    }

    bool checkFlag = false;
    QList<int> channelCheckList;
    for (int i = SOUND_TYPE_ERROR_WARNING; i < (int)type; ++i)
    {
        channelCheckList.append(i);
    }

    int count = channelCheckList.count();
    for (int i = 0; i < count; ++i)
    {
        checkFlag |= _checkSoundChannelIsPlaying((SoundType)channelCheckList.at(i));
    }

    //有高优先级的声音正在播放，直接返回
    if (checkFlag)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 * 功能：检查声音是否在播放。
 * 返回值:
 * false,该类型声音没有播放；true，该类型声音正在播放
 *************************************************************************************************/
bool SoundManager::_checkSoundChannelIsPlaying(SoundType type)
{
    if (type >= SOUND_TYPE_MAX)
    {
        return false;
    }

    SoundChannel *channel = _getSoundChannel(type);
    if (NULL == channel)
    {
        return false;
    }

    switch (type)
    {
        case SOUND_TYPE_TECH_ALARM:
        case SOUND_TYPE_PHY_ALARM:
            if (_curAlarmType != type)
            {
                return false;
            }
            break;

        case SOUND_TYPE_ALARM_OFF_PROMPT:
            if (_curAlarmType != SOUND_TYPE_MAX)
            {
                return false;
            }
            break;

        default:
            break;
    }

    return channel->isPlaying();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SoundManager::SoundManager()
{
    _alarmTimer = NULL;
    _curAlarmType = SOUND_TYPE_NONE;
    _curPromptType = SOUND_TYPE_NONE;
    _pulseToneSoundId = -1;
    _isHrValid = false;
    _isPrValid = false;
    _isSmartPulseTone = false;
    _mixer = NULL;
    _minVolume = 80; //195
    _maxVolume = 255;//100

    //声道初始化，注意插入顺序和声音类型一致
    SoundChannel *soundChannel = NULL;
    _channelList.clear();

    // 错误警告声道。
    soundChannel = new SoundChannel(this);
    soundChannel->open("plug:error");
    _channelList.append(soundChannel);
    soundChannel = NULL;

    // 报警音声道
    soundChannel = new SoundChannel(this);
    soundChannel->open("plug:alarm");
    _channelList.append(soundChannel);
    soundChannel = NULL;

    // 心跳音声道
    soundChannel = new SoundChannel(this);
    soundChannel->open("plug:heartbeat");
    _channelList.append(soundChannel);
    soundChannel = NULL;

    // 初始加载。
    _initLoad();

    // 报警音间隔控制
    _alarmTimer = new QTimer(this);
    connect(_alarmTimer, SIGNAL(timeout()), this, SLOT(_repeatAlarmSound()));

    //初始化混音器元素
    _volumeList.clear();
    snd_mixer_elem_t *tmpMixerElem = NULL;
    for (int i = 0; i < SOUND_VOLUME_CHANNEL_NR; ++i)
    {
        _volumeList.append(tmpMixerElem);
    }

    // 初始化音量控制
    _initMixer();

    //mute contorl device
    _muteCtrlFd = open(MUTE_CTRL_DEV, O_RDWR);
    if(_muteCtrlFd < 0)
    {
        debug("Failed to open %s", MUTE_CTRL_DEV);
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SoundManager::~SoundManager()
{
     if (_mixer)
    {
        snd_mixer_close(_mixer);
        _mixer = NULL;
    }

    while (!_channelList.isEmpty())
    {
        SoundChannel *channel = _channelList.takeAt(0);
        if (NULL != channel)
        {
            delete channel;
            channel = NULL;
        }
    }

    if (_alarmTimer != NULL)
    {
        delete _alarmTimer;
    }

    _volumeList.clear();
}
