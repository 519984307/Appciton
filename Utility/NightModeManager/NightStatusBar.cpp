/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/20
 **/

#include "NightStatusBar.h"
#include <QLayout>
#include <QLabel>
#include <QMap>
#include "IConfig.h"

NightStatusBar *NightStatusBar::_selfObj = NULL;

class NightStatusBarPrivate
{
public:
    NightStatusBarPrivate()
        : icon(NULL)
    {}
    ~NightStatusBarPrivate(){}

    QMap<nightMode, QPixmap> icons;
    QLabel *icon;
};

NightStatusBar::~NightStatusBar()
{
    delete d_ptr;
}

void NightStatusBar::changeIcon(nightMode status)
{
    d_ptr->icon->setPixmap(d_ptr->icons[status]);
}

NightStatusBar::NightStatusBar()
    : IWidget("NightStatusBar"), d_ptr(new NightStatusBarPrivate)
{
    d_ptr->icons[NIGHT_MODE_ON] = QPixmap("/usr/local/nPM/icons/moon.png");
    d_ptr->icons[NIGHT_MODE_OFF] = QPixmap("");
    d_ptr->icons[NIGHT_MODE_NR] = QPixmap();
    setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    IWidget *w = new IWidget();
    w->setAttribute(Qt::WA_NoSystemBackground);
    w->setFocusPolicy(Qt::NoFocus);
    QHBoxLayout *layout = new QHBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->icon = new QLabel();
    int index = 0;
    systemConfig.getNumValue("NightMode|EnterNightMode", index);
    nightMode nightModeIndex = static_cast<nightMode>(index);
    d_ptr->icon->setPixmap(d_ptr->icons[nightModeIndex]);
    layout->addWidget(d_ptr->icon, 0, Qt::AlignCenter);
    mainLayout->addWidget(w);
}
