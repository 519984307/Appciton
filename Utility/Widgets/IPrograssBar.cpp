#include "IPrograssBar.h"
#include <QPainter>

/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
IPrograssBar::IPrograssBar() : QWidget()
{
    _curValue = 0;

    _prograssBgColor = Qt::black;
    _prograssFgColor = Qt::yellow;

    _boardWidth = 0;
    _radius = 8.0;

    setFixedHeight(12);
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
IPrograssBar::~IPrograssBar()
{

}

/**************************************************************************************************
 * 绘画。
 *************************************************************************************************/
void IPrograssBar::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QRect r = rect();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setBrush(_prograssBgColor);
    p.setPen(QPen(_prograssBgColor, 2));
    p.drawRoundedRect(r, _radius, _radius);

    QRect srcRect(r.x() + _boardWidth + 1, r.y() + _boardWidth,
            r.width() - 2 * (_boardWidth + 1),
            r.height() - 2 * _boardWidth);

    if (_curValue > 0 && _curValue <= 100)
    {
        QRect dstRect(r.x() + _boardWidth + 1, r.y() + _boardWidth,
                0, r.height() - 2 * _boardWidth);
        dstRect.setWidth((100 == _curValue) ? srcRect.width() :
                srcRect.width() * _curValue / 100 - 2 * (_boardWidth + 1));
        p.setBrush(_prograssFgColor);
        p.drawRoundedRect(dstRect, _radius / 2, _radius / 2);
    }
}

/**************************************************************************************************
 * 设置当前进度条的值。
 *************************************************************************************************/
void IPrograssBar::setPrograssValue(int value)
{
    if (value >= 100)
    {
        value = 100;
    }

    _curValue = value;
    if (_curValue == 100)
    {
        repaint();
    }
    else
    {
        update();
    }
}

/**************************************************************************************************
 * 设置进度条背景色。
 *************************************************************************************************/
void IPrograssBar::setBarBackgroundColor(QColor color)
{
    _prograssBgColor = color;
}

/**************************************************************************************************
 * 设置进度条前景色。
 *************************************************************************************************/
void IPrograssBar::setBarForegroundColor(QColor color)
{
    _prograssFgColor = color;
}

/**************************************************************************************************
 * 设置边框幅度。
 *************************************************************************************************/
void IPrograssBar::setBoardRadius(double radius)
{
    _radius = radius;
}

/**************************************************************************************************
 * 设置边框宽度。
 *************************************************************************************************/
void IPrograssBar::setBoardWidth(int width)
{
    _boardWidth = width;
}


