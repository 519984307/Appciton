/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/27
 **/


#include "SPO2WaveWidget.h"
#include "SPO2Param.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "PopupList.h"
#include "IConfig.h"
#include "Debug.h"
#include "WindowManager.h"

/**************************************************************************************************
 * 设置波形增益
 *************************************************************************************************/
void SPO2WaveWidget::setGain(SPO2Gain gain)
{
    if (!_gain)
    {
        return;
    }

    _initValueRange(gain);
    QString text = NULL;
    switch (gain)
    {
    case SPO2_GAIN_X10:
    case SPO2_GAIN_X20:
    case SPO2_GAIN_X40:
    case SPO2_GAIN_X80:
        text += SPO2Symbol::convert(gain);
        break;

    default:
        text += "X1";
        break;
    }

    _gain->setText(text);
}

/**************************************************************************************************
 * 获取极值。
 *************************************************************************************************/
void SPO2WaveWidget::getGainToValue(SPO2Gain gain, int &min, int &max)
{
    int mid = spo2Param.getSPO2MaxValue() / 2;
    int diff;
    switch (gain)
    {
    case SPO2_GAIN_X10:
        min = 0;
        max = spo2Param.getSPO2MaxValue();
        break;

    case SPO2_GAIN_X20:
        diff = spo2Param.getSPO2MaxValue() / 4;
        min = mid - diff;
        max = mid + diff;
        break;

    case SPO2_GAIN_X40:
        diff = spo2Param.getSPO2MaxValue() / 8;
        min = mid - diff;
        max = mid + diff;
        break;

    case SPO2_GAIN_X80:
        diff = spo2Param.getSPO2MaxValue() / 16;
        min = mid - diff;
        max = mid + diff;
        break;

    default:
        min = 0;
        max = spo2Param.getSPO2MaxValue();
        break;
    }
}

/**************************************************************************************************
 * wave is enable。
 *************************************************************************************************/
bool SPO2WaveWidget::waveEnable()
{
    return spo2Param.isEnabled();
}

void SPO2WaveWidget::_initValueRange(SPO2Gain gain)
{
    int min = 0;
    int max = 0;
    getGainToValue(gain, min, max);
    setValueRange(min, max);
}

/**************************************************************************************************
 * 响应窗体大小调整事件。
 * 参数:
 *      e: 事件对象
 *************************************************************************************************/
void SPO2WaveWidget::resizeEvent(QResizeEvent *e)
{
    WaveWidget::resizeEvent(e);

    if (!_name || !_gain || !_notify)
    {
        return;
    }

    _name->move(0, 0);
    _gain->move(0 + _name->rect().width(), 0);

    // 设定为固定的高度和宽度后，居中显示。
    _notify->move((width() - _notify->width()) / 2,
                  qmargins().top() + (height() - qmargins().top()) / 2 - _notify->height() - 1);

    _initValueRange(spo2Param.getGain());
}

/**************************************************************************************************
 * 功能： 焦点事件。
 * 参数：
 *      e: 事件。
 *************************************************************************************************/
void SPO2WaveWidget::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e);
    _name->setFocus();
}

void SPO2WaveWidget::paintEvent(QPaintEvent *e)
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    setPalette(palette);
    WaveWidget::paintEvent(e);
}

/**************************************************************************************************
 * 载入配置。
 *************************************************************************************************/
void SPO2WaveWidget::_loadConfig(void)
{
    SPO2WaveVelocity speed = (SPO2WaveVelocity)spo2Param.getSweepSpeed();

    if (speed == SPO2_WAVE_VELOCITY_62D5)
    {
        setWaveSpeed(6.25);
    }
    else if (speed == SPO2_WAVE_VELOCITY_125)
    {
        setWaveSpeed(12.5);
    }
    else if (speed == SPO2_WAVE_VELOCITY_250)
    {
        setWaveSpeed(25.0);
    }

    setGain(spo2Param.getGain());
}

void SPO2WaveWidget::setNotify(bool enable, QString str)
{
    if (enable)
    {
        _notify->setVisible(true);
        _notify->setText(str);
    }
    else
    {
        _notify->setVisible(false);
        _notify->setText(" ");
    }
}


/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
SPO2WaveWidget::SPO2WaveWidget(const QString &waveName, const QString &title)
    : WaveWidget(waveName, title)
    , _gain(NULL)
    , _notify(NULL)
{
    setFocusPolicy(Qt::NoFocus);
    setID(WAVE_SPO2);
//    setValueRange(64, 192);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    setPalette(palette);

    int infoFont = 14;
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(infoFont)) + 4;
    _name->setFont(fontManager.textFont(infoFont));
    _name->setFixedSize(130, fontH);
    _name->setText(title);

    _gain = new WaveWidgetLabel("", Qt::AlignLeft | Qt::AlignVCenter, this);
    _gain->setFont(fontManager.textFont(infoFont));
    _gain->setFixedSize(120, fontH);
    _gain->setText("");
    addItem(_gain);

    _notify = new WaveWidgetLabel(" ", Qt::AlignCenter, this);
    _notify->setFont(fontManager.textFont(infoFont));
    _notify->setFocusPolicy(Qt::NoFocus);
    _notify->setFixedSize(200, fontH);
    _notify->setText("");
    _notify->setVisible(false);
    addItem(_notify);
    // 加载配置
    _loadConfig();

    setMargin(QMargins(WAVE_X_OFFSET, fontH, 2, 2));
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
SPO2WaveWidget::~SPO2WaveWidget()
{
}
