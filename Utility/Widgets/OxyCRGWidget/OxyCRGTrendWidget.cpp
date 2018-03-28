#include <QResizeEvent>
#include "OxyCRGTrendWidget.h"
#include "OxyCRGTrendWidgetRuler.h"
#include "FontManager.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include <QPainter>
#include "WaveWidgetSelectMenu.h"

/**************************************************************************************************
 * 尺寸变化。
 *************************************************************************************************/
void OxyCRGTrendWidget::resizeEvent(QResizeEvent *e)
{
    _name->move(margin(), margin());

    _ruler->resize(2, margin(), width() - 2, height() - margin() * 2);
    WaveWidget::resizeEvent(e);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
OxyCRGTrendWidget::OxyCRGTrendWidget(const QString &waveName, const QString &title)
    : WaveWidget(waveName, title)
{
    setMargin(QMargins(2,0,2,0));

    setFocusPolicy(Qt::NoFocus);

    int fontSize = fontManager.getFontSize(7);
//    _name = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    _name->setFont(fontManager.textFont(fontSize));
    _name->setFixedSize(130, 30);
    _name->setText(title);

    _ruler = new OxyCRGTrendWidgetRuler(this);
    _ruler->setFont(fontManager.textFont(fontManager.getFontSize(0)));
    addItem(_ruler);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
OxyCRGTrendWidget::~OxyCRGTrendWidget()
{

}

void OxyCRGTrendWidget::paintEvent(QPaintEvent *e)
{
    WaveWidget::paintEvent(e);

    QPainter painter(this);

    // 绘制边框。
    painter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    painter.drawRect(rect());
}
