/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/13
 **/

#include "O2Param.h"
#include "O2ProviderIFace.h"
#include "O2TrendWidget.h"
#include "IConfig.h"
#include "RunningStatusBar.h"
#include "PatientManager.h"
#include "ConfigManager.h"

class O2ParamPrivate
{
public:
    O2ParamPrivate() : provider(NULL), trendWidget(NULL),
        o2Value(InvData()), calibConcentration(InvData()),
        calibResult(false), calibReply(false), motorSta(0),
        updateApneaStimulationSta(false)
    {}
    O2ProviderIFace *provider;
    O2TrendWidget *trendWidget;
    int16_t o2Value;
    int calibConcentration;
    int calibResult;
    int calibReply;
    int motorSta;
    int updateApneaStimulationSta;
    QTimer updateVibrationTmr;
};

O2Param &O2Param::getInstance()
{
    static O2Param *instance = NULL;
    if (instance == NULL)
    {
        instance = new O2Param();
        O2ParamInterface *old = registerO2ParamInterface(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

void O2Param::initParam()
{
}

void O2Param::handDemoTrendData()
{
    d_ptr->o2Value = 25;
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setO2Value(d_ptr->o2Value);
    }
}

void O2Param::exitDemo()
{
    d_ptr->o2Value = InvData();
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setO2Value(d_ptr->o2Value);
    }
}

int16_t O2Param::getSubParamValue(SubParamID id)
{
    if (id == SUB_PARAM_O2)
    {
        return getO2Concentration();
    }
    return InvData();
}

void O2Param::showSubParamValue()
{
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->showValue();
    }
}

UnitType O2Param::getCurrentUnit(SubParamID id)
{
    Q_UNUSED(id)
    return UNIT_PERCENT;
}

void O2Param::setProvider(O2ProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    d_ptr->provider = provider;
    d_ptr->provider->sendO2SelfTest();
    d_ptr->provider->sendProbeState();
}

bool O2Param::isServiceProviderOk()
{
    if (d_ptr->provider)
    {
        return true;
    }
    return false;
}

void O2Param::setTrendWidget(O2TrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    d_ptr->trendWidget = trendWidget;
}

void O2Param::setO2Concentration(int16_t o2)
{
    paramUpdateTimer->start(PARAM_UPDATE_TIMEOUT);
    if (d_ptr->o2Value == o2)
    {
        return;
    }
    d_ptr->o2Value = o2;
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setO2Value(d_ptr->o2Value);
    }
}

int16_t O2Param::getO2Concentration()
{
    return d_ptr->o2Value;
}

void O2Param::noticeLimitAlarm(bool flag)
{
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->isAlarm(flag);
    }
}

void O2Param::reset()
{
    if (NULL == d_ptr->provider)
    {
        return;
    }
    d_ptr->provider->sendO2SelfTest();
    d_ptr->provider->sendProbeState();
}

void O2Param::updateSubParamLimit(SubParamID id)
{
    if (id == SUB_PARAM_O2)
    {
        d_ptr->trendWidget->updateLimit();
    }
}

void O2Param::setApneaAwakeStatus(bool sta)
{
    systemConfig.setNumValue("PrimaryCfg|O2|ApneaAwake", sta);
    PatientType type = patientManager.getType();
    if (type == PATIENT_TYPE_NEO && sta)
    {
        updateApneaStimulationStatus();
    }
    else
    {
        o2Param.setVibration(false);
        runningStatus.setShakeStatus(SHAKE_OFF);
    }
}

bool O2Param::getApneaAwakeStatus()
{
    bool sta = false;
    systemConfig.getNumValue("PrimaryCfg|O2|ApneaAwake", sta);
    PatientType type = patientManager.getType();
    return sta && (type == PATIENT_TYPE_NEO);
}

void O2Param::updateApneaStimulationStatus()
{
    d_ptr->updateApneaStimulationSta = true;
}

void O2Param::setVibration(bool vibrate)
{
    if (d_ptr->provider)
    {
        d_ptr->provider->sendMotorControl(vibrate);
    }

    if (vibrate)
    {
        runningStatus.setShakeStatus(SHAKING);
    }
    else
    {
        runningStatus.setShakeStatus(SHAKE_ON);
    }
}

void O2Param::vibrationIntensityControl(int intensity)
{
    if (d_ptr->provider)
    {
        d_ptr->provider->sendVibrationIntensity(intensity);
    }
}

void O2Param::sendCalibration(int concentration)
{
    if (d_ptr->provider)
    {
        d_ptr->calibConcentration = concentration;
        d_ptr->provider->sendCalibration(concentration);
    }
}

void O2Param::calibrationResult(unsigned char *packet)
{
    if (packet[1] == d_ptr->calibConcentration)
    {
        d_ptr->calibReply = true;
        d_ptr->calibResult = packet[2];
    }
}

bool O2Param::getCalibrationReply()
{
    bool reply = d_ptr->calibReply;
    if (reply)
    {
        d_ptr->calibReply = false;
    }
    return reply;
}

bool O2Param::getCalibrationResult()
{
    return d_ptr->calibResult;
}

void O2Param::setVibrationReason(ApneaStimulationReason reason, bool sta)
{
    if (getApneaAwakeStatus())
    {
        // 唤醒器之前状态
        bool preMotorSta = d_ptr->motorSta;
        if (sta)
        {
            d_ptr->motorSta |= 1 << reason;
        }
        else
        {
            d_ptr->motorSta &= ~(1 << reason);
        }
        // 唤醒器当前状态
        bool curMotorSta = d_ptr->motorSta;

        // 状态切换或者新建病人都要重新刷新
        if ((preMotorSta != curMotorSta) || (d_ptr->updateApneaStimulationSta))
        {
            if ((reason == APNEASTIMULATION_REASON_RESP
                 || reason == APNEASTIMULATION_REASON_HR
                 || reason == APNEASTIMULATION_REASON_SPO2)
                    && (!d_ptr->updateApneaStimulationSta))
            {
                // 若是原因为低于RESP或HR时，设置定时器，等值稳定后再设置到呼吸窒息
                d_ptr->updateVibrationTmr.start(1000);
                return;
            }

            setVibration(d_ptr->motorSta);

            if (d_ptr->updateApneaStimulationSta)
            {
                d_ptr->updateApneaStimulationSta = false;
            }
        }
    }
}

void O2Param::paramUpdateTimeout()
{
    d_ptr->o2Value = InvData();
    if (d_ptr->trendWidget != NULL)
    {
        d_ptr->trendWidget->setO2Value(d_ptr->o2Value);
    }
}

void O2Param::updateVibrationTimeout()
{
    d_ptr->updateVibrationTmr.stop();
    setVibration(d_ptr->motorSta);
}

O2Param::O2Param() :
    Param(PARAM_O2),
    O2ParamInterface(),
    d_ptr(new O2ParamPrivate())
{
    connect(&(d_ptr->updateVibrationTmr), SIGNAL(timeout()), this, SLOT(updateVibrationTimeout()));
}

O2Param::~O2Param()
{
    delete d_ptr;
}
