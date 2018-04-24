#include <poll.h>
#include "SoundChannel.h"
#include "WavFile.h"
#include "Debug.h"

/***************************************************************************************************
 * 说明:
 *      构造函数
 * 参数:
 *      无
 * 返回值:
 *      无
***************************************************************************************************/
SoundChannel::SoundChannel(QObject *parent)
    : QObject(parent)
    , _frames(0)
    , buffer_size(0)
    , _maxframes(0)
    , _pcm(NULL)
    , _currentWavFile(NULL)
{
}

/***************************************************************************************************
 * 说明:
 *      析构函数
 * 参数:
 *      无
 * 返回值:
 *      无
***************************************************************************************************/
SoundChannel::~SoundChannel()
{
    close();

    for (int i = 0; i < _wavFiles.size(); i++)
    {
        delete _wavFiles[i];
    }

    _wavFiles.clear();
}

/***************************************************************************************************
 * 说明:
 *      打开PCM声道
 * 参数:
 *      pcmName: PCM声道名称
 * 返回值:
 *      true 成功；false 失败。
***************************************************************************************************/
bool SoundChannel::open(const char *pcmName)
{
    if (!pcmName)
    {
        return false;
    }

    // 清除状态
    close();

    // 打开PCM通道
    int ret = snd_pcm_open(&_pcm, pcmName, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
    if (ret < 0)
    {
        debug("Error opening PCM '%s': %s", pcmName, snd_strerror(ret));
        _pcm = NULL;
        return false;
    }

    // NOTE:
    // ALSA的softvol插件有bug，其mixer的音量控制selem在第一次snd_pcm_open前会访问不到，
    // 第一次snd_pcm_open后才可以访问到但不允许修改音量，直到第一次snd_pcm_close后才可
    // 正常访问和修改音量，因此这里需要关闭然后再重新打开音频设备。
    snd_pcm_close(_pcm);
    _pcm = NULL;
    ret = snd_pcm_open(&_pcm, pcmName, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
    if (ret < 0)
    {
        debug("Error opening PCM '%s': %s", pcmName, snd_strerror(ret));
        _pcm = NULL;
        return false;
    }

    // 初始化音频设备硬件参数
    if (!_wavFiles.isEmpty())
    {
        return _setHwParams(_wavFiles.first());
    }

    return true;
}

/***************************************************************************************************
 * 说明:
 *      关闭声道
 * 参数:
 *      无
 * 返回值:
 *      无
***************************************************************************************************/
void SoundChannel::close()
{
    if (_pcm)
    {
        snd_pcm_drop(_pcm);
        snd_pcm_close(_pcm);
        _pcm = NULL;
    }

    _mutex.lock();
    _currentWavFile = NULL;
    _pendingWavFiles.clear();
    _mutex.unlock();
}

/***************************************************************************************************
 * 说明:
 *      添加音频文件
 * 参数:
 *      fileName: 文件名
 * 返回值:
 *      音频ID
***************************************************************************************************/
int SoundChannel::addSound(const char *fileName)
{
    if (!fileName)
    {
        return -1;
    }

    WavFile *wav = new WavFile();
    if (!wav->load(fileName))
    {
        delete wav;
        debug("load wav file error:%s", fileName);
        return -1;
    }

    if (!_wavFiles.isEmpty() && !_wavFiles.first()->checkFormat(wav))
    {
        debug("Sound file format not unique: %s", fileName);
        delete wav;
        return -1;
    }

    if (_pcm && (snd_pcm_state(_pcm) == SND_PCM_STATE_OPEN))
    {
        if (!_setHwParams(wav))
        {
            delete wav;
            return -1;
        }
    }

    _wavFiles.append(wav);
    return (_wavFiles.size() - 1);
}

/***************************************************************************************************
 * 说明:
 *      播放指定音频
 * 参数:
 *      soundId: 音频ID
 *      preempt: true 清空当前播放队列，并播放指定的音频文件；false 添加指定的音频文件到播放队列末尾。
 * 返回值:
 *      无
***************************************************************************************************/
void SoundChannel::play(int soundId, bool preempt)
{
    if (!_pcm)
    {
#if 0
        debug("Open PCM channel first!");
#endif
        return;
    }

    if ((soundId < 0) || (soundId >= _wavFiles.size()))
    {
        debug("Invalid sound Id %d", soundId);
        return;
    }

    if (preempt) // 抢占
    {
        _mutex.lock();
        _currentWavFile = _wavFiles.at(soundId);
        _currentWavFile->rewind();
        _pendingWavFiles.clear();
        _mutex.unlock();
    }
    else
    {
        if (!_mutex.tryLock())
        {
            return;
        }

        if (_pendingWavFiles.count() <= 1)
        {
            _pendingWavFiles.enqueue(_wavFiles.at(soundId));
        }

        _mutex.unlock();
    }
}

/***************************************************************************************************
 * 说明:
 *      停止音频播放
 * 参数:
 *      无
 * 返回值:
 *      无
***************************************************************************************************/
void SoundChannel::stop()
{
    _mutex.lock();
    _currentWavFile = NULL;
    _pendingWavFiles.clear();
    _mutex.unlock();
}

/***************************************************************************************************
 * 说明:
 *      是否正在播放
 * 参数:
 *      无
 * 返回值:
 *      无
***************************************************************************************************/
bool SoundChannel::isPlaying()
{
    if(!_pcm)
    {
        return false;
    }

    QMutexLocker locker(&_mutex);
    return (_currentWavFile != NULL || !_pendingWavFiles.empty());
}

/***************************************************************************************************
 * check when the channel is idle (no playing pcm frames)
 **************************************************************************************************/
bool SoundChannel::isIdle()
{
    if(!_pcm)
    {
        return true;
    }

    snd_pcm_sframes_t availFrames = snd_pcm_avail_update(_pcm);
    if(availFrames < 0)
    {
        debug("Stream status error: %s", snd_strerror(availFrames));
        return true;
    }

    return availFrames >= _maxframes;
}

/***************************************************************************************************
 * 说明:
 *      处理音频设备可写事件
 * 参数:
 *      isWriteToDevice:是否向下写数据
 * 返回值:
 *      无
***************************************************************************************************/
void SoundChannel::onWriteReady(bool isWriteToDevice)
{
    if (!_pcm)
    {
        return;
    }

    _mutex.lock();
    if (!_currentWavFile && _pendingWavFiles.isEmpty())
    {
        _mutex.unlock();
        return;
    }

    if (!_currentWavFile)
    {
        _currentWavFile = _pendingWavFiles.dequeue();
        _currentWavFile->rewind();
    }

    FeedPcmState state = _feedPcm(_currentWavFile, isWriteToDevice);
    if (state == FEED_PCM_DONE)
    {
        if (!_pendingWavFiles.isEmpty())
        {
            _currentWavFile = _pendingWavFiles.dequeue();
            _currentWavFile->rewind();
        }
        else
        {
            _currentWavFile = NULL;
        }
    }

    _mutex.unlock();
}

/***************************************************************************************************
 * 说明:
 *      初始化音频设备以适合播放指定音频文件
 * 参数:
 *      wav: 音频文件
 * 返回值:
 *      true 成功；false 失败。
***************************************************************************************************/
bool SoundChannel::_setHwParams(const WavFile *wav)
{
    if (!_pcm || !wav)
    {
        return false;
    }

    int err;
    int dir = 0;
    snd_pcm_hw_params_t *params = NULL;
    snd_pcm_uframes_t size;
    int resample = 1;                               // enable alsa-lib resampling
//    unsigned int buffer_time = 500000;              // ring buffer length in us
//    unsigned int period_time = 100000;              // period time in us

    // Allocate hardware parameter structure (on stack)
    snd_pcm_hw_params_alloca(&params);

    err = snd_pcm_hw_params_any(_pcm, params);
    if (err < 0)
    {
        debug("Broken configuration for playback: no configurations available: %s", snd_strerror(err));
        return false;
    }

    //set hardware resampling
    err = snd_pcm_hw_params_set_rate_resample(_pcm, params, resample);
    if (err < 0)
    {
        debug("Resampling setup failed for playback: %s", snd_strerror(err));
        return false;
    }

    // set the interleaved read/write format */
    err = snd_pcm_hw_params_set_access(_pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0)
    {
        debug("Access type not available for playback: %s", snd_strerror(err));
        return false;
    }

    // Set sample format
    switch (wav->bitsPerSample())
    {
        case 8:
            err = snd_pcm_hw_params_set_format(_pcm, params, SND_PCM_FORMAT_U8);
            break;
        case 16:
            err = snd_pcm_hw_params_set_format(_pcm, params, SND_PCM_FORMAT_S16_LE);
            break;
        case 24:
            err = snd_pcm_hw_params_set_format(_pcm, params, SND_PCM_FORMAT_S24_LE);
            break;
        case 32:
            err = snd_pcm_hw_params_set_format(_pcm, params, SND_PCM_FORMAT_S32_LE);
            break;
        default:
            debug("unknow bits per sample: %d", wav->bitsPerSample());
            return false;
    }
    if (err < 0)
    {
        debug("fail to set format: %s", snd_strerror(err));
        return false;
    }

    // set the count of channels
    unsigned int channels = wav->numChannels();
    err = snd_pcm_hw_params_set_channels(_pcm, params, channels);
    if (err < 0)
    {
        debug("Channels count (%i) not available for playbacks: %s", channels, snd_strerror(err));
        return false;
    }

    // Set sample rate
    unsigned int rate = wav->sampleRate();
    err = snd_pcm_hw_params_set_rate_near(_pcm, params, &rate, 0);
    if (err < 0)
    {
        debug("Rate %iHz not available for playback: %s", rate, snd_strerror(err));
        return false;
    }
//    if (rate != wav->sampleRate())
//    {
//        debug("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
//        return -EINVAL;
//    }

    // set the buffer time
//    err = snd_pcm_hw_params_set_buffer_time_near(_pcm, params, &buffer_time, &dir);
//    if (err < 0)
//    {
//        debug("Unable to set buffer time %i for playback: %s\n", buffer_time, snd_strerror(err));
//        return false;
//    }

    // get the buffer time
//    err = snd_pcm_hw_params_get_buffer_size(params, &size);
//    if (err < 0)
//    {
//        debug("Unable to get buffer size for playback: %s\n", snd_strerror(err));
//        return false;
//    }
//    buffer_size = size;

    // set the period time
//    err = snd_pcm_hw_params_set_period_time_near(_pcm, params, &period_time, &dir);
//    if (err < 0)
//    {
//        debug("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
//        return false;
//    }

    // write the parameters to device
    err = snd_pcm_hw_params(_pcm, params);
    if (err < 0)
    {
        debug("Unable to set hw params for playback: %s", snd_strerror(err));
        return false;
    }

    // get the period time
    err = snd_pcm_hw_params_get_period_size(params, &size, &dir);
    if (err < 0)
    {
        debug("Unable to get period size for playback: %s", snd_strerror(err));
        return err;
    }
    _frames = size;

    snd_pcm_sframes_t maxFrames = 0;
    maxFrames = snd_pcm_avail_update(_pcm);
    if (maxFrames < 0)
    {
        debug("Stream status error: %s", snd_strerror(maxFrames));
        return false;
    }

    _maxframes = (int) maxFrames;

    return true;
}

int SoundChannel::xrun_recovery(snd_pcm_t *handle, int err)
{
    if (err == -EPIPE)
    {
        // under-run
        err = snd_pcm_prepare(handle);
        if (err < 0)
        {
            debug("Can't recovery from underrun, prepare failed: %s", snd_strerror(err));
        }
        return 0;
    }
    else if (err == -ESTRPIPE)
    {
        while ((err = snd_pcm_resume(handle)) == -EAGAIN)
        {
            sleep(1);       // wait until the suspend flag is released
        }
        if (err < 0)
        {
            err = snd_pcm_prepare(handle);
            if (err < 0)
            {
                debug("Can't recovery from suspend, prepare failed: %s", snd_strerror(err));
            }
        }
        return 0;
    }
    return err;
}

/***************************************************************************************************
 * 说明:
 *      发送音频数据
 * 参数:
 *      wav: 音频文件
 *      isWriteToDevice:是否写硬件
 * 返回值:
 *      true 成功；false 失败或无音频数据。
***************************************************************************************************/
FeedPcmState SoundChannel::_feedPcm(WavFile *wav, bool isWriteToDevice)
{
    if (!_pcm || !wav)
    {
        return FEED_PCM_FAILED;
    }

    int blockAlign = wav->blockAlign();
    if (!blockAlign)
    {
        return FEED_PCM_FAILED;
    }

    int ret = 0;
    int size = wav->size();
    int index = wav->index();
    unsigned char *data = wav->data();

    if (size == index)
    {
        if (isWriteToDevice)
        {
            snd_pcm_drain(_pcm);
            return FEED_PCM_DONE;
        }
        else
        {
            return FEED_PCM_SUCCESS;
        }
    }

    int frame = _frames;
    unsigned char buf[frame * blockAlign];

    memset(buf, 0, frame * blockAlign);
    if ((size - index) / blockAlign >= (int)frame)
    {
        memcpy(buf, data + index, frame * blockAlign);
    }
    else
    {
        memcpy(buf, data + index, size - index);
    }

    if (isWriteToDevice)
    {
        ret = snd_pcm_writei(_pcm, buf, frame);
    }
    else
    {
        ret = frame;
    }
    if (ret > 0)
    {
        wav->updateIndex(ret * blockAlign);
        return FEED_PCM_SUCCESS;
    }

//    if (index > 10)
//    {
//        debug("err = %d   index = %d\n",ret,index);
//    }
    switch (ret)
    {
        case -EINTR:
        case -EPIPE:
        case -ESTRPIPE:
            ret = snd_pcm_prepare(_pcm);
            if (ret < 0)
            {
                debug("%s", snd_strerror(ret));
                break;
            }
            return FEED_PCM_RECOVERED;
            break;

        default:
            break;
    }

    return FEED_PCM_FAILED;
}
