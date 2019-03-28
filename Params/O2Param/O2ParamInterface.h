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

enum ApneaStimulationFactor
{
    APNEASTIMULATION_FACTOR_RESP,
    APNEASTIMULATION_FACTOR_HR,
    APNEASTIMULATION_FACTOR_SPO2,
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
    virtual void setMotorRelationParam(ApneaStimulationFactor factor, bool sta) = 0;

    /**
     * @brief getMotorRelationParam
     * @return
     */
    virtual int getMotorRelationParam() = 0;

    /**
     * @brief setApneaAwakeStatus set apnea awake status
     * @param flag
     */
    virtual void setApneaAwakeStatus(bool sta) = 0;

    /**
     * @brief getApneaAwakeStatus get apnea awake status
     * @return
     */
    virtual bool getApneaAwakeStatus() = 0;

    /**
     * @brief sendMotorControl
     * @param control
     * @param selfTest
     */
     virtual void sendMotorControl(int control, bool selfTest = false) = 0;
};
