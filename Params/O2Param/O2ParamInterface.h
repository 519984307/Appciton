/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/27
 **/

#pragma once

enum ApneaStimulationReason
{
    APNEASTIMULATION_REASON_RESP,
    APNEASTIMULATION_REASON_HR,
    APNEASTIMULATION_REASON_SPO2,
    APNEASTIMULATION_REASON_ETCO2,
    APNEASTIMULATION_REASON_SELFTEST
};

class O2ParamInterface
{
public:
    O2ParamInterface(){}
    virtual ~O2ParamInterface(){}

    static O2ParamInterface *registerO2ParamInterface(O2ParamInterface *interface);

    static O2ParamInterface *getO2ParamInterface();

    /**
     * @brief setMotorRelationParam
     * @param factor
     * @param sta
     */
    virtual void setVibrationReason(ApneaStimulationReason reason, bool sta) = 0;

    /**
     * @brief setApneaAwakeStatus set apnea awake status
     * @param flag
     */
    virtual void setApneaAwakeStatus(bool sta) = 0;

    /**
     * @brief sendMotorControl
     * @param control
     * @param selfTest
     */
    virtual void setVibration(int control) = 0;

    /**
     * @brief updateApneaStimulationStatus
     */
    virtual void updateApneaStimulationStatus() = 0;
};
