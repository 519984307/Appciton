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

#include "AGWaveWidget.h"
#include "Framework/Language/LanguageManager.h"
#include "ColorManager.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "AGWaveRuler.h"
#include "AGSymbol.h"
#include "AGParam.h"
#include "WindowManager.h"
#include "PopupList.h"

/**************************************************************************************************
 * 设置麻醉剂类型。
 *************************************************************************************************/
void AGWaveWidget::setAnestheticType(AGAnaestheticType type)
{
    if (type == AG_ANAESTHETIC_NO)
    {
        _name->setText(getTitle());
    }
    else
    {
        _name->setText(AGSymbol::convert(type));
    }
}

/**************************************************************************************************
 * 添加波形数据。
 *************************************************************************************************/
void AGWaveWidget::addWaveformData(short data, int flag)
{
    addData(data, flag);
}

/**************************************************************************************************
 * 设置波形上下限。
 *************************************************************************************************/
void AGWaveWidget::setLimit(int low, int high)
{
    setValueRange(low, high);
}

/**************************************************************************************************
 * 设置标尺的标签值。
 *************************************************************************************************/
void AGWaveWidget::setRuler(AGDisplayZoom zoom)
{
    float zoomValue = 0;
    switch (zoom)
    {
    case AG_DISPLAY_ZOOM_4:
        _ruler->setRuler(4.0, 2.0, 0);
        zoomValue = 4.0;
        break;
    case AG_DISPLAY_ZOOM_8:
        _ruler->setRuler(8.0, 4.0, 0);
        zoomValue = 8.0;
        break;
    case AG_DISPLAY_ZOOM_15:
        _ruler->setRuler(15.0, 7.5, 0);
        zoomValue = 15.0;
        break;
    default:
        break;
    }

    QString str;
    str = QString("0.0~%1").arg(QString::number(zoomValue, 'f', 1));
//    str = QString("0.0~%1").number(zoomValue, 'f', 1);
    str += " ";
    str += trs("percent");
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AGWaveWidget::AGWaveWidget(WaveformID id, const QString &waveName, const AGTypeGas gasType)
    : WaveWidget(waveName, AGSymbol::convert(gasType))
    , _currentItemIndex(-1)
{
    setFocusPolicy(Qt::NoFocus);
    setID(id);

    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    setPalette(palette);

    int fontSize = fontManager.getFontSize(4);
    int fontH = fontManager.textHeightInPixels(fontManager.textFont(fontSize)) + 4;
    _name->setFont(fontManager.textFont(fontSize));
    _name->setFixedSize(130, fontH);
    _name->setText(getTitle());

    _ruler = new AGWaveRuler(this);
    _ruler->setPalette(palette);
    _ruler->setFont(fontManager.textFont(fontManager.getFontSize(0)));
    addItem(_ruler);

    setMargin(QMargins(WAVE_X_OFFSET, 2, 2, 2));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AGWaveWidget::~AGWaveWidget()
{
}

void AGWaveWidget::updateWidgetConfig()
{
    WaveWidget::updateWidgetConfig();
}

/**************************************************************************************************
 * 尺寸变化。
 *************************************************************************************************/
void AGWaveWidget::resizeEvent(QResizeEvent *e)
{
    _name->move(0, 0);
    _ruler->resize(RULER_X_OFFSET, 0,
                   width() - RULER_X_OFFSET,
                   height());
    WaveWidget::resizeEvent(e);
}

/**************************************************************************************************
 * 焦点进入。
 *************************************************************************************************/
void AGWaveWidget::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e);
    if (Qt::NoFocus != _name->focusPolicy())
    {
        _name->setFocus();
    }
}

void AGWaveWidget::_getItemIndex(int index)
{
    _currentItemIndex = index;
}

void AGWaveWidget::updatePalette(const QPalette &pal)
{
    _ruler->setPalette(pal);
    setPalette(pal);
    updateBackground();
}
