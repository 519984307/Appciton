/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/6/10
 **/

#include "PITrendWidget.h"
#include <QLabel>
#include <QLayout>
#include "BaseDefine.h"
#include "ParamInfo.h"
#include "TrendWidgetLabel.h"
#include "AlarmConfig.h"
#include "MeasureSettingWindow.h"

class PITrendWidgetPrivate
{
public:
    PITrendWidgetPrivate()
        : piValue(NULL),
          scale(1), isAlarm(false)
    {}
    ~PITrendWidgetPrivate(){}

    QLabel *piValue;
    short scale;
    bool isAlarm;
    QString piString;
};

void PITrendWidget::setPIValue(int16_t pi)
{
    if (0 <= pi && pi < 1000)
    {
        // PI value range 0%-9.99%, set the resolution to 0.01
        d_ptr->piString = QString::number(pi / (d_ptr->scale * 1.0), 'f', 2);
    }
    else if (pi >= 1000 && pi <= 10000)
    {
        // PI value range 10.0%-100%, set the resolution to 0.1
        d_ptr->piString = QString::number(pi / (d_ptr->scale * 1.0), 'f', 1);
    }
    else
    {
        d_ptr->piString = InvStr();
    }
    d_ptr->piValue->setText(d_ptr->piString);
}

void PITrendWidget::updateLimit()
{
    d_ptr->scale = 100;
}

void PITrendWidget::isAlarm(bool flag)
{
    d_ptr->isAlarm = flag;
}

void PITrendWidget::showValue()
{
    QPalette psrc;
    psrc.setColor(QPalette::WindowText, Qt::white);
    if (d_ptr->isAlarm && d_ptr->piString != InvStr())
    {
        showAlarmStatus(d_ptr->piValue);
        showAlarmParamLimit(d_ptr->piValue, d_ptr->piString, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
    }
}

PITrendWidget::PITrendWidget()
    : TrendWidget("PITrendWidget"),
      d_ptr(new PITrendWidgetPrivate)
{
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_PI)));
    setUnit(trs(Unit::getSymbol(UNIT_PERCENT)));

    d_ptr->piValue = new QLabel();
    d_ptr->piValue->setAlignment(Qt::AlignCenter);
    d_ptr->piValue->setText(InvStr());

    contentLayout->addWidget(d_ptr->piValue, 5);
    connect(this, SIGNAL(released()), this, SLOT(onRelease()));

    updateLimit();
}

PITrendWidget::~PITrendWidget()
{
    delete d_ptr;
}

QList<SubParamID> PITrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_PI);
    return list;
}

void PITrendWidget::doRestoreNormalStatus()
{
    QPalette psrc;
    psrc.setColor(QPalette::WindowText, Qt::white);
    showNormalStatus(psrc);
}

void PITrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontsize = fontManager.adjustNumFontSize(r, true, "99999");
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->piValue->setFont(font);
}

void PITrendWidget::onRelease()
{
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("SPO2Menu"));
}

