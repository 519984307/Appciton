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
public:
    virtual void sendVersion() { }
    // 实现Provider的接口。
    virtual bool attachParam(Param &param);
    virtual void detachParam(Param &param);
    virtual void dataArrived(void);

    // 实现ECG的接口。
    virtual int getWaveformSample(void) {return _waveSampleRate;}
    virtual void setWaveformSample(int rate) {_waveSampleRate = rate;}

    virtual int getBaseLine(void) {return 128;}
    virtual void get05mV(int &p05mv, int &n05mv){p05mv = 192; n05mv = 64;}
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

    // 实现SPO2的接口。
    virtual void setSensitive(SPO2Sensitive /*sens*/) { }
    virtual void sendStatus(void) { }
    virtual int getSPO2WaveformSample(void) { return 50; }
    virtual int getSPO2BaseLine(void) {return 0x80;}
    virtual int getSPO2MaxValue(void) {return 256;}

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
    virtual int getCO2MaxWaveform(void) { return 50; }
    virtual int getCO2BaseLine(void) {return 0;}
    virtual void setWorkMode(CO2WorkMode /*mode*/) {}

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
