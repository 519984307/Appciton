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
#include "SupervisorTimeWindow.h"
#include "UserMaintainWindow.h"
#include "SoftWareVersionWindow.h"
#include "MonitorInfoWindow.h"
#include "SystemManager.h"
#include "DemoModeWindow.h"
#include "WindowManager.h"
#ifdef Q_WS_QWS
#include "TSCalibrationWindow.h"
#include <QWSServer>
#endif

class SystemMaintenanceMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_CONFIG_MANAGERMENT = 0,
#ifdef Q_WS_QWS
        ITEM_BTN_TOUCH_SCREEN_CALIBRATION,
#endif
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

#ifdef Q_WS_QWS
    // touch screen calibration
    btn = new Button(trs("TouchScreenCalibration"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    btn->setProperty("Item", qVariantFromValue(itemBtn));
    itemBtn++;
    glayout->addWidget(btn, row, cloumn);
    d_ptr->btns.insert(SystemMaintenanceMenuContentPrivate
                       ::ITEM_BTN_USER_MAINTENANCE, btn);
    row++;
    if (!systemManager.isSupport(CONFIG_TOUCH))
    {
        // touch screen is not support
        btn->setEnabled(false);
    }
#endif

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

void SystemMaintenanceMenuContent::onBtnReleased()
{
    Button *btn = qobject_cast<Button*>(sender());
    int indexType = btn->property("Item").toInt();

    switch (indexType) {
        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_CONFIG_MANAGERMENT:
        {
            ConfigManagerWindow w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;
#ifdef Q_WS_QWS
        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_TOUCH_SCREEN_CALIBRATION:
        {
            windowManager.closeAllWidows();
            if (systemManager.isTouchScreenOn())
            {
                QWSServer::instance()->closeMouse();
            }

            TSCalibrationWindow w;
            w.exec();

            if (systemManager.isTouchScreenOn())
            {
                QWSServer::instance()->openMouse();
            }
        }
        break;
#endif
        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_USER_MAINTENANCE:
        {
            UserMaintainWindow w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;

        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_FACTORY_MAINTENANCE:
        {
            FactoryMaintainWindow w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;
       case SystemMaintenanceMenuContentPrivate::ITEM_BTN_SYSTEM_TIME:
        {
            SupervisorTimeWindow w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;

        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_MONITOR_INFO:
        {
            MonitorInfoWindow w;
            windowManager.showWindow(&w,
                                     WindowManager::ShowBehaviorModal);
        }
        break;

//        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_SOFTWARE_VERSION:
//        {
//            SoftWareVersionWindow w;
//            windowManager.showWindow(&w,
//                                     WindowManager::ShowBehaviorModal);
//        }
//        break;

//        case SystemMaintenanceMenuContentPrivate::ITEM_BTN_DEMO_MODE:
//        {
//            if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
//            {
//                systemManager.setWorkMode(WORK_MODE_NORMAL);
//                d_ptr->btns[SystemMaintenanceMenuContentPrivate
//                        ::ITEM_BTN_DEMO_MODE]->setText(trs("DemoMode"));
//                windowManager.closeAllWidows();
//                break;
//            }

//            DemoModeWindow w;
//            windowManager.showWindow(&w,
//                                     WindowManager::ShowBehaviorModal);
//            if (!w.isUserInputCorrect())
//            {
//                break;
//            }

//            systemManager.setWorkMode(WORK_MODE_DEMO);
//            d_ptr->btns[SystemMaintenanceMenuContentPrivate
//                    ::ITEM_BTN_DEMO_MODE]->setText(trs("ExitDemoMode"));

//            windowManager.closeAllWidows();
//        }
//        break;
    }
}
