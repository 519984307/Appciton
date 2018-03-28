#pragma once
#include <QObject>
#include <QList>
#include <QQueue>
#include <alsa/asoundlib.h>
#include <QMutex>

class WavFile;

enum FeedPcmState
{
    FEED_PCM_FAILED,
    FEED_PCM_RECOVERED,
    FEED_PCM_SUCCESS,
    FEED_PCM_DONE
};

/***************************************************************************************************
 * SoundChannel实现一个PCM声道中多个音频文件的播放调度
 **************************************************************************************************/
class SoundChannel: public QObject
{
    Q_OBJECT
public:
    SoundChannel(QObject *parent = NULL);
    ~SoundChannel();

    bool open(const char *pcmName);
    void close();
    int addSound(const char *fileName);
    void play(int soundId, bool preempt = false);
    void stop();
    bool isPlaying();
    bool isIdle();
    void onWriteReady(bool isWriteToDevice);

private:
    bool _setHwParams(const WavFile *wav);
    int xrun_recovery(snd_pcm_t *handle, int err);
    FeedPcmState _feedPcm(WavFile *wav, bool isWriteToDevice);

    snd_pcm_sframes_t _frames;    // PCM周期帧数
    snd_pcm_sframes_t buffer_size;
    int _maxframes;                   // maximum frames ready to write
    snd_pcm_t *_pcm;                   // PCM声道句柄
    QList<WavFile*> _wavFiles;         // 音频文件列表

private:
    QMutex _mutex;                     // 线程锁
    WavFile *_currentWavFile;          // 当前播放的音频文件
    QQueue<WavFile*> _pendingWavFiles; // 音频播放队列

#ifdef CONFIG_UNIT_TEST
    friend class TestSoundManager;
#endif
};
