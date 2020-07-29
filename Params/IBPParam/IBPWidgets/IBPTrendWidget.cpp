/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/7
 **/


#include "IBPTrendWidget.h"
#include "IBPParam.h"
#include "TrendWidgetLabel.h"
#include "ParamManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QGroupBox>
#include "MeasureSettingWindow.h"
#include "AlarmConfig.h"
#include "ParamManager.h"

/**************************************************************************************************
 * 设置测量结果的数据。
 *************************************************************************************************/
void IBPTrendWidget::setData(int16_t sys, int16_t dia, int16_t map)
{
    if ((_entitle < IBP_LABEL_CVP) || (_entitle > IBP_LABEL_ICP))
    {
        if (!_isZero)
        {
            setShowStacked(0);
        }
        else if ((sys == InvData()) || (dia == InvData()) || (map == InvData()))
        {
            setShowStacked(1);
            _sysString = InvStr();
            _diaString = InvStr();
            _mapString = InvStr();
        }
        else
        {
            setShowStacked(1);
            UnitType unit = ibpParam.getUnit();
            _sysString = Unit::convert(unit, UNIT_MMHG, sys);
            _diaString = Unit::convert(unit, UNIT_MMHG, dia);
            _mapString = "(" + Unit::convert(unit, UNIT_MMHG, map) + ")";
        }
    }
    else
    {
        if (!_isZero)
        {
            setShowStacked(0);
        }
        else if (map == InvData())
        {
            setShowStacked(2);
            _veinString = InvStr();
        }
        else
        {
            setShowStacked(2);
            UnitType unit = ibpParam.getUnit();
            _veinString = Unit::convert(unit, UNIT_MMHG, map);
            _mapString = _veinString;
        }
    }

    _sysValue->setText(_sysString);
    _diaValue->setText(_diaString);
    _mapValue->setText(_mapString);
    _veinValue->setText(_veinString);

    return;
}

void IBPTrendWidget::updateLimit()
{
    SubParamID id;
    switch (_entitle)
    {
    case IBP_LABEL_ART:
        id = SUB_PARAM_ART_SYS;
        break;
    case IBP_LABEL_PA:
        id = SUB_PARAM_PA_SYS;
        break;
    case IBP_LABEL_CVP:
        id = SUB_PARAM_CVP_MAP;
        break;
    case IBP_LABEL_LAP:
        id = SUB_PARAM_LAP_MAP;
        break;
    case IBP_LABEL_RAP:
        id = SUB_PARAM_RAP_MAP;
        break;
    case IBP_LABEL_ICP:
        id = SUB_PARAM_ICP_MAP;
        break;
    case IBP_LABEL_AUXP1:
        id = SUB_PARAM_AUXP1_SYS;
        break;
    case IBP_LABEL_AUXP2:
        id = SUB_PARAM_AUXP2_SYS;
        break;
    default:
        return;
    }
    UnitType unitType = paramManager.getSubParamUnit(PARAM_IBP, id);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(id, unitType);
    setLimit(config.highLimit, config.lowLimit, config.scale);
}

/**************************************************************************************************
 * 设置标名。
 *************************************************************************************************/
void IBPTrendWidget::setEntitle(IBPLabel entitle)
{
    setName(IBPSymbol::convert(entitle));
    _entitle = entitle;
    // 未校零，提示校零
    if (!_isZero)
    {
        setShowStacked(0);
        return;
    }

    if (entitle >= IBP_LABEL_CVP && entitle <= IBP_LABEL_ICP)
    {
        setShowStacked(2);
    }
    else
    {
        setShowStacked(1);
    }
}

/**************************************************************************************************
 * IBP显示栏设置。
 * 0、提示校零
 * 1、动脉压数据信息
 * 2、静脉压数据信息
*************************************************************************************************/
void IBPTrendWidget::setShowStacked(int num)
{
    _stackedwidget->setCurrentIndex(num);
}

/**************************************************************************************************
 * 设置校零标志。
 *************************************************************************************************/
void IBPTrendWidget::setZeroFlag(bool isEnabled)
{
    _isZero = isEnabled;

    // Zero failed or sensor off, show the "Zero Required" prompt.
    if (!_isZero)
    {
        // reset value
        _sysString = InvStr();
        _diaString = InvStr();
        _mapString = InvStr();
        _veinString = InvStr();
        setShowStacked(0);
        return;
    }
    // After zero successfully, close the "Zero Required" prompt.
    if (_entitle >= IBP_LABEL_CVP && _entitle <= IBP_LABEL_ICP)
    {
        setShowStacked(2);
    }
    else
    {
        setShowStacked(1);
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void IBPTrendWidget::isAlarm(int id, bool flag)
{
    SubParamID subID = (SubParamID)id;
    switch (subID)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP2_SYS:
        _sysAlarm = flag;
        break;
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP2_DIA:
        _diaAlarm = flag;
        break;
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP2_MAP:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_RAP_MAP:
        _mapAlarm = flag;
        break;
    default:
        break;
    }

    updateAlarm(_sysAlarm || _diaAlarm || _mapAlarm);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void IBPTrendWidget::showValue()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_IBP));
    if (_sysAlarm || _diaAlarm || _mapAlarm)
    {
        if (!_sysAlarm)
        {
            showNormalStatus(_sysValue, psrc);
        }

        if (!_diaAlarm)
        {
            showNormalStatus(_diaValue, psrc);
        }

        if (!_mapAlarm)
        {
            showNormalStatus(_mapValue, psrc);
            showNormalStatus(_veinValue, psrc);
        }

        if (_sysAlarm)
        {
            showAlarmStatus(_sysValue);
            switch (_entitle)
            {
            case IBP_LABEL_ART:
            case IBP_LABEL_PA:
            case IBP_LABEL_AUXP1:
            case IBP_LABEL_AUXP2:
                showAlarmParamLimit(_sysValue, _sysString, psrc);
                break;
            default:
                break;
            }
        }

        if (_diaAlarm)
        {
            showAlarmStatus(_diaValue);
        }

        if (_mapAlarm)
        {
            showAlarmStatus(_mapValue);
            showAlarmStatus(_veinValue);
            switch (_entitle)
            {
            case IBP_LABEL_ICP:
            case IBP_LABEL_LAP:
            case IBP_LABEL_RAP:
            case IBP_LABEL_CVP:
                showAlarmParamLimit(_mapValue, _mapString, psrc);
                showAlarmParamLimit(_veinValue, _veinString, psrc);
                break;
            default:
                break;
            }
        }
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(_stackedwidget->currentWidget()->layout(), psrc);
    }
}

void IBPTrendWidget::updateUnit(UnitType unit)
{
    setUnit(Unit::getSymbol(unit));
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPTrendWidget::IBPTrendWidget(const QString &trendName, const IBPChannel &ibpChn)
    : TrendWidget(trendName),  _ibpChn(ibpChn), _entitle(ibpParam.getEntitle(ibpChn)),
      _isZero(false), _sysAlarm(false),
      _diaAlarm(false), _mapAlarm(false)
{
    _sysString = InvStr();
    _diaString = InvStr();
    _mapString = InvStr();
    _veinString = InvStr();

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_IBP));
    setPalette(palette);
    setName(IBPSymbol::convert(_entitle));
    updateUnit(ibpParam.getUnit());

    // 设置上下限
    updateLimit();

    // 设置报警关闭标志
    showAlarmOff();

    // 构造出所有控件。

    _zeroWarn = new QLabel();
    _zeroWarn->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _zeroWarn->setPalette(palette);
    _zeroWarn->setText(trs("ZeroRequired"));

    _ibpValue = new QLabel();
    _ibpValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _ibpValue->setPalette(palette);
    _ibpValue->setText("/");

    _sysValue = new QLabel();
    _sysValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _sysValue->setPalette(palette);
    _sysValue->setText(InvStr());

    _diaValue = new QLabel();
    _diaValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _diaValue->setPalette(palette);
    _diaValue->setText(InvStr());

    _mapValue = new QLabel();
    _mapValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _mapValue->setPalette(palette);
    _mapValue->setText(InvStr());

    _veinValue = new QLabel();
    _veinValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _veinValue->setPalette(palette);
    _veinValue->setText(InvStr());

    // 开始布局
    QGroupBox *_groupBox0 = new QGroupBox();
    _groupBox0->setStyleSheet("border:none");
    QHBoxLayout *hLayout0 = new QHBoxLayout();
    hLayout0->setMargin(_margin);
    hLayout0->addStretch();
    hLayout0->addWidget(_zeroWarn);
    hLayout0->addStretch();
    hLayout0->setAlignment(Qt::AlignVCenter);
    _groupBox0->setLayout(hLayout0);

    QGroupBox *_groupBox1 = new QGroupBox();
    _groupBox1->setStyleSheet("border:none");
    QHBoxLayout *hLayout1 = new QHBoxLayout();
    hLayout1->setMargin(_margin);
    hLayout1->setSpacing(10);
    hLayout1->addStretch();
    hLayout1->addWidget(_sysValue);
    hLayout1->addWidget(_ibpValue);
    hLayout1->addWidget(_diaValue);
    hLayout1->addWidget(_mapValue);
    hLayout1->addStretch();
    hLayout1->setAlignment(Qt::AlignVCenter);
    _groupBox1->setLayout(hLayout1);

    QGroupBox *_groupBox2 = new QGroupBox();
    _groupBox2->setStyleSheet("border:none");
    QHBoxLayout *hLayout2 = new QHBoxLayout();
    hLayout2->setMargin(_margin);
    hLayout2->addStretch();
    hLayout2->addWidget(_veinValue);
    hLayout2->addStretch();
    hLayout2->setAlignment(Qt::AlignVCenter);
    _groupBox2->setLayout(hLayout2);

    _stackedwidget = new QStackedWidget();
    _stackedwidget->addWidget(_groupBox0);
    _stackedwidget->addWidget(_groupBox1);
    _stackedwidget->addWidget(_groupBox2);

    contentLayout->addWidget(_stackedwidget, 7, Qt::AlignCenter);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IBPTrendWidget::~IBPTrendWidget()
{
}

QList<SubParamID> IBPTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    switch (_entitle) {
    case IBP_LABEL_ART:
        list << SUB_PARAM_ART_SYS << SUB_PARAM_ART_DIA << SUB_PARAM_ART_MAP;
        break;
    case IBP_LABEL_PA:
        list << SUB_PARAM_PA_SYS << SUB_PARAM_PA_DIA << SUB_PARAM_PA_MAP;
        break;
    case IBP_LABEL_CVP:
        list << SUB_PARAM_CVP_MAP;
        break;
    case IBP_LABEL_LAP:
        list << SUB_PARAM_LAP_MAP;
        break;
    case IBP_LABEL_RAP:
        list << SUB_PARAM_RAP_MAP;
        break;
    case IBP_LABEL_AUXP1:
        list << SUB_PARAM_AUXP1_SYS << SUB_PARAM_AUXP1_DIA << SUB_PARAM_AUXP1_MAP;
        break;
    case IBP_LABEL_AUXP2:
        list << SUB_PARAM_AUXP2_SYS << SUB_PARAM_AUXP2_DIA << SUB_PARAM_AUXP2_MAP;
        break;
    default:
        break;
    }
    return list;
}

void IBPTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_IBP));
    showNormalStatus(_stackedwidget->currentWidget()->layout(), psrc);
    showNormalStatus(psrc);
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void IBPTrendWidget::setTextSize()
{
    QRect r;
    r.setSize(QSize(((width() - nameLabel->width()) / 5), ((height() / 4) * 3)));
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _ibpValue->setFont(font);
    _sysValue->setFont(font);
    _diaValue->setFont(font);

    _veinValue->setFont(font);

    font = fontManager.numFont(fontsize - 10, true);
    font.setWeight(QFont::Black);
    _mapValue->setFont(font);
    _zeroWarn->setFont(font);

    font = fontManager.numFont(fontsize - 20, false);
    font.setWeight(QFont::Normal);
}

void IBPTrendWidget::loadConfig()
{
    const QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_IBP));
    setPalette(palette);
    _zeroWarn->setPalette(palette);
    _ibpValue->setPalette(palette);
    _sysValue->setPalette(palette);
    _diaValue->setPalette(palette);
    _mapValue->setPalette(palette);
    _veinValue->setPalette(palette);

    _entitle = ibpParam.getEntitle(_ibpChn);
    setEntitle(_entitle);
    updateUnit(ibpParam.getUnit());

    // 设置上下限
    updateLimit();

    // 设置报警关闭标志
    showAlarmOff();
}

void IBPTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("IBPMenu"));
}
