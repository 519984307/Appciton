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
#include <QTimer>

/**************************************************************************************************
 * 尺寸变化。
 *************************************************************************************************/
void OxyCRGTrendWidget::resizeEvent(QResizeEvent *e)
{
    _name->move(2, 2);

    _ruler->resize(2, 2, width() - 2, height() - 2 * 2);

    WaveWidget::resizeEvent(e);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
OxyCRGTrendWidget::OxyCRGTrendWidget(const QString &waveName, const QString &title)
    : WaveWidget(waveName, title),_dataSizeLast(0)
{
    setMargin(QMargins(50,0,2,0));/*调整波形位置*/

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

int OxyCRGTrendWidget::getRulerWidth()const
{
    return _ruler->width();
}

void OxyCRGTrendWidget::addDataBuf(int value, int flag)
{
    _dataBuf->push(value);
    _falgBuf->push(flag);
    _dataBufIndex ++;
    if(_dataBufIndex>=_dataBufLen)
    {
        _dataBufIndex = _dataBufLen - 1;
    }
}

int OxyCRGTrendWidget::getRuleHeight()const
{
    return _ruler->height();
}

float OxyCRGTrendWidget::getRulerPixWidth()const
{
    return _ruler->getPixWidth();
}

void OxyCRGTrendWidget::setRuler(int up, int mid, int low)
{
    _ruler->setRuler(up, mid, low);
}

void OxyCRGTrendWidget::_resetBuffer()
{
    WaveWidget::_resetBuffer();

    if(bufSize() != _dataSizeLast)
    {
        _dataSizeLast = bufSize();

        if(_dataSizeLast > _dataBufLen)
        {
            _dataSizeLast = _dataBufLen;
        }

        if(_dataSizeLast > _dataBufIndex)
        {
            _dataSizeLast = _dataBufIndex;
        }

        QTimer::singleShot(0, this, SLOT(onTimeout()));
    }
}

void OxyCRGTrendWidget::onTimeout()
{
    for(int i=0; i<_dataSizeLast && i<_dataBuf->dataSize(); i++)
    {
        addData(_dataBuf->at(i), _falgBuf->at(i), false);
    }

    update();
}

void OxyCRGTrendWidget::showEvent(QShowEvent *event)
{
    WaveWidget::showEvent(event);
     _dataBufIndex = 0;
}
void OxyCRGTrendWidget::hideEvent(QHideEvent *event)
{
    WaveWidget::hideEvent(event);
     _dataBufIndex = 0;
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
