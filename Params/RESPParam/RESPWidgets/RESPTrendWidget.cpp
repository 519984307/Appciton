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
#include "FontManager.h"
#include "RESPDupParam.h"
#include "ConfigManager.h"
#include "CO2Param.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void RESPTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("RESPMenu"));
}

void RESPTrendWidget::_onBrSourceStatusUpdate()
{
    bool isAuto = respDupParam.isAutoBrSourceEnabled();
    RESPDupParam::BrSourceType type = respDupParam.getBrSource();

    _rrSource->setVisible(false);
    _rrSource->setText("");
    if (respDupParam.getRR() == InvData())
    {
        return;
    }
    if (isAuto == true)
    {
        if (type == RESPDupParam::BR_SOURCE_CO2)
        {
            _rrSource->setVisible(true);
            _rrSource->setText(trs("AutoOfCO2"));
        }
        return;
    }
    if (type == RESPDupParam::BR_SOURCE_CO2)
    {
        _rrSource->setVisible(true);
        _rrSource->setText(trs("SourceOfCO2"));
    }
}

void RESPTrendWidget::loadConfig()
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    setPalette(palette);
    _rrValue->setPalette(palette);

    setName(trs(paramInfo.getSubParamName(SUB_PARAM_RR_BR)));
    setUnit(Unit::getSymbol(UNIT_RPM));

    TrendWidget::loadConfig();
}

/**************************************************************************************************
 * 设置PR的值。
 *************************************************************************************************/
void RESPTrendWidget::setRRValue(int16_t rr , bool isRR, bool isAutoType)
{
    if (isRR)
    {
        setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_RR)));
    }
    else
    {
        setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_BR)));
    }

    if ((rr == InvData() && _rrString != InvStr())  // invalid value  first
            || (rr != InvData() && _rrString == InvStr())  // valid value  first
            || (isAutoType == true))  // if br/rr value is from auto type
    {
        _onBrSourceStatusUpdate();
    }

    if (rr != InvData())
    {
        if (respParam.isRRInaccurate() && isRR)
        {
            _rrString = QString("%1?").arg(QString::number(rr));
        }
        else
        {
            _rrString = QString::number(rr);
        }
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

    // RR值。
    _rrValue = new QLabel();
    _rrValue->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _rrValue->setText(InvStr());

    // RR来源
    _rrSource = new QLabel();
    _rrSource->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _rrSource->setFont(fontManager.textFont(18));

    // 布局。
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(_rrValue);
    mainLayout->addWidget(_rrSource);

    contentLayout->addLayout(mainLayout, 7);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    connect(&respDupParam, SIGNAL(brSourceStatusUpdate()), this, SLOT(_onBrSourceStatusUpdate()));
    _onBrSourceStatusUpdate();

    loadConfig();
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
