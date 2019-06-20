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
#include "FontManager.h"
#include <QBoxLayout>
#include "PatientManager.h"
#include <QLabel>
#include "IConfig.h"
#include "WindowManager.h"
#include "PatientInfoWindow.h"

void PatientInfoWidget::loadPatientInfo()
{
    QString nameStr = patientManager.getName();
    QString typeStr = trs(patientManager.getTypeStr());
    if (nameStr.length() > 8)
    {
        nameStr = nameStr.left(8);
        nameStr += "...";
    }
    _patientName->setText(nameStr);
    _patientType->setText(typeStr);
    _bed->setText(patientManager.getBedNum());
}

void PatientInfoWidget::setAlarmPause(int seconds)
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
            s = QString("%1 %2s").arg(trs("AlarmPause")).arg(seconds);
        }

        int fontSize = fontManager.getFontSize(4);
        _alarmPauseMessage->setFont(fontManager.textFont(fontSize));
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

void PatientInfoWidget::_releaseHandle(IWidget *iWidget)
{
    if (iWidget == NULL)
    {
        return;
    }
    windowManager.showWindow(&patientInfoWindow,
                             WindowManager::ShowBehaviorCloseIfVisiable|
                             WindowManager::ShowBehaviorCloseOthers);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientInfoWidget::PatientInfoWidget(QWidget *parent) : IWidget("PatientInfoWidget", parent)
{
    int fontSize = fontManager.getFontSize(4);

    _stack = new QStackedWidget();
    QWidget *w = new QWidget();

    _bed = new QLabel();
    _bed->setFont(fontManager.textFont(fontSize));
    _bed->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _bed->setText(patientManager.getBedNum());

    _patientName = new QLabel();
    _patientName->setFont(fontManager.textFont(fontSize));
    _patientName->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QString nameStr = patientManager.getName();
    _patientName->setText(nameStr);


    _patientType = new QLabel();
    _patientType->setFont(fontManager.textFont(fontSize));
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

    loadPatientInfo();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientInfoWidget::~PatientInfoWidget()
{
}
