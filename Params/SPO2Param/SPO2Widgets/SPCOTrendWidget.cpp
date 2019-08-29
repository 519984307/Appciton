/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/6/19
 **/

#include "SPCOTrendWidget.h"
#include "TrendWidgetLabel.h"
#include "BaseDefine.h"
#include <QLabel>
#include <QLayout>
#include "ParamInfo.h"
#include "AlarmConfig.h"
#include "MeasureSettingWindow.h"

class SPCOTrendWidgetPrivate
{
public:
    SPCOTrendWidgetPrivate()
        : spcoValue(NULL),
          isAlarm(false)
    {}
    ~SPCOTrendWidgetPrivate(){}

    QLabel *spcoValue;
    bool isAlarm;
    QString spcoString;
};

SPCOTrendWidget::SPCOTrendWidget()
    : TrendWidget("SpCOTrendWidget"),
      d_ptr(new SPCOTrendWidgetPrivate())
{
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_SPCO)));
    setUnit(trs(Unit::getSymbol(UNIT_PERCENT)));

    d_ptr->spcoValue = new QLabel();
    d_ptr->spcoValue->setText(InvStr());
    d_ptr->spcoValue->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(d_ptr->spcoValue, 3);

    connect(this, SIGNAL(released()), this, SLOT(onRelease()));
    loadConfig();
}

SPCOTrendWidget::~SPCOTrendWidget()
{
    delete d_ptr;
}

void SPCOTrendWidget::setSPCOValue(int16_t spco)
{
    if (spco >= 0)
    {
        d_ptr->spcoString = QString::number(spco);
    }
    else
    {
        d_ptr->spcoString = InvStr();
    }
    d_ptr->spcoValue->setText(d_ptr->spcoString);
}

void SPCOTrendWidget::isAlarm(bool flag)
{
    d_ptr->isAlarm = flag;
}

void SPCOTrendWidget::showValue()
{
    QPalette psrc;
    psrc.setColor(QPalette::WindowText, Qt::white);
    if (d_ptr->isAlarm && d_ptr->spcoString != InvStr())
    {
        showAlarmStatus(d_ptr->spcoValue);
        showAlarmParamLimit(d_ptr->spcoValue, d_ptr->spcoString, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
    }
}

void SPCOTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc;
    psrc.setColor(QPalette::WindowText, Qt::white);
    showNormalStatus(psrc);
}

void SPCOTrendWidget::updateLimit()
{
    LimitAlarmConfig limit = alarmConfig.getLimitAlarmConfig(SUB_PARAM_SPCO, UNIT_PERCENT);
    setLimit(limit.highLimit, limit.lowLimit, limit.scale);
}

QList<SubParamID> SPCOTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_SPCO);
    return list;
}

void SPCOTrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->spcoValue->setFont(font);
}

void SPCOTrendWidget::onRelease()
{
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("SPO2Menu"));
}
