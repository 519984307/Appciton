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
#include "MeasureSettingWindow.h"

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
    QString pviString;
};

void PVITrendWidget::setPVIValue(int16_t pvi)
{
    if (pvi >= 0)
    {
        d_ptr->pviString = QString::number(pvi);
    }
    else
    {
        d_ptr->pviString = InvStr();
    }
    d_ptr->pviValue->setText(d_ptr->pviString);
}

void PVITrendWidget::updateLimit()
{
    LimitAlarmConfig limit = alarmConfig.getLimitAlarmConfig(SUB_PARAM_PVI, UNIT_PERCENT);
    setLimit(limit.highLimit, limit.lowLimit, limit.scale);
}

void PVITrendWidget::isAlarm(bool flag)
{
    d_ptr->isAlarm = flag;
}

void PVITrendWidget::showValue()
{
    QPalette psrc;
    psrc.setColor(QPalette::WindowText, Qt::white);
    if (d_ptr->isAlarm && d_ptr->pviString != InvStr())
    {
        showAlarmStatus(d_ptr->pviValue);
        showAlarmParamLimit(d_ptr->pviValue, d_ptr->pviString, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
    }
}

void PVITrendWidget::doRestoreNormalStatus()
{
    QPalette psrc;
    psrc.setColor(QPalette::WindowText, Qt::white);
    showNormalStatus(psrc);
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

    contentLayout->addWidget(d_ptr->pviValue, 3);
    connect(this, SIGNAL(released()), this, SLOT(onRelease()));
    loadConfig();
}

PVITrendWidget::~PVITrendWidget()
{
    delete d_ptr;
}

QList<SubParamID> PVITrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_PVI);
    return list;
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

void PVITrendWidget::onRelease()
{
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("SPO2Menu"));
}
