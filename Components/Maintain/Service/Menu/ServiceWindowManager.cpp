/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/12
 **/

#include "ServiceWindowManager.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QBitmap>
#include <QScrollArea>
#include <QScrollBar>
#include "ColorManager.h"
#include "SubMenu.h"
#include "Debug.h"
#include "LabelButton.h"
#include "MenuManager.h"
#include "NIBPRepairMenuManager.h"
#include "NIBPCalibrate.h"
#include "NIBPManometer.h"
#include "NIBPZeroPoint.h"
#include "NIBPPressureControl.h"
#include "ServiceCPUTemp.h"
#include "ServiceErrorLogMenu.h"

ServiceWindowManager *ServiceWindowManager::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceWindowManager::ServiceWindowManager() : MenuWidget(trs("ServiceSystem"))
{
    int _submenuWidth = menuManager.getSubmenuWidth();
    int _submenuHeight = menuManager.getSubmenuHeight();
    setFixedSize(_submenuWidth, _submenuHeight);

    int fontSize = fontManager.getFontSize(3);
    //定义按钮，并以序号命名，方便后面使用
    _nibpButton = new IButton(trs("NIBP"));
    _nibpButton->setFont(fontManager.textFont(fontSize));
    _nibpButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_nibpButton, SIGNAL(realReleased()), this,
            SLOT(_nibpButtonSlot()));

    _upgradeButton = new IButton(trs("ServiceUpgrade"));
    _upgradeButton->setFont(fontManager.textFont(fontSize));
    _upgradeButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_upgradeButton, SIGNAL(realReleased()), this,
            SLOT(_upgradeButtonSlot()));

    _versionButton = new IButton(trs("ServiceVersion"));
    _versionButton->setFont(fontManager.textFont(fontSize));
    _versionButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_versionButton, SIGNAL(realReleased()), this,
            SLOT(_versionButtonSlot()));

    _tempButton = new IButton(trs("CPUTemp"));
    _tempButton->setFont(fontManager.textFont(fontSize));
    _tempButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_tempButton, SIGNAL(realReleased()), this,
            SLOT(_tempButtonSlot()));

    _errorLogButton = new IButton(trs("ErrorLog"));
    _errorLogButton->setFont(fontManager.textFont(fontSize));
    _errorLogButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_errorLogButton, SIGNAL(realReleased()), this,
            SLOT(_errorLogButtonSlot()));

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setMargin(10);
    vLayout->setSpacing(20);

    vLayout->addStretch();
    vLayout->addWidget(_nibpButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_upgradeButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_versionButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_tempButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_errorLogButton, 0, Qt::AlignCenter);
    vLayout->addStretch();

    mainLayout->addLayout(vLayout);

    nibpRepairMenuManager.Construation();
    nibpcalibrate.construction();
    nibpRepairMenuManager.addSubMenu(&nibpcalibrate);
    nibpmanometer.construction();
    nibpRepairMenuManager.addSubMenu(&nibpmanometer);
    nibpzeropoint.construction();
    nibpRepairMenuManager.addSubMenu(&nibpzeropoint);
    nibppressurecontrol.construction();
    nibpRepairMenuManager.addSubMenu(&nibppressurecontrol);

//    serviceUpgrade.Construation();
//    serviceVersion.Construation();
//    serviceCPUTemp.Construation();
//    serviceErrorLogMenu.construction();
}

/**************************************************************************************************
 * 功能：初始化。
 *************************************************************************************************/
void ServiceWindowManager::init()
{
    // 增加一个列表项。
    _nibpButton->setFocus();
}

void ServiceWindowManager::showEvent(QShowEvent *e)
{
    _nibpButton->setFocus();
    QWidget::showEvent(e);
}

void ServiceWindowManager::_nibpButtonSlot()
{
    nibpRepairMenuManager.popup();
    nibpRepairMenuManager.init();
}

void ServiceWindowManager::_upgradeButtonSlot()
{
}

void ServiceWindowManager::_versionButtonSlot()
{
}

void ServiceWindowManager::_tempButtonSlot()
{
    serviceCPUTemp.popup();
    serviceCPUTemp.init();
}

void ServiceWindowManager::_errorLogButtonSlot()
{
    serviceErrorLogMenu.popup();
    serviceErrorLogMenu.init();
}

/**************************************************************************************************
 * 重新聚焦菜单列表。
 *************************************************************************************************/
void ServiceWindowManager::_returnMenuList(SubMenu *m)
{
    Q_UNUSED(m)
}

/**************************************************************************************************
 * 需要更新标题。
 *************************************************************************************************/
void ServiceWindowManager::_titleChanged(void)
{
}

/**************************************************************************************************
 * 子菜单页聚焦
 *************************************************************************************************/
void ServiceWindowManager::_itemClicked()
{
    SubMenu *m = qobject_cast<SubMenu *>(_scorllArea->widget());
    m->focusFirstItem();
}

/**************************************************************************************************
 * 改变滚动区域滚动条的值
 *************************************************************************************************/
void ServiceWindowManager::_changeScrollValue(int value)
{
    QScrollBar *bar = _scorllArea->verticalScrollBar();
    if (NULL != bar)
    {
        if (1 == value)
        {
            bar->setValue(bar->maximum());
        }
        else
        {
            bar->setValue(0);
        }
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceWindowManager::~ServiceWindowManager()
{
}
