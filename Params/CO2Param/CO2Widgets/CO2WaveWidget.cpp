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


#include <QResizeEvent>
#include "CO2WaveWidget.h"
#include "CO2WaveRuler.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "PopupList.h"
#include "CO2Param.h"
#include "WindowManager.h"
#include "ThemeManager.h"

/**************************************************************************************************
 * 尺寸变化。
 *************************************************************************************************/
void CO2WaveWidget::resizeEvent(QResizeEvent *e)
{
    _name->move(0, 0);
//    _zoom->move(_name->rect().width(), 0);

    _ruler->resize(RULER_X_OFFSET, 0,
                   width() - RULER_X_OFFSET,
                   height());
    WaveWidget::resizeEvent(e);
}

/**************************************************************************************************
 * 功能： 焦点事件。
 * 参数：
 *      e: 事件。
 *************************************************************************************************/
void CO2WaveWidget::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e);
    _name->setFocus();
}

/**************************************************************************************************
 * 功能： hide event。
 *************************************************************************************************/
void CO2WaveWidget::hideEvent(QHideEvent *e)
{
    WaveWidget::hideEvent(e);
}

/**************************************************************************************************
 * 设置波形模式。
 *************************************************************************************************/
void CO2WaveWidget::setWaveformMode(CO2SweepMode mode)
{
    if (mode == CO2_SWEEP_MODE_FILLED)
    {
        enableFill(true);
    }
    else
    {
        enableFill(false);
    }
}

/**************************************************************************************************
 * 设置标尺的标签值。
 *************************************************************************************************/
void CO2WaveWidget::setRuler(CO2DisplayZoom zoom)
{
    switch (zoom)
    {
    case CO2_DISPLAY_ZOOM_4:
        _ruler->setRuler(4.0, 2.0, 0);
        break;
    case CO2_DISPLAY_ZOOM_8:
        _ruler->setRuler(8.0, 4.0, 0);
        break;
    case CO2_DISPLAY_ZOOM_12:
        if (co2Param.getUnit() == UNIT_MMHG)
        {
            // 需求CO2-14, 标尺的上下限范围为0%-4%， 0-8%， 0-12%，0-20%
            // 或0-30mmhg, 0-60mmhg, 0-100 mmhg, 0-150mmhg,
            // 其中12%与100换算不对等：100mmhg按照软件代码中的算法换算应该为13%,
            // 这里为了显示与需求保持一致，特地将90mmhg显示为100mmhg
            _ruler->setRuler(13.0, 6.0, 0);
        }
        else
        {
            _ruler->setRuler(12.0, 6.0, 0);
        }
        break;
    case CO2_DISPLAY_ZOOM_20:
        _ruler->setRuler(20.0, 10.0, 0);
        break;
    default:
        break;
    }
}

void CO2WaveWidget::updatePalette(const QPalette &pal)
{
    _ruler->setPalette(pal);
    setPalette(pal);
    updateBackground();
}

/**************************************************************************************************
 * 窗口是否使能。
 *************************************************************************************************/
bool CO2WaveWidget::waveEnable()
{
    return co2Param.isEnabled();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CO2WaveWidget::CO2WaveWidget(const QString &waveName, const QString &title)
    : WaveWidget(waveName, title),
      _currentItemIndex(-1)
{
    setFocusPolicy(Qt::NoFocus);
    setID(WAVE_CO2);

    int infoFont = fontManager.getFontSize(4);
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(infoFont)) + 4;
//    _name = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    _name->setFont(fontManager.textFont(infoFont));
    _name->setFixedSize(130, fontH);
    _name->setText(title);
//    addItem(_name);

    _ruler = new CO2WaveRuler(this);
    _ruler->setFont(fontManager.textFont(infoFont - 2));
    addItem(_ruler);

    setMargin(QMargins(WAVE_X_OFFSET, 2, 2, 2));

    _loadConfig();
}

void CO2WaveWidget::_getItemIndex(int index)
{
    _currentItemIndex = index;
}

void CO2WaveWidget::_loadConfig()
{
    QPalette palette = colorManager.getPalette(paramInfo.getParamName(PARAM_CO2));
    setPalette(palette);
    // 标尺的颜色更深。
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    palette.setColor(QPalette::WindowText, color);
    _ruler->setPalette(palette);
    updateBackground();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CO2WaveWidget::~CO2WaveWidget()
{
}

void CO2WaveWidget::updateWidgetConfig()
{
    _loadConfig();
    // 待加入波形配置代码
    WaveWidget::updateWidgetConfig();
}
