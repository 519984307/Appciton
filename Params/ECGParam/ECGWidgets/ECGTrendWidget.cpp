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
#include <QVBoxLayout>
#include <QPixmap>
#include <QTimer>
#include "ECGTrendWidget.h"
#include "ParamManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "ECGDupParam.h"
#include "Debug.h"
#include "TrendWidgetLabel.h"
#include "ECGParam.h"
#include "ECGDupParam.h"
#include "qpainter.h"
#include "MeasureSettingWindow.h"
#include "LayoutManager.h"
#include "AlarmConfig.h"
#include "ParamManager.h"
#include "ConfigManager.h"
#include <QBitmap>
#include "SPO2Param.h"
#include <QStackedWidget>

#define beatIconPath "/usr/local/nPM/icons/beat.png"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void ECGTrendWidget::_releaseHandle(IWidget *iWidget)
{
    Q_UNUSED(iWidget)
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
    p->popup(trs("ECGMenu"));
}

/**************************************************************************************************
 * 定时时间到。
 *************************************************************************************************/
void ECGTrendWidget::_timeOut()
{
    _timer->stop();
    _hrBeatIcon->setPixmap(QPixmap());
}

void ECGTrendWidget::_drawBeatIcon(QColor color)
{
    if (lastIconColor == color)
    {
        return;
    }
    QImage destImage(beatIconPath);
    QImage srcImage(destImage.width(), destImage.width(), QImage::Format_ARGB32);
    srcImage.fill(color);
    QPainter painter(&srcImage);
    QPoint point(0, 0);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.drawImage(point, destImage);
    painter.end();
    beatPixmap =  QPixmap::fromImage(srcImage);
    lastIconColor = color;
}

void ECGTrendWidget::loadConfig()
{
    // 设置标题栏的相关信息。
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    setPalette(palette);

    _drawBeatIcon(palette.windowText().color());
    _hrValue->setPalette(palette);

    int type = 0;
    currentConfig.getNumValue("ECG|HRSource", type);
    SubDupParamID subId;
    switch (type)
    {
        case HR_SOURCE_NR:
        case HR_SOURCE_AUTO:
        case HR_SOURCE_ECG:
            subId = SUB_DUP_PARAM_HR;
        break;
        case HR_SOURCE_SPO2:
        case HR_SOURCE_IBP:
            subId = SUB_DUP_PARAM_PR;
        break;
    }
    setName(trs(paramInfo.getSubParamName(subId)));
    setUnit(Unit::getSymbol(UNIT_BPM));

    // 设置滤波模式
    int mode = ECG_FILTERMODE_MONITOR;
    currentConfig.getNumValue("ECG|FilterMode", mode);
    ecgParam.setFilterMode(mode);

    TrendWidget::loadConfig();
}

void ECGTrendWidget::setShowStacked(int num)
{
    if (num >= _stackedwidget->count())
    {
        return;
    }
    if (_stackedwidget)
    {
        _stackedwidget->setCurrentIndex(num);
    }
}

/**************************************************************************************************
 * 设置HR的值。
 *************************************************************************************************/
void ECGTrendWidget::setHRValue(short hr, HRSourceType type)
{
    if (type == HR_SOURCE_ECG || type == HR_SOURCE_AUTO)
    {
        setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_HR)));
    }
    else
    {
        if (_hrBeatIcon->isVisible())
        {
            _hrBeatIcon->setPixmap(QPixmap());
        }
        setName(trs(paramInfo.getSubParamName(SUB_DUP_PARAM_PR)));
    }

    if (hr >= 0 && type == HR_SOURCE_SPO2 && spo2Param.getPerfusionStatus())
    {
        _hrString = QString("%1?").arg(QString::number(hr));
    }
    else if (hr >= 0)
    {
        _hrString = QString::number(hr);
    }
    else
    {
        _hrString = InvStr();
    }
    _hrValue->setText(_hrString);
}

void ECGTrendWidget::setPluginPR(short pr)
{
    if (pr >= 0 && spo2Param.getPerfusionStatus())
    {
        _pluginPRString = QString("%1?").arg(QString::number(pr));
    }
    else if (pr >= 0)
    {
        _pluginPRString = QString::number(pr);
    }
    else
    {
        _pluginPRString = InvStr();
    }
    _pluginPRValue->setText(_pluginPRString);
}

void ECGTrendWidget::updateLimit()
{
    UnitType unitType = paramManager.getSubParamUnit(PARAM_ECG, SUB_PARAM_HR_PR);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_HR_PR, unitType);
    setLimit(config.highLimit, config.lowLimit, config.scale);
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void ECGTrendWidget::isAlarm(bool isAlarm)
{
    _isAlarm = isAlarm;

    updateAlarm(isAlarm);
}

void ECGTrendWidget::isPluginPRAlarm(bool isAlarm)
{
    _ispluginPRAlarm = isAlarm;

    updateAlarm(isAlarm);
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void ECGTrendWidget::showValue(void)
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    if (_isAlarm || _ispluginPRAlarm)
    {
        if (_isAlarm)
        {
            showAlarmStatus(_hrValue);
            showAlarmParamLimit(_hrValue, _hrString, psrc);
        }

        if (_ispluginPRAlarm)
        {
            showAlarmStatus(_pluginPRValue);
        }
        restoreNormalStatusLater();
    }
    else
    {
        QLayout *lay = _stackedwidget->currentWidget()->layout();
        showNormalStatus(lay, psrc);
        showNormalStatus(psrc);
    }
}

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void ECGTrendWidget::showEvent(QShowEvent *e)
{
    TrendWidget::showEvent(e);
}

/**************************************************************************************************
 * 根据布局大小自动调整字体大小。
 *************************************************************************************************/
void ECGTrendWidget::setTextSize(void)
{
    QRect r = this->rect();
    if (spo2Param.isConnected(true))
    {
        r.adjust(nameLabel->width() * 2, 0, 0, 0);
        r.setWidth(r.width() / 2);
    }
    else
    {
        r.adjust(nameLabel->width(), 0, -_hrBeatIcon->width(), 0);
    }

    // 字体。
    int fontsize = fontManager.adjustNumFontSize(r, true, "9999");
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _hrValue->setFont(font);
    _pluginPRValue->setFont(font);

    fontsize = fontManager.getFontSize(3);
    font = fontManager.textFont(fontsize);
    _pluginPRName->setFont(font);
}

/**************************************************************************************************
 * 闪烁心跳图标。
 *************************************************************************************************/
void ECGTrendWidget::blinkBeatPixmap()
{
    _hrBeatIcon->setPixmap(beatPixmap);
    _timer->start();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGTrendWidget::ECGTrendWidget()
    : TrendWidget("ECGTrendWidget")
    , _hrBeatIcon(NULL)
    , _hrValue(NULL)
    , _pluginPRName(NULL)
    , _pluginPRValue(NULL)
    , _stackedwidget(NULL)
    , _hrString(InvStr())
    , _pluginPRString(InvStr())
    , _isAlarm(false)
    , _ispluginPRAlarm(false)
{
    // 开始布局。
    _hrBeatIcon = new QLabel();
    _hrBeatIcon->setPixmap(QPixmap());

    _hrValue = new QLabel();
    _hrValue->setAlignment(Qt::AlignCenter);
    _hrValue->setText(InvStr());

    // plugin RR layout
    QHBoxLayout *hLayout = new QHBoxLayout();
    _pluginPRName = new QLabel();
    _pluginPRName->setAlignment(Qt::AlignRight | Qt::AlignTop);
    _pluginPRName->setText(trs(paramInfo.getSubParamName(SUB_PARAM_PLUGIN_PR)));
    hLayout->addWidget(_pluginPRName, 1);

    _pluginPRValue = new QLabel();
    _pluginPRValue->setAlignment(Qt::AlignCenter);
    _pluginPRValue->setText(InvStr());
    hLayout->addWidget(_pluginPRValue, 3);

    QWidget *groupBox0 = new QWidget();
    QHBoxLayout *layout0 = new QHBoxLayout(groupBox0);
    layout0->setMargin(0);
    layout0->setSpacing(0);
    layout0->addWidget(_hrValue, 3);
    layout0->addLayout(hLayout, 4);

    QWidget *groupBox1 = new QWidget();
    QHBoxLayout *layout1 = new QHBoxLayout(groupBox1);
    layout1->setMargin(0);
    layout1->setSpacing(0);
    layout1->addWidget(_hrValue, 6);
    layout1->addWidget(_hrBeatIcon, 1);

    _stackedwidget = new QStackedWidget();
    _stackedwidget->addWidget(groupBox0);
    _stackedwidget->addWidget(groupBox1);
    _stackedwidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_stackedwidget, 1);

    contentLayout->addLayout(layout, 7);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    _timer = new QTimer();
    _timer->setInterval(190);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOut()));

    loadConfig();
    updateTrendWidget();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGTrendWidget::~ECGTrendWidget()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}

QList<SubParamID> ECGTrendWidget::getShortTrendSubParams() const
{
    QList<SubParamID> list;
    list.append(SUB_PARAM_HR_PR);
    return list;
}

void ECGTrendWidget::updateTrendWidget(bool isPluginConnected)
{
    if (isPluginConnected)
    {
        setShowStacked(0);
    }
    else
    {
        setShowStacked(1);
    }
    setTextSize();
}

void ECGTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    QLayout *lay = _stackedwidget->currentWidget()->layout();
    showNormalStatus(lay, psrc);
    showNormalStatus(psrc);
}
