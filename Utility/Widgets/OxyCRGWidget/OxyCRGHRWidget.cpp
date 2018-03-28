#include <QResizeEvent>
#include "OxyCRGHRWidget.h"
#include "OxyCRGTrendWidgetRuler.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "WaveWidgetSelectMenu.h"

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

    setValueRange(0, 300);
    setWaveSpeed(1);
    selectMode(SCROLL_MODE);

    _ruler->setRuler(300, 150, 0);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGHRWidget::~OxyCRGHRWidget()
{

}
