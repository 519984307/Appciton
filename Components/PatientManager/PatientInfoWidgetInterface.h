/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/14
 **/

#pragma once
#include "IWidget.h"

class PatientInfo;
class PatientInfoWidgetInterface : public IWidget
{
public:
    explicit PatientInfoWidgetInterface(QWidget *parent = NULL)
        : IWidget("PatientInfoWidget", parent)
    {}
    virtual ~PatientInfoWidgetInterface(){}

    virtual void loadPatientInfo(const PatientInfo &info, const QString &bed) = 0;

    /**
     * @brief setAlarmPause show alarm pause time on patient widget
     * @param seconds
     */
    virtual void setAlarmPauseTime(int seconds) = 0;
};
