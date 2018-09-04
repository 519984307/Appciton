/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/3
 **/
#include "SystemMaintenance.h"
#include "TSCalibrationWindow.h"
#include <QMap>
#include "Button.h"
#include "LanguageManager.h"
#include <QGridLayout>
#include <QVariant>
#include "WindowManager.h"
#include "ConfigManagerMenuContent.h"
#include "FactoryMaintainMenuContent.h"
#include "UserMaintainMenuContent.h"
#include "SupervisorTimeMenuContent.h"
#include "SoftWareVersionContent.h"
#include "MonitorInfoContent.h"
#include "SystemManager.h"

class SystemMaintenancePrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_CONFIG_MANAGERMENT = 0,
        ITEM_BTN_USER_MAINTENANCE,
        ITEM_BTN_FACTORY_MAINTENANCE,
        ITEM_BTN_TOUCH_CALIBRATION,
        ITEM_BTN_SYSTEM_TIME,
        ITEM_BTN_MONITOR_INFO,
        ITEM_BTN_SOFTWARE_VERSION,
        ITEM_BTN_DEMO_MODE,
    };

    SystemMaintenancePrivate() {}
    /**
     * @brief loadOptions
     */
    void loadOptions(void);

    QMap<MenuItem, Button*> btns;
};

SystemMaintenance::SystemMaintenance()
                 : MenuContent(trs("SystemMaintenance"),
                               trs("SystemMaintenanceDesc")),
                   d_ptr(new SystemMaintenancePrivate)
{
}

SystemMaintenance::~SystemMaintenance()
{
    delete d_ptr;
}

void SystemMaintenance::readyShow()
{
    d_ptr->loadOptions();
}

void SystemMaintenance::layoutExec()
{
    if (layout())
    {
        return;
    }

    int verticalSpace = 50;
    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);
    glayout->setRowMinimumHeight(0, verticalSpace);
    glayout->setVerticalSpacing(20);
    glayout->setHorizontalSpacing(20);

    Button *btn;
    int row = 1;
    int itemBtn = 0;
    int cloumnLast = 2;

    // config mananger
    btn = new Button(trs("ConfigManager"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, 0);
    d_ptr->btns.insert(SystemMaintenancePrivate
                       ::ITEM_BTN_CONFIG_MANAGERMENT, btn);

    // user maintain
    btn = new Button(trs("UserMaintainSystem"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, cloumnLast);
    d_ptr->btns.insert(SystemMaintenancePrivate
                       ::ITEM_BTN_USER_MAINTENANCE, btn);

    row++;

    // factory maintain
    btn = new Button(trs("FactoryMaintainSystem"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, 0);
    d_ptr->btns.insert(SystemMaintenancePrivate
                       ::ITEM_BTN_FACTORY_MAINTENANCE, btn);

    // touch calibration
    btn = new Button(trs("TouchScreenCalibration"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, cloumnLast);
    d_ptr->btns.insert(SystemMaintenancePrivate
                       ::ITEM_BTN_TOUCH_CALIBRATION, btn);

    row++;

    // system time
    btn = new Button(trs("SupervisorTimeAndDataMenu"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, 0);
    d_ptr->btns.insert(SystemMaintenancePrivate
                       ::ITEM_BTN_SYSTEM_TIME, btn);

    // monitor info
    btn = new Button(trs("MonitorInfoMenu"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, cloumnLast);
    d_ptr->btns.insert(SystemMaintenancePrivate
                       ::ITEM_BTN_MONITOR_INFO, btn);

    row++;

    // software version
    btn = new Button(trs("SystemSoftwareVersion"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, 0);
    d_ptr->btns.insert(SystemMaintenancePrivate
                       ::ITEM_BTN_SOFTWARE_VERSION, btn);

    // demo mode
    btn = new Button(trs("DemoMode"));
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        btn->setText(trs("ExitDemoMode"));
    }
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    glayout->addWidget(btn, row, cloumnLast);
    d_ptr->btns.insert(SystemMaintenancePrivate
                       ::ITEM_BTN_DEMO_MODE, btn);

    row++;

    glayout->setRowMinimumHeight(row, verticalSpace);
}

void SystemMaintenance::onBtnReleased()
{
    Button *btn = qobject_cast<Button*>(sender());
    int indexType = btn->property("Item").toInt();

    switch (indexType) {
        case SystemMaintenancePrivate::ITEM_BTN_CONFIG_MANAGERMENT:
        {
            ConfigManagerMenuContent w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;

        case SystemMaintenancePrivate::ITEM_BTN_USER_MAINTENANCE:
        {
            UserMaintainMenuContent w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;

        case SystemMaintenancePrivate::ITEM_BTN_FACTORY_MAINTENANCE:
        {
            FactoryMaintainMenuContent w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;

        case SystemMaintenancePrivate::ITEM_BTN_TOUCH_CALIBRATION:
        {

//            TSCalibrationWindow w;  // 待加入
//            w.exec();
        }
        break;

        case SystemMaintenancePrivate::ITEM_BTN_SYSTEM_TIME:
        {
            SupervisorTimeMenuContent w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;

        case SystemMaintenancePrivate::ITEM_BTN_MONITOR_INFO:
        {
            MonitorInfoContent w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;

        case SystemMaintenancePrivate::ITEM_BTN_SOFTWARE_VERSION:
        {
            SoftWareVersionContent w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;

        case SystemMaintenancePrivate::ITEM_BTN_DEMO_MODE:
        {
            if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
            {
                systemManager.setWorkMode(WORK_MODE_NORMAL);
                d_ptr->btns[SystemMaintenancePrivate
                        ::ITEM_BTN_DEMO_MODE]->setText(trs("DemoMode"));
            }
            else
            {
                systemManager.setWorkMode(WORK_MODE_DEMO);
                d_ptr->btns[SystemMaintenancePrivate
                        ::ITEM_BTN_DEMO_MODE]->setText(trs("ExitDemoMode"));
            }
            windowManager.closeAllWidows();
        }
        break;
    }
}

void SystemMaintenancePrivate::loadOptions()
{
}
