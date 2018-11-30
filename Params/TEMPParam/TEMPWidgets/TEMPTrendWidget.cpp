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
#include "TEMPParam.h"
#include "WindowManager.h"
#include "TrendWidgetLabel.h"
#include "MeasureSettingWindow.h"
#include "AlarmConfig.h"
#include "ParamManager.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void TEMPTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("TEMPMenuContent"));
}

void TEMPTrendWidget::onTempNameUpdate(TEMPChannelIndex channel, TEMPChannelType type)
{
    if (channel == TEMP_CHANNEL_ONE)
    {
       _t1Name->setText(trs(TEMPSymbol::convert(type)));
    }
    else
    {
       _t2Name->setText(trs(TEMPSymbol::convert(type)));
    }
    update();
}

/**************************************************************************************************
 * 设置TEMP的值。
 *************************************************************************************************/
void TEMPTrendWidget::setTEMPValue(int16_t t1, int16_t t2, int16_t td)
{
    UnitType type = tempParam.getUnit();
    if (t1 == InvData() || t1 < 0 || t1 > 500)
    {
        _t1Str = InvStr();
    }
    else
    {
        _t1Str = Unit::convert(type, UNIT_TC, t1 / 10.0);
    }

    if (t2 == InvData() || t2 < 0 || t2 > 500)
    {
        _t2Str = InvStr();
    }
    else
    {
        _t2Str = Unit::convert(type, UNIT_TC, t2 / 10.0);
    }

    if (td == InvData())
    {
        _tdStr = InvStr();
    }
    else
    {
        _tdStr = QString("%1")
                .number(fabs(_t1Str.toDouble()
                             - _t2Str.toDouble()), 'f', 1);
    }

    _t1Value->setText(_t1Str);
    _t2Value->setText(_t2Str);
    _tdValue->setText(_tdStr);
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
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_TEMP));
    if (_t1Alarm || _t2Alarm || _tdAlarm)
    {
        if (!_t1Alarm)
        {
            showNormalStatus(_t1Value, psrc);
        }

        if (!_t2Alarm)
        {
            showNormalStatus(_t2Value, psrc);
        }

        if (!_tdAlarm)
        {
            showNormalStatus(_tdValue, psrc);
        }

        if (_t1Alarm)
        {
            showAlarmStatus(_t1Value);
            showAlarmParamLimit(_t1Value, _t1Str, psrc);
        }

        if (_t2Alarm)
        {
            showAlarmStatus(_t2Value);
        }

        if (_tdAlarm)
        {
            showAlarmStatus(_tdValue);
        }
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
    setName("Temp");

    // 设置报警关闭标志
    showAlarmOff();

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

    QHBoxLayout *hLayout = new QHBoxLayout;
    QVBoxLayout *vLayout = new QVBoxLayout();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(1);
    mainLayout->setSpacing(1);

    _t1Name = new QLabel();
    _t1Name->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    _t1Name->setPalette(palette);
    _t1Name->setText(trs("T1"));
    _t2Name = new QLabel();
    _t2Name->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    _t2Name->setPalette(palette);
    _t2Name->setText(trs("T2"));
    vLayout->addWidget(_t1Name);
    vLayout->addWidget(_t2Name);
    hLayout->addLayout(vLayout);

    _t1Value = new QLabel();
    _t1Value->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _t1Value->setPalette(palette);
    _t1Value->setText(InvStr());
    _t2Value = new QLabel();
    _t2Value->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _t2Value->setPalette(palette);
    _t2Value->setText(InvStr());
    vLayout = new QVBoxLayout();
    vLayout->addWidget(_t1Value);
    vLayout->addWidget(_t2Value);
    hLayout->addLayout(vLayout);
    mainLayout->addStretch(1);
    mainLayout->addLayout(hLayout);

    _tdName = new QLabel();
    _tdName->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    _tdName->setPalette(palette);
    _tdName->setText(trs("TD"));
    _tdValue = new QLabel();
    _tdValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    _tdValue->setPalette(palette);
    _tdValue->setText(InvStr());
    vLayout = new QVBoxLayout();
    vLayout->addStretch();
    vLayout->addWidget(_tdName);
    vLayout->addWidget(_tdValue);
    vLayout->addStretch();
    mainLayout->addStretch(1);
    mainLayout->addLayout(vLayout);
    mainLayout->addStretch(1);

    contentLayout->addLayout(mainLayout, 3);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    // 增加更新温度通道名称链接-- 暂时屏蔽温度通道名称更改链接
//    connect(&tempParam, SIGNAL(updateTempName(TEMPChannelIndex,TEMPChannelType)),
//            this, SLOT(onTempNameUpdate(TEMPChannelIndex,TEMPChannelType)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TEMPTrendWidget::~TEMPTrendWidget()
{
}

QList<SubParamID> TEMPTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_T1);
    list.append(SUB_PARAM_T2);
    return list;
}

void TEMPTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_TEMP));
    showNormalStatus(psrc);
}
