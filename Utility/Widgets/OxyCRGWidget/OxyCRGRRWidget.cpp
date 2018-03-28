#include <QResizeEvent>
#include "OxyCRGRRWidget.h"
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
OxyCRGRRWidget::OxyCRGRRWidget(const QString &waveName, const QString &title)
    : OxyCRGTrendWidget(waveName, title)
{
    QPalette palette = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    setPalette(palette);

    // 标尺的颜色更深。
    QColor color = palette.color(QPalette::WindowText);
    palette.setColor(QPalette::Highlight, color);
    color.setRed(color.red() * 2 / 3);
    color.setGreen(color.green() * 2 / 3);
    color.setBlue(color.blue() * 2 / 3);
    palette.setColor(QPalette::WindowText, color);

    _ruler->setPalette(palette);

    setValueRange(0, 30);
    setWaveSpeed(1);
    selectMode(SCROLL_MODE);

    _ruler->setRuler(30, 15, 0);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGRRWidget::~OxyCRGRRWidget()
{

}
