#include "CPRBarWidget.h"
#include "SystemInfo.h"
#include "FontManager.h"
#include "ColorManager.h"
#include "ParamInfo.h"
#include "Debug.h"
#include <QPainter>
#include <QLabel>
#include <QBoxLayout>
#include <QPaintEvent>

/**************************************************************************************************
 * 绘制标尺。
 *************************************************************************************************/
void CPRBarWidget::_paintRuler(QPainter &painter)
{
    QPen pen = painter.pen();

    // 绘制标尺。
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(_drawRect.left() + 1, _drawRect.top(), _drawRect.right(), _drawRect.top());
    if (_unit == UNIT_INCH)
    {
        painter.drawText(_drawRect.x() - 30, _drawRect.y() + 5, "0.0");
    }
    else
    {
        painter.drawText(_drawRect.x() - 30, _drawRect.y() + 5, "0.00");
    }

    QFontMetrics metrics(font());
    QString u("(inch)");
    int fontw = metrics.width(u) - metrics.width("0.0");
    if (_unit == UNIT_CM)
    {
        u = "(cm)";
        fontw = metrics.width(u) - metrics.width("0.00");
    }
    painter.drawText(_drawRect.x() - 30 - fontw / 2, _drawRect.y() + 15, u);

    // 最下面两条标尺线。
    pen.setStyle(Qt::DotLine);
    pen.setWidth(1);
    painter.setPen(pen);

    // 2.0inch标尺。
    int y = _drawRect.height() * 4 / 5 + _drawRect.top();
    painter.drawLine(_drawRect.left(), y, _drawRect.right(), y);
    if (_unit == UNIT_INCH)
    {
        painter.drawText(_drawRect.x() - 30, y + 5, "2.0");
    }
    else
    {
        painter.drawText(_drawRect.x() - 30, y + 5, "5.08");
    }

    // 2.5inch标尺。
    y = _drawRect.bottom();
    painter.drawLine(_drawRect.left(), y, _drawRect.right(), y);
    if (_unit == UNIT_INCH)
    {
        painter.drawText(_drawRect.x() - 30, y + 5, "2.5");
    }
    else
    {
        painter.drawText(_drawRect.x() - 30, y + 5, "6.35");
    }
}

/**************************************************************************************************
 * 绘制Bar图。
 *************************************************************************************************/
void CPRBarWidget::_paintBar(QPainter &painter)
{
    if (_barValue.isEmpty())
    {
        return;
    }

    QPainter pixPainter(_background);
    painter.setPen(Qt::NoPen);
    pixPainter.setPen(Qt::NoPen);

    while (_barValue.size() >= _dataNRPerBar)
    {

        int w = (int)(_pixelPerBar + _drawError);
        int v = 0;
        _drawError = (_pixelPerBar + _drawError) - w;
        if (_isDrawBar)
        {
            v = _barValue[0];
        }
        _isDrawBar = !_isDrawBar;

        // 先清除历史数据。这里只清除了pixmap中的历史数据，而控件的历史数据被CPRWidget清除了，所以这里不再清除。
        pixPainter.setBrush(QBrush(palette().color(QPalette::Window)));
        pixPainter.drawRect(_xPos - _drawRect.x(), 0, w, _drawRect.height());

        // 绘制不同bar图。
        painter.setBrush(QBrush(palette().color(QPalette::WindowText)));
        pixPainter.setBrush(QBrush(palette().color(QPalette::WindowText)));

        int y = v * _drawRect.height() / (_maxValue - _minValue) + _drawRect.y();
        if (v != 0)
        {
            painter.drawRect(_xPos, _drawRect.y(), w, y);
            pixPainter.drawRect(_xPos - _drawRect.x(), 0, w, y);
        }
        _xPos += w;
        _xPos = (_xPos >= _drawRect.width() + _drawRect.x()) ? _drawRect.x() : _xPos;

        // 绘制完成后清除数据。
        _barValue.remove(0, _dataNRPerBar);
    }
}

/**************************************************************************************************
 * 窗口重绘事件。
 *************************************************************************************************/
void CPRBarWidget::paintEvent(QPaintEvent *e)
{
    if (_resetBackground)
    {
        _resetBackground = false;
        if (_background != NULL)
        {
            if ((_background->width() != width()) || (_background->height() != height()))
            {
                delete _background;
                _background = NULL;
            }
        }

        if (_background == NULL)
        {
            _background = new QPixmap(_drawRect.width(), _drawRect.height());
        }

        // 底色
        QPainter painter(_background);
        painter.fillRect(_background->rect(), palette().window());
    }

    QPainter painter(this);

    // 补绘所有的Bar图。在bar更新时只会无效很窄的区域，但区域变大说明该控件需要全部重绘。
    if (e->rect().width() > (int)(_pixelPerBar + _blankBarWidth))
    {
        painter.drawPixmap(_drawRect, *_background);
    }

    _paintRuler(painter);
    _paintBar(painter);
}

/**************************************************************************************************
 * 窗口大小改变事件。
 *************************************************************************************************/
void CPRBarWidget::resizeEvent(QResizeEvent *e)
{
    _drawRect = rect().adjusted(50, 10, 0, -10);
    _xPos = _drawRect.x();
    _resetBackground = true;

    QWidget::resizeEvent(e);
}

/**************************************************************************************************
 * 更新数据。
 *************************************************************************************************/
void CPRBarWidget::updateValue(short value[], int len)
{
    for (int i = 0 ; i < len; i++)
    {
        _barValue.append(value[i]);
    }

    // 需要绘制1个Bar。
    if (_barValue.size() >= _dataNRPerBar)
    {
        int x = _xPos;
        int y = _drawRect.y() + 1;
        int w = (int)(_pixelPerBar + _blankBarWidth);
        int h = _drawRect.height() - 1;
        update(x, y, w, h);
    }
}

/**************************************************************************************************
 * 设置数据采样率。
 *************************************************************************************************/
void CPRBarWidget::setSampleRate(short sample)
{
    _pixelPerBar = _barWidth / systemInfo.getScreenPixelPitch();
    _dataNRPerBar = _barWidth * (sample / _speed);
}

/**************************************************************************************************
 * 设置数据的范围。
 *************************************************************************************************/
void CPRBarWidget::setRange(int min, int max)
{
    _minValue = min;
    _maxValue = max;
}

/**************************************************************************************************
 * 设置显示单位。
 *************************************************************************************************/
void CPRBarWidget::setUnit(UnitType unit)
{
    if (unit == _unit)
    {
        return;
    }

    _unit = unit;
    update();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CPRBarWidget::CPRBarWidget()
{
    _xPos = 0;
    _drawError = 0.0;
    _minValue = 0;
    _maxValue = 1000;
    _isDrawBar = true;
    _pixelPerBar = 5;
    _dataNRPerBar = 25;
    _unit = UNIT_INCH;
    _background = NULL;
    _resetBackground = true;

    setFont(fontManager.textFont(12));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CPRBarWidget::~CPRBarWidget()
{
    if (_background != NULL)
    {
        delete _background;
    }
}
