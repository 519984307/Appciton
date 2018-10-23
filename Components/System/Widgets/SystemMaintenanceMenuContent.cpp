/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/3
 **/
#include "SystemMaintenanceMenuContent.h"
#include <QMap>
#include "Button.h"
#include "LanguageManager.h"
#include <QGridLayout>
#include <QVariant>
#include "ConfigManagerWindow.h"
#include "FactoryMaintainWindow.h"
#include "UserMaintainWindow.h"
#include "SoftWareVersionWindow.h"
#include "TimeEditWindow.h"
#include "MonitorInfoWindow.h"
#include "SystemManager.h"
#include "DemoModeWindow.h"
#include "WindowManager.h"

class SystemMaintenanceMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_CONFIG_MANAGERMENT = 0,
        ITEM_BTN_USER_MAINTENANCE,
        ITEM_BTN_FACTORY_MAINTENANCE,
        ITEM_BTN_SYSTEM_TIME,
        ITEM_BTN_MONITOR_INFO,
    };

    SystemMaintenanceMenuContentPrivate() {}

    QMap<MenuItem, Button*> btns;
};

SystemMaintenanceMenuContent::SystemMaintenanceMenuContent()
                 : MenuContent(trs("SystemMaintenance"),
                               trs("SystemMaintenanceDesc")),
                   d_ptr(new SystemMaintenanceMenuContentPrivate)
{
}

SystemMaintenanceMenuContent::~SystemMaintenanceMenuContent()
{
    delete d_ptr;
}

void SystemMaintenanceMenuContent::layoutExec()
{
    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);

    Button *btn;
    int row = 0;
    int itemBtn = 0;
    int cloumn = 1;
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(cloumn, 1);

    // config mananger
    btn = new Button(trs("ConfigManager"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, cloumn);
    d_ptr->btns.insert(SystemMaintenanceMenuContentPrivate
                       ::ITEM_BTN_CONFIG_MANAGERMENT, btn);
    row++;

    // user maintain
    btn = new Button(trs("UserMaintainSystem"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, cloumn);
    d_ptr->btns.insert(SystemMaintenanceMenuContentPrivate
                       ::ITEM_BTN_USER_MAINTENANCE, btn);
    row++;

    // factory maintain
    btn = new Button(trs("FactoryMaintainSystem"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, cloumn);
    d_ptr->btns.insert(SystemMaintenanceMenuContentPrivate
                       ::ITEM_BTN_FACTORY_MAINTENANCE, btn);

    row++;

    // system time
    btn = new Button(trs("SupervisorTimeAndDataMenu"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, cloumn);
    d_ptr->btns.insert(SystemMaintenanceMenuContentPrivate
                       ::ITEM_BTN_SYSTEM_TIME, btn);
    row++;

   // monitor info
    btn = new Button(trs("MonitorInfoMenu"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, cloumn);
    d_ptr->btns.insert(SystemMaintenanceMenuContentPrivate
                       ::ITEM_BTN_MONITOR_INFO, btn);
    row++;

    glayout->setRowStretch(row, 1);
}

void SystemMaintenanceMenuContent::readyShow()
{
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        d_ptr->btns[SystemMaintenanceMenuContentPrivate::ITEM_BTN_FACTORY_MAINTENANCE]->setEnabled(false);
        d_ptr->btns[SystemMaintenanceMenuContentPrivate::ITEM_BTN_USER_MAINTENANCE]->setEnabled(false);
    }
    else
    {
        d_ptr->btns[SystemMaintenanceMenuContentPrivate::ITEM_BTN_FACTORY_MAINTENANCE]->setEnabled(true);
        d_ptr->btns[SystemMaintenanceMenuContentPrivate::ITEM_BTN_USER_MAINTENANCE]->setEnabled(true);
    }
}

void SystemMaintenanceMenuContent::onBtnReleased()
{
    Button *btn = qobject_cast<Button*>(sender());
    int indexType = btn->property("Item").toInt();

    switch (indexType) {
        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_CONFIG_MANAGERMENT:
        {
            ConfigManagerWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;
        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_USER_MAINTENANCE:
        {
            UserMaintainWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;

        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_FACTORY_MAINTENANCE:
        {
            FactoryMaintainWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;
       case SystemMaintenanceMenuContentPrivate::ITEM_BTN_SYSTEM_TIME:
        {
            TimeEditWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;

        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_MONITOR_INFO:
        {
            MonitorInfoWindow w;
            windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
        }
        break;
    }
}
