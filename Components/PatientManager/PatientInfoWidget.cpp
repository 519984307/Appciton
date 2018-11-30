/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/29
 **/


#include "PatientInfoWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include <QBoxLayout>
#include "PatientManager.h"
#include <QLabel>
#include "IConfig.h"
#include "WindowManager.h"
#include "PatientInfoWindow.h"

void PatientInfoWidget::loadPatientInfo()
{
    QString nameStr = patientManager.getName();
    QString typeStr = trs(patientManager.getTypeStr());
    if (nameStr.length() > 8)
    {
        nameStr = nameStr.left(8);
        nameStr += "...";
    }
    _patientName->setText(nameStr);
    _patientType->setText(typeStr);
    _bed->setText(patientManager.getBedNum());
}

void PatientInfoWidget::_releaseHandle(IWidget *iWidget)
{
    if (iWidget == NULL)
    {
        return;
    }
    windowManager.showWindow(&patientInfoWindow,
                             WindowManager::ShowBehaviorCloseIfVisiable|
                             WindowManager::ShowBehaviorCloseOthers);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientInfoWidget::PatientInfoWidget(QWidget *parent) : IWidget("PatientInfoWidget", parent)
{
    int fontSize = fontManager.getFontSize(4);

    _bed = new QLabel(this);
    _bed->setFont(fontManager.textFont(fontSize));
    _bed->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _bed->setText(patientManager.getBedNum());

    _patientName = new QLabel(this);
    _patientName->setFont(fontManager.textFont(fontSize));
    _patientName->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QString nameStr = patientManager.getName();
    _patientName->setText(nameStr);


    _patientType = new QLabel(this);
    _patientType->setFont(fontManager.textFont(fontSize));
    _patientType->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QString typeStr = trs(patientManager.getTypeStr());
    _patientType->setText(typeStr);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(5);
    hLayout->addStretch();
    hLayout->addWidget(_bed);
    hLayout->addStretch();
    hLayout->addWidget(_patientName);
    hLayout->addStretch();
    hLayout->addWidget(_patientType);
    hLayout->addStretch();
    setLayout(hLayout);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    loadPatientInfo();

    setFixedWidth(260);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientInfoWidget::~PatientInfoWidget()
{
}
