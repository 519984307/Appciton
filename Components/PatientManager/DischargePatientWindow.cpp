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
#include "StandbyWindow.h"
#include "PatientManager.h"

class DischaregePatientWindowPrivate
{
public:
    DischaregePatientWindowPrivate();
    Button *standbyChk;                 // 进入待机。
    Button *yesBtn;                     // 确定按键
    Button *noBtn;                      // 确定按键
    bool isStandby;                     // 是否待机
};

DischaregePatientWindowPrivate::DischaregePatientWindowPrivate()
    : standbyChk(NULL),
      yesBtn(NULL),
      noBtn(NULL),
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
    QHBoxLayout *hlayout = new QHBoxLayout();

    d_ptr->standbyChk = new Button;
    d_ptr->standbyChk->setButtonStyle(Button::ButtonIconOnly);
    d_ptr->standbyChk->setIcon(QIcon("/usr/local/nPM/icons/checkbox_uncheck.png"));
    d_ptr->standbyChk->setIconSize(QSize(32, 32));
    d_ptr->standbyChk->setBorderWidth(0);
    d_ptr->standbyChk->setFixedSize(32, 32);

    QLabel *lbl = new QLabel;
    lbl->setText(trs("Standby"));
    lbl->setFixedWidth(100);
    lbl->setAlignment(Qt::AlignCenter);
    hlayout->addStretch();
    hlayout->addWidget(d_ptr->standbyChk);
    hlayout->addWidget(lbl);
    hlayout->addStretch();
    layout->addLayout(hlayout);

    connect(d_ptr->standbyChk, SIGNAL(released()), this, SLOT(onBtnRelease()));

    d_ptr->yesBtn = new Button(trs("Yes"));
    d_ptr->yesBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->yesBtn->setFixedWidth(130);
    connect(d_ptr->yesBtn, SIGNAL(released()), this, SLOT(onBtnRelease()));
    d_ptr->noBtn = new Button(trs("No"));
    d_ptr->noBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->noBtn->setFixedWidth(130);
    connect(d_ptr->noBtn, SIGNAL(released()), this, SLOT(onBtnRelease()));


    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(d_ptr->yesBtn);
    hlayout->addWidget(d_ptr->noBtn);
    hlayout->addStretch();

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
    if (btn == d_ptr->standbyChk)
    {
        d_ptr->isStandby = !d_ptr->isStandby;
        if (d_ptr->isStandby)
        {
            d_ptr->standbyChk->setIcon(QIcon("/usr/local/nPM/icons/checkbox_check.png"));
        }
        else
        {
            d_ptr->standbyChk->setIcon(QIcon("/usr/local/nPM/icons/checkbox_uncheck.png"));
        }
    }
    else if (btn == d_ptr->yesBtn)
    {
        this->accept();
        windowManager.closeAllWidows();
        if (d_ptr->isStandby)
        {
            StandbyWindow standyWin;
            standyWin.exec();
        }
    }
    else if (btn == d_ptr->noBtn)
    {
        this->reject();
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

