/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "CO2TrendWidget.h"
#include "CO2Param.h"
#include "ParamManager.h"
#include "ColorManager.h"
#include "LanguageManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "PublicMenuManager.h"
#include "TrendWidgetLabel.h"
#include "MeasureSettingWindow.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void CO2TrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("CO2Menu"));
}

/**************************************************************************************************
 * 转换值。
 *************************************************************************************************/
void CO2TrendWidget::_setValue(int16_t v, QString &str)
{
    if (v == InvData())
    {
        str = InvStr();
        return;
    }

    UnitType unit = co2Param.getUnit();
    UnitType dunit = paramInfo.getUnitOfSubParam(SUB_PARAM_ETCO2);
    int mul = paramInfo.getMultiOfSubParam(SUB_PARAM_ETCO2);
    if (unit == dunit)
    {
        if (1 == mul)
        {
            str = QString::number(v);
        }
        else
        {
            str = QString("%1").number(v * 1.0 / mul, 'f', 1);
        }

        return;
    }


    str = Unit::convert(unit, dunit, v * 1.0 / mul, co2Param.getBaro());
}

/**************************************************************************************************
 * 窗口是否使能。
 *************************************************************************************************/
bool CO2TrendWidget::enable()
{
    return co2Param.isEnabled();
}

/**************************************************************************************************
 * 设置ETCO2的值。
 *************************************************************************************************/
void CO2TrendWidget::setEtCO2Value(int16_t v)
{
    _setValue(v, _etco2Str);
}

/**************************************************************************************************
 * 设置FICO2的值。
 *************************************************************************************************/
void CO2TrendWidget::setFiCO2Value(int16_t v)
{
    _setValue(v, _fico2Str);
}

/**************************************************************************************************
 * 单位更改。
 *************************************************************************************************/
void CO2TrendWidget::setUNit(UnitType unit)
{
    setUnit(Unit::getSymbol(unit));
}

/**************************************************************************************************
 * 设置FiCO2显示。
 *************************************************************************************************/
void CO2TrendWidget::setFiCO2Display(CO2FICO2Display disp)
{
    if (disp == CO2_FICO2_DISPLAY_OFF)
    {
        _fico2Label->hide();
        _fico2Value->hide();
        _fico2Value->setText(InvStr());
        setTextSize();
    }
    else if (disp == CO2_FICO2_DISPLAY_ON)
    {
        _fico2Label->show();
        _fico2Value->show();
        _fico2Value->setText(_fico2Str);
        setTextSize();
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void CO2TrendWidget::isAlarm(int id, bool flag)
{
    switch (id)
    {
    case SUB_PARAM_ETCO2:
        _etco2Alarm = flag;
        break;
    case SUB_PARAM_FICO2:
        _fico2Alarm = flag;
        break;
    default:
        break;
    }

    updateAlarm(_fico2Alarm || _etco2Alarm);
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void CO2TrendWidget::showValue()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_CO2));
    QPalette fgColor = normalPalette(psrc);
    if (_fico2Alarm || _etco2Alarm)
    {
        if (!_etco2Alarm)
        {
            showNormalStatus(_etco2Value, fgColor);
        }

        if (!_fico2Alarm)
        {
            showNormalStatus(_fico2Value, fgColor);
            showNormalStatus(_fico2Label, fgColor);
        }

        if (_etco2Alarm)
        {
            showAlarmStatus(_etco2Value, fgColor);
        }

        if (_fico2Alarm)
        {
            showAlarmStatus(_fico2Value, fgColor, false);
            showAlarmStatus(_fico2Label, fgColor, false);
        }
    }
    else
    {
        showNormalStatus(_etco2Value, fgColor);
        showNormalStatus(_fico2Value, fgColor);
        showNormalStatus(_fico2Label, fgColor);
    }

    _etco2Value->setText(_etco2Str);

    if (!_fico2Value->isHidden())
    {
        _fico2Value->setText(_fico2Str);
    }
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void CO2TrendWidget::setTextSize()
{
    QRect r;
    int h = height();
    int w = (width() - nameLabel->width())/2;
    r.setSize(QSize(w, h));
    // 字体。
    int fontsize = fontManager.adjustNumFontSize(r, true, "2222");
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _etco2Value->setFont(font);

    fontsize = fontManager.adjustNumFontSize(r, true, "2222");
    font = fontManager.numFont(fontsize / 1.5, true);
    font.setWeight(QFont::Black);
    _fico2Value->setFont(font);

    QFont fico2Font = fontManager.textFont(fontManager.getFontSize(9));
    _fico2Label->setFont(fico2Font);
}

// 立刻显示数据。
void CO2TrendWidget::flushValue(void)
{
    _fico2Value->setText(_fico2Str);
    _etco2Value->setText(_etco2Str);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CO2TrendWidget::CO2TrendWidget() : TrendWidget("CO2TrendWidget")
{
    _etco2Str = InvStr();
    _fico2Str = InvStr();
    _etco2Alarm = false;
    _fico2Alarm = false;

    // 高度设定为与RESP的一致。
//    setFixedHeight(86);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_CO2));
    setPalette(palette);
    setName(trs(paramInfo.getSubParamName(SUB_PARAM_ETCO2)));
    setUnit(Unit::localeSymbol(co2Param.getUnit()));

//    // 构造资源。
    _etco2Value = new QLabel();
    _etco2Value->setAlignment(Qt::AlignHCenter | Qt::AlignTop|Qt::AlignVCenter);

    _etco2Value->setPalette(palette);
    _etco2Value->setText(InvStr());

    _fico2Label = new QLabel(trs("FICO2"), this);
    _fico2Label->setPalette(palette);
    _fico2Label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    _fico2Value = new QLabel(this);
    _fico2Value->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _fico2Value->setPalette(palette);
    _fico2Value->setText(InvStr());

    contentLayout->addWidget(_etco2Value, 3);
    contentLayout->addWidget(_fico2Label, 1);
    contentLayout->addWidget(_fico2Value, 3);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CO2TrendWidget::~CO2TrendWidget()
{
}

QList<SubParamID> CO2TrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_ETCO2);
    list.append(SUB_PARAM_FICO2);
    return list;
}
