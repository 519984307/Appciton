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

/**************************************************************************************************
 * 设置HR的值。
 *************************************************************************************************/
void ECGTrendWidget::setHRValue(int16_t hr, HRSourceType type)
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

/**************************************************************************************************
 * 是否显示。
 *************************************************************************************************/
void ECGTrendWidget::showValue(void)
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    if (_isAlarm)
    {
        showAlarmStatus(_hrValue);
        showAlarmParamLimit(_hrValue, _hrString, psrc);
        restoreNormalStatusLater();
    }
    else
    {
        showNormalStatus(psrc);
        _drawBeatIcon(psrc.windowText().color());
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
    r.adjust(nameLabel->width(), 0, -_hrBeatIcon->width(), 0);
    // 字体。
    int fontsize = fontManager.adjustNumFontSize(r, true, "9999");
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);

    _hrValue->setFont(font);
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
ECGTrendWidget::ECGTrendWidget() : TrendWidget("ECGTrendWidget"),
    _hrString(InvStr()), _isAlarm(false)
{
    // 开始布局。
    _hrBeatIcon = new QLabel();
    _hrBeatIcon->setPixmap(QPixmap());

    _hrValue = new QLabel();
    _hrValue->setAlignment(Qt::AlignCenter);
    _hrValue->setText(InvStr());

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->addWidget(_hrValue, 6);
    hlayout->addWidget(_hrBeatIcon, 1);

    contentLayout->addLayout(hlayout, 7);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    _timer = new QTimer();
    _timer->setInterval(190);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOut()));

    loadConfig();
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

void ECGTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    showNormalStatus(psrc);
    _drawBeatIcon(psrc.windowText().color());
}
