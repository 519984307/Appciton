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

    int valueLow=0;
    int valueHigh=0;
    currentConfig.getNumValue("OxyCRG|Ruler|HRLow", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|HRHigh", valueHigh);
    setValueRange(valueLow, valueHigh);
    setDataRate(1);

    selectMode(SCROLL_MODE);

    _dataBufIndex = 0;
    _dataBufLen = 256;
    _spaceFlag = new int[_dataBufLen];
    _dataBuf = new int[_dataBufLen];

    _ruler->setRuler(valueHigh, (valueLow+valueHigh)/2, valueLow);

    setMargin(QMargins(50-2,0,2,0));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGHRWidget::~OxyCRGHRWidget()
{

}
