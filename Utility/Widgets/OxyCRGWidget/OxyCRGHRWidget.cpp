/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/20
 **/



#include <QResizeEvent>
#include "OxyCRGHRWidget.h"
#include "OxyCRGTrendWidgetRuler.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "WaveWidgetSelectMenu.h"
#include "ConfigManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
OxyCRGHRWidget::OxyCRGHRWidget(const QString &waveName, const QString &title)
    : OxyCRGTrendWidget(waveName, title)
{
    QPalette palette = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    setPalette(palette);

    // 标尺的颜色更深。
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    palette.setColor(QPalette::WindowText, color);

    _ruler->setPalette(palette);

    int valueLow = 0;
    int valueHigh = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|HRLow", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|HRHigh", valueHigh);
    setValueRange(valueLow, valueHigh);
    setDataRate(1);

    selectMode(SCROLL_MODE);

    _dataBufIndex = 0;
    _dataBufLen = dataRate() * 8 * 60; // 最大8分钟数据
    _dataBuf = new RingBuff<int>(_dataBufLen);
    _falgBuf = new RingBuff<int>(_dataBufLen);

    _ruler->setRuler(valueHigh, (valueLow + valueHigh) / 2, valueLow);

    setMargin(QMargins(50 - 2, 0, 2, 0));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGHRWidget::~OxyCRGHRWidget()
{
}
