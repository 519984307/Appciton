/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/15
 **/

#include "PatientInfoWindowInterface.h"
#include <stddef.h>

static PatientInfoWindowInterface *patientInfoWindow = NULL;

PatientInfoWindowInterface *PatientInfoWindowInterface::registerPatientInfoWindow(PatientInfoWindowInterface *instance)
{
    PatientInfoWindowInterface *old = patientInfoWindow;
    patientInfoWindow = instance;
    return old;
}

PatientInfoWindowInterface *PatientInfoWindowInterface::getPatientInfoWindow()
{
    return patientInfoWindow;
}
