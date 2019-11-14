/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/17
 **/
#include "PowerOffWindow.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/UI/ThemeManager.h"
#include <QLabel>
#include <QLayout>
#include "FontManager.h"
#include "WindowManager.h"

#define TITLE_BAR_HEIGHT 30

PowerOffWindow* PowerOffWindow::_selfObj = NULL;
class PowerOffWindowPrivate
{
public:
    explicit PowerOffWindowPrivate(PowerOffWindow *p)
        : q_ptr(p)
    {}
    ~PowerOffWindowPrivate(){}
    void layoutExec();
    PowerOffWindow *q_ptr;
};

PowerOffWindow::PowerOffWindow()
    : QDialog(NULL, Qt::FramelessWindowHint),
      d_ptr(new PowerOffWindowPrivate(this))
{
    this->setWindowModality(Qt::ApplicationModal);  // 设置窗口模式为应用程序模态
    setFixedSize(300, 200);
    setFont(fontManager.textFont(themeManager.defaultFontPixSize()));
    QPalette pal = palette();
    themeManager.setupPalette(ThemeManager::ControlWindow, &pal);
    setPalette(pal);
    d_ptr->layoutExec();
}

PowerOffWindow::~PowerOffWindow()
{
    delete d_ptr;
}

int PowerOffWindow::exec()
{
    windowManager.closeAllWidows();
    return QDialog::exec();
}

void PowerOffWindowPrivate::layoutExec()
{
    QLabel *titleLbl = new QLabel();
    titleLbl->setFixedHeight(TITLE_BAR_HEIGHT);
    titleLbl->setText(trs("Prompt"));
    titleLbl->setAlignment(Qt::AlignCenter);

    QLabel *tip = new QLabel;
    tip->setText(trs("ShuttingDown"));
    tip->setAlignment(Qt::AlignCenter|Qt::AlignHCenter);
    tip->setFont(fontManager.textFont(themeManager.defaultFontPixSize()));

    QFrame *line = new QFrame;
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setLineWidth(0);
    line->setMidLineWidth(1);

    QVBoxLayout *layout = new QVBoxLayout(q_ptr);
    layout->addWidget(titleLbl);
    layout->addWidget(line);
    layout->addStretch();
    layout->addWidget(tip);
    layout->addStretch();
}
