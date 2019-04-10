/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/


#include "ServiceCPUTemp.h"
#include <QVBoxLayout>
#include "SystemBoardProvider.h"
#include "SystemManager.h"
#include <QTextTable>
#include <QScrollBar>
#include "Debug.h"
#include "ParamManager.h"
#include "MenuManager.h"
#include "FontManager.h"
#include "LanguageManager.h"

ServiceCPUTemp *ServiceCPUTemp::_selfObj = NULL;

/**************************************************************************************************
 * 功能：初始化。
 *************************************************************************************************/
void ServiceCPUTemp::init()
{
   _coreVersion();
   _Timer->start(1 * 1000);
}

/**************************************************************************************************
 * 返回。
 *************************************************************************************************/
void ServiceCPUTemp::_returnBtnReleased()
{
    _Timer->stop();
    menuManager.returnPrevious();
}

void ServiceCPUTemp::_TimeOut()
{
    _coreVersion();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceCPUTemp::ServiceCPUTemp() : MenuWidget(trs("ServiceCPUTemp"))
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setFixedSize(submenuW, submenuH);

    int fontSize = fontManager.getFontSize(1);
    int btnWidth = 500;

    _CORE = new LabeledLabel(trs("TEMP") + ":  ", "");
    _CORE->setFont(fontManager.textFont(fontSize));
    _CORE->setFixedSize(btnWidth, ITEM_H);

    _btnReturn = new LButtonEn();
    _btnReturn->setText(trs("VersionReturn"));
    _btnReturn->setFont(fontManager.textFont(fontSize));
    _btnReturn->setFixedSize(150, ITEM_H);
    connect(_btnReturn, SIGNAL(realReleased()), this, SLOT(_returnBtnReleased()));

    QVBoxLayout *labelLayout = new QVBoxLayout();
    labelLayout->setContentsMargins(50, 0, 50, 20);
    labelLayout->setSpacing(2);
    labelLayout->setAlignment(Qt::AlignTop);
    labelLayout->addWidget(_CORE, 0, Qt::AlignCenter);
    labelLayout->addStretch();
    labelLayout->addWidget(_btnReturn, 0, Qt::AlignRight);
//    labelLayout->addStretch();

    mainLayout->addLayout(labelLayout);

    _Timer = new QTimer(this);
    connect(_Timer, SIGNAL(timeout()), this, SLOT(_TimeOut()));
}

void ServiceCPUTemp::_coreVersion(void)
{
    QFile file("/sys/class/thermal/thermal_zone1/temp");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QString fileStr = file.readAll();
    file.close();

    int temp = fileStr.toInt();

    QString Str;
    Str = QString::number(temp / 1000.000, 'f', 3);
    Str += "\r\r";
    Str += trs(Unit::getSymbol(UNIT_TC));

    _CORE->setText(Str);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceCPUTemp::~ServiceCPUTemp()
{
}
