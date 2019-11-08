/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/29
 **/


#include "PatientInfoWidget.h"
#include "LanguageManager.h"
#include "FontManagerInterface.h"
#include <QBoxLayout>
#include <QLabel>
#include "IConfig.h"
#include "WindowManagerInterface.h"
#include "PatientInfoWindowInterface.h"
#include "PatientManager.h"
#include "SoundManagerInterface.h"

void PatientInfoWidget::loadPatientInfo(const PatientInfo &info, const QString &bed)
{
    QString nameStr = info.name;
    QString typeStr = trs(PatientSymbol::convert(info.type));
    if (nameStr.length() > 8)
    {
        nameStr = nameStr.left(8);
        nameStr += "...";
    }
    _patientName->setText(nameStr);
    _patientType->setText(typeStr);
    _bed->setText(bed);
}

void PatientInfoWidget::setAlarmPauseTime(int seconds)
{
    if (seconds > 0)
    {
        _stack->setCurrentIndex(1);
        QString s;
        if (seconds == INT_MAX)
        {
            s = QString(trs("AlarmOff"));
        }
        else
        {
            s = QString("%1 %2 : %3").arg(trs("AlarmPause"))
                                    .arg(seconds / 60, 2, 10, QLatin1Char('0'))
                                    .arg(seconds % 60, 2, 10, QLatin1Char('0'));
        }

        FontMangerInterface *fontManager = FontMangerInterface::getFontManager();
        if (fontManager)
        {
            int fontSize = fontManager->getFontSize(4);
            _alarmPauseMessage->setFont(fontManager->textFont(fontSize));
        }
        _alarmPauseMessage->setText(s);
    }
    else
    {
        _stack->setCurrentIndex(0);
    }
}

void PatientInfoWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    QPainter painter(this);
    int border = focusedBorderWidth() - 1;
    QRect tempRect = rect().adjusted(border, border, -border, -border);
    if (_stack->currentIndex() != 0)
    {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush(QColor(0, 175, 219));
        painter.setPen(Qt::black);
        painter.drawRoundedRect(tempRect, 4, 4);
    }
}

void PatientInfoWidget::mousePressEvent(QMouseEvent *e)
{
    IWidget::mousePressEvent(e);
    // 触屏点击播放按键音
    SoundManagerInterface *sound = SoundManagerInterface::getSoundManager();
    if (sound)
    {
        sound->keyPressTone();
    }
}

void PatientInfoWidget::_releaseHandle(IWidget *iWidget)
{
    if (iWidget == NULL || _stack->currentIndex() == 1)
    {
        // 报警状态下，不可点击进入病人信息
        return;
    }

    WindowManagerInterface *windowManager = WindowManagerInterface::getWindowManager();
    PatientInfoWindowInterface *patientInfoWindow = PatientInfoWindowInterface::getPatientInfoWindow();
    if (windowManager && patientInfoWindow)
    {
        windowManager->showWindow(patientInfoWindow,
                                 WindowManagerInterface::ShowBehaviorCloseIfVisiable |
                                 WindowManagerInterface::ShowBehaviorCloseOthers);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientInfoWidget::PatientInfoWidget(QWidget *parent) : PatientInfoWidgetInterface(parent)
{
    FontMangerInterface *fontManager = FontMangerInterface::getFontManager();
    if (!fontManager)
    {
        return;
    }
    int fontSize = fontManager->getFontSize(4);

    _stack = new QStackedWidget();
    QWidget *w = new QWidget();

    _bed = new QLabel();
    _bed->setFont(fontManager->textFont(fontSize));
    _bed->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _bed->setText(patientManager.getBedNum());

    _patientName = new QLabel();
    _patientName->setFont(fontManager->textFont(fontSize));
    _patientName->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QString nameStr = patientManager.getName();
    _patientName->setText(nameStr);


    _patientType = new QLabel();
    _patientType->setFont(fontManager->textFont(fontSize));
    _patientType->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QString typeStr = trs(patientManager.getTypeStr());
    _patientType->setText(typeStr);


    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(5);
    hLayout->addStretch();
    hLayout->addWidget(_bed);
    hLayout->addStretch();
    hLayout->addWidget(_patientName);
    hLayout->addStretch();
    hLayout->addWidget(_patientType);
    hLayout->addStretch();
    w->setLayout(hLayout);
    _stack->addWidget(w);
    _alarmPauseMessage = new QLabel();
    QPalette pal = _alarmPauseMessage->palette();
    pal.setColor(QPalette::WindowText, Qt::black);
    _alarmPauseMessage->setPalette(pal);
    _alarmPauseMessage->setAlignment(Qt::AlignCenter);
    _stack->addWidget(_alarmPauseMessage);

    hLayout = new QHBoxLayout();
    hLayout->addWidget(_stack);
    hLayout->setMargin(0);
    setLayout(hLayout);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientInfoWidget::~PatientInfoWidget()
{
}
