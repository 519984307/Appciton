/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/7
 **/


#include <QHBoxLayout>
#include "TEMPTrendWidget.h"
#include "UnitManager.h"
#include "ParamManager.h"
#include "ParamInfo.h"
#include "PublicMenuManager.h"
#include "TEMPParam.h"
#include "WindowManager.h"
#include "AlarmLimitMenu.h"
#include "TrendWidgetLabel.h"
#include "MeasureSettingWindow.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void TEMPTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("TEMPMenu"));
}

/**************************************************************************************************
 * 报警指示处理。
 *************************************************************************************************/
void TEMPTrendWidget::_alarmIndicate(bool isAlarm)
{
    QPalette p = palette();
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_TEMP));

    if (isAlarm)
    {
        p.setColor(QPalette::Window, Qt::white);
        p.setColor(QPalette::WindowText, Qt::red);
        setPalette(p);
        _t1Value->setPalette(p);
        _t2Value->setPalette(p);
        _tdValue->setPalette(p);
    }
    else
    {
        setPalette(psrc);
        _t1Value->setPalette(psrc);
        _t2Value->setPalette(psrc);
        _tdValue->setPalette(psrc);
    }

    updateAlarm(isAlarm);
}

/**************************************************************************************************
 * 设置TEMP的值。
 *************************************************************************************************/
void TEMPTrendWidget::setTEMPValue(int16_t t1, int16_t t2, int16_t td)
{
    UnitType type = tempParam.getUnit();
    if (type == UNIT_TC)
    {
        if (t1 == InvData())
        {
            _t1Str = InvStr();
        }
        else
        {
            if (t1 < 0)
            {
                _t1Str = "< 0";
            }
            else if (t1 > 500)
            {
                _t1Str = "> 50";
            }
            else
            {
//                _t1Str.sprintf("%.1f", t1 / 10.0);
                _t1Str = QString("%1").number(t1 / 10.0 , 'f' , 1);
            }
        }

        if (t2 == InvData())
        {
            _t2Str = InvStr();
        }
        else
        {
            if (t2 < 0)
            {
                _t2Str = "< 0";
            }
            else if (t2 > 500)
            {
                _t2Str = "> 50";
            }
            else
            {
//                _t2Str.sprintf("%.1f", t2 / 10.0);
                _t2Str = QString("%1").number(t2 / 10.0 , 'f' , 1);
            }
        }

        if (td == InvData())
        {
            _tdStr = InvStr();
        }
        else
        {
//            _tdStr.sprintf("%.1f", td / 10.0);
            _tdStr = QString("%1").number(td / 10.0 , 'f' , 1);
        }

        return;
    }

    if (t1 == InvData())
    {
        _t1Str = InvStr();
    }
    else
    {
        if (t1 < 0)
        {
            _t1Str = "< " + Unit::convert(type, UNIT_TC, 0);
        }
        else if (t1 > 500)
        {
            _t1Str = "> " + Unit::convert(type, UNIT_TC, 50);
        }
        else
        {
            _t1Str = Unit::convert(type, UNIT_TC, t1 / 10.0);
        }
    }

    if (t2 == InvData())
    {
        _t2Str = InvStr();
    }
    else
    {
        if (t2 < 0)
        {
            _t2Str = "< " + Unit::convert(type, UNIT_TC, 0);
        }
        else if (t2 > 500)
        {
            _t2Str = "< " + Unit::convert(type, UNIT_TC, 50);
        }
        else
        {
            _t2Str = Unit::convert(type, UNIT_TC, t2 / 10.0);
        }
    }

    if (td == InvData())
    {
        _tdStr = InvStr();
    }
    else
    {
//        _tdStr = Unit::convert(type, UNIT_TC, td / 10.0);
//        _tdStr.sprintf("%.1f", _tdStr.toDouble() - 32);
//        _tdStr.sprintf("%.1f", fabs(_t1Str.toDouble() - _t2Str.toDouble()));
        _tdStr = QString("%1").number(fabs(_t1Str.toDouble() - _t2Str.toDouble())
                                      , 'f'
                                      , 1);
    }
}

/**************************************************************************************************
 * 设置TEMP的单位。
 *************************************************************************************************/
void TEMPTrendWidget::setUNit(UnitType u)
{
    switch (u)
    {
    case UNIT_TC:
        setUnit("Td (" + Unit::localeSymbol(UNIT_TC) + ")");
        break;

    case UNIT_TF:
        setUnit("Td (" + Unit::localeSymbol(UNIT_TF) + ")");
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void TEMPTrendWidget::isAlarm(int id, bool flag)
{
    switch (id)
    {
    case SUB_PARAM_T1:
        _t1Alarm = flag;
        break;

    case SUB_PARAM_T2:
        _t2Alarm = flag;
        break;

    case SUB_PARAM_TD:
        _tdAlarm = flag;
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 显示报警。
 *************************************************************************************************/
void TEMPTrendWidget::showValue(void)
{
    if (_t1Str != InvStr())
    {
        _alarmIndicate(_t1Alarm);
        _t1Value->setText(_t1Str);
    }
    if (_t2Str != InvStr())
    {
        _alarmIndicate(_t2Alarm);
        _t2Value->setText(_t2Str);
    }
    if (_tdStr != InvStr())
    {
        _alarmIndicate(_tdAlarm);
        _tdValue->setText(_tdStr);
    }
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void TEMPTrendWidget::setTextSize()
{
    QRect r;
    int widgetWidth = width();
    int nameLabelWidth = nameLabel->width();
    r.setSize(QSize((widgetWidth- nameLabelWidth)/2, height()/2));
    // 字体。
    int fontsize = fontManager.adjustNumFontSize(r, true, "3888");
    int nameFontSize = fontsize / 2;
    QFont font = fontManager.numFont(fontsize, true);
    QFont nameFont = fontManager.numFont(nameFontSize);
    font.setWeight(QFont::Black);

    _t1Value->setFont(font);
    _t2Value->setFont(font);
    _tdValue->setFont(font);

    _t1Name->setFont(nameFont);
    _t2Name->setFont(nameFont);
    _tdName->setFont(nameFont);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TEMPTrendWidget::TEMPTrendWidget() : TrendWidget("TEMPTrendWidget")
{
    _t1Alarm = false;
    _t2Alarm = false;
    _tdAlarm = false;
    _t1Str = InvStr();
    _t2Str = InvStr();
    _tdStr = InvStr();
//    setFocusPolicy(Qt::NoFocus);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_TEMP));
    setPalette(palette);

    // 标签设定。
    setName(trs("TEMP"));

    // 设置单位。
    UnitType u = tempParam.getUnit();
    if (UNIT_TC == u)
    {
        setUnit(Unit::localeSymbol(UNIT_TC));
    }
    else
    {
        setUnit(Unit::localeSymbol(UNIT_TF));
    }

    QHBoxLayout *temperature = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);

    _t1Name = new QLabel();
    _t1Name->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    _t1Name->setPalette(palette);
    _t1Name->setText(trs("T1"));
    _t1Value = new QLabel();
    _t1Value->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _t1Value->setPalette(palette);
    _t1Value->setText(InvStr());
    temperature->addWidget(_t1Name);
    temperature->addWidget(_t1Value);
    vLayout->addStretch(1);
    vLayout->addLayout(temperature);
    vLayout->addStretch(1);

    _t2Name = new QLabel();
    _t2Name->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    _t2Name->setPalette(palette);
    _t2Name->setText(trs("T2"));
    _t2Value = new QLabel();
    _t2Value->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _t2Value->setPalette(palette);
    _t2Value->setText(InvStr());
    temperature = new QHBoxLayout;
    temperature->addWidget(_t2Name);
    temperature->addWidget(_t2Value);
    vLayout->addLayout(temperature);
    vLayout->addStretch(1);
    mainLayout->addStretch(1);
    mainLayout->addLayout(vLayout);
    mainLayout->addStretch(1);

    _tdName = new QLabel();
    _tdName->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    _tdName->setPalette(palette);
    _tdName->setText(trs("TD"));
    _tdValue = new QLabel();
    _tdValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _tdValue->setPalette(palette);
    _tdValue->setText(InvStr());
    temperature = new QHBoxLayout;
    temperature->addWidget(_tdName);
    temperature->addWidget(_tdValue);
    vLayout = new QVBoxLayout;
    vLayout->addStretch(1);
    vLayout->addLayout(temperature);
    vLayout->addStretch(1);
    mainLayout->addLayout(vLayout);
    mainLayout->addStretch(1);

    contentLayout->addLayout(mainLayout);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TEMPTrendWidget::~TEMPTrendWidget()
{
}
