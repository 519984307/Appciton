/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/11
 **/

#include "StandbyWindow.h"
#include <QEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include "FontManager.h"
#include "WindowManager.h"
#include "Framework/Language/LanguageManager.h"
#include "IConfig.h"
#include "SystemManager.h"
#include "LayoutManager.h"

StandbyWindow::StandbyWindow()
            : QDialog(NULL, Qt::FramelessWindowHint)
{
    QPalette pal;
    pal.setColor(QPalette::Background, Qt::black);
    this->setPalette(pal);

    setFixedSize(windowManager.size());
    this->move(windowManager.geometry().x(),
               windowManager.geometry().y());
}

StandbyWindow::~StandbyWindow()
{
    layoutManager.resetWave();
}


void StandbyWindow::paintEvent(QPaintEvent *ev)
{
    QDialog::paintEvent(ev);

    // 画大字 standy
    int fontSize = 80;
    QRect textRect = rect().adjusted(0, 0, 0, 0);
    QPainter painter(this);
    painter.setPen(QPen(Qt::white, 3, Qt::SolidLine));
    painter.setFont(fontManager.textFont(fontSize));
    painter.drawText(textRect,  Qt::AlignCenter, "Standby");

    // 画提示语
    fontSize = 24;
    textRect = rect().adjusted(0, rect().bottom() - 160, 0, 0);
    painter.setPen(QPen(Qt::white, 2, Qt::SolidLine));
    painter.setFont(fontManager.textFont(fontSize));
    painter.drawText(textRect,  Qt::AlignCenter, trs("PressAnyKeysToExit"));
}

void StandbyWindow::keyReleaseEvent(QKeyEvent *ev)
{
    QDialog::keyReleaseEvent(ev);
    done(0);
}

void StandbyWindow::mouseReleaseEvent(QMouseEvent *ev)
{
    QDialog::mouseReleaseEvent(ev);
    done(0);
}

void StandbyWindow::hideEvent(QHideEvent *ev)
{
    QDialog::hideEvent(ev);
    systemManager.setWorkMode(WORK_MODE_NORMAL);
}
