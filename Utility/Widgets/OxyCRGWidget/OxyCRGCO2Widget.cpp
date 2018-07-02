#include <QResizeEvent>
#include "OxyCRGCO2Widget.h"
#include "OxyCRGTrendWidgetRuler.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "WaveWidgetSelectMenu.h"
#include "SPO2Define.h"
#include "SPO2Param.h"
#include "ConfigManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
OxyCRGCO2Widget::OxyCRGCO2Widget(const QString &waveName, const QString &title)
    : OxyCRGTrendWidget(waveName, title)
{
    QPalette palette = colorManager.getPalette(paramInfo.getParamName(PARAM_CO2));
    setPalette(palette);
    setID(WAVE_CO2);
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
    currentConfig.getNumValue("OxyCRG|Ruler|CO2Low", valueLow);
    currentConfig.getNumValue("OxyCRG|Ruler|CO2High", valueHigh);
    setValueRange(valueLow, valueHigh);
    _ruler->setRuler(valueHigh, (valueLow+valueHigh)/2, valueLow);

    selectMode(SCROLL_MODE);

    _dataBufIndex = 0;
    _dataBufLen = dataRate()*4*60; //最大4分钟数据
    _dataBuf = new RingBuff<int>(_dataBufLen);
    _falgBuf = new RingBuff<int>(_dataBufLen);

    setMargin(QMargins(50,0,2,0));
}
/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGCO2Widget::~OxyCRGCO2Widget()
{

}
