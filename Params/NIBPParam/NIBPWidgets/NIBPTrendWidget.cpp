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
#include "LayoutManager.h"

#define stretchCount 1
class NIBPTrendWidgetPrivate
{
public:
    NIBPTrendWidgetPrivate() :
        nibpLabel(NULL), sysLabel(NULL),
        diaLabel(NULL), mapLabel(NULL),
        nibpValue(NULL), sysValue(NULL),
        diaValue(NULL), mapValue(NULL),
        pressureValue(NULL), lastMeasureCount(NULL),
        message(NULL), model(NULL),
        countDown(NULL), widget0(NULL),
        widget1(NULL), stackedwidget(NULL),
        sysString(InvStr()), diaString(InvStr()),
        mapString(QString("(") + InvStr() + QString(")")),
        pressureString(InvStr()), measureTime(""),
        sysAlarm(false), diaAlarm(false),
        mapAlarm(false), effective(false),
        messageFontSize(100), messageInvFontSize(100),
        lastTime(0), valueLayout(NULL),
        nibpValueLayout(NULL), sysLayout(NULL),
        diaLayout(NULL), mapLayout(NULL)
    {}
    ~NIBPTrendWidgetPrivate() {}

    QLabel *nibpLabel;
    QLabel *sysLabel;
    QLabel *diaLabel;
    QLabel *mapLabel;
    QLabel *nibpValue;
    QLabel *sysValue;
    QLabel *diaValue;
    QLabel *mapValue;
    QLabel *pressureValue;
    QLabel *lastMeasureCount;  // 用于标记最近一次测量过了多少时间，todo。
    QLabel *message;           // 用于提示“wait”等信息
    QLabel *model;           // 用于提示“wait”等信息
    QLabel *countDown;

    QWidget *widget0;
    QWidget *widget1;

    QStackedWidget *stackedwidget;

    QString sysString;
    QString diaString;
    QString mapString;
    QString pressureString;
    QString measureTime;
    bool sysAlarm;
    bool diaAlarm;
    bool mapAlarm;
    bool effective;           //有效测量数据
    int messageFontSize;      // 非虚线显示时字体大小
    int messageInvFontSize;   // 虚线显示时字体大小
    unsigned lastTime;

    static const int margin = 1;
    void setCountDown(const QString &time);
    void layoutExec(QHBoxLayout *layout);  // 布局
    void adjustValueLayout();
    void updateMeasureTime();
    QVBoxLayout *valueLayout;
    QVBoxLayout *nibpValueLayout;
    QVBoxLayout *sysLayout;
    QVBoxLayout *diaLayout;
    QVBoxLayout *mapLayout;
};

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
    d_ptr->pressureString = InvStr();
    d_ptr->lastTime = time;

    if (systemManager.getCurWorkMode() == WORK_MODE_NORMAL)
    {
        // 正常模式下，才保存设置值
        saveResults(sys , dia , map , time);
    }

    if ((sys == InvData()) || (dia == InvData()) || (map == InvData()))
    {
        d_ptr->effective = false;
        setShowStacked(2);  //显示"---"
        d_ptr->sysString = InvStr();
        d_ptr->diaString = InvStr();
        d_ptr->mapString = InvStr();
    }
    else
    {
        UnitType unit = nibpParam.getUnit();
        d_ptr->sysString = Unit::convert(unit, UNIT_MMHG, sys);
        d_ptr->diaString = Unit::convert(unit, UNIT_MMHG, dia);
        d_ptr->mapString = "(" + Unit::convert(unit, UNIT_MMHG, map) + ")";
        showValue();
        d_ptr->effective = true;
        setShowStacked(0);  // 显示测量结果
    }

    d_ptr->sysValue->setText(d_ptr->sysString);
    d_ptr->diaValue->setText(d_ptr->diaString);
    d_ptr->mapValue->setText(d_ptr->mapString);
    d_ptr->pressureValue->setText(d_ptr->pressureString);
    d_ptr->lastMeasureCount->setText(d_ptr->measureTime);
}

void NIBPTrendWidget::updateLimit()
{
    UnitType unitType = paramManager.getSubParamUnit(PARAM_NIBP, SUB_PARAM_NIBP_SYS);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_NIBP_SYS, unitType);
    setLimit(config.highLimit, config.lowLimit, config.scale);
}

/**************************************************************************************************
 * 恢复测量结果的数据。
 *************************************************************************************************/
void NIBPTrendWidget::recoverResults(int16_t &sys, int16_t &dia, int16_t &map, unsigned &time)
{
    int data = InvData();

    if (timeManager.getPowerOnSession() == POWER_ON_SESSION_CONTINUE)
    {
        systemConfig.getNumValue("PrimaryCfg|NIBP|NIBPsys", data);
        sys = data;

        systemConfig.getNumValue("PrimaryCfg|NIBP|NIBPdia", data);
        dia = data;

        systemConfig.getNumValue("PrimaryCfg|NIBP|NIBPmap", data);
        map = data;

        systemConfig.getNumValue("PrimaryCfg|NIBP|MeasureTime", data);
        time = data;
    }
    else
    {
        sys = data;
        dia = data;
        map = data;
        time = 0;
    }
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
    d_ptr->pressureString = Unit::convert(unit, UNIT_MMHG, p);
    d_ptr->pressureValue->setText(d_ptr->pressureString);
}

/**************************************************************************************************
 * 设置倒计时。
 *************************************************************************************************/
void NIBPTrendWidget::setCountdown(int t)
{
    if (d_ptr->stackedwidget->currentIndex() == 1)
    {
        return;
    }
    if (t == -1)
    {
        d_ptr->setCountDown("");
        return;
    }

    int hour = t / 3600;
    int min = (t - hour * 3600) / 60;
    int sec = t % 60;
    QString str;
    if (hour != 0)
    {
        str = QString("%1:%2:%3").arg(hour, 2).arg(min, 2).arg(sec, 2);
    }
    else
    {
        str = QString("%1:%2").arg(min, 2).arg(sec, 2);
    }
    d_ptr->setCountDown(str);
}

/**************************************************************************************************
 * 倒计时时间。
 *************************************************************************************************/
void NIBPTrendWidgetPrivate::setCountDown(const QString &time)
{
    countDown->setText(time);
}

void NIBPTrendWidgetPrivate::layoutExec(QHBoxLayout *layout)
{
    // 构造出所有控件。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));

    nibpLabel = new QLabel();
    nibpLabel->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    nibpLabel->setText(" ");

    nibpValue = new QLabel();
    nibpValue->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    nibpValue->setPalette(palette);
    nibpValue->setText("/");

    sysLabel = new QLabel();
    sysLabel->setAlignment(Qt::AlignCenter);
    sysLabel->setPalette(palette);
    sysLabel->setText(trs("SYS"));

    sysValue = new QLabel();
    sysValue->setAlignment(Qt::AlignCenter);
    sysValue->setPalette(palette);
    sysValue->setText(InvStr());

    diaLabel = new QLabel();
    diaLabel->setAlignment(Qt::AlignCenter);
    diaLabel->setPalette(palette);
    diaLabel->setText(trs("DIA"));

    diaValue = new QLabel();
    diaValue->setAlignment(Qt::AlignCenter);
    diaValue->setPalette(palette);
    diaValue->setText(InvStr());

    mapLabel = new QLabel();
    mapLabel->setAlignment(Qt::AlignCenter);
    mapLabel->setPalette(palette);
    mapLabel->setText(trs("MAP"));

    mapValue = new QLabel();
    mapValue->setAlignment(Qt::AlignCenter);
    mapValue->setPalette(palette);
    mapValue->setText(InvStr());

    pressureValue = new QLabel();
    pressureValue->setAlignment(Qt::AlignCenter);
    pressureValue->setPalette(palette);
    pressureValue->setText(InvStr());

    int fontSize = fontManager.getFontSize(7);

    countDown = new QLabel();
    countDown->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    countDown->setPalette(palette);
    countDown->setText("");
    countDown->setFont(fontManager.textFont(fontSize));
    countDown->setVisible(false);

    lastMeasureCount = new QLabel();
    lastMeasureCount->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    lastMeasureCount->setPalette(palette);
    lastMeasureCount->setFont(fontManager.textFont(fontSize));

    model = new QLabel();
    model->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    model->setPalette(palette);
    model->setFont(fontManager.textFont(fontSize));
    model->setText("");

    message = new QLabel();
    message->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    message->setPalette(palette);
    message->setFont(fontManager.textFont(messageFontSize));
    message->setText(InvStr());

    // 将数值显示和倒计时放到一起。
    QWidget *groupBox0 = new QWidget();
    valueLayout = new QVBoxLayout(groupBox0);
    sysLayout = new QVBoxLayout();
    sysLayout->setSpacing(0);
    sysLayout->addWidget(sysLabel);
    sysLayout->addWidget(sysValue);
    diaLayout = new QVBoxLayout();
    diaLayout->setSpacing(0);
    diaLayout->addWidget(diaLabel);
    diaLayout->addWidget(diaValue);
    mapLayout = new QVBoxLayout();
    mapLayout->addStretch();
    mapLayout->addWidget(mapLabel);
    mapLayout->addStretch();
    mapLayout->addWidget(mapValue);
    mapLayout->addStretch();
    nibpValueLayout = new QVBoxLayout();
    nibpValueLayout->setSpacing(0);
    nibpValueLayout->addWidget(nibpLabel);
    nibpValueLayout->addWidget(nibpValue);

    QHBoxLayout *hLayout0 = new QHBoxLayout();
    hLayout0->setMargin(0);
    hLayout0->setSpacing(10);
    hLayout0->addStretch();
    hLayout0->addLayout(sysLayout);
    hLayout0->addLayout(nibpValueLayout);
    hLayout0->addLayout(diaLayout);
    hLayout0->addLayout(mapLayout);
    hLayout0->addStretch();

    valueLayout->addLayout(hLayout0);


    QWidget *groupBox1 = new QWidget();
    QVBoxLayout *vLayout1 = new QVBoxLayout(groupBox1);
    vLayout1->setMargin(1);
    vLayout1->setSpacing(0);
    vLayout1->addWidget(pressureValue, 0, Qt::AlignCenter);

    QWidget *groupBox2 = new QWidget();
    QVBoxLayout *vLayout2 = new QVBoxLayout(groupBox2);
    vLayout2->setMargin(1);
    vLayout2->setSpacing(0);
    vLayout2->addWidget(message, 0, Qt::AlignCenter);

    QWidget *groupBox3 = new QWidget();
    QVBoxLayout *vLayout3 = new QVBoxLayout(groupBox3);
    vLayout3->setMargin(1);
    vLayout3->setSpacing(0);
    vLayout3->addWidget(countDown, 0, Qt::AlignCenter);

    stackedwidget = new QStackedWidget();
    stackedwidget->addWidget(groupBox0);
    stackedwidget->addWidget(groupBox1);
    stackedwidget->addWidget(groupBox2);
    stackedwidget->addWidget(groupBox3);
    stackedwidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    // 将最后测量时间和模式放到一起。
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(5, 0, 5, 0);
    hLayout->addStretch();
    hLayout->addWidget(lastMeasureCount);
    hLayout->addSpacing(10);
    hLayout->addWidget(model);
    hLayout->addStretch();

    // 第二组。
    QVBoxLayout *vLayout5 = new QVBoxLayout();
    vLayout5->setMargin(0);
    vLayout5->setSpacing(0);
    vLayout5->addWidget(stackedwidget, 1);
    vLayout5->addLayout(hLayout);

    layout->addLayout(vLayout5, 7);
}

void NIBPTrendWidgetPrivate::adjustValueLayout()
{
    UserFaceType type = layoutManager.getUFaceType();
    QLayout *layout = valueLayout->itemAt(0)->layout();
    if (layout == NULL)
    {
        return;
    }
    QHBoxLayout *hLayout = reinterpret_cast<QHBoxLayout *>(layout);
    if (type == UFACE_MONITOR_BIGFONT && valueLayout->count() == stretchCount)
    {
        // 大字体界面且单行布置值时
        hLayout->removeItem(mapLayout);
        valueLayout->insertLayout(1, mapLayout);
    }
    else if (type != UFACE_MONITOR_BIGFONT && valueLayout->count() == stretchCount + 1)
    {
        //　非大字体界面且双行布置值时
        valueLayout->removeItem(mapLayout);
        hLayout->insertLayout(hLayout->count() - 1, mapLayout);
    }
}

void NIBPTrendWidgetPrivate::updateMeasureTime()
{
    if (lastTime == 0)
    {
        measureTime = "";
    }
    else
    {
        QString timeStr("@ ");
        QString tmpStr;
        timeDate.getTime(lastTime, tmpStr, false);
        timeStr += tmpStr;
        measureTime = timeStr;
    }
    lastMeasureCount->setText(measureTime);
}

/**************************************************************************************************
 * 单位更改。
 *************************************************************************************************/
void NIBPTrendWidget::updateUnit(UnitType unit)
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
        d_ptr->sysAlarm = flag;
        break;
    case SUB_PARAM_NIBP_DIA:
        d_ptr->diaAlarm = flag;
        break;
    case SUB_PARAM_NIBP_MAP:
        d_ptr->mapAlarm = flag;
        break;
    default:
        break;
    }

    updateAlarm(d_ptr->sysAlarm || d_ptr->diaAlarm || d_ptr->mapAlarm);
}

/**************************************************************************************************
 * 显示血压值。
 *************************************************************************************************/
void NIBPTrendWidget::showValue(void)
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    if (d_ptr->sysAlarm || d_ptr->diaAlarm || d_ptr->mapAlarm)
    {
        if (!d_ptr->sysAlarm)
        {
            showNormalStatus(d_ptr->sysValue, psrc);
        }

        if (!d_ptr->diaAlarm)
        {
            showNormalStatus(d_ptr->diaValue, psrc);
        }

        if (!d_ptr->mapAlarm)
        {
            showNormalStatus(d_ptr->mapValue, psrc);
        }

        if (d_ptr->sysAlarm)
        {
            showAlarmStatus(d_ptr->sysValue);
            showAlarmParamLimit(d_ptr->sysValue, d_ptr->sysString, psrc);
        }

        if (d_ptr->diaAlarm)
        {
            showAlarmStatus(d_ptr->diaValue);
        }

        if (d_ptr->mapAlarm)
        {
            showAlarmStatus(d_ptr->mapValue);
        }
        restoreNormalStatusLater();
    }
    else
    {
        QLayout *lay = d_ptr->stackedwidget->currentWidget()->layout();
        showNormalStatus(lay, psrc);
        showNormalStatus(d_ptr->lastMeasureCount, psrc);
        showNormalStatus(d_ptr->countDown, psrc);
        showNormalStatus(d_ptr->model, psrc);
    }

    d_ptr->updateMeasureTime();
    d_ptr->adjustValueLayout();
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
        d_ptr->mapString = InvStr();
        d_ptr->pressureString = InvStr();
        d_ptr->lastMeasureCount->setText("");
    }
    d_ptr->stackedwidget->setCurrentIndex(num);
}

/**************************************************************************************************
 * 显示上次测量的时间。
 *************************************************************************************************/
void NIBPTrendWidget::setShowMeasureCount(bool lastMeasureCountflag)
{
    if (lastMeasureCountflag)
    {
        d_ptr->lastMeasureCount->setVisible(true);
    }
    else
    {
        d_ptr->lastMeasureCount->setVisible(false);
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
        if (d_ptr->effective)
        {
            if (d_ptr->stackedwidget->currentIndex() != 0)
            {
                setShowStacked(0);
            }
            return;
        }
        d_ptr->message->setFont(fontManager.numFont(d_ptr->messageInvFontSize, true));
    }
    else
    {
        d_ptr->message->setFont(fontManager.numFont(d_ptr->messageFontSize, true));
    }

    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    normalPalette(psrc);
    if (!d_ptr->sysAlarm && !d_ptr->diaAlarm && !d_ptr->mapAlarm)
    {
        showNormalStatus(d_ptr->message, psrc);
    }

    d_ptr->message->setText(text);
    setShowStacked(2);
}

/**************************************************************************************************
 * 显示模式。
 *************************************************************************************************/
void NIBPTrendWidget::showModelText(const QString &text)
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    normalPalette(psrc);
    if (!d_ptr->sysAlarm && !d_ptr->diaAlarm && !d_ptr->mapAlarm)
    {
        showNormalStatus(d_ptr->model, psrc);
    }
    d_ptr->model->setText(text);
    if (text.isEmpty())
    {
        d_ptr->model->setVisible(false);
    }
    else
    {
        d_ptr->model->setVisible(true);
    }
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void NIBPTrendWidget::setTextSize()
{
    QRect r;
    r.setSize(QSize(((width() - nameLabel->width()) / 4), ((height() / 5) * 3)));
    int fontsize = fontManager.adjustNumFontSize(r, true);
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    d_ptr->nibpValue->setFont(font);
    d_ptr->sysValue->setFont(font);
    d_ptr->diaValue->setFont(font);
    d_ptr->pressureValue->setFont(font);

    font = fontManager.numFont(fontsize / 1.5, true);
    font.setWeight(QFont::Black);
    d_ptr->mapValue->setFont(font);

    font = fontManager.numFont(fontsize / 3.0, true);
    d_ptr->nibpLabel->setFont(font);
    d_ptr->sysLabel->setFont(font);
    d_ptr->diaLabel->setFont(font);
    d_ptr->mapLabel->setFont(font);

    r.setSize(QSize(width() - nameLabel->width(), height()));
    d_ptr->messageInvFontSize = fontManager.adjustNumFontSize(r, true);
    r.setSize(QSize((width() - nameLabel->width()) * 3 / 4, (height() / 3)));
    d_ptr->messageFontSize = fontManager.adjustNumFontSize(r, true);

    if (d_ptr->message->text() == InvStr())
    {
        d_ptr->message->setFont(fontManager.numFont(d_ptr->messageInvFontSize, true));
    }
    else
    {
        d_ptr->message->setFont(fontManager.numFont(d_ptr->messageFontSize, true));
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPTrendWidget::NIBPTrendWidget()
    : TrendWidget("NIBPTrendWidget"),
      d_ptr(new NIBPTrendWidgetPrivate)
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    setPalette(palette);
    setName(trs(paramInfo.getParamName(PARAM_NIBP)));
    setUnit(Unit::getSymbol(nibpParam.getUnit()));

    // 设置上下限
    updateLimit();

    // 设置布局
    d_ptr->layoutExec(contentLayout);

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

void NIBPTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    QLayout *lay = d_ptr->stackedwidget->currentWidget()->layout();   // 将窗体所有内容显示正常颜色
    showNormalStatus(lay, psrc);
    showNormalParamLimit(psrc);
}

void NIBPTrendWidget::updatePalette(const QPalette &pal)
{
    setPalette(pal);
    d_ptr->nibpLabel->setPalette(pal);
    d_ptr->nibpLabel->setPalette(pal);
    d_ptr->nibpLabel->setPalette(pal);
    d_ptr->nibpValue->setPalette(pal);
    d_ptr->sysValue->setPalette(pal);
    d_ptr->diaValue->setPalette(pal);
    d_ptr->mapValue->setPalette(pal);
    d_ptr->pressureValue->setPalette(pal);
    d_ptr->countDown->setPalette(pal);
    d_ptr->lastMeasureCount->setPalette(pal);
    d_ptr->model->setPalette(pal);
    d_ptr->message->setPalette(pal);
}

void NIBPTrendWidget::updateWidgetConfig()
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    setPalette(palette);
    setName(trs(paramInfo.getParamName(PARAM_NIBP)));
    setUnit(Unit::getSymbol(nibpParam.getUnit()));
    // 设置上下限
    updateLimit();

    // 设置报警关闭标志
    showAlarmOff();

    d_ptr->nibpLabel->setPalette(palette);
    d_ptr->sysLabel->setPalette(palette);
    d_ptr->diaLabel->setPalette(palette);
    d_ptr->mapLabel->setPalette(palette);
    d_ptr->nibpValue->setPalette(palette);
    d_ptr->sysValue->setPalette(palette);
    d_ptr->diaValue->setPalette(palette);
    d_ptr->mapValue->setPalette(palette);
    d_ptr->pressureValue->setPalette(palette);
    d_ptr->countDown->setPalette(palette);
    d_ptr->lastMeasureCount->setPalette(palette);
    d_ptr->model->setPalette(palette);
    d_ptr->message->setPalette(palette);
}
