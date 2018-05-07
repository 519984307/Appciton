#pragma once
#include "EventDataDefine.h"
#include <QScopedPointer>
#include <QByteArray>

class OxyCRGStorageItemPrivate;
class OxyCRGStorageItem
{
public:
    OxyCRGStorageItem();
    ~OxyCRGStorageItem();

    //start collect trend and waveform data, return false if already start
    bool startCollectTrendAndWaveformData();

private:
    QScopedPointer<OxyCRGStorageItemPrivate> d_ptr;
};
