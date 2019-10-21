/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/31
 **/


#include "DateTimeWidget.h"
#include "FontManager.h"
#include <QHBoxLayout>
#include <QLabel>
#include "WindowManager.h"
#include "TimeEditWindow.h"
#include "SoundManagerInterface.h"

/**************************************************************************************************
 * 功能： 设置显示的内容。
 * 参数：
 *      str：显示的内容。
 *************************************************************************************************/
void DateTimeWidget::setText(const QString &str)
{
    _datetimeLabel->setText(str);
}

void DateTimeWidget::mousePressEvent(QMouseEvent *e)
{
    IWidget::mousePressEvent(e);
    // 触屏点击播放按键音
    SoundManagerInterface *sound = SoundManagerInterface::getSoundManager();
    if (sound)
    {
        sound->keyPressTone();
    }
}

void DateTimeWidget::_release(IWidget *iWidget)
{
    if (iWidget == NULL)
    {
        return;
    }
    TimeEditWindow w;
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
DateTimeWidget::DateTimeWidget(QWidget *parent)
    : IWidget("DateTimeWidget", parent)
{
    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::lightGray);

    _datetimeLabel = new QLabel("", this);
    _datetimeLabel->setFont(fontManager.textFont(fontManager.getFontSize(3)));
    _datetimeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _datetimeLabel->setPalette(palette);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(_datetimeLabel);
    hLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(hLayout);

    connect(this , SIGNAL(released(IWidget*)), this , SLOT(_release(IWidget*)));
}
