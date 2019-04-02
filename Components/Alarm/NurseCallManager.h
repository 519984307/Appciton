/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/4/2
 **/

#pragma once
#include "NurseCallManagerInterface.h"
#include <QObject>

class NurseCallManagerPrivate;
class NurseCallManager : public QObject , public NurseCallManagerInterface
{
    Q_OBJECT
public:
    static NurseCallManager &getInstance();

public:
    virtual bool getAlarmLevelHigh();
    virtual bool getAlarmLevelMed();
    virtual bool getAlarmLevelLow();
    virtual bool getAlarmTypeTech();
    virtual bool getAlarmTypePhy();
    virtual void callNurse(bool signalSta);

protected:
    void timerEvent(QTimerEvent *ev);

private:
    NurseCallManager();
    ~NurseCallManager();

    NurseCallManagerPrivate * const d_ptr;
};
