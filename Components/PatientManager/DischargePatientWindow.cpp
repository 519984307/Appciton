/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/27
 **/
#include "DischargePatientWindow.h"
#include <QHBoxLayout>
#include "Button.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "DataStorageDirManager.h"
#include "PatientInfoWindow.h"
#include <QGridLayout>
#include "LayoutManager.h"
#include "StandyWindow.h"

class DischaregePatientWindowPrivate
{
public:
    DischaregePatientWindowPrivate();
    QLabel *standby;                 // 进入待机。
    Button *yes;                    // 确定按键
    Button *no;                    // 确定按键
};

DischaregePatientWindowPrivate::DischaregePatientWindowPrivate()
    : standby(NULL),
      yes(NULL),
      no(NULL)
{
}

DischargePatientWindow* DischargePatientWindow::getInstance()
{
    static DischargePatientWindow* instance = NULL;
    if (!instance)
    {
        instance = new DischargePatientWindow;
    }
    return instance;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
DischargePatientWindow::~DischargePatientWindow()
{
    delete d_ptr;
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void DischargePatientWindow::layoutExec()
{
    setWindowTitle(trs("RelievePatient"));

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    d_ptr->standby = new QLabel(trs("WhetherStandby"));
    d_ptr->yes = new Button(trs("Yes"));
    d_ptr->yes->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->no = new Button(trs("No"));
    d_ptr->no->setButtonStyle(Button::ButtonTextOnly);


    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(d_ptr->yes);
    hlayout->addWidget(d_ptr->no);

    layout->addWidget(d_ptr->standby, 0, 0);
    layout->addLayout(hlayout, 1, 0);

    layout->setRowStretch(2, 1);
    setWindowLayout(layout);

    connect(d_ptr->yes, SIGNAL(released()), this, SLOT(onYesReleased()));
    connect(d_ptr->no, SIGNAL(released()), this, SLOT(onNoReleased()));
}

/***************************************************************************************************
 * 显示事件
 **************************************************************************************************/
void DischargePatientWindow::showEvent(QShowEvent *e)
{
    Window::showEvent(e);
    QRect r = layoutManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

/**************************************************************************************************
 * 重写X退出槽函数。
 *************************************************************************************************/
void DischargePatientWindow::exit()
{
    emit exitFlag(false);
    hide();
}

/***************************************************************************************************
 * 进入待机模式槽函数
 **************************************************************************************************/
void DischargePatientWindow::onYesReleased()
{
    emit exitFlag(true);
    hide();
    dataStorageDirManager.createDir(true);
    StandyWindow win;
    win.exec();
}

/***************************************************************************************************
 * 不进入待机模式槽函数
 **************************************************************************************************/
void DischargePatientWindow::onNoReleased()
{
    emit exitFlag(false);
    hide();
    dataStorageDirManager.createDir(true);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
DischargePatientWindow::DischargePatientWindow()
    : Window(),
      d_ptr(new DischaregePatientWindowPrivate)
{
    layoutExec();
}

