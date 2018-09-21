/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/9
 **/

#pragma once
#include "BLMProvider.h"
#include "ECGProviderIFace.h"
#include "RESPProviderIFace.h"
#include "ECGAlg2SoftInterface.h"
#include <QScopedPointer>

class E5ProviderPrivate;
class E5Provider : public BLMProvider, public ECGProviderIFace, public RESPProviderIFace
{

public:
    E5Provider();
    ~E5Provider();

public:
    // provider interface
    // attach to param
    virtual bool attachParam(Param &param);

    // detach from param
    virtual void detachParam(Param &param);

    // handle packet
    virtual void handlePacket(unsigned char *data, int len);

    // send command to get version
    virtual void sendVersion(void);

    // disconnected callback
    virtual void disconnected(void);

    // reconnect callback
    virtual void reconnected(void);

public:
    // ECG Provider interface
    // get the waveform sample
    virtual int getWaveformSample(void);

    // set the sample rate
    virtual void setWaveformSample(int rate);

    // waveform baseline
    virtual int getBaseLine(void)
    {
        return 0;
    }

    // the corespond value for +/- 0.5mv
    virtual void get05mV(int &p05mv, int &n05mv);

    // get the lead type
    virtual void getLeadCabelType();

    // set lead system
    virtual void setLeadSystem(ECGLeadMode leadSystem);

    // set calculate lead
    virtual void setCalcLead(ECGLead lead);

    // set patient type
    virtual void setPatientType(unsigned char type);

    // set bandwidth
    virtual void setBandwidth(ECGBandwidth bandwidth);

    // set filter mode
    virtual void setFilterMode(ECGFilterMode mode);

    // set the pacer status
    virtual void enablePacermaker(ECGPaceMode onoff);

    // set the notch filter
    virtual void setNotchFilter(ECGNotchFilter notch);

    // enabel ST analysis
    virtual void enableSTAnalysis(bool onoff);

    // ST point setting
    virtual void setSTPoints(int iso, int st);

    // set self learn
    virtual void setSelfLearn(bool onOff);

    // set threshold
    virtual void setARRThreshold(ECGAlg::ARRPara parameter, short value);

public:
    // RESP provider interface
    // max wave value
    virtual int maxRESPWaveValue()
    {
//        return 0x3FFF;
        return 255;
    }

    // min wave value
    virtual int minRESPWaveValue()
    {
//        return -0x4000;
        return 0;
    }

    // get wave sample rate
    virtual int getRESPWaveformSample(void);

    // get the waveform baseline
    virtual int getRESPBaseLine()
    {
        return 0;
    }

    // disable apnea handling
    virtual void disableApnea();

    // set apnea time
    virtual void setApneaTime(ApneaAlarmTime t);

    // set the waveform gain
    virtual void setWaveformZoom(RESPZoom zoom);

    // set the resp calculate lead
    virtual void setRESPCalcLead(RESPLead lead);

    // enable resp
    virtual void enableRESPCalc(bool enable);


private:
    QScopedPointer<E5ProviderPrivate> d_ptr;
};
