/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/11
 **/

#include "StandyWindow.h"
#include <QEvent>
#include <QLabel>
#include <QRect>
#include <QPalette>
#include <QVBoxLayout>
#include <QPainter>
#include "FontManager.h"
#include <QPaintEvent>
#include "WindowManager.h"
#include "LanguageManager.h"
#include "AlarmIndicator.h"
#include "Alarm.h"
#include "IConfig.h"

class StandyWindowPrivate
{
public:
    StandyWindowPrivate()
            : standyShow("Standby"),
              noticeShow(trs("PressAnyKeysToExit"))
    {}
    QString standyShow;
    QString noticeShow;
};

StandyWindow::StandyWindow()
            : QDialog(NULL, Qt::FramelessWindowHint),
              d_ptr(new StandyWindowPrivate)
{
    QPalette pal;
    pal.setColor(QPalette::Background, Qt::black);
    this->setPalette(pal);

    setFixedSize(windowManager.size());
    this->move(windowManager.geometry().x(),
               windowManager.geometry().y());

    alertor.updateMuteKeyStatus(true);
    alertor.updateMuteKeyStatus(false);
    alertor.updateStandbyMode(true);
}

StandyWindow::~StandyWindow()
{
    delete d_ptr;
}


void StandyWindow::paintEvent(QPaintEvent *ev)
{
    QDialog::paintEvent(ev);

    // 画大字 standy
    int fontSize = 80;
    QRect textRect = rect().adjusted(0, 0, 0, 0);
    QPainter painter(this);
    painter.setPen(QPen(Qt::white, 3, Qt::SolidLine));
    painter.setFont(fontManager.textFont(fontSize));
    painter.drawText(textRect,  Qt::AlignCenter, d_ptr->standyShow);

    // 画提示语
    fontSize = 24;
    textRect = rect().adjusted(0, rect().bottom() - 160, 0, 0);
    painter.setPen(QPen(Qt::white, 2, Qt::SolidLine));
    painter.setFont(fontManager.textFont(fontSize));
    painter.drawText(textRect,  Qt::AlignCenter, d_ptr->noticeShow);
}

void StandyWindow::keyPressEvent(QKeyEvent *ev)
{
    QDialog::keyPressEvent(ev);
    alertor.updateMuteKeyStatus(true);
    alertor.updateMuteKeyStatus(false);
    alertor.updateStandbyMode(false);

    int boltLockIndex = 0;
    systemConfig.getNumValue("Alarms|PhyParAlarmLatchlockOn", boltLockIndex);
    alertor.setLatchLockSta(!!boltLockIndex);

    done(0);
}

void StandyWindow::mousePressEvent(QMouseEvent *ev)
{
    QDialog::mousePressEvent(ev);
    alertor.updateMuteKeyStatus(true);
    alertor.updateMuteKeyStatus(false);
    alertor.updateStandbyMode(false);

    int boltLockIndex = 0;
    systemConfig.getNumValue("Alarms|PhyParAlarmLatchlockOn", boltLockIndex);
    alertor.setLatchLockSta(!!boltLockIndex);

    done(0);
}
