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
    _spo2Value1->setPalette(palette);

    palette.setColor(QPalette::WindowText, Qt::white);
    _spo2Value2->setPalette(palette);
    _spo2Name2->setPalette(palette);
    _spo2DeltaValue->setPalette(palette);
    _spo2DeltaName->setPalette(palette);
    TrendWidget::loadConfig();
}

/**************************************************************************************************
 * 设置SPO2的值。
 *************************************************************************************************/
void SPO2TrendWidget::setSPO2Value(int16_t spo2)
{
    if (spo2 >= 0 && spo2Param.getPerfusionStatus())
    {
        _spo2String1 = QString("%1?").arg(QString::number(spo2));
    }
    else if (spo2 >= 0)
    {
        _spo2String1 = QString::number(spo2);
    }
    else
    {
        _spo2String1 = InvStr();
    }
    _spo2Value1->setText(_spo2String1);
}

void SPO2TrendWidget::setPlugInSPO2Value(int16_t spo2)
{
    if (spo2 >= 0 && spo2Param.getPerfusionStatus(true))
    {
        _spo2String2 = QString("%1?").arg(QString::number(spo2));
    }
    else if (spo2 >= 0)
    {
        _spo2String2 = QString::number(spo2);
    }
    else
    {
        _spo2String2 = InvStr();
    }
    _spo2Value2->setText(_spo2String2);
}

void SPO2TrendWidget::setSPO2DeltaValue(int16_t spo2)
{
    if (spo2 >= 0)
    {
        _spo2StringD = QString::number(spo2);
    }
    else
    {
        _spo2StringD = InvStr();
    }
    _spo2DeltaValue->setText(_spo2StringD);
}

void SPO2TrendWidget::setPIValue(int16_t pi)
{
    if (pi >= 0)
    {
        _piString = QString::number(pi / (100 * 1.0), 'f', 2);
    }
    else
    {
        _piString = InvStr();
    }
    _piValue->setText(_piString);
}

void SPO2TrendWidget::updateLimit()
{
    UnitType unitType = paramManager.getSubParamUnit(PARAM_SPO2, SUB_PARAM_SPO2);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_SPO2, unitType);
    setLimit(config.highLimit, config.lowLimit, config.scale);
}

void SPO2TrendWidget::isAlarmSPO22(bool flag)
{
    _isAlarmSPO2_2 = flag;

    updateAlarm(flag);
}

void SPO2TrendWidget::isAlarmSPO2D(bool flag)
{
    _isAlarmSPO2_D = flag;

    updateAlarm(flag);
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void SPO2TrendWidget::isAlarmSPO2(bool flag)
{
    _isAlarmSPO2 = flag;

    updateAlarm(flag);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void SPO2TrendWidget::showValue(void)
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    QPalette psrc2 = psrc;
    psrc2.setColor(QPalette::WindowText, Qt::white);

    // SPO2_2
    if (_isAlarmSPO2_2 && _spo2String2 != InvStr())
    {
        showAlarmStatus(_spo2Value2);
        showAlarmParamLimit(_spo2Value2, _spo2String2, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        _spo2Value2->setPalette(psrc2);
    }

    // SPO2_D
    if (_isAlarmSPO2_D && _spo2StringD != InvStr())
    {
        showAlarmStatus(_spo2DeltaValue);
        showAlarmParamLimit(_spo2DeltaValue, _spo2StringD, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        _spo2DeltaValue->setPalette(psrc2);
    }

    // SPO2
    if (_isAlarmSPO2 && _spo2String1 != InvStr())
    {
        showAlarmStatus(_spo2Value1);
        showAlarmParamLimit(_spo2Value1, _spo2String1, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(_spo2Value1, psrc);
    }

    _spo2Bar->setPalette(psrc);
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void SPO2TrendWidget::setTextSize()
{
    QRect r = this->rect();
    r.adjust(nameLabel->width(), 0, 0, 0);
    r.setWidth(r.width() / 2);
    // 字体。
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);
    _spo2Value1->setFont(font);

    r.setHeight(r.height() / 2);
    fontsize = fontManager.adjustNumFontSize(r, true);
    font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);
    _spo2Value2->setFont(font);
    _spo2DeltaValue->setFont(font);
    _piValue->setFont(font);

    int fontSize = fontManager.getFontSize(3);
    font = fontManager.textFont(fontSize);
    _spo2Name2->setFont(font);
    _spo2DeltaName->setFont(font);
    _piName->setFont(font);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SPO2TrendWidget::SPO2TrendWidget() : TrendWidget("SPO2TrendWidget")
{
    _isAlarmSPO2 = false;
    _isAlarmSPO2_2 = false;
    _isAlarmSPO2_D = false;
    _spo2String1 = InvStr();
    _spo2String2 = InvStr();
    setName(trs(paramInfo.getParamName(PARAM_SPO2)));
    setUnit(trs(Unit::getSymbol(UNIT_PERCENT)));

    // 血氧值。
    _spo2Value1 = new QLabel();
    _spo2Value1->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _spo2Value1->setText(InvStr());

    // 棒图。
    _spo2Bar = new SPO2BarWidget(0, 15);
    _spo2Bar->setFixedWidth(20);

    QVBoxLayout *vLayout = new QVBoxLayout();
    QHBoxLayout *hLayout = new QHBoxLayout();
    _spo2Name2 = new QLabel();
    _spo2Name2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _spo2Name2->setText(trs(paramInfo.getSubParamName(SUB_PARAM_SPO2_2)));

    _spo2Value2 = new QLabel();
    _spo2Value2->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _spo2Value2->setText(InvStr());
    hLayout->addWidget(_spo2Name2);
    hLayout->addWidget(_spo2Value2);
    vLayout->addLayout(hLayout);

    _spo2DeltaName = new QLabel();
    _spo2DeltaName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _spo2DeltaName->setText(trs("SPO2_D"));

    _spo2DeltaValue = new QLabel();
    _spo2DeltaValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _spo2DeltaValue->setText(InvStr());

    hLayout = new QHBoxLayout();
    hLayout->addWidget(_spo2DeltaName);
    hLayout->addWidget(_spo2DeltaValue);
    vLayout->addLayout(hLayout);

    _piName = new QLabel();
    _piName->setText(trs("PI"));

    _piValue = new QLabel();
    _piValue->setText(InvStr());

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(10);
    layout->addWidget(_spo2Value1, 3);
    layout->addWidget(_spo2Bar, 1);
    layout->addWidget(_piName, 1, Qt::AlignCenter);
    layout->addWidget(_piValue, 2, Qt::AlignBottom | Qt::AlignHCenter);
    layout->addLayout(vLayout);

    contentLayout->addLayout(layout, 7);

    updateTrendWidget();

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

void SPO2TrendWidget::updateTrendWidget()
{
    if (spo2Param.isConnected(true))
    {
        _spo2Name2->setVisible(true);
        _spo2Value2->setVisible(true);
        _spo2DeltaName->setVisible(true);
        _spo2DeltaValue->setVisible(true);
        _piName->setVisible(false);
        _piValue->setVisible(false);
    }
    else
    {
        _spo2Name2->setVisible(false);
        _spo2Value2->setVisible(false);
        _spo2DeltaName->setVisible(false);
        _spo2DeltaValue->setVisible(false);
        _piName->setVisible(true);
        _piValue->setVisible(true);
    }
}

void SPO2TrendWidget::setBarValue(int16_t value)
{
    if (value == InvData())
    {
        _spo2Bar->setValue(value, false);
    }
    else
    {
        _spo2Bar->setValue(value, true);
    }
}

void SPO2TrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    QPalette psrc2 = psrc;
    psrc2.setColor(QPalette::WindowText, Qt::white);
    showNormalStatus(_spo2Value2, psrc2);
    showNormalStatus(_spo2DeltaValue, psrc2);
    showNormalStatus(_spo2Value1, psrc);
    showNormalStatus(_spo2Bar, psrc);
}
