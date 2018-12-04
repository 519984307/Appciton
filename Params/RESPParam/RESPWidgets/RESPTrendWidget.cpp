/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "RESPTrendWidget.h"
#include "ParamManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include "RESPParam.h"
#include "TrendWidgetLabel.h"
#include "MeasureSettingWindow.h"
#include "AlarmConfig.h"
#include "ParamManager.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void RESPTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("RESPMenu"));
}

void RESPTrendWidget::_loadConfig()
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    setPalette(palette);
    _rrValue->setPalette(palette);

    setName(trs(paramInfo.getSubParamName(SUB_PARAM_RR_BR)));
    setUnit(Unit::getSymbol(UNIT_RPM));
    updateLimit();
}

/**************************************************************************************************
 * 设置PR的值。
 *************************************************************************************************/
void RESPTrendWidget::setRRValue(int16_t rr , bool isRR)
{
    if (isRR)
    {
        setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_RR)));
    }
    else
    {
        setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_BR)));
    }

    if (rr != InvData())
    {
        _rrString = QString::number(rr);
    }
    else
    {
        _rrString = InvStr();
    }
    _rrValue->setText(_rrString);
}

void RESPTrendWidget::updateLimit()
{
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_RR_BR, UNIT_RPM);
    setLimit(config.highLimit, config.lowLimit, config.scale);
}


/**************************************************************************************************
 * 窗口是否使能。
 *************************************************************************************************/
bool RESPTrendWidget::enable()
{
    return respParam.isEnabled();
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void RESPTrendWidget::isAlarm(bool flag)
{
    _isAlarm = flag;

    updateAlarm(flag);
}

/**************************************************************************************************
 * 显示报警。
 *************************************************************************************************/
void RESPTrendWidget::showValue(void)
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    if (_isAlarm)
    {
        showAlarmStatus(_rrValue);
        showAlarmParamLimit(_rrValue, _rrString, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
    }
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void RESPTrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    // 字体。
//    int fontsize = fontManager.adjustNumFontSizeXML(r);
//    fontsize = fontManager.getFontSize(fontsize);
    int fontsize = fontManager.adjustNumFontSize(r , true);
    QFont font = fontManager.numFont(fontsize, true);
//    font.setStretch(105); // 横向放大。
    font.setWeight(QFont::Black);

    _rrValue->setFont(font);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RESPTrendWidget::RESPTrendWidget() : TrendWidget("RESPTrendWidget")
{
    _isAlarm = false;
    _rrString = InvStr();

    // 设置报警关闭标志
    showAlarmOff();

    // RR值。
    _rrValue = new QLabel();
    _rrValue->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _rrValue->setText(InvStr());

    // 布局。
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addStretch(1);
    mainLayout->addWidget(_rrValue);
    mainLayout->addStretch(1);

    contentLayout->addStretch(1);
    contentLayout->addLayout(mainLayout, 3);
    contentLayout->addStretch(1);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    _loadConfig();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RESPTrendWidget::~RESPTrendWidget()
{
}

QList<SubParamID> RESPTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_RR_BR);
    return list;
}

void RESPTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    showNormalStatus(psrc);
}

void RESPTrendWidget::updateWidgetConfig()
{
    _loadConfig();
}
