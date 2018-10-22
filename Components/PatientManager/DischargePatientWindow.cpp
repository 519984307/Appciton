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
#include "PatientManager.h"

class DischaregePatientWindowPrivate
{
public:
    DischaregePatientWindowPrivate();
    Button *standby;                 // 进入待机。
    Button *yes;                     // 确定按键
    Button *no;                      // 确定按键
    bool isStandby;                  // 是否待机
};

DischaregePatientWindowPrivate::DischaregePatientWindowPrivate()
    : standby(NULL),
      yes(NULL),
      no(NULL),
      isStandby(false)
{
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
    setFixedSize(300, 180);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(10);


    d_ptr->standby = new Button(trs("Standby"));
    d_ptr->standby->setButtonStyle(Button::ButtonTextBesideIcon);
    d_ptr->standby->setIcon(QIcon("/usr/local/nPM/icons/checkbox_uncheck.png"));
    d_ptr->standby->setBorderWidth(0);
    d_ptr->standby->setFixedWidth(280);
    connect(d_ptr->standby, SIGNAL(released()), this, SLOT(onBtnRelease()));

    d_ptr->yes = new Button(trs("Yes"));
    d_ptr->yes->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->yes->setFixedWidth(100);
    connect(d_ptr->yes, SIGNAL(released()), this, SLOT(onBtnRelease()));
    d_ptr->no = new Button(trs("No"));
    d_ptr->no->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->no->setFixedWidth(100);
    connect(d_ptr->no, SIGNAL(released()), this, SLOT(onBtnRelease()));


    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(d_ptr->yes);
    hlayout->addWidget(d_ptr->no);
    hlayout->addStretch();

    layout->addWidget(d_ptr->standby, 0, Qt::AlignHCenter);
    layout->addStretch();
    layout->addLayout(hlayout);

    setWindowLayout(layout);
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

void DischargePatientWindow::onBtnRelease()
{
    Button *btn = qobject_cast<Button*>(sender());
    if (btn == d_ptr->standby)
    {
        d_ptr->isStandby = !d_ptr->isStandby;
        if (d_ptr->isStandby)
        {
            d_ptr->standby->setIcon(QIcon("/usr/local/nPM/icons/checkbox_check.png"));
        }
        else
        {
            d_ptr->standby->setIcon(QIcon("/usr/local/nPM/icons/checkbox_uncheck.png"));
        }
    }
    else if (btn == d_ptr->yes)
    {
        windowManager.closeAllWidows();
        if (patientManager.isWarding())
        {
            patientManager.setWard(false);
            patientManager.newPatient();
        }
        else
        {
            // TODO 清除当前病人历史数据
        }
        if (d_ptr->isStandby)
        {
            StandyWindow standyWin;
            standyWin.exec();
        }
    }
    else if (btn == d_ptr->no)
    {
        if (d_ptr->isStandby)
        {
            StandyWindow standyWin;
            standyWin.exec();
        }
        this->close();
    }
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

