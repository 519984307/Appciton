/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/6/10
 **/

#include "PVITrendWidget.h"
#include "TrendWidgetLabel.h"
#include "BaseDefine.h"
#include <QLabel>
#include "ParamInfo.h"
#include <QLayout>
#include "AlarmConfig.h"

class PVITrendWidgetPrivate
{
public:
    PVITrendWidgetPrivate()
        : pviValue(NULL),
          isAlarm(false)
    {}
    ~PVITrendWidgetPrivate(){}

    QLabel *pviValue;
    bool isAlarm;
};

void PVITrendWidget::setPVIValue(int16_t pvi)
{
    QString pviString;
    if (pvi >= 0)
    {
        pviString = QString::number(pvi);
    }
    else
    {
        pviString = InvStr();
    }
    d_ptr->pviValue->setText(pviString);
}

void PVITrendWidget::updateLimit()
{
    LimitAlarmConfig limit = alarmConfig.getLimitAlarmConfig(SUB_PARAM_PVI, UNIT_PERCENT);
    setLimit(limit.highLimit, limit.lowLimit, limit.scale);
}

void PVITrendWidget::isAlarm(bool flag)
{
    Q_UNUSED(flag);
}

void PVITrendWidget::showValue()
{
}

void PVITrendWidget::doRestoreNormalStatus()
{
}

PVITrendWidget::PVITrendWidget()
    : TrendWidget("PVITrendWidget"),
      d_ptr(new PVITrendWidgetPrivate())
{
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_PVI)));
    setUnit(trs(Unit::getSymbol(UNIT_PERCENT)));

    d_ptr->pviValue = new QLabel;
    d_ptr->pviValue->setText(InvStr());
    d_ptr->pviValue->setAlignment(Qt::AlignCenter);

    contentLayout->addWidget(d_ptr->pviValue, 5);
    loadConfig();
}

PVITrendWidget::~PVITrendWidget()
{
    delete d_ptr;
}

void PVITrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->pviValue->setFont(font);
}

void PVITrendWidget::loadConfig()
{
    TrendWidget::loadConfig();
}
