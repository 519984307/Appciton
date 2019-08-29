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
#include "AlarmConfig.h"
#include "MeasureSettingWindow.h"

class SPOCTrendWidgetPrivate
{
public:
    SPOCTrendWidgetPrivate()
        : spocValue(NULL),
          isAlarm(false)
    {}
    ~SPOCTrendWidgetPrivate(){}

    QLabel *spocValue;
    bool isAlarm;
    QString spocString;
};

void SPOCTrendWidget::setSPOCValue(int16_t spoc)
{
    if (spoc >= 0)
    {
        d_ptr->spocString = QString::number(spoc);
    }
    else
    {
        d_ptr->spocString = InvStr();
    }
    d_ptr->spocValue->setText(d_ptr->spocString);
}

void SPOCTrendWidget::updateLimit()
{
    LimitAlarmConfig limit = alarmConfig.getLimitAlarmConfig(SUB_PARAM_SPOC, UNIT_MLDL);
    setLimit(limit.highLimit, limit.lowLimit, limit.scale);
}

void SPOCTrendWidget::isAlarm(bool flag)
{
    d_ptr->isAlarm = flag;
}

void SPOCTrendWidget::showValue()
{
    QPalette psrc;
    psrc.setColor(QPalette::WindowText, Qt::white);
    if (d_ptr->isAlarm && d_ptr->spocString != InvStr())
    {
        showAlarmStatus(d_ptr->spocValue);
        showAlarmParamLimit(d_ptr->spocValue, d_ptr->spocString, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
    }
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

    contentLayout->addWidget(d_ptr->spocValue, 3);
    connect(this, SIGNAL(released()), this, SLOT(onRelease()));
    loadConfig();
}

SPOCTrendWidget::~SPOCTrendWidget()
{
    delete d_ptr;
}

QList<SubParamID> SPOCTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_SPOC);
    return list;
}

void SPOCTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc;
    psrc.setColor(QPalette::WindowText, Qt::white);
    showNormalStatus(psrc);
}

void SPOCTrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->spocValue->setFont(font);
}

void SPOCTrendWidget::onRelease()
{
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("SPO2Menu"));
}
