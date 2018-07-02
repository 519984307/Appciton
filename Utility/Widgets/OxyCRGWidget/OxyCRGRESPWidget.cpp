#include <QResizeEvent>
#include "OxyCRGRESPWidget.h"
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
OxyCRGRESPWidget::OxyCRGRESPWidget(const QString &waveName, const QString &title)
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

    setValueRange(-256, 255);
    selectMode(SCROLL_MODE);

    _dataBufIndex = 0;
    _dataBufLen = dataRate()*4*60; //最大4分钟数据
    _dataBuf = new RingBuff<int>(_dataBufLen);
    _falgBuf = new RingBuff<int>(_dataBufLen);

    _ruler->setRuler(InvData(), InvData(), InvData());
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGRESPWidget::~OxyCRGRESPWidget()
{

}
