/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/12
 **/


#pragma once
#include <QLabel>
#include "IWidget.h"
#include "PatientManager.h"

class PatientSelectWidget;
class PatientBarWidget : public IWidget
{
    Q_OBJECT

public:
    explicit PatientBarWidget(QWidget *parent = 0);
    ~PatientBarWidget();

    // 设置病人类型。
    void setPatientType(PatientType newType);

    // 设置病人信息。
    void setPatientInfo(const PatientInfo &info);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_bedtype;
    QLabel *_bedNum;
    QLabel *_patientName;
    QLabel *_patientTypeLabel;
    IWidget *_gain;         // 增益标签
    PatientSelectWidget *_patientTypeSelWidget;
};
