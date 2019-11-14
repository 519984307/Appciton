/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "SPO2TrendWidget.h"
#include "ParamManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include "SPO2Param.h"
#include "TrendWidgetLabel.h"
#include "MeasureSettingWindow.h"
#include "AlarmConfig.h"
#include "ParamManager.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void SPO2TrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("SPO2Menu"));
}

void SPO2TrendWidget::loadConfig()
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    setPalette(palette);
    _spo2Value->setPalette(palette);
    _piName->setPalette(palette);
    _piValue->setPalette(palette);
    TrendWidget::loadConfig();
}

/**************************************************************************************************
 * 设置SPO2的值。
 *************************************************************************************************/
void SPO2TrendWidget::setSPO2Value(int16_t spo2)
{
    if ((spo2 >= 0 && spo2Param.getPerfusionStatus()) || (spo2 >= 0 && spo2 <= 60))
    {
        _spo2String = QString("%1?").arg(QString::number(spo2));
    }
    else if (spo2 >= 0)
    {
        _spo2String = QString::number(spo2);
    }
    else
    {
        _spo2String = InvStr();
    }
    _spo2Value->setText(_spo2String);
}

void SPO2TrendWidget::updateLimit()
{
    UnitType unitType = paramManager.getSubParamUnit(PARAM_SPO2, SUB_PARAM_SPO2);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_SPO2, unitType);
    setLimit(config.highLimit, config.lowLimit, config.scale);
}

/**************************************************************************************************
 * 设置bar图的值。
 *************************************************************************************************/
void SPO2TrendWidget::setBarValue(int16_t bar)
{
    if (_piString == InvStr())
    {
        _spo2Bar->setValue(bar, false);
    }
    else
    {
        _spo2Bar->setValue(bar, true);
    }
}

void SPO2TrendWidget::setPIValue(int16_t pi)
{
    if (pi == InvData())
    {
        _piString = InvStr();
    }
    else
    {
        int16_t piInt = pi / 10;
        int16_t piDes = pi % 10;
        _piString = QString::number(piInt) + "." + QString::number(piDes);
    }
    _piValue->setText(_piString);
}

/**************************************************************************************************
 * 显示搜索脉搏的提示信息。
 *************************************************************************************************/
void SPO2TrendWidget::setSearchForPulse(bool isSearching)
{
    if (isSearching)
    {
//        setInfo(trs("SPO2SearchPulse"));
    }
    else
    {
//        setInfo(" ");
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void SPO2TrendWidget::isAlarm(bool flag)
{
    _isAlarm = flag;

    updateAlarm(flag);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void SPO2TrendWidget::showValue(void)
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    if (_isAlarm && _spo2String != InvStr())
    {
        showAlarmStatus(_spo2Value);
        showAlarmParamLimit(_spo2Value, _spo2String, psrc);
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
void SPO2TrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    // 字体。
//    int fontsize = fontManager.adjustNumFontSizeXML(r);
//    fontsize = fontManager.getFontSize(fontsize);
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);

//    font.setStretch(105); // 横向放大。
    font.setWeight(QFont::Black);

    _spo2Value->setFont(font);

    font = fontManager.numFont(fontsize / 2, true);
    font.setWeight(QFont::Black);
    _piValue->setFont(font);

    int fontSize = fontManager.getFontSize(3);
    font = fontManager.textFont(fontSize);
    _piName->setFont(font);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SPO2TrendWidget::SPO2TrendWidget() : TrendWidget("SPO2TrendWidget")
{
    _isAlarm = false;
    _spo2String = InvStr();
    _piString = InvStr();
    setName(trs(paramInfo.getParamName(PARAM_SPO2)));
    setUnit(trs(Unit::getSymbol(UNIT_PERCENT)));

    // 血氧值。
    _spo2Value = new QLabel();
    _spo2Value->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _spo2Value->setText(InvStr());

    // 棒图。
    _spo2Bar = new SPO2BarWidget(0, 15);
    _spo2Bar->setFixedWidth(20);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(_spo2Bar);

    _piName = new QLabel();
    _piName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _piName->setText("PI");

    _piValue = new QLabel();
    _piValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _piValue->setText(InvStr());

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(_spo2Value, 5);
    layout->addLayout(vLayout, 1);
    layout->addWidget(_piName, 1);
    layout->addWidget(_piValue, 3);

    contentLayout->addLayout(layout, 7);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    loadConfig();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SPO2TrendWidget::~SPO2TrendWidget()
{
}

QList<SubParamID> SPO2TrendWidget::getShortTrendSubParams() const
{
   QList<SubParamID> list;
   list.append(SUB_PARAM_SPO2);
   return list;
}

void SPO2TrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    showNormalStatus(psrc);
}
