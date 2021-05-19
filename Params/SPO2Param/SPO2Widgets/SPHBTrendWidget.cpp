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
#include "MeasureSettingWindow.h"
#include "ParamManager.h"
#include "SPO2Param.h"

class SPHBTrendWidgetPrivate
{
public:
    SPHBTrendWidgetPrivate()
        : sphbValue(NULL),
          isAlarm(false),
          scale(1)
    {}
    ~SPHBTrendWidgetPrivate(){}

    QLabel *sphbValue;
    bool isAlarm;
    QString sphbString;
    short scale;
};

void SPHBTrendWidget::setSPHBValue(int16_t sphb)
{
    if (sphb >= 0)
    {
        UnitType unit = paramManager.getSubParamUnit(PARAM_SPO2, SUB_PARAM_SPHB);

        d_ptr->sphbString = Unit::convert(unit, UNIT_GDL, sphb / (d_ptr->scale * 1.0));
    }
    else
    {
        d_ptr->sphbString = InvStr();
    }
    d_ptr->sphbValue->setText(d_ptr->sphbString);
}

void SPHBTrendWidget::updateLimit()
{
    UnitType unit = paramManager.getSubParamUnit(PARAM_SPO2, SUB_PARAM_SPHB);
    LimitAlarmConfig limit = alarmConfig.getLimitAlarmConfig(SUB_PARAM_SPHB, unit);
    setLimit(limit.highLimit, limit.lowLimit, limit.scale);
    d_ptr->scale = limit.scale;
}

void SPHBTrendWidget::updateUnit(UnitType unit)
{
    setUnit(trs(Unit::getSymbol(unit)));
    updateLimit();
}

void SPHBTrendWidget::isAlarm(bool flag)
{
    d_ptr->isAlarm = flag;
}

void SPHBTrendWidget::showValue()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getSubParamName(SUB_PARAM_SPHB));
    if (d_ptr->isAlarm && d_ptr->sphbString != InvStr())
    {
        showAlarmStatus(d_ptr->sphbValue);
        showAlarmParamLimit(d_ptr->sphbValue, d_ptr->sphbString, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
    }
}

SPHBTrendWidget::SPHBTrendWidget()
    : TrendWidget("SpHbTrendWidget"),
      d_ptr(new SPHBTrendWidgetPrivate())
{
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_SPHB)));
    setUnit(trs(Unit::getSymbol(paramManager.getSubParamUnit(PARAM_SPO2, SUB_PARAM_SPHB))));

    d_ptr->sphbValue = new QLabel();
    d_ptr->sphbValue->setText(InvStr());
    d_ptr->sphbValue->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(d_ptr->sphbValue, 3);

    QPalette pal = colorManager.getPalette(paramInfo.getSubParamName(SUB_PARAM_SPHB));
    setPalette(pal);

    connect(this, SIGNAL(released()), this, SLOT(onRelease()));
    loadConfig();
}

SPHBTrendWidget::~SPHBTrendWidget()
{
    delete d_ptr;
}

QList<SubParamID> SPHBTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_SPHB);
    return list;
}

void SPHBTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getSubParamName(SUB_PARAM_SPHB));
    showNormalStatus(psrc);
}

void SPHBTrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontsize = fontManager.adjustNumFontSize(r, true, "9999");
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->sphbValue->setFont(font);
}

void SPHBTrendWidget::loadConfig()
{
    SpHbUnitType unit = spo2Param.getSpHbUnit();
    UnitType u = unit == SPHB_UNIT_G_DL ? UNIT_GDL : UNIT_MMOL_L;
    setUnit(trs(Unit::getSymbol(u)));
    TrendWidget::loadConfig();
}

void SPHBTrendWidget::onRelease()
{
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("SPO2Menu"));
}
