/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "NIBPTrendWidget.h"
#include "NIBPParam.h"
#include "ParamManager.h"
#include "ParamInfo.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "TimeDate.h"
#include "TimeManager.h"
#include "WindowManager.h"
#include "IConfig.h"
#include "TrendWidgetLabel.h"
#include "MeasureSettingWindow.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void NIBPTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("NIBPMenu"));
}

/**************************************************************************************************
 * 设置测量结果的数据。
 *************************************************************************************************/
void NIBPTrendWidget::setResults(int16_t sys, int16_t dia, int16_t map, unsigned time)
{
    //当测量结束，实时压力值显示“---”
    _pressureString = InvStr();

    if (0 == time)
    {
        _measureTime = "";
    }
    else
    {
        QString timeStr("@ ");
        QString tmpStr;
        timeDate.getTime(time , tmpStr , false);
//        tmpStr.sprintf("%02d:%02d", timeDate.getTimeHour(time), timeDate.getTimeMinute(time));
        timeStr += tmpStr;
        _measureTime = timeStr;
    }

    saveResults(sys , dia , map , time);

    if ((sys == InvData()) || (dia == InvData()) || (map == InvData()))
    {
        _effective = false;
        setShowStacked(2);//显示"---"
        _sysString = InvStr();
        _diaString = InvStr();
        _mapString = InvStr();
        return;
    }

    UnitType unit = nibpParam.getUnit();
    if (unit == UNIT_MMHG)
    {
        _sysString = QString::number(sys);
        _diaString = QString::number(dia);
        _mapString = "(" + QString::number(map) + ")";
    }
    else
    {
        _sysString = Unit::convert(unit, UNIT_MMHG, sys);
        _diaString = Unit::convert(unit, UNIT_MMHG, dia);
        _mapString = Unit::convert(unit, UNIT_MMHG, map);
    }

    showValue();
    _effective = true;
    setShowStacked(0);//显示测量结果
}

/**************************************************************************************************
 * 恢复测量结果的数据。
 *************************************************************************************************/
void NIBPTrendWidget::recoverResults(int16_t &sys, int16_t &dia, int16_t &map, unsigned &time)
{
    QString str = InvStr();

    systemConfig.getStrValue("PrimaryCfg|NIBP|NIBPsys", str);
    sys = str.toShort();

    systemConfig.getStrValue("PrimaryCfg|NIBP|NIBPdia", str);
    dia = str.toShort();

    systemConfig.getStrValue("PrimaryCfg|NIBP|NIBPmap", str);
    map = str.toShort();

    systemConfig.getStrValue("PrimaryCfg|NIBP|MeasureTime", str);
    time = str.toInt();
}

/**************************************************************************************************
 * 保存测量结果的数据。
 *************************************************************************************************/
void NIBPTrendWidget::saveResults(int16_t sys, int16_t dia, int16_t map, unsigned time)
{
    systemConfig.setStrValue("PrimaryCfg|NIBP|NIBPsys", QString::number(sys));
    systemConfig.setStrValue("PrimaryCfg|NIBP|NIBPdia", QString::number(dia));
    systemConfig.setStrValue("PrimaryCfg|NIBP|NIBPmap", QString::number(map));
    systemConfig.setStrValue("PrimaryCfg|NIBP|MeasureTime", QString::number(time));

    showValue();
}

/**************************************************************************************************
 * 设置实时袖带压。
 *************************************************************************************************/
void NIBPTrendWidget::setCuffPressure(int p)
{
    setShowStacked(1);
    UnitType unit = nibpParam.getUnit();
    if (unit == UNIT_MMHG)
    {
        _pressureString = QString::number(p);
        return;
    }

    _pressureString = Unit::convert(unit, UNIT_MMHG, p);
}

/**************************************************************************************************
 * 设置倒计时。
 *************************************************************************************************/
void NIBPTrendWidget::setCountdown(int t)
{
    if (_stackedwidget->currentIndex() == 1)
    {
        return;
    }
    if (t == -1)
    {
        setCountDown("");
//        _countDown->setVisible(false);
        return;
    }

    int hour = t / 3600;
    int min = (t - hour * 3600) / 60;
    int sec = t % 60;
    QString str;
    if (hour != 0)
    {
//        str.sprintf("%.2d:%.2d:%.2d", hour, min, sec);
        str = QString("%1:%2:%3").arg(hour).arg(min).arg(sec);
    }
    else
    {
//        str.sprintf("%.2d:%.2d", min, sec);
        str = QString("%1:%2").arg(min).arg(sec);
    }
//    setInfo(str);
    setCountDown(str);
//    _countDown->setVisible(true);
}

/**************************************************************************************************
 * 倒计时时间。
 *************************************************************************************************/
void NIBPTrendWidget::setCountDown(const QString &time)
{
    _countDown->setText(time);
}

/**************************************************************************************************
 * 单位更改。
 *************************************************************************************************/
void NIBPTrendWidget::setUNit(UnitType unit)
{
    setUnit(Unit::getSymbol(unit));
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void NIBPTrendWidget::isAlarm(int id, bool flag)
{
    switch (id)
    {
    case SUB_PARAM_NIBP_SYS:
        _sysAlarm = flag;
        break;
    case SUB_PARAM_NIBP_DIA:
        _diaAlarm = flag;
        break;
    case SUB_PARAM_NIBP_MAP:
        _mapAlarm = flag;
        break;
    default:
        break;
    }

    updateAlarm(_sysAlarm || _diaAlarm || _mapAlarm);
}

/**************************************************************************************************
 * 显示血压值。
 *************************************************************************************************/
void NIBPTrendWidget::showValue(void)
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    psrc = normalPalette(psrc);
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
        }

        if (_sysAlarm)
        {
            showAlarmStatus(_sysValue, psrc);
        }

        if (_diaAlarm)
        {
            showAlarmStatus(_diaValue, psrc);
        }

        if (_mapAlarm)
        {
            showAlarmStatus(_mapValue, psrc);
        }
    }
    else
    {
        showNormalStatus(_sysValue, psrc);
        showNormalStatus(_diaValue, psrc);
        showNormalStatus(_mapValue, psrc);
        showNormalStatus(_nibpValue, psrc);
        showNormalStatus(_pressureValue, psrc);
        showNormalStatus(_lastMeasureCount, psrc);
    }
    _sysValue->setText(_sysString);
    _diaValue->setText(_diaString);
    _mapValue->setText(_mapString);
    _pressureValue->setText(_pressureString);
    _lastMeasureCount->setText(_measureTime);
}

/**************************************************************************************************
 * NIBP显示栏设置。
 * 0、NIBP测量结果
 * 1、NIBP实时压力值
 * 2、提示信息
 * 3、倒计时
*************************************************************************************************/
void NIBPTrendWidget::setShowStacked(int num)
{
    //当开始测量时，将上次测量时间清除
    if (num == 1)
    {
        _mapString = InvStr();
        _pressureString = InvStr();
        _lastMeasureCount->setText("");
//        _countDown->setVisible(false);
    }
    _stackedwidget->setCurrentIndex(num);
}

/**************************************************************************************************
 * 显示上次测量的时间。
 *************************************************************************************************/
void NIBPTrendWidget::setShowMeasureCount(bool lastMeasureCountflag)
{
    if (lastMeasureCountflag)
    {
        _lastMeasureCount->setVisible(true);
    }
    else
    {
        _lastMeasureCount->setVisible(false);
    }
}

/**************************************************************************************************
 * 显示提示信息。
 *************************************************************************************************/
void NIBPTrendWidget::showText(QString text)
{
    if (text == InvStr())
    {
        //有效测量数据
        if (_effective)
        {
            if (_stackedwidget->currentIndex() != 0)
            {
                setShowStacked(0);
            }
            return;
        }
        _message->setFont(fontManager.numFont(_messageInvFontSize, true));
    }
    else
    {
        _message->setFont(fontManager.numFont(_messageFontSize, true));
    }

    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    psrc = normalPalette(psrc);
    if (!_sysAlarm && !_diaAlarm && !_mapAlarm)
    {
        showNormalStatus(_message, psrc);
    }

    _message->setText(text);
    setShowStacked(2);
}

/**************************************************************************************************
 * 显示模式。
 *************************************************************************************************/
void NIBPTrendWidget::showModelText(const QString &text)
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    psrc = normalPalette(psrc);
    if (!_sysAlarm && !_diaAlarm && !_mapAlarm)
    {
        showNormalStatus(_model, psrc);
    }
    _model->setText(text);
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void NIBPTrendWidget::setTextSize()
{
    QRect r;
    r.setSize(QSize(((width() - nameLabel->width()) / 4), ((height() / 4) * 3)));
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _nibpValue->setFont(font);
    _sysValue->setFont(font);
    _diaValue->setFont(font);
    _pressureValue->setFont(font);

    font = fontManager.numFont(fontsize - 10, true);
    font.setWeight(QFont::Black);

    _mapValue->setFont(font);

    r.setSize(QSize(width() - nameLabel->width(), height()));
    _messageInvFontSize = fontManager.adjustNumFontSize(r, true);
    r.setSize(QSize((width() - nameLabel->width()) *3 / 4, (height() / 3)));
    _messageFontSize = fontManager.adjustNumFontSize(r, true);

    if (_message->text() == InvStr())
    {
        _message->setFont(fontManager.numFont(_messageInvFontSize, true));
    }
    else
    {
        _message->setFont(fontManager.numFont(_messageFontSize, true));
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPTrendWidget::NIBPTrendWidget()
    : TrendWidget("NIBPTrendWidget")
    , _sysString(InvStr()), _diaString(InvStr())
    , _mapString(QString("(") + InvStr() + QString(")"))
    , _pressureString(InvStr()), _measureTime("")
    , _sysAlarm(false), _diaAlarm(false)
    , _mapAlarm(false), _effective(false)
    , _messageFontSize(100), _messageInvFontSize(100)
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    setPalette(palette);
    setName(trs(paramInfo.getParamName(PARAM_NIBP)));
    setUnit(Unit::getSymbol(nibpParam.getUnit()));

    // 构造出所有控件。
    _nibpValue = new QLabel();
    _nibpValue->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    _nibpValue->setPalette(palette);
    _nibpValue->setText("/");

    _sysValue = new QLabel();
    _sysValue->setAlignment(Qt::AlignCenter);
    _sysValue->setPalette(palette);
    _sysValue->setText(InvStr());

    _diaValue = new QLabel();
    _diaValue->setAlignment(Qt::AlignCenter);
    _diaValue->setPalette(palette);
    _diaValue->setText(InvStr());

    _mapValue = new QLabel();
    _mapValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _mapValue->setPalette(palette);
    _mapValue->setText(InvStr());

    _pressureValue = new QLabel();
    _pressureValue->setAlignment(Qt::AlignCenter);
    _pressureValue->setPalette(palette);
    _pressureValue->setText(InvStr());

    int fontSize = fontManager.getFontSize(7);

    _countDown = new QLabel();
    _countDown->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _countDown->setPalette(palette);
    _countDown->setText("");
    _countDown->setFont(fontManager.textFont(fontSize));
    _countDown->setVisible(false);

    _lastMeasureCount = new QLabel();
    _lastMeasureCount->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _lastMeasureCount->setPalette(palette);
    _lastMeasureCount->setFont(fontManager.textFont(fontSize));
//    _lastMeasureCount->setText("");

    _model = new QLabel();
    _model->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _model->setPalette(palette);
    _model->setFont(fontManager.textFont(fontSize));
    _model->setText("");

    _message = new QLabel();
    _message->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _message->setPalette(palette);
    _message->setFont(fontManager.textFont(_messageFontSize));
    _message->setText(InvStr());

    // 将数值显示和倒计时放到一起。
    QWidget *groupBox0 = new QWidget();
    QHBoxLayout *hLayout0 = new QHBoxLayout(groupBox0);
    hLayout0->setMargin(_margin);
    hLayout0->setSpacing(10);
    hLayout0->addStretch();
    hLayout0->addWidget(_sysValue);
    hLayout0->addWidget(_nibpValue);
    hLayout0->addWidget(_diaValue);
    hLayout0->addWidget(_mapValue);
    hLayout0->addStretch();

    QWidget *groupBox1 = new QWidget();
    QVBoxLayout *vLayout2 = new QVBoxLayout(groupBox1);
    vLayout2->setMargin(1);
    vLayout2->setSpacing(0);
    vLayout2->addWidget(_pressureValue, 0, Qt::AlignCenter);

    QWidget *groupBox2 = new QWidget();
    QVBoxLayout *vLayout3 = new QVBoxLayout(groupBox2);
    vLayout3->setMargin(1);
    vLayout3->setSpacing(0);
    vLayout3->addWidget(_message, 0, Qt::AlignCenter);

    QWidget *groupBox3 = new QWidget();
    QVBoxLayout *vLayout4 = new QVBoxLayout(groupBox3);
    vLayout4->setMargin(1);
    vLayout4->setSpacing(0);
    vLayout4->addWidget(_countDown, 0, Qt::AlignCenter);

    _stackedwidget = new QStackedWidget();
    _stackedwidget->addWidget(groupBox0);
    _stackedwidget->addWidget(groupBox1);
    _stackedwidget->addWidget(groupBox2);
    _stackedwidget->addWidget(groupBox3);
    _stackedwidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    // 将最后测量时间和模式放到一起。
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(5, 0, 5, 0);
    hLayout->addStretch();
    hLayout->addWidget(_lastMeasureCount);
    hLayout->addWidget(_model);
    hLayout->addStretch();

    // 第二组。
    QVBoxLayout *vLayout5 = new QVBoxLayout();
    vLayout5->setMargin(1);
    vLayout5->setSpacing(0);
    vLayout5->addWidget(_stackedwidget, 1);
    vLayout5->addLayout(hLayout);

    contentLayout->addLayout(vLayout5, 1);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPTrendWidget::~NIBPTrendWidget()
{
}

QList<SubParamID> NIBPTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list << SUB_PARAM_NIBP_SYS << SUB_PARAM_NIBP_DIA << SUB_PARAM_NIBP_MAP;
    return list;
}
