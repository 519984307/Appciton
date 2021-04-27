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
#include "MeasureSettingWindow.h"

class PITrendWidgetPrivate
{
public:
    PITrendWidgetPrivate()
        : piValue(NULL), pluginPiName(NULL), pluginPiValue(NULL),
          scale(1), isAlarm(false), isPluginPIAlarm(false)
    {}
    ~PITrendWidgetPrivate(){}

    QLabel *piValue;
    QLabel *pluginPiName;       // plugin module pi name and value
    QLabel *pluginPiValue;
    short scale;
    bool isAlarm;
    bool isPluginPIAlarm;
    QString piString;
    QString pluginPIString;
};

void PITrendWidget::setPIValue(short pi, bool isPlugin)
{
    QString valueStr;
    if (0 <= pi && pi < 1000)
    {
        // PI value range 0%-9.99%, set the resolution to 0.01
        valueStr = QString::number(pi / (d_ptr->scale * 1.0), 'f', 2);
    }
    else if (pi >= 1000 && pi <= 10000)
    {
        // PI value range 10.0%-100%, set the resolution to 0.1
        valueStr = QString::number(pi / (d_ptr->scale * 1.0), 'f', 1);
    }
    else
    {
        valueStr = InvStr();
    }
    if (!isPlugin)
    {
        d_ptr->piString = valueStr;
        d_ptr->piValue->setText(d_ptr->piString);
    }
    else
    {
        d_ptr->pluginPIString = valueStr;
        d_ptr->pluginPiValue->setText(d_ptr->piString);
    }
}

void PITrendWidget::updateLimit()
{
    d_ptr->scale = 100;
}

void PITrendWidget::isAlarm(bool flag)
{
    d_ptr->isAlarm = flag;
}

void PITrendWidget::isPluginPIAlarm(bool flag)
{
    d_ptr->isPluginPIAlarm = flag;
}

void PITrendWidget::showValue()
{
    QPalette psrc;
    psrc.setColor(QPalette::WindowText, Qt::white);
    if (d_ptr->isAlarm && d_ptr->piString != InvStr())
    {
        showAlarmStatus(d_ptr->piValue);
        showAlarmParamLimit(d_ptr->piValue, d_ptr->piString, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
    }

    if (d_ptr->isPluginPIAlarm && d_ptr->pluginPIString != InvStr())
    {
        showAlarmStatus(d_ptr->pluginPiValue);
        showAlarmParamLimit(d_ptr->pluginPiValue, d_ptr->pluginPIString, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
    }
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

    QHBoxLayout *pluginLayout = new QHBoxLayout();

    d_ptr->pluginPiName = new QLabel(trs(paramInfo.getSubParamName(SUB_PARAM_PLUGIN_PI)));
    d_ptr->pluginPiName->setAlignment(Qt::AlignCenter);
    pluginLayout->addWidget(d_ptr->pluginPiName);
    d_ptr->pluginPiValue = new QLabel();
    d_ptr->pluginPiValue->setAlignment(Qt::AlignCenter);
    d_ptr->pluginPiValue->setText(InvStr());
    pluginLayout->addWidget(d_ptr->pluginPiValue);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(10);
    layout->addWidget(d_ptr->piValue);
    layout->addLayout(pluginLayout);

    contentLayout->addLayout(layout, 7);
    connect(this, SIGNAL(released()), this, SLOT(onRelease()));

    updateLimit();
}

PITrendWidget::~PITrendWidget()
{
    delete d_ptr;
}

QList<SubParamID> PITrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_PI);
    return list;
}

void PITrendWidget::doRestoreNormalStatus()
{
    QPalette psrc;
    psrc.setColor(QPalette::WindowText, Qt::white);
    showNormalStatus(psrc);
}

void PITrendWidget::setTextSize()
{
    QRect r = this->rect();
    if (d_ptr->pluginPiName->isVisible() && d_ptr->pluginPiValue->isVisible())
    {
        r.adjust(nameLabel->width() * 3, 0, 0, 0);
        r.setHeight(r.height() / 2);
    }
    else
    {
        r.adjust(nameLabel->width(), 0, 0, 0);
    }

    int fontsize = fontManager.adjustNumFontSize(r, true, "99999");
    QFont font = fontManager.numFont(fontsize, true);
    d_ptr->piValue->setFont(font);
    d_ptr->pluginPiValue->setFont(font);

    int fontSize = fontManager.getFontSize(3);
    font = fontManager.textFont(fontSize);
    d_ptr->pluginPiName->setFont(font);
}

void PITrendWidget::onRelease()
{
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("SPO2Menu"));
}

