/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/9/19
 **/


#include "WavPlayer.h"
#include <alsa/asoundlib.h>
#include <QDebug>
#include <QMutex>
#include "WavFile.h"
#include <QTimer>
#include <QApplication>
#include <qmath.h>

#define PLAYBACK_DEVICE "plughw:0,0"
#define DELIVER_MAX_FRAME 4096
#define HP_DAC_ELEM "HP DAC"
#define PCM_ELEM "PCM"

class WavPlayerPrivate
{
public:
    WavPlayerPrivate()
        : playing(false), requestStop(false), curVolume(50),
          playback_handle(NULL),  buffer_time(0), period_time(0),
          chunk_size(0), chunk_bytes(0), buffer_size(0), audiobuf(NULL),
          bits_per_frame(0), channels(0), format(SND_PCM_FORMAT_UNKNOWN), mixer_handle(NULL),
          pcm_elem(NULL)
    {
        int err;
        if ((err = snd_pcm_open(&playback_handle, PLAYBACK_DEVICE, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)) < 0)
        {
            qWarning() << "Cannot open playback device " << PLAYBACK_DEVICE << ":"
                       << snd_strerror(err);
            return;
        }

        audiobuf = (unsigned char *)malloc(1024);
        if (audiobuf == NULL)
        {
            qWarning() << "no enough memory";
        }

        if ((err = snd_mixer_open(&mixer_handle, 0)) < 0)
        {
            qWarning() << "Mixer open error:" << snd_strerror(err);
            return;
        }

        if ((err = snd_mixer_attach(mixer_handle, "default")) < 0)
        {
            qWarning() << "Mixer attach error:" << snd_strerror(err);
            snd_mixer_close(mixer_handle);
            mixer_handle = NULL;
            return;
        }

        if ((err = snd_mixer_selem_register(mixer_handle, NULL, NULL)) < 0)
        {
            qWarning() << "Mixer register error";
            snd_mixer_close(mixer_handle);
            mixer_handle = NULL;
            return;
        }

        err = snd_mixer_load(mixer_handle);
        if (err  < 0)
        {
            qWarning() << "Mixer load error";
            snd_mixer_close(mixer_handle);
            mixer_handle = NULL;
            return;
        }

        snd_mixer_selem_id_t *sid;
        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, HP_DAC_ELEM);

        snd_mixer_elem_t *elem = snd_mixer_find_selem(mixer_handle, sid);
        if (!elem)
        {
            qWarning("Unable to find simple control '%s',%i\n",
                     snd_mixer_selem_id_get_name(sid),
                     snd_mixer_selem_id_get_index(sid));
            snd_mixer_close(mixer_handle);
            mixer_handle = NULL;
            return;
        }

        // set HD DAC valume to 100%
        setVolume(elem, 100);

        snd_mixer_selem_id_set_name(sid, PCM_ELEM);

        elem = snd_mixer_find_selem(mixer_handle, sid);
        if (!elem)
        {
            qWarning("Unable to find simple control '%s',%i\n",
                     snd_mixer_selem_id_get_name(sid),
                     snd_mixer_selem_id_get_index(sid));
        }
        else
        {
            pcm_elem = elem;
        }
    }

    WavPlayerPrivate(const WavPlayerPrivate &c)
    {
        playing = c.playing;
        requestStop = c.requestStop;
        curVolume = c.curVolume;
        playback_handle = c.playback_handle;
        buffer_time = c.buffer_time;
        period_time = c.period_time;
        chunk_size = c.chunk_size;
        chunk_bytes = c.chunk_bytes;
        buffer_size = c.buffer_size;
        audiobuf = NULL;
        bits_per_frame = c.bits_per_frame;
        channels = c.channels;
        format = c.format;
        mixer_handle = c.mixer_handle;
        pcm_elem = c.pcm_elem;
    }
    ~WavPlayerPrivate()
    {
        if (audiobuf)
        {
            free(audiobuf);
        }

        if (playback_handle)
        {
            snd_pcm_close(playback_handle);
        }

        if (mixer_handle)
        {
            snd_mixer_close(mixer_handle);
        }
    }

    /* set the pcm params */
    bool setParams(const WavFile *file)
    {
        snd_pcm_hw_params_t *params;
        snd_pcm_sw_params_t *swparams;
        int err;
        unsigned int rate;
        snd_pcm_uframes_t  startThreshold, stopThreshold;

        if (file == NULL)
        {
            return false;
        }

        snd_pcm_hw_params_alloca(&params);
        snd_pcm_sw_params_alloca(&swparams);

        err = snd_pcm_hw_params_any(playback_handle, params);
        if (err < 0)
        {
            qWarning() << "Broken configuration for this PCM: no configurations avaliable";
            return false;
        }

        err = snd_pcm_hw_params_set_access(playback_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
        if (err < 0)
        {
            qWarning() << "Access type no avaliable";
            return false;
        }


        switch (file->bitsPerSample())
        {
        case 8:
            format = SND_PCM_FORMAT_U8;
            break;
        case 16:
            format = SND_PCM_FORMAT_S16_LE;
            break;
        case 24:
            format = SND_PCM_FORMAT_S24_LE;
            break;
        case 32:
            format = SND_PCM_FORMAT_S32_LE;
            break;
        default:
            qWarning() << "unsupport format " << file->bitsPerSample();
            return false;
        }
        err = snd_pcm_hw_params_set_format(playback_handle, params, format);
        if (err < 0)
        {
            qWarning() << "Sample format no avaliable";
            return false;
        }

        channels = file->numChannels();
        err = snd_pcm_hw_params_set_channels(playback_handle, params, channels);
        if (err < 0)
        {
            qWarning() << "Channels count not avaliable";
            return false;
        }

        rate = file->sampleRate();
        err = snd_pcm_hw_params_set_rate_near(playback_handle, params, &rate, 0);
        if (err < 0)
        {
            qWarning() << "Unable to set sample rate";
            return false;
        }

        if (buffer_time == 0)
        {
            err = snd_pcm_hw_params_get_buffer_time_max(params, &buffer_time, 0);
            Q_ASSERT(err >= 0);
            if (buffer_time > 500000)
            {
                buffer_time = 500000;
            }
        }

        if (period_time == 0)
        {
            period_time = buffer_time / 4;
        }

        if (period_time > 0)
        {
            err = snd_pcm_hw_params_set_period_time_near(playback_handle,
                    params, &period_time, 0);
            Q_ASSERT(err >= 0);
        }

        if (buffer_time > 0)
        {
            err = snd_pcm_hw_params_set_buffer_time_near(playback_handle, params,
                    &buffer_time, 0);
            Q_ASSERT(err >= 0);
        }

        err = snd_pcm_hw_params(playback_handle, params);
        if (err < 0)
        {
            qWarning() << "Unable to install hw params";
            return false;
        }

        snd_pcm_hw_params_get_period_size(params, &chunk_size, 0);
        snd_pcm_hw_params_get_buffer_size(params, &buffer_size);
        if (chunk_size ==  buffer_size)
        {
            qWarning() << "Can't use period equal to buffer size";
            return false;
        }

        snd_pcm_sw_params_current(playback_handle, swparams);

        snd_pcm_sw_params_set_avail_min(playback_handle, swparams, chunk_size);

        startThreshold = 1;
        stopThreshold = buffer_size;

        err = snd_pcm_sw_params_set_start_threshold(playback_handle, swparams, startThreshold);
        Q_ASSERT(err >= 0);
        err = snd_pcm_sw_params_set_stop_threshold(playback_handle, swparams, stopThreshold);
        Q_ASSERT(err >= 0);

        if (snd_pcm_sw_params(playback_handle, swparams) < 0)
        {
            qWarning() << "Unable to install sw params";
            return false;
        }

        size_t bits_per_sample;
        bits_per_sample = snd_pcm_format_physical_width(format);
        bits_per_frame = bits_per_sample * file->numChannels();

        chunk_bytes = chunk_size * bits_per_frame / 8;
        unsigned char *newAudioBuf = (unsigned char *)realloc(audiobuf, chunk_bytes);
        if (newAudioBuf == NULL)
        {
            free(audiobuf);
            audiobuf = NULL;
            qWarning() << "No enough memory";
            return false;
        }
        audiobuf = newAudioBuf;

        return true;
    }

    /* write function */
    ssize_t pcm_write(unsigned char *data, size_t count)
    {
        ssize_t result = 0;

        if (count < chunk_size)
        {
            snd_pcm_format_set_silence(format, data + count * bits_per_frame / 8, (chunk_size - count) * channels);
            count = chunk_size;
        }

        while (count > 0 && !requestStop)
        {
            ssize_t r = 0;
            r = snd_pcm_writei(playback_handle, data, count);
            if (r == -EAGAIN || (r >= 0 && (size_t)r < count))
            {
                snd_pcm_wait(playback_handle, 100);
            }
            else if (r == -EPIPE)
            {
                if (!xrun())
                {
                    return -1;
                }
            }
            else if (r == -ESTRPIPE)
            {
                if (!suspend())
                {
                    return -1;
                }
            }
            else if (r < 0)
            {
                qWarning("Write error: %s\n", snd_strerror(r));
                return -1;
            }

            if (r > 0)
            {
                result += r;
                count -= r;
                data += r * bits_per_frame / 8;
            }
            QApplication::processEvents();
        }
        return result;
    }

    /* I/O suspend handler */
    bool suspend()
    {
        int res;
        qWarning() << "Suspended, trying resume...";
        while ((res = snd_pcm_resume(playback_handle)) == -EAGAIN)
        {
            sleep(1);
        }
        if (res < 0)
        {
            qWarning() << "Failed, restart stream...";
            if ((res = snd_pcm_prepare(playback_handle)) < 0)
            {
                qWarning("suspend: perpare error:%s\n", snd_strerror(res));
                return false;
            }
        }
        return true;
    }

    /* I/O error handler */
    bool xrun()
    {
        snd_pcm_status_t *status;
        int res;
        snd_pcm_status_alloca(&status);
        if ((res = snd_pcm_status(playback_handle, status)) < 0)
        {
            qWarning("Status error:%s\n", snd_strerror(res));
            return false;
        }

        if (snd_pcm_status_get_state(status) == SND_PCM_STATE_XRUN)
        {
            if ((res = snd_pcm_prepare(playback_handle)) < 0)
            {
                qWarning("xrun: perpare error:%s\n", snd_strerror(res));
                return false;
            }
            return true;
        }
        return false;
    }

    /* send volume for mixer elem, vol in range of (0~100)% */
    void setVolume(snd_mixer_elem_t *elem, int vol)
    {
        if (!mixer_handle || !elem)
        {
            return;
        }

        int err = 0;
        long min = 0;
        long max = 0;
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

        long v = qCeil(vol * (max - min) * 0.01 + min);

        err = snd_mixer_selem_set_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, v);
        if (err < 0)
        {
            qWarning() << "Set lef volume fail";
        }
        err = snd_mixer_selem_set_playback_volume(elem, SND_MIXER_SCHN_FRONT_RIGHT, v);
        if (err < 0)
        {
            qWarning() << "Set right volume fail";
        }
    }

    bool playing;
    bool requestStop;
    int curVolume;
    snd_pcm_t *playback_handle;
    unsigned buffer_time;
    unsigned period_time;
    snd_pcm_uframes_t chunk_size;
    unsigned chunk_bytes;
    snd_pcm_uframes_t buffer_size;
    unsigned char *audiobuf;
    size_t bits_per_frame;
    int channels;
    snd_pcm_format_t format;

    snd_mixer_t *mixer_handle;
    snd_mixer_elem_t *pcm_elem;
};

WavPlayer::WavPlayer()
    : QObject(), d_ptr(new WavPlayerPrivate())
{
}

WavPlayer::~WavPlayer()
{
}

bool WavPlayer::isPlaying() const
{
    return d_ptr->playing;
}

bool WavPlayer::isValid() const
{
    return d_ptr->playback_handle != NULL;
}

void WavPlayer::setVolume(int vol)
{
    if (d_ptr->mixer_handle && d_ptr->pcm_elem)
    {
        vol = vol < 0 ? 0 : vol;
        vol = vol > 100 ? 100 : vol;
        d_ptr->setVolume(d_ptr->pcm_elem, vol);
    }
}

void WavPlayer::play(WavFile *wavfile)
{
    d_ptr->playing = true;
    if (!wavfile || !wavfile->data() || !d_ptr->playback_handle)
    {
        d_ptr->playing = false;
        emit finished();
        return;
    }

    wavfile->rewind();

    if (snd_pcm_state(d_ptr->playback_handle) == SND_PCM_STATE_RUNNING)
    {
        d_ptr->playing = false;
        emit finished();
        return;
    }

    if (!d_ptr->setParams(wavfile))
    {
        d_ptr->playing = false;
        emit finished();
        return;
    }

    int err;
    if ((err = snd_pcm_prepare(d_ptr->playback_handle)) < 0)
    {
        qDebug() << "Cannot prepare audio interface for use" << snd_strerror(err);
        d_ptr->playing = false;
        emit finished();
        return;
    }

    while (!d_ptr->requestStop)
    {
        int size = wavfile->size();
        int index = wavfile->index();
        unsigned char *buf = wavfile->data();
        size_t count;
        if (index == size)
        {
            // set to block mode and flush data
            snd_pcm_nonblock(d_ptr->playback_handle, 0);
            snd_pcm_drain(d_ptr->playback_handle);
            snd_pcm_nonblock(d_ptr->playback_handle, 1);
            break;
        }

        QApplication::processEvents();

        if (size -  index < static_cast<int>(d_ptr->chunk_bytes))
        {
            memcpy(d_ptr->audiobuf, buf + index, size - index);
            wavfile->updateIndex(size - index);
            count = (size - index) * 8 / d_ptr->bits_per_frame;
        }
        else
        {
            memcpy(d_ptr->audiobuf, buf + index, d_ptr->chunk_bytes);
            wavfile->updateIndex(d_ptr->chunk_bytes);
            count = d_ptr->chunk_size;
        }

        int ret = d_ptr->pcm_write(d_ptr->audiobuf, count);

        if (ret == -1)
        {
            break;
        }
    }

    if (d_ptr->requestStop)
    {
        // drop the frames
        snd_pcm_drop(d_ptr->playback_handle);
        // set to block mode and flush data
        snd_pcm_nonblock(d_ptr->playback_handle, 0);
        snd_pcm_drain(d_ptr->playback_handle);
        snd_pcm_nonblock(d_ptr->playback_handle, 1);
        d_ptr->playing = false;
        d_ptr->requestStop = false;
    }
    else
    {
        d_ptr->playing = false;
    }
    emit finished();
}

void WavPlayer::stop()
{
    if (d_ptr->playback_handle && d_ptr->playing)
    {
        d_ptr->requestStop = true;
    }
}
