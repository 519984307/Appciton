/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/15
 **/

#pragma once
#include <QObject>
#include <QTimer>
#include <QTime>
#include <QFile>
#include "Provider.h"
#include "ECGProviderIFace.h"
#include "SPO2ProviderIFace.h"
#include "RESPProviderIFace.h"
#include "NIBPProviderIFace.h"
#include "CO2ProviderIFace.h"
#include "IBPProviderIFace.h"
#include "COProviderIFace.h"
#include "AGProviderIFace.h"
#include "AlarmDefine.h"


class ECGParam;
class SPO2Param;
class NIBPParam;
class CO2Param;
class DemoProvider: public Provider,
        public ECGProviderIFace,
        public SPO2ProviderIFace,
        public RESPProviderIFace,
        public NIBPProviderIFace,
        public CO2ProviderIFace,
        public IBPProviderIFace,
        public COProviderIFace,
        public AGProviderIFace
{
    Q_OBJECT
public:
    virtual void sendVersion() { }
    // 实现Provider的接口。
    virtual bool attachParam(Param *param);
    virtual void detachParam(Param *param);
    virtual void dataArrived(void);
    virtual void checkConnection() {}
    virtual bool connected() {return true;}

    // 实现ECG的接口。
    virtual int getWaveformSample(void) {return _waveSampleRate;}
    virtual void setWaveformSample(int) {}

    virtual int getBaseLine(void) {return 128;}
    virtual int getP05mV(){return 192;}
    virtual int getN05mV(){return 64;}

    virtual void getLeadCabelType() { }
    virtual void setLeadSystem(ECGLeadMode /*leadMode*/) { }
    virtual void setCalcLead(ECGLead /*lead*/) { }
    virtual void setPatientType(unsigned char /*type*/){}
    virtual void setBandwidth(ECGBandwidth /*bandwidth*/) { }
    virtual void setFilterMode(ECGFilterMode /*mode*/) {}
    virtual void enablePacermaker(ECGPaceMode /*onoff*/) { }
    virtual void setNotchFilter(ECGNotchFilter /*lotch*/) {}
    virtual void enableSTAnalysis(bool /*onoff*/) { }
    virtual void setSTPoints(int /*iso*/, int /*st*/) { }
    virtual void setSelfLearn(bool /*onOff*/) { }
    virtual void setARRThreshold(ECGAlg::ARRPara /*parameter*/, short /*value*/) {}
    virtual void enableRawData(bool /*onOff*/) {}

    // 实现SPO2的接口。
    virtual void setSensitive(SPO2Sensitive /*sens*/) { }
    virtual void sendStatus(void) { }
    virtual int getSPO2WaveformSample(void) { return 250; }
    virtual int getSPO2BaseLine(void) {return 0x0;}
    virtual int getSPO2MaxValue(void) {return 128;}

    // 实现RESP的接口。
    virtual int maxRESPWaveValue() {return 255;}
    virtual int minRESPWaveValue() {return 0;}
    virtual int getRESPWaveformSample(void) { return 250; }
    virtual int getRESPBaseLine(void) {return 0x80;}
    virtual void disableApnea(void) { }
    virtual void setApneaTime(ApneaAlarmTime /*t*/) { }
    virtual void setWaveformZoom(RESPZoom /*zoom*/) { }
    virtual void setRESPCalcLead(RESPLead /*lead*/) { }
    virtual void enableRESPCalc(bool /*enable*/) {}
    /* reimplement */
    RESPModuleType getRespModuleType() const { return MODULE_NR;}

    // 实现NIBP的接口。
    virtual void setPatientType(int) { }
    virtual void startMeasure(unsigned char /*type*/) { }
    virtual void stopMeasure(void) { }
    virtual void setInitInflate(int /*adultPress*/, int /*pedPress*/, int /*neoPress*/) { }

    // 实现CO2的接口。
    virtual void zeroCalibration(void) { }
    virtual void setApneaTimeout(ApneaAlarmTime /*t*/) { }
    virtual void setN2OCompensation(int /*comp*/) { }
    virtual void setO2Compensation(int /*comp*/) { }
    virtual int getCO2WaveformSample(void) { return 20; }
    virtual int getCO2MaxWaveform(void) { return 500; }
    virtual int getCO2BaseLine(void) {return 0;}
    virtual void setWorkMode(CO2WorkMode /*mode*/) {}
    virtual void enterUpgradeMode() {}
    virtual void sendCalibrateData(int) {}
    virtual CO2ModuleType getCo2ModuleType() const { return MODULE_CO2_NR;}

    // 实现AG的接口。
    virtual int getN2OWaveformSample(void) {return 50;}
    virtual int getAA1WaveformSample(void) {return 50;}
    virtual int getAA2WaveformSample(void) {return 50;}
    virtual int getO2WaveformSample(void) {return 50;}
    virtual int getN2OBaseLine(void) {return 0;}
    virtual int getAA1BaseLine(void) {return 0;}
    virtual int getAA2BaseLine(void) {return 0;}
    virtual int getO2BaseLine(void) {return 0;}
    virtual int getN2OMaxWaveform(void) {return 2500;}
    virtual int getAA1MaxWaveform(void) {return 2500;}
    virtual int getAA2MaxWaveform(void) {return 2500;}
    virtual int getO2MaxWaveform(void) {return 2500;}

    // 实现IBP的接口
    virtual int getIBPWaveformSample(void) {return 250;}
    virtual int getIBPMaxWaveform(void){return 300;}
    virtual int getIBPBaseLine(void){return 0;}


    /**
     * @brief getDemoWaveData get the demo waveform data
     * @param waveid the waveform id
     * @return the data of the waveform, each byte present one wave value
     */
    QByteArray getDemoWaveData(WaveformID waveid);


    DemoProvider();
    virtual ~DemoProvider();

protected:
    virtual void timerEvent(QTimerEvent *event);
    virtual void disconnected(void) { }
    virtual void reconnected(void) { }

private:
    void _produceDemoData(void);
    QFile _demoFile[WAVE_NR];
    int _timerID;
    QTime _time;

    int _waveSampleRate;
};
