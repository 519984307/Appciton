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
#include "SystemManager.h"

/**************************************************************************************************
 * wave is enable。
 *************************************************************************************************/
bool SPO2WaveWidget::waveEnable()
{
    return spo2Param.isEnabled();
}

void SPO2WaveWidget::updateWidgetConfig()
{
    _loadConfig();
    WaveWidget::updateWidgetConfig();
}

/**************************************************************************************************
 * 响应窗体大小调整事件。
 * 参数:
 *      e: 事件对象
 *************************************************************************************************/
void SPO2WaveWidget::resizeEvent(QResizeEvent *e)
{
    WaveWidget::resizeEvent(e);

    if (!_name || !_notify)
    {
        return;
    }

    _name->move(0, 0);

    // 设定为固定的高度和宽度后，居中显示。
    _notify->move((width() - _notify->width()) / 2,
                  qmargins().top() + (height() - qmargins().top()) / 2 - _notify->height() - 1);
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

/**************************************************************************************************
 * 载入配置。
 *************************************************************************************************/
void SPO2WaveWidget::_loadConfig(void)
{
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    setPalette(palette);

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
}

void SPO2WaveWidget::setNotify(bool enable, QString str)
{
    // demo模式notify下强制更新为正常标志
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        enable = false;
    }

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
    , _notify(NULL)
{
    setFocusPolicy(Qt::NoFocus);
    setID(WAVE_SPO2);
//    setValueRange(64, 192);

    int fontSize = fontManager.getFontSize(4);
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(fontSize)) + 4;
    _name->setFont(fontManager.textFont(fontSize));
    _name->setFixedSize(130, fontH);
    _name->setText(title);

    _notify = new WaveWidgetLabel(" ", Qt::AlignCenter, this);
    _notify->setFont(fontManager.textFont(fontSize));
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
