/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/27
 **/
#include "RelievePatientWindow.h"
#include <QHBoxLayout>
#include "Button.h"
#include "WindowManager.h"
#include "LanguageManager.h"
#include "DataStorageDirManager.h"
#include "PatientInfoWindow.h"
#include <QGridLayout>

class RelievePatientWindowPrivate
{
public:
    RelievePatientWindowPrivate();
    QLabel *standby;                 // 进入待机。
    Button *yes;                    // 确定按键
    Button *no;                    // 确定按键
};

RelievePatientWindowPrivate::RelievePatientWindowPrivate()
    : standby(NULL),
      yes(NULL),
      no(NULL)
{
}

RelievePatientWindow* RelievePatientWindow::getInstance()
{
    static RelievePatientWindow* instance = NULL;
    if (!instance)
    {
        instance = new RelievePatientWindow;
    }
    return instance;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RelievePatientWindow::~RelievePatientWindow()
{
    delete d_ptr;
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void RelievePatientWindow::layoutExec()
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
void RelievePatientWindow::showEvent(QShowEvent *e)
{
    Window::showEvent(e);
    QRect r = windowManager.getMenuArea();
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

/**************************************************************************************************
 * 重写X退出槽函数。
 *************************************************************************************************/
void RelievePatientWindow::exit()
{
    hide();
}

/***************************************************************************************************
 * 进入待机模式槽函数
 **************************************************************************************************/
void RelievePatientWindow::onYesReleased()
{
    hide();
    dataStorageDirManager.createDir(true);
}

/***************************************************************************************************
 * 不进入待机模式槽函数
 **************************************************************************************************/
void RelievePatientWindow::onNoReleased()
{
    hide();
    dataStorageDirManager.createDir(true);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RelievePatientWindow::RelievePatientWindow()
    : Window(),
      d_ptr(new RelievePatientWindowPrivate)
{
    layoutExec();
}

