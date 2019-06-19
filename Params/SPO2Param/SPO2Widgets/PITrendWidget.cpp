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

class PITrendWidgetPrivate
{
public:
    PITrendWidgetPrivate()
        : piValue(NULL),
          scale(1)
    {}
    ~PITrendWidgetPrivate(){}

    QLabel *piValue;
    short scale;
};

void PITrendWidget::setPIValue(int16_t pi)
{
    if (pi >= 0)
    {
        d_ptr->piValue->setText(QString::number(pi / (d_ptr->scale * 1.0), 'f', 1));
    }
    else
    {
        d_ptr->piValue->setText(InvStr());
    }
}

void PITrendWidget::updateLimit()
{
    LimitAlarmConfig limit = alarmConfig.getLimitAlarmConfig(SUB_PARAM_PI, UNIT_PERCENT);
    setLimit(limit.highLimit, limit.lowLimit, limit.scale);
    d_ptr->scale = limit.scale;
}

void PITrendWidget::isAlarm(bool flag)
{
    Q_UNUSED(flag)
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

    loadConfig();
}

PITrendWidget::~PITrendWidget()
{
    delete d_ptr;
}

void PITrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->piValue->setFont(font);
}

