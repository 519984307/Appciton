/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "AGTrendWidget.h"
#include "ColorManager.h"
#include "FontManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include "TrendWidgetLabel.h"
#include "AGSymbol.h"
#include <QDebug>
#include "MeasureSettingWindow.h"
#include "AlarmConfig.h"
#include "ParamManager.h"

/**************************************************************************************************
 * 设置麻醉剂类型。
 *************************************************************************************************/
void AGTrendWidget::setAnestheticType(AGAnaestheticType type)
{
    if (type == AG_ANAESTHETIC_NO)
    {
        setName((QString)AGSymbol::convert(_gasType));
        _fiName->setText("Fi");
    }
    else
    {
        setName((QString)AGSymbol::convert(type));
        _fiName->setText("Fi");
    }
}

void AGTrendWidget::updateLimit()
{
    SubParamID id;
    switch (_gasType)
    {
    case AG_TYPE_N2O:
        id = SUB_PARAM_ETN2O;
        break;
    case AG_TYPE_AA1:
        id = SUB_PARAM_ETAA1;
        break;
    case AG_TYPE_AA2:
        id = SUB_PARAM_ETAA2;
        break;
    case AG_TYPE_O2:
        id = SUB_PARAM_ETO2;
        break;
    default:
        break;
    }
    UnitType unitType = paramManager.getSubParamUnit(PARAM_AG, id);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(id, unitType);
    setLimit(config.highLimit, config.lowLimit, config.scale);
}

/**************************************************************************************************
 * 设置et结果数据。
 *************************************************************************************************/
void AGTrendWidget::setEtData(int16_t etValue)
{
    if (etValue == InvData())
    {
        _etStr = InvStr();
    }
    else
    {
        if (_gasType == AG_TYPE_N2O || _gasType == AG_TYPE_O2)
        {
            _etStr = QString::number(etValue);
        }
        else
        {
            int16_t etValueInt = etValue / 10;
            int16_t etValueDes = etValue % 10;
            _etStr = QString::number(etValueInt) + "." + QString::number(etValueDes);
        }
    }

    _etValue->setText(_etStr);
}

/**************************************************************************************************
 * 设置fi结果数据。
 *************************************************************************************************/
void AGTrendWidget::setFiData(int16_t fiValue)
{
    if (fiValue == InvData())
    {
        _fiStr = InvStr();
    }
    else if (_gasType == AG_TYPE_N2O || _gasType == AG_TYPE_O2)
    {
        _fiStr = QString::number(fiValue);
    }
    else
    {
        unsigned char fiValueInt = fiValue / 10;
        unsigned char fiValueDes = fiValue % 10;
        _fiStr = QString::number(fiValueInt) + "." + QString::number(fiValueDes);
    }

    _fiValue->setText(_fiStr);
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void AGTrendWidget::isAlarm(int id, bool flag)
{
    SubParamID subID = (SubParamID)id;
    switch (subID)
    {
    case SUB_PARAM_ETN2O:
    case SUB_PARAM_ETAA1:
    case SUB_PARAM_ETAA2:
    case SUB_PARAM_ETO2:
        _etAlarm = flag;
        break;
    case SUB_PARAM_FIN2O:
    case SUB_PARAM_FIAA1:
    case SUB_PARAM_FIAA2:
    case SUB_PARAM_FIO2:
        _fiAlarm = flag;
        break;
    default:
        break;
    }

    updateAlarm(_etAlarm || _fiAlarm);
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void AGTrendWidget::showValue()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    if (_fiAlarm || _etAlarm)
    {
        if (!_etAlarm)
        {
            showNormalStatus(_etValue, psrc);
        }

        if (!_fiAlarm)
        {
            showNormalStatus(_fiValue, psrc);
        }

        if (_etAlarm)
        {
            showAlarmStatus(_etValue);
            showAlarmParamLimit(_etValue, _etStr, psrc);
        }

        if (_fiAlarm)
        {
            showAlarmStatus(_fiValue);
        }

        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AGTrendWidget::AGTrendWidget(const QString &trendName, const AGTypeGas gasType)
    : TrendWidget(trendName), _gasType(gasType), _etAlarm(false), _fiAlarm(false)
{
    _etStr = InvStr();
    _fiStr = InvStr();

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    setPalette(palette);

    setName((QString)AGSymbol::convert(gasType));
    setUnit("%");

    // 设置上下限
    updateLimit();

    // 设置报警关闭标志
    showAlarmOff();

    // 构造资源。
    _etName = new QLabel();
    _etName->setPalette(palette);
    _etName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _etName->setText("Et");

    _etValue = new QLabel();
    _etValue->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _etValue->setPalette(palette);
    _etValue->setText(InvStr());

    _fiName = new QLabel();
    _fiName->setPalette(palette);
    _fiName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _fiName->setText("Fi");

    _fiValue = new QLabel();
    _fiValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _fiValue->setPalette(palette);
    _fiValue->setText(InvStr());

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(_etName, 1);
    layout->addWidget(_etValue, 3);
    layout->addWidget(_fiName, 1);
    layout->addWidget(_fiValue, 3);
    contentLayout->addLayout(layout, 7);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AGTrendWidget::~AGTrendWidget()
{
}

QList<SubParamID> AGTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    switch (_gasType) {
    case AG_TYPE_CO2:
        list << SUB_PARAM_ETCO2 << SUB_PARAM_FICO2;
        break;
    case AG_TYPE_N2O:
        list << SUB_PARAM_ETN2O << SUB_PARAM_FIN2O;
        break;
    case AG_TYPE_AA1:
        list << SUB_PARAM_ETAA1 << SUB_PARAM_FIAA1;
        break;
    case AG_TYPE_AA2:
        list << SUB_PARAM_ETAA2 << SUB_PARAM_FIAA2;
        break;
    case AG_TYPE_O2:
        list << SUB_PARAM_ETO2 << SUB_PARAM_FIO2;
        break;
    default:
        break;
    }
    return list;
}

void AGTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    showNormalStatus(psrc);
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void AGTrendWidget::setTextSize()
{
    QRect r;
    int h = height();
    int w = (width() - nameLabel->width()) / 2;
    r.setSize(QSize(w, h));

    int fontsize = fontManager.adjustNumFontSize(r, true, "2222");
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);
    _etValue->setFont(font);

    font = fontManager.numFont(fontsize/1.5, true);
    font.setWeight(QFont::Black);
    _fiValue->setFont(font);

    int fontSize = fontManager.getFontSize(3);
    font = fontManager.textFont(fontSize);
    _fiName->setFont(font);
    _etName->setFont(font);
}


/**************************************************************************************************
 * 趋势槽函数。
 *************************************************************************************************/
void AGTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("AGMenu"));
}
