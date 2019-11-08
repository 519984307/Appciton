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
#include <QWidget>
#include "TEMPTrendWidget.h"
#include "Framework/Utility/Unit.h"
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

void TEMPTrendWidget::loadConfig()
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_TEMP));
    setPalette(palette);
    _t1Name->setPalette(palette);
    _t2Name->setPalette(palette);
    _tdName->setPalette(palette);
    _t1UpLimit->setPalette(palette);
    _t1DownLimit->setPalette(palette);
    _t2UpLimit->setPalette(palette);
    _t2DownLimit->setPalette(palette);
    _t1Value->setPalette(palette);
    _t2Value->setPalette(palette);
    _tdValue->setPalette(palette);
    _message->setPalette(palette);
    TrendWidget::loadConfig();
}

/**************************************************************************************************
 * 设置TEMP的值。
 *************************************************************************************************/
void TEMPTrendWidget::setTEMPValue(int16_t t1, int16_t t2, int16_t td)
{
    UnitType type = tempParam.getUnit();
    if (t1 == InvData())
    {
        _t1Str = InvStr();
    }
    else
    {
        _t1Str = Unit::convert(type, UNIT_TC, t1 / 10.0);
    }

    if (t2 == InvData())
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
        setUnit(trs(Unit::getSymbol(UNIT_TC)));
        break;

    case UNIT_TF:
        setUnit(trs(Unit::getSymbol(UNIT_TF)));
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
            showAlarmParamLimit(_t2Value, _t2Str, psrc);
        }

        if (_tdAlarm)
        {
            showAlarmStatus(_tdValue);
        }
        restoreNormalStatusLater();
    }
    else
    {
        updatePalette(psrc);
    }
}

void TEMPTrendWidget::updateLimit()
{
    UnitType unitType = paramManager.getSubParamUnit(PARAM_TEMP, SUB_PARAM_T1);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_T1, unitType);
    if (config.scale == 1)
    {
        _t1UpLimit->setText(QString::number(config.highLimit));
        _t1DownLimit->setText(QString::number(config.lowLimit));
    }
    else
    {
        _t1UpLimit->setText(QString::number(config.highLimit / config.scale)
                            + "."
                            + QString::number(config.highLimit % config.scale));

        _t1DownLimit->setText(QString::number(config.lowLimit / config.scale)
                              + "."
                              + QString::number(config.lowLimit % config.scale));
    }

    unitType = paramManager.getSubParamUnit(PARAM_TEMP, SUB_PARAM_T2);
    config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_T2, unitType);
    if (config.scale == 1)
    {
        _t2UpLimit->setText(QString::number(config.highLimit));
        _t2DownLimit->setText(QString::number(config.lowLimit));
    }
    else
    {
        _t2UpLimit->setText(QString::number(config.highLimit / config.scale)
                            + "."
                            + QString::number(config.highLimit % config.scale));

        _t2DownLimit->setText(QString::number(config.lowLimit / config.scale)
                              + "."
                              + QString::number(config.lowLimit % config.scale));
    }
}

void TEMPTrendWidget::showErrorStatckedWidget(bool error)
{
    if (error == true)
    {
        statckedWidget->setCurrentIndex(1);
    }
    else
    {
        statckedWidget->setCurrentIndex(0);
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
    r.setSize(QSize((widgetWidth - nameLabelWidth * 3)/3, height()));
    // 字体。
    int fontsize = fontManager.adjustNumFontSize(r, true, "3888");
    QFont font = fontManager.numFont(fontsize, true);
    font.setWeight(QFont::Black);
    _t1Value->setFont(font);
    _t2Value->setFont(font);
    _tdValue->setFont(font);

    // name
    QFont nameFont = fontManager.textFont(fontManager.getFontSize(3));
    _t1Name->setFont(nameFont);
    _t2Name->setFont(nameFont);
    _tdName->setFont(nameFont);

    // limit font
    QFont limitFont = fontManager.textFont(fontManager.getFontSize(1));
    _t1UpLimit->setFont(limitFont);
    _t1DownLimit->setFont(limitFont);
    _t2UpLimit->setFont(limitFont);
    _t2DownLimit->setFont(limitFont);

    r.setSize(QSize(widgetWidth * 3 / 4, height() / 3));
    int messageFont = fontManager.adjustNumFontSize(r, true);
    _message->setFont(fontManager.numFont(messageFont, true));
}

void TEMPTrendWidget::showAlarmParamLimit(QWidget *valueWidget, const QString &valueStr, QPalette psrc)
{
    normalPalette(&psrc);
    double value = valueStr.toDouble();
    if (valueWidget == _t1Value)
    {
        double up = _t1UpLimit->text().toDouble();
        double down = _t1DownLimit->text().toDouble();
        if (value > up)
        {
            QPalette darkerPal = valueWidget->palette();
            darkerPalette(darkerPal);
            _t1UpLimit->setPalette(darkerPal);
        }
        else
        {
            darkerPalette(psrc);
            _t1UpLimit->setPalette(psrc);
        }

        if (value < down)
        {
            QPalette darkerPal = valueWidget->palette();
            darkerPalette(darkerPal);
            _t1DownLimit->setPalette(darkerPal);
        }
        else
        {
            darkerPalette(psrc);
            _t1DownLimit->setPalette(psrc);
        }
    }
    if (valueWidget == _t2Value)
    {
        double up = _t2UpLimit->text().toDouble();
        double down = _t2DownLimit->text().toDouble();
        if (value > up)
        {
            QPalette darkerPal = valueWidget->palette();
            darkerPalette(darkerPal);
            _t2UpLimit->setPalette(darkerPal);
        }
        else
        {
            darkerPalette(psrc);
            _t2UpLimit->setPalette(psrc);
        }

        if (value < down)
        {
            QPalette darkerPal = valueWidget->palette();
            darkerPalette(darkerPal);
            _t2DownLimit->setPalette(darkerPal);
        }
        else
        {
            darkerPalette(psrc);
            _t2DownLimit->setPalette(psrc);
        }
    }
}

void TEMPTrendWidget::darkerPalette(QPalette pal)
{
    // 使颜色变暗
    QColor c = pal.color(QPalette::WindowText);
    pal.setColor(QPalette::WindowText, c.darker(150));
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

    // 标签设定。
    setName(trs(paramInfo.getParamName(PARAM_TEMP)));

    // 设置单位。
    UnitType u = tempParam.getUnit();
    if (UNIT_TC == u)
    {
        setUnit(trs(Unit::getSymbol(UNIT_TC)));
    }
    else
    {
        setUnit(trs(Unit::getSymbol(UNIT_TF)));
    }

    QWidget* groupBox0 = new QWidget();
    QHBoxLayout *hLayout0 = new QHBoxLayout(groupBox0);
    QVBoxLayout *vLayout = new QVBoxLayout();

    // T1
    _t1Name = new QLabel();
    _t1Name->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    _t1Name->setText(trs("T1"));
    _t1UpLimit = new QLabel();
    _t1UpLimit->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _t1UpLimit->setText("");
    _t1DownLimit = new QLabel();
    _t1DownLimit->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _t1DownLimit->setText("");
    vLayout->addWidget(_t1Name);
    vLayout->addStretch();
    vLayout->addWidget(_t1UpLimit);
    vLayout->addWidget(_t1DownLimit);
    vLayout->addStretch();
    _t1Value = new QLabel();
    _t1Value->setAlignment(Qt::AlignCenter);
    _t1Value->setText(InvStr());
    hLayout0->addLayout(vLayout);
    hLayout0->addWidget(_t1Value);

    // T2
    _t2Name = new QLabel();
    _t2Name->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    _t2Name->setText(trs("T2"));
    _t2UpLimit = new QLabel();
    _t2UpLimit->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _t2UpLimit->setText("");
    _t2DownLimit = new QLabel();
    _t2DownLimit->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _t2DownLimit->setText("");
    vLayout = new QVBoxLayout();
    vLayout->addWidget(_t2Name);
    vLayout->addStretch();
    vLayout->addWidget(_t2UpLimit);
    vLayout->addWidget(_t2DownLimit);
    vLayout->addStretch();
    _t2Value = new QLabel();
    _t2Value->setAlignment(Qt::AlignCenter);
    _t2Value->setText(InvStr());
    hLayout0->addLayout(vLayout);
    hLayout0->addWidget(_t2Value);

    // TD
    _tdName = new QLabel();
    _tdName->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    _tdName->setText(trs("TD"));
    _tdValue = new QLabel();
    _tdValue->setAlignment(Qt::AlignCenter);
    _tdValue->setText(InvStr());
    hLayout0->addWidget(_tdName);
    hLayout0->addWidget(_tdValue);

    QWidget *groupBox1 = new QWidget();
    _message = new QLabel();
    _message->setText(trs("TEMPDisable"));
    QHBoxLayout *hLayout1 = new QHBoxLayout(groupBox1);
    hLayout1->setMargin(1);
    hLayout1->setSpacing(0);
    hLayout1->addWidget(_message, 0, Qt::AlignCenter);

    statckedWidget = new QStackedWidget();
    statckedWidget->addWidget(groupBox0);
    statckedWidget->addWidget(groupBox1);
    statckedWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(statckedWidget);
    contentLayout->addLayout(hLayout, 7);

    // 释放事件。
    connect(this, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    loadConfig();
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
    list.append(SUB_PARAM_TD);
    return list;
}

void TEMPTrendWidget::doRestoreNormalStatus()
{
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_TEMP));
    showNormalStatus(_t1Value, psrc);
    showNormalStatus(_t2Value, psrc);
    showNormalStatus(_tdValue, psrc);
    QPalette darkerPal = psrc;
    darkerPalette(darkerPal);
    showNormalStatus(_t1UpLimit, darkerPal);
    showNormalStatus(_t1DownLimit, darkerPal);
    showNormalStatus(_t2UpLimit, darkerPal);
    showNormalStatus(_t2DownLimit, darkerPal);
    showNormalStatus(_message, psrc);
}

void TEMPTrendWidget::updatePalette(const QPalette &pal)
{
    setPalette(pal);
    _t1Name->setPalette(pal);
    _t2Name->setPalette(pal);
    _tdName->setPalette(pal);
    _t1Value->setPalette(pal);
    _t2Value->setPalette(pal);
    _tdValue->setPalette(pal);
    QPalette darkerPal = pal;
    darkerPalette(darkerPal);
    _t1UpLimit->setPalette(darkerPal);
    _t1DownLimit->setPalette(darkerPal);
    _t2UpLimit->setPalette(darkerPal);
    _t2DownLimit->setPalette(darkerPal);
    _message->setPalette(pal);
}

