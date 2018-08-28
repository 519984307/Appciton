/**
 ** This file is part of the nPM project.
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
#include "WaveWidgetSelectMenu.h"
#include "PopupList.h"
#include "CO2Param.h"
#include "ThemeManager.h"

/**************************************************************************************************
 * 释放事件，弹出菜单。
 *************************************************************************************************/
void CO2WaveWidget::_releaseHandle(IWidget *iWidget)
{
    QWidget *p = static_cast<QWidget *>(parent());
    if (p == NULL)
    {
        return;
    }

    QRect prect = p->geometry();
    QRect r = geometry();

    waveWidgetSelectMenu.setTopWaveform(false);
    waveWidgetSelectMenu.setWaveformName(name());
    waveWidgetSelectMenu.setShowPoint(prect.x() + r.x() + 50, prect.y() + r.y());
    waveWidgetSelectMenu.autoShow();
}

/**************************************************************************************************
 * 尺寸变化。
 *************************************************************************************************/
void CO2WaveWidget::resizeEvent(QResizeEvent *e)
{
    _name->move(0, 0);
    _zoom->move(_name->rect().width(), 0);

    _ruler->resize(qmargins().left(), qmargins().top(),
                   width() - qmargins().left() - qmargins().right(),
                   height() - qmargins().top() - qmargins().bottom());
    WaveWidget::resizeEvent(e);

    if (NULL != _gainList)
    {
        _gainList->close();
    }
}

/**************************************************************************************************
 * 功能： 焦点事件。
 * 参数：
 *      e: 事件。
 *************************************************************************************************/
void CO2WaveWidget::focusInEvent(QFocusEvent *e)
{
    _name->setFocus();
}

/**************************************************************************************************
 * 功能： hide event。
 *************************************************************************************************/
void CO2WaveWidget::hideEvent(QHideEvent *e)
{
    WaveWidget::hideEvent(e);

    if (NULL != _gainList)
    {
        _gainList->close();
    }
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
    float zoomValue = 0;
    switch (zoom)
    {
    case CO2_DISPLAY_ZOOM_4:
        _ruler->setRuler(4.0, 2.0, 0);
        zoomValue = 4.0;
        break;
    case CO2_DISPLAY_ZOOM_8:
        _ruler->setRuler(8.0, 4.0, 0);
        zoomValue = 8.0;
        break;
    case CO2_DISPLAY_ZOOM_12:
        _ruler->setRuler(12.0, 6.0, 0);
        zoomValue = 12.0;
        break;
    case CO2_DISPLAY_ZOOM_20:
        _ruler->setRuler(20.0, 10.0, 0);
        zoomValue = 20.0;
        break;
    default:
        break;
    }

    UnitType unit = co2Param.getUnit();
    QString str;
    if (unit == UNIT_KPA)
    {
        float tempVal = Unit::convert(UNIT_KPA, UNIT_PERCENT, zoomValue).toFloat();
//        str.sprintf("0.0~%.1f", (float)(int)(tempVal + 0.5));
        float num = static_cast<int>(tempVal + 0.5);
        str = QString("0.0~%1").number(num, 'f' , 1);
    }
    else if (unit == UNIT_MMHG)
    {
        str = "0~";
        int tempVal = Unit::convert(UNIT_MMHG, UNIT_PERCENT, zoomValue).toInt();
        tempVal = (tempVal + 5) / 10 * 10;
        str += QString::number(tempVal);
    }
    else
    {
        str = QString("0.0~%1").number(zoomValue, 'f', 1);
    }
    str += " ";
    str += Unit::localeSymbol(unit);
    _zoom->setText(str);
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
    : WaveWidget(waveName, title), _gainList(NULL),
      _currentItemIndex(-1)
{
    setFocusPolicy(Qt::NoFocus);
    setID(WAVE_CO2);

    QPalette palette = colorManager.getPalette(paramInfo.getParamName(PARAM_CO2));
    setPalette(palette);

    // 标尺的颜色更深。
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    palette.setColor(QPalette::WindowText, color);

    int infoFont = 14;
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(infoFont)) + 4;
//    _name = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    _name->setFont(fontManager.textFont(infoFont));
    _name->setFixedSize(130, fontH);
    _name->setText(title);
//    addItem(_name);
    connect(_name, SIGNAL(released(IWidget *)), this, SLOT(_releaseHandle(IWidget *)));

    _ruler = new CO2WaveRuler(this);
    _ruler->setPalette(palette);
    _ruler->setFont(fontManager.textFont(infoFont));
    addItem(_ruler);

    _zoom = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    _zoom->setFont(fontManager.textFont(infoFont));
    _zoom->setFixedSize(120, fontH);
    _zoom->setText(title);
    addItem(_zoom);
    connect(_zoom, SIGNAL(released(IWidget *)), this, SLOT(_zoomChangeSlot(IWidget *)));

    setMargin(QMargins(WAVE_X_OFFSET, fontH, 2, 2));
}

/**************************************************************************************************
 * 增益改变。
 *************************************************************************************************/
void CO2WaveWidget::_zoomChangeSlot(IWidget *widget)
{
    if (NULL == _gainList)
    {
        CO2DisplayZoom zoom = co2Param.getDisplayZoom();
        int maxZoom = CO2_DISPLAY_ZOOM_NR;
        _gainList = new PopupList(_zoom, false);
        float zoomArray[] = {4.0, 8.0, 12.0, 20.0};
        QString str;
        UnitType unit = co2Param.getUnit();
        for (int i = 0; i < maxZoom; i++)
        {
            str.clear();
            if (unit == UNIT_KPA)
            {
                float tempVal = Unit::convert(UNIT_KPA, UNIT_PERCENT, zoomArray[i]).toFloat();
                float num = static_cast<int>(tempVal + 0.5);
                str = QString("0.0~%1").number(num, 'f', 1);
            }
            else if (unit == UNIT_MMHG)
            {
                str = "0~";
                int tempVal = Unit::convert(UNIT_MMHG, UNIT_PERCENT, zoomArray[i]).toInt();
                tempVal = (tempVal + 5) / 10 * 10;
                str += QString::number(tempVal);
            }
            else
            {
                str = QString("0.0~%1").number(zoomArray[i], 'f', 1);
            }
            str += " ";
            str += Unit::localeSymbol(unit);
            _gainList->addItemText(str);
        }
        int fontSize = fontManager.getFontSize(3);
        _gainList->setFont(fontManager.textFont(fontSize));
        connect(_gainList, SIGNAL(destroyed()), this, SLOT(_popupDestroyed()));
        connect(_gainList, SIGNAL(selectItemChanged(int)), this , SLOT(_getItemIndex(int)));
    }

    _gainList->show();
}

/**************************************************************************************************
 * 弹出菜单销毁。
 *************************************************************************************************/
void CO2WaveWidget::_popupDestroyed(void)
{
    if (_currentItemIndex == -1)
    {
        _gainList = NULL;
        return;
    }

    co2Param.setDisplayZoom((CO2DisplayZoom)_currentItemIndex);
    _gainList = NULL;
}

void CO2WaveWidget::_getItemIndex(int index)
{
    _currentItemIndex = index;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CO2WaveWidget::~CO2WaveWidget()
{
}
