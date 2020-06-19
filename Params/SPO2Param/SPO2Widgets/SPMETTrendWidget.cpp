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
#include "AlarmConfig.h"
#include "ColorManager.h"
#include "MeasureSettingWindow.h"

class SPMETTrendWidgetPrivate
{
public:
    SPMETTrendWidgetPrivate()
        : spmetValue(NULL),
          scale(1),
          isAlarm(false)
    {}
    ~SPMETTrendWidgetPrivate(){}

    QLabel *spmetValue;
    short scale;
    bool isAlarm;
    QString spmetString;
};

void SPMETTrendWidget::setSpMetValue(int16_t spmet)
{
    if (spmet >= 0)
    {
        d_ptr->spmetString = QString::number(spmet / (d_ptr->scale * 1.0), 'f', 1);
    }
    else
    {
        d_ptr->spmetString = InvStr();
    }
    d_ptr->spmetValue->setText(d_ptr->spmetString);
}

void SPMETTrendWidget::updateLimit()
{
    LimitAlarmConfig limit = alarmConfig.getLimitAlarmConfig(SUB_PARAM_SPMET, UNIT_PERCENT);
    setLimit(limit.highLimit, limit.lowLimit, limit.scale);
    d_ptr->scale = limit.scale;
}

void SPMETTrendWidget::isAlarm(bool flag)
{
    d_ptr->isAlarm = flag;
}

void SPMETTrendWidget::showValue()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getSubParamName(SUB_PARAM_SPMET));
    if (d_ptr->isAlarm && d_ptr->spmetString != InvStr())
    {
        showAlarmStatus(d_ptr->spmetValue);
        showAlarmParamLimit(d_ptr->spmetValue, d_ptr->spmetString, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
    }
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

    contentLayout->addWidget(d_ptr->spmetValue, 3);
    QPalette pal = colorManager.getPalette(paramInfo.getSubParamName(SUB_PARAM_SPMET));
    setPalette(pal);

    connect(this, SIGNAL(released()), this, SLOT(onRelease()));
    loadConfig();
}

SPMETTrendWidget::~SPMETTrendWidget()
{
    delete d_ptr;
}

QList<SubParamID> SPMETTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_SPMET);
    return list;
}

void SPMETTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getSubParamName(SUB_PARAM_SPMET));
    showNormalStatus(psrc);
}

void SPMETTrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontsize = fontManager.adjustNumFontSize(r, true, "9999");
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->spmetValue->setFont(font);
}

void SPMETTrendWidget::onRelease()
{
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("SPO2Menu"));
}

