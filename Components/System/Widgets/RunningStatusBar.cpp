/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/20
 **/

#include "RunningStatusBar.h"
#include <QLayout>
#include <QLabel>
#include <QMap>
#include "IConfig.h"

enum RunningStatusType
{
    PACER_STATUS,
    NIGHT_MODE_STATUS,
    TOUCH_STATUS,
    SHAKE_OFF_STATUS,
    SHAKE_ON_STATUS,
    SHAKING_STATUS,
    RUNING_STATUS_NR,
};

static const char *iconPaths[RUNING_STATUS_NR] =
{
    "/usr/local/nPM/icons/PaceMarkerOff.png",
    "/usr/local/nPM/icons/moon.png",
    "/usr/local/nPM/icons/untouch.png",
    "/usr/local/nPM/icons/ShakeOff.png",
    "/usr/local/nPM/icons/ShakeOn.png",
    "/usr/local/nPM/icons/Shaking.png",
};


class RunningStatusBarPrivate
{
public:
    RunningStatusBarPrivate()
        : icons(RUNING_STATUS_NR, NULL),
          status(SHAKE_ON)
    {}
    ~RunningStatusBarPrivate() {}

    QVector<QLabel *> icons;
    ShakeStatus status;
};

RunningStatusBar &RunningStatusBar::getInstance()
{
    static RunningStatusBar *instance = NULL;
    if (instance == NULL)
    {
        instance = new RunningStatusBar();
        RunningStatusBarInterface *old = registerRunningStatusBar(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

RunningStatusBar::~RunningStatusBar()
{
    delete d_ptr;
}

void RunningStatusBar::setPacerStatus(bool onOff)
{
    if (onOff)
    {
        d_ptr->icons[PACER_STATUS]->setPixmap(QPixmap());
    }
    else
    {
        d_ptr->icons[PACER_STATUS]->setPixmap(QPixmap(iconPaths[PACER_STATUS]));
    }
}

void RunningStatusBar::setNightModeStatus(bool onOff)
{
    if (onOff)
    {
        d_ptr->icons[NIGHT_MODE_STATUS]->setPixmap(QPixmap(iconPaths[NIGHT_MODE_STATUS]));
    }
    else
    {
        d_ptr->icons[NIGHT_MODE_STATUS]->setPixmap(QPixmap());
    }
}

void RunningStatusBar::setTouchStatus(bool onOff)
{
    if (onOff)
    {
        d_ptr->icons[TOUCH_STATUS]->setPixmap(QPixmap());
    }
    else
    {
        d_ptr->icons[TOUCH_STATUS]->setPixmap(QPixmap(iconPaths[TOUCH_STATUS]));
    }
}

void RunningStatusBar::clearTouchStatus()
{
    d_ptr->icons[TOUCH_STATUS]->setPixmap(QPixmap());
}

void RunningStatusBar::setShakeStatus(ShakeStatus sta)
{
    QPixmap pix;
    d_ptr->status = sta;
    if (sta == SHAKE_OFF)
    {
        pix.load(iconPaths[SHAKE_OFF_STATUS]);
    }
    else if (sta == SHAKE_ON)
    {
        pix.load(iconPaths[SHAKE_ON_STATUS]);
    }
    else if (sta == SHAKING)
    {
        pix.load(iconPaths[SHAKING_STATUS]);
    }
    pix = pix.scaled(pix.width(), pix.height(), Qt::KeepAspectRatio);
    d_ptr->icons[SHAKING_STATUS]->setPixmap(pix);
}

ShakeStatus RunningStatusBar::getShakeStatus()
{
    return d_ptr->status;
}

RunningStatusBar::RunningStatusBar()
    : IWidget("RunningStatusBar"), d_ptr(new RunningStatusBarPrivate)
{
    setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    d_ptr->icons[PACER_STATUS] = new QLabel();
    mainLayout->addWidget(d_ptr->icons[PACER_STATUS], 1);
    d_ptr->icons[NIGHT_MODE_STATUS] = new QLabel();
    mainLayout->addWidget(d_ptr->icons[NIGHT_MODE_STATUS], 1);
    d_ptr->icons[TOUCH_STATUS] = new QLabel();
    mainLayout->addWidget(d_ptr->icons[TOUCH_STATUS], 1);
    d_ptr->icons[SHAKING_STATUS] = new QLabel();
    mainLayout->addWidget(d_ptr->icons[SHAKING_STATUS], 1);
}
