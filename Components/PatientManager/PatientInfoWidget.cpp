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
#include "FontManagerInterface.h"
#include <QBoxLayout>
#include <QLabel>
#include "IConfig.h"
#include "WindowManagerInterface.h"
#include "PatientInfoWindowInterface.h"

void PatientInfoWidget::loadPatientInfo(const PatientInfo &info, const QString &bed)
{
    QString nameStr = info.name;
    QString typeStr = trs(PatientSymbol::convert(info.type));
    if (nameStr.length() > 8)
    {
        nameStr = nameStr.left(8);
        nameStr += "...";
    }
    _patientName->setText(nameStr);
    _patientType->setText(typeStr);
    _bed->setText(bed);
}

void PatientInfoWidget::getText(QString &bed, QString &name, QString &type)
{
    bed = _bed->text();
    name = _patientName->text();
    type = _patientType->text();
}

void PatientInfoWidget::_releaseHandle(IWidget *iWidget)
{
    if (iWidget == NULL)
    {
        return;
    }

    WindowManagerInterface *windowManager = WindowManagerInterface::getWindowManager();
    PatientInfoWindowInterface *patientInfoWindow = PatientInfoWindowInterface::getPatientInfoWindow();
    if (windowManager && patientInfoWindow)
    {
        windowManager->showWindow(patientInfoWindow,
                                 WindowManagerInterface::ShowBehaviorCloseIfVisiable |
                                 WindowManagerInterface::ShowBehaviorCloseOthers);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientInfoWidget::PatientInfoWidget(QWidget *parent) : PatientInfoWidgetInterface(parent)
{
    FontMangerInterface *fontManager = FontMangerInterface::getFontManager();
    if (!fontManager)
    {
        return;
    }
    int fontSize = fontManager->getFontSize(4);

    _bed = new QLabel(this);
    _bed->setFont(fontManager->textFont(fontSize));
    _bed->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    _patientName = new QLabel(this);
    _patientName->setFont(fontManager->textFont(fontSize));
    _patientName->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    _patientType = new QLabel(this);
    _patientType->setFont(fontManager->textFont(fontSize));
    _patientType->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

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
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientInfoWidget::~PatientInfoWidget()
{
}
