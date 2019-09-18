/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/12
 **/

#include "O2TrendWidget.h"
#include <QLabel>
#include "ParamInfo.h"
#include "ParamManager.h"
#include <QHBoxLayout>
#include "TrendWidgetLabel.h"
#include "MeasureSettingWindow.h"
#include "AlarmConfig.h"

void O2TrendWidget::setO2Value(int16_t o2)
{
    if (((o2 & 0xffff) != 0xffff) && o2 != InvData())
    {
        _o2String = QString("%1").number(o2);
    }
    else
    {
        _o2String = InvStr();
    }
    _o2Value->setText(_o2String);
}

void O2TrendWidget::updateLimit()
{
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_O2, UNIT_PERCENT);
    setLimit(config.highLimit, config.lowLimit, config.scale);
}

void O2TrendWidget::isAlarm(bool flag)
{
    _isAlarm = flag;
    updateAlarm(flag);
}

void O2TrendWidget::showValue()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_O2));
    normalPalette(psrc);
    if (_isAlarm)
    {
        showAlarmStatus(_o2Value);
        showAlarmParamLimit(_o2Value, _o2String, psrc);
        restoreNormalStatusLater();
    }
}

O2TrendWidget::O2TrendWidget() : TrendWidget("O2TrendWidget"),
    _o2Value(NULL), _isAlarm(false), _o2String(InvStr())
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_O2));
    setPalette(palette);
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_O2)));
    setUnit(trs(Unit::getSymbol(UNIT_PERCENT)));

    // 设置上下限
    updateLimit();

    // 设置报警关闭标志
    showAlarmOff();

    // O2值
    _o2Value = new QLabel();
    _o2Value->setAlignment(Qt::AlignCenter);
    _o2Value->setPalette(palette);
    _o2Value->setText(InvStr());

    // 布局
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addStretch(1);
    mainLayout->addWidget(_o2Value);
    mainLayout->addStretch(1);

    contentLayout->addStretch(1);
    contentLayout->addLayout(mainLayout, 3);
    contentLayout->addStretch(1);

    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));
}

O2TrendWidget::~O2TrendWidget()
{
}

QList<SubParamID> O2TrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_O2);
    return list;
}

void O2TrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_O2));
    normalPalette(psrc);
    showNormalParamLimit(psrc);
    showNormalStatus(_o2Value, psrc);
}

void O2TrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    int fontSize = fontManager.adjustNumFontSize(r, true, "9999");
    QFont font = fontManager.numFont(fontSize, true);
    font.setWeight(QFont::Black);
    _o2Value->setFont(font);
}

void O2TrendWidget::_releaseHandle(IWidget *w)
{
    Q_UNUSED(w)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("O2Menu"));
}
