#include "FreezeDataModel.h"
#include "WaveformCache.h"
#include <QVector>
#include <QMap>
#include <QDebug>

class FreezeDataModelPrivate
{
public:
    FreezeDataModelPrivate(unsigned t, WaveformID id)
        :t(t),
          id(id),
          sampleRate(waveformCache.getSampleRate(id)),
          duration(waveformCache.channelDuration()),
          reviewTime(0)
    {
        int sampleRate = waveformCache.getSampleRate(id);
        unsigned duration = waveformCache.channelDuration();
        int wavesize = sampleRate * duration;

        wavedata.resize(wavesize);

        waveformCache.readStorageChannel(id, wavedata.data(), duration, false);
    }

    unsigned t;
    WaveformID id;
    int sampleRate;
    unsigned duration;
    unsigned reviewTime;
    QVector<WaveDataType> wavedata;
};

FreezeDataModel::FreezeDataModel(unsigned t, WaveformID id, QObject *parent)
    :QObject(parent), d_ptr(new FreezeDataModelPrivate(t, id))
{

}

FreezeDataModel::~FreezeDataModel()
{
    qDebug("FreezedataModel(%d, %d) destroy", d_ptr->t,d_ptr->id);
}

unsigned FreezeDataModel::timestamp() const
{
    return d_ptr->t;
}

unsigned FreezeDataModel::totalReveiwTime() const
{
    return waveformCache.channelDuration();
}

unsigned FreezeDataModel::getReviewStartSecond() const
{
    return d_ptr->reviewTime;
}

void FreezeDataModel::setReviewStartSecond(int second)
{
    if(d_ptr->reviewTime == second || second > d_ptr->duration)
    {
        return;
    }

    d_ptr->reviewTime = second;
    emit dataChanged();
}

void FreezeDataModel::getWaveData(WaveDataType *buff, int size)
{
    int leftSize = (d_ptr->duration - d_ptr->reviewTime) * d_ptr->sampleRate;

    if(leftSize > size)
    {
        //yeah, we have enough data to fill the buff
        int startIndex = d_ptr->wavedata.size() - d_ptr->reviewTime * d_ptr->sampleRate -  size;
        qMemCopy(buff, d_ptr->wavedata.constData() + startIndex, size * sizeof(WaveDataType));
    }
    else
    {
        // no enough data
        int invalidDataLength = size - leftSize;
        qMemCopy(buff + invalidDataLength, d_ptr->wavedata.constData(), leftSize * sizeof(WaveDataType));

        //fill invalid data
        qFill(buff, buff + invalidDataLength, INVALID_WAVE_FALG_BIT);
    }
}
