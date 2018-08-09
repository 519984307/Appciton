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
#include "PublicMenuManager.h"
#include "TimeDate.h"
#include "TimeManager.h"
#include "WindowManager.h"
#include "IConfig.h"
#include "TrendWidgetLabel.h"
#include "MainMenuWindow.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void NIBPTrendWidget::_releaseHandle(IWidget *iWidget)
{
    MainMenuWindow *p = MainMenuWindow::getInstance();
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
    if (_stackedwidget0->currentIndex() == 1)
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
    QPalette p = palette();
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    QColor fgColor = (psrc.windowText().color() == Qt::white) ? Qt::black :
            psrc.windowText().color();
    if (_sysAlarm || _diaAlarm || _mapAlarm)
    {
        if (p.window().color() != Qt::white)
        {
            p.setColor(QPalette::Window, Qt::white);
            p.setColor(QPalette::WindowText, Qt::red);
            setPalette(p);
        }

        if (_sysAlarm)
        {
            p.setColor(QPalette::WindowText, Qt::red);
            _sysValue->setPalette(p);
        }
        else
        {
            p.setColor(QPalette::WindowText, fgColor);
            _sysValue->setPalette(p);
        }

        if (_diaAlarm)
        {
            p.setColor(QPalette::WindowText, Qt::red);
            _diaValue->setPalette(p);
        }
        else
        {
            p.setColor(QPalette::WindowText, fgColor);
            _diaValue->setPalette(p);
        }

        if (_mapAlarm)
        {
            p.setColor(QPalette::WindowText, Qt::red);
            _mapValue->setPalette(p);
        }
        else
        {
            p.setColor(QPalette::WindowText, fgColor);
            _mapValue->setPalette(p);
        }
    }
    else
    {
        if (p.window().color() != Qt::black)
        {
            setPalette(psrc);
            _sysValue->setPalette(psrc);
            _diaValue->setPalette(psrc);
            _mapValue->setPalette(psrc);
        }
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
    _stackedwidget0->setCurrentIndex(num);
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
            if (_stackedwidget0->currentIndex() != 0)
            {
                setShowStacked(0);
            }
            return;
        }
        QFont font = fontManager.numFont(40, true);
        font.setWeight(QFont::Black);
        _message->setFont(font);
    }
    else
    {
        _message->setFont(fontManager.numFont(15, true));
    }

    _message->setText(text);
    setShowStacked(2);
}

/**************************************************************************************************
 * 显示模式。
 *************************************************************************************************/
void NIBPTrendWidget::showModelText(QString text)
{
    _model->setText(text);
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void NIBPTrendWidget::setTextSize()
{
    QRect r;
    r.setSize(QSize(((width() - nameLabel->width())/4), ((height() / 4) * 3)));
//    int fontsize = fontManager.adjustNumFontSizeXML(r);
//    int size = fontManager.getFontSize(fontsize);
//    QFont font = fontManager.numFont(size, true);
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
//    font.setStretch(105); // 横向放大。
    font.setWeight(QFont::Black);

    _nibpValue->setFont(font);
    _sysValue->setFont(font);
    _diaValue->setFont(font);
    _pressureValue->setFont(font);

    font = fontManager.numFont(fontsize - 10, true);
//    font.setStretch(105);
    font.setWeight(QFont::Black);

    _mapValue->setFont(font);

//    r.setHeight(height() / 4);
//    // 字体。
////    fontsize = fontManager.adjustNumFontSizeXML(r);
////    size = fontManager.getFontSize(fontsize);
////    font = fontManager.numFont(size, true);
//    fontsize = fontManager.adjustTextFontSize(r);
//    font = fontManager.textFont(fontsize);
////    font.setStretch(105); // 横向放大。
//    font.setWeight(QFont::Black);
//    _countDown->setFont(font);
//    _lastMeasureCount->setFont(font);
//    _model->setFont(font);
//    _message->setFont(font);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPTrendWidget::NIBPTrendWidget() : TrendWidget("NIBPTrendWidget")
{
    _sysString = InvStr();
    _diaString = InvStr();
    _mapString = QString("(") + InvStr() + QString(")");
    _pressureString = InvStr();
    _measureTime = "";
    _sysAlarm = false;
    _diaAlarm = false;
    _mapAlarm = false;
    _effective = false;
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    setPalette(palette);
    setName(trs(paramInfo.getParamName(PARAM_NIBP)));
    setUnit(Unit::getSymbol(nibpParam.getUnit()));

    // 构造出所有控件。
    _nibpValue = new QLabel();
    _nibpValue->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
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

    bar = new BarWidget();
    bar->setPalette(palette);
    bar->setFixedHeight(2);

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
    _message->setFont(fontManager.textFont(fontSize));
    _message->setText(InvStr());

    // 将数值显示和倒计时放到一起。
    QGroupBox *_groupBox0 = new QGroupBox();
    _groupBox0->setStyleSheet("border:none");
    QHBoxLayout *hLayout0 = new QHBoxLayout();
    hLayout0->setMargin(_margin);
    hLayout0->setSpacing(10);
    hLayout0->addWidget(_sysValue);
    hLayout0->addWidget(_nibpValue);
    hLayout0->addWidget(_diaValue);
    hLayout0->addWidget(_mapValue);
    _groupBox0->setLayout(hLayout0);

    QGroupBox *_groupBox1 = new QGroupBox();
    _groupBox1->setStyleSheet("border:none");
    QVBoxLayout *vLayout2 = new QVBoxLayout(_groupBox1);
    vLayout2->setMargin(1);
    vLayout2->setSpacing(0);
    vLayout2->addWidget(_pressureValue);
    _groupBox1->setLayout(vLayout2);

    QGroupBox *_groupBox2 = new QGroupBox();
    _groupBox2->setStyleSheet("border:none");
    QVBoxLayout *vLayout3 = new QVBoxLayout(_groupBox2);
    vLayout3->setMargin(1);
    vLayout3->setSpacing(0);
    vLayout3->addWidget(_message);
    _groupBox2->setLayout(vLayout3);

    QGroupBox *_groupBox3 = new QGroupBox();
    _groupBox3->setStyleSheet("border:none");
    QVBoxLayout *vLayout4 = new QVBoxLayout(_groupBox3);
    vLayout4->setMargin(1);
    vLayout4->setSpacing(0);
    vLayout4->addWidget(_countDown);
    _groupBox3->setLayout(vLayout4);

    _stackedwidget0 = new QStackedWidget();
    _stackedwidget0->addWidget(_groupBox0);
    _stackedwidget0->addWidget(_groupBox1);
    _stackedwidget0->addWidget(_groupBox2);
//    _stackedwidget0->addWidget(_groupBox3);

    // 将最后测量时间和模式放到一起。
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(5, 0, 5, 0);
    hLayout->addWidget(_lastMeasureCount);
    hLayout->addWidget(_model);

    // 第二组。
    QVBoxLayout *vLayout5 = new QVBoxLayout();
    vLayout5->setMargin(1);
    vLayout5->setSpacing(0);
    vLayout5->addWidget(_stackedwidget0);
    vLayout5->addLayout(hLayout);

    contentLayout->addStretch();
    contentLayout->addLayout(vLayout5);
    contentLayout->addStretch();

//    setShowStacked(0);
//    showValue();
//    showText(InvStr());
//    recoverResults();

    setFixedHeight(142);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget*)), this, SLOT(_releaseHandle(IWidget*)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPTrendWidget::~NIBPTrendWidget()
{
}
