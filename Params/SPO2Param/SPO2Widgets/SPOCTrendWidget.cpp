/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/6/10
 **/

#include "SPOCTrendWidget.h"
#include "BaseDefine.h"
#include "ParamInfo.h"
#include <QLabel>
#include "TrendWidgetLabel.h"
#include <QLayout>

class SPOCTrendWidgetPrivate
{
public:
    SPOCTrendWidgetPrivate()
        : spocValue(NULL)
    {}
    ~SPOCTrendWidgetPrivate(){}

    QLabel *spocValue;
};

void SPOCTrendWidget::setSPOCValue(int16_t spoc)
{
    if (spoc >= 0)
    {
        d_ptr->spocValue->setText(QString::number(spoc));
    }
    else
    {
        d_ptr->spocValue->setText(InvStr());
    }
}

void SPOCTrendWidget::updateLimit()
{
    setLimit(25, 10, 1);
}

void SPOCTrendWidget::isAlarm(bool flag)
{
    Q_UNUSED(flag)
}

SPOCTrendWidget::SPOCTrendWidget()
    : TrendWidget("SPOCTrendWidget"),
      d_ptr(new SPOCTrendWidgetPrivate())
{
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_SPOC)));
    setUnit(trs(Unit::getSymbol(UNIT_MLDL)));

    d_ptr->spocValue = new QLabel();
    d_ptr->spocValue->setAlignment(Qt::AlignCenter);
    d_ptr->spocValue->setText(InvStr());

    contentLayout->addWidget(d_ptr->spocValue, 5);
    loadConfig();
}

SPOCTrendWidget::~SPOCTrendWidget()
{
    delete d_ptr;
}

void SPOCTrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->spocValue->setFont(font);
}
