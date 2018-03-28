#pragma once
#include <QList>
#include <QTimer>
#include <alsa/asoundlib.h>
#include "AlarmDefine.h"
#include <QSemaphore>

//error tone type
enum
{
    ERROR_TONE_WARNING,
    ERROR_TONE_NON_ALARM_PROMPT,
    ERROR_TONE_ATTACH_PADS_BEEP,
};

//脉搏音信息
struct PulseToneInfo
{
    short minSpo2;             // 血氧饱和度下限。
    short maxSpo2;             // 血氧饱和度上限。
    const char *fileName;      // 脉搏音文件名。
    int soundId;               // 脉搏音ID。
};

//除颤提示语信息
struct DefibPromptInfo
{
    int sequence;              // 序号，和外部模块使用的一致。
    const char *fileName;      // 英文提示语文件名。
    int soundId;               // ID。
};

//除颤充电/准备信息
struct ChargingSoundInfo
{
    int sequence;              // 序号，和外部模块使用的一致。
    const char *fileName;      // 声音文件名。
    int soundId;               // ID。
};

//错误警告音信息
struct ErrorSoundInfo
{
    int sequence;              // 序号，和外部模块使用的一致。
    const char *fileName;      // 声音文件名。
    int soundId;               // ID。
};

//metronome beep info
struct MetronomeSoundInfo
{
    int sequence;              // 序号，和外部模块使用的一致。
    const char *fileName;      // 声音文件名。
    int soundId;               // ID。
};

//报警音信息
struct AlarmSoundInfo
{
    AlarmPriority priority;    // 报警优先级。
    const char *fileName;      // 报警音文件名。
    int soundId;               // 报警音ID。
};

//心跳音信息
struct HeartBeatInfo
{
    int sequence;              // 序号，和外部模块使用的一致。
    const char *fileName;      // 声音文件名。
    int soundId;               // ID。
};

//按键音信息
struct KeySoundInfo
{
    int sequence;              // 序号，和外部模块使用的一致。
    const char *fileName;      // 声音文件名。
    int soundId;               // ID。
};

enum SoundVolume
{
    SOUND_VOL_0,
    SOUND_VOL_1,
    SOUND_VOL_2,
    SOUND_VOL_3,
    SOUND_VOL_4,
    SOUND_VOL_5,
    SOUND_VOL_MAX = SOUND_VOL_5,
    SOUND_VOL_NR,
};

//声音类型，优先级也是这个顺序
enum SoundType
{
    SOUND_TYPE_NONE = -1,
    SOUND_TYPE_ERROR_WARNING,     // 操作错误警告
    SOUND_TYPE_PHY_ALARM,         // 生理报警
    SOUND_TYPE_TECH_ALARM,        // 技术报警
    SOUND_TYPE_ALARM_OFF_PROMPT,  // 报警关闭提示
    SOUND_TYPE_HEART_BEAT,        // 心跳，脉搏，QRS
    SOUND_TYPE_MAX,
};

//声音通道类型，用于控制音量及播放
enum SoundVolumeChannel
{
    SOUND_VOLUME_CHANNEL_ERROR_WARNING,
    SOUND_VOLUME_CHANNEL_ALARM,
    SOUND_VOLUME_CHANNEL_HEART_BEAT,
    SOUND_VOLUME_CHANNEL_NR
};

class SoundChannel;
class SoundManager : public QObject // SoundManager管理各个声道的音频播放及音量控制。
{
    Q_OBJECT

public:
    static SoundManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SoundManager();
        }
        return *_selfObj;
    }
    static SoundManager *_selfObj;
    ~SoundManager();

public:
    //更新报警级别
    void updateAlarm(SoundType type, bool hasAlarm, AlarmPriority priority);

    //设置心率值
    void setHr(short hr);

    //设置脉率值
    void setPr(short pr);

    //播放心跳音
    void heartBeat(void);

    //播放脉搏音
    void pluse(short spo2Value = 0);

    //智能脉搏音
    void SmartPulse(int sens);

    // 自检测。
    void selfTest(void);

    // 错误报警音
    void errorTone(int id = 0);

    // 设置报警音量
    void setAlarmVolume(SoundVolume volume);

    // 获取不同类型的音量
    SoundVolume getVolume(SoundVolumeChannel type);

    // 设置声道音量，供外部调用。
    bool setSoundVolume(SoundVolumeChannel type, SoundVolume level);

    // 播放指定的音频文件，供外部调用。
    bool playSound(SoundType type, int id, bool preempt = false);

    // 主运行。
    void run(void);

    // 停止播放。
    void stop(SoundType type);

private slots:
    void _volumeInit();
    void _repeatAlarmSound();

private:
    SoundManager();

    // 初始化混音对象。
    void _initMixer();

    // 载入。
    void _initLoad(void);

    //设置音量
    void _setVolume(snd_mixer_elem_t *elem, SoundVolume level);

    //获取声音类型对应的通道
    SoundChannel *_getSoundChannel(SoundType type);

    //获取音频ID
    int _getSoundId(SoundType type, SoundChannel *channel, int id);

    //检查声音优先级
    bool _checkSoundPriority(SoundType type);

    //检查声音是否在播放
    bool _checkSoundChannelIsPlaying(SoundType type);

    //设置智能脉搏音ID
    void _setPulseTone(short spo2);

    //mute
    void _setMute(bool mute);
    bool _isMute();

    static int alarmInterval[];

    const static SoundVolume errorVolume = SOUND_VOL_4;

    static ErrorSoundInfo _errorSoundInfo[];      // 错误警告音信息
    static const int _errorSoundInfoCount;        // 错误警告音数量

    static AlarmSoundInfo _alarmSoundInfo[];      // 报警音信息
    static const int _alarmSoundInfoCount;        // 报警音音数量

    static HeartBeatInfo _HeartBeatSoundInfo[];   // 心跳音信息
    static const int _HeartBeatSoundInfoCount;    // 心跳音数量

    static PulseToneInfo _pulseToneInfo[];        // 脉搏调制音映射表
    static const int _pulseToneInfoCount;         // 脉搏调制音数量

    QTimer *_alarmTimer;                          // 报警音定时器
    SoundType _curAlarmType;                      // 当前报警类型
    SoundType _curPromptType;                     // 当前语音类型

    int _pulseToneSoundId;                        // 脉搏音ID
    bool _isHrValid;                              // 心率有效标记
    bool _isPrValid;                              // 脉率有效标记
    bool _isSmartPulseTone;                       // 脉率有效标记

    snd_mixer_t *_mixer;                          // 混音器句柄
    int _minVolume;                               // 最小音量
    int _maxVolume;                               // 最大音量
    int _muteCtrlFd;                             // device file descriptor to control mute
    QSemaphore _playSemaphore;                   // play semaphore

    QList<SoundChannel *> _channelList;           // 声道列表，目前有6种
    typedef QList<snd_mixer_elem_t *>::Iterator VolListIter;
    QList<snd_mixer_elem_t *> _volumeList;        // 声道音量列表

#ifdef CONFIG_UNIT_TEST
    friend class TestSoundManager;
#endif
};
#define soundManager (SoundManager::construction())
#define deleteSoundManager() (delete SoundManager::_selfObj)
