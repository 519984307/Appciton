/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/15
 **/

#pragma once
#include "Framework/UI/Dialog.h"

class PatientInfoWindowInterface : public Dialog
{
public:
    PatientInfoWindowInterface()
        : Dialog()
    {}

    virtual ~PatientInfoWindowInterface(){}

    static PatientInfoWindowInterface *registerPatientInfoWindow(PatientInfoWindowInterface *instance);

    static PatientInfoWindowInterface *getPatientInfoWindow(void);
};
