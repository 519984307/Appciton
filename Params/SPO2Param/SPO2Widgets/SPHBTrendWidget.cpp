/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/6/10
 **/

#include "SPHBTrendWidget.h"
#include "BaseDefine.h"
#include "ParamInfo.h"
#include <QLabel>
#include <QLayout>
#include "TrendWidgetLabel.h"
#include "AlarmConfig.h"

class SPHBTrendWidgetPrivate
{
public:
    SPHBTrendWidgetPrivate()
        : sphbValue(NULL)
    {}
    ~SPHBTrendWidgetPrivate(){}

    QLabel *sphbValue;
};

void SPHBTrendWidget::setSPHBValue(int16_t sphb)
{
    if (sphb >= 0)
    {
        d_ptr->sphbValue->setText(QString::number(sphb));
    }
    else
    {
        d_ptr->sphbValue->setText(InvStr());
    }
}

void SPHBTrendWidget::updateLimit()
{
    LimitAlarmConfig limit = alarmConfig.getLimitAlarmConfig(SUB_PARAM_SPHB, UNIT_GDL);
    setLimit(limit.highLimit, limit.lowLimit, limit.scale);
}

void SPHBTrendWidget::isAlarm(bool flag)
{
    Q_UNUSED(flag)
}

SPHBTrendWidget::SPHBTrendWidget()
    : TrendWidget("SpHbTrendWidget"),
      d_ptr(new SPHBTrendWidgetPrivate())
{
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_SPHB)));
    setUnit(trs(Unit::getSymbol(UNIT_GDL)));

    d_ptr->sphbValue = new QLabel();
    d_ptr->sphbValue->setText(InvStr());
    d_ptr->sphbValue->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(d_ptr->sphbValue, 5);

    QPalette pal = colorManager.getPalette(paramInfo.getSubParamName(SUB_PARAM_SPHB));
    setPalette(pal);

    loadConfig();
}

SPHBTrendWidget::~SPHBTrendWidget()
{
    delete d_ptr;
}

void SPHBTrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->sphbValue->setFont(font);
}
