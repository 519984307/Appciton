/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/6/10
 **/

#include "SPMETTrendWidget.h"
#include <QLayout>
#include "TrendWidgetLabel.h"
#include <QLabel>
#include "ParamInfo.h"
#include "BaseDefine.h"

class SPMETTrendWidgetPrivate
{
public:
    SPMETTrendWidgetPrivate()
        : spmetValue(NULL)
    {}
    ~SPMETTrendWidgetPrivate(){}

    QLabel *spmetValue;
};

void SPMETTrendWidget::setSpMetValue(int16_t spmet)
{
    if (spmet >= 0)
    {
        d_ptr->spmetValue->setText(QString::number(spmet));
    }
    else
    {
        d_ptr->spmetValue->setText(InvStr());
    }
}

void SPMETTrendWidget::updateLimit()
{
    setLimit(30, 0, 10);
}

void SPMETTrendWidget::isAlarm(bool flag)
{
    Q_UNUSED(flag)
}

SPMETTrendWidget::SPMETTrendWidget()
    : TrendWidget("SPMETTrendWidget"),
      d_ptr(new SPMETTrendWidgetPrivate)
{
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_SPMET)));
    setUnit(trs(Unit::getSymbol(UNIT_PERCENT)));

    d_ptr->spmetValue = new QLabel();
    d_ptr->spmetValue->setAlignment(Qt::AlignCenter);
    d_ptr->spmetValue->setText(InvStr());

    contentLayout->addWidget(d_ptr->spmetValue, 5);
    loadConfig();
}

SPMETTrendWidget::~SPMETTrendWidget()
{
    delete d_ptr;
}

void SPMETTrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->spmetValue->setFont(font);
}

