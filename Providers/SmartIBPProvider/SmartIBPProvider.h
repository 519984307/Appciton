/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/4/26
 **/

#pragma once
#include "Providers/Provider.h"
#include "IBPProviderIFace.h"
#include <QScopedPointer>

class SmartIBPProviderPrivate;
class SmartIBPProvider : public Provider, public IBPProviderIFace
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit SmartIBPProvider(const QString);
    ~SmartIBPProvider();

    /* reimplement */
    bool attachParam(Param *param);

    /* reimplement */
    void sendVersion();
    /* reimplement */
    void disconnected();
    /* reimplement */
    void reconnected();

    /* reimplement */
    void setZero(IBPChannel IBP, IBPCalibration calibration, unsigned short value);

    /* get wave sample rate */
    virtual int getIBPWaveformSample(void);

    /* get the base line */
    virtual int getIBPBaseLine(void);

    /* get the maximum waveform value */
    virtual int getIBPMaxWaveform(void);

    /* reimplement */
    IBPModuleType getIBPModuleType() const { return IBP_MODULE_SMART_IBP; }

protected:
    void dataArrived();

private:
    const QScopedPointer<SmartIBPProviderPrivate> pimpl;
    friend class SmartIBPProviderPrivate;
};
