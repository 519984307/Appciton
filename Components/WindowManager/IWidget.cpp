#include <QPainter>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include "IWidget.h"

/***************************************************************************************************
 * 功能：构造函数
 *
 * 参数:
 *      name: 窗体部件名称，用于窗体布局
 *      parent: 父窗体对象指针
 **************************************************************************************************/
IWidget::IWidget(const QString &name, QWidget *parent)
    : QWidget(parent)
    , _name(name)
    , _focusedBorderColor(Qt::white)
{
    setFocusPolicy(Qt::StrongFocus);
}

/***************************************************************************************************
 * 功能：大小调整事件，更新聚焦边框区域
 **************************************************************************************************/
void IWidget::resizeEvent(QResizeEvent */*e*/)
{
    _updateFocusRegion();
}

/***************************************************************************************************
 * 功能：响应聚焦事件
 **************************************************************************************************/
void IWidget::focusInEvent(QFocusEvent */*e*/)
{
    if (_focusRegion.isEmpty())
    {
        _updateFocusRegion();
    }

    update(_focusRegion);
}

/***************************************************************************************************
 * 功能：响应失焦事件
 **************************************************************************************************/
void IWidget::focusOutEvent(QFocusEvent */*e*/)
{
    if (_focusRegion.isEmpty())
    {
        _updateFocusRegion();
    }

    update(_focusRegion);
}

/***************************************************************************************************
 * 功能：响应重绘事件，绘制聚焦框
 **************************************************************************************************/
void IWidget::paintEvent(QPaintEvent *e)
{
    if (hasFocus())
    {
        QPainter painter(this);
        painter.setPen(QPen(_focusedBorderColor, _focusedBorderWidth / 2));
        painter.setRenderHint(QPainter::Antialiasing, true);
        QRect r = rect();
        r.adjust(_focusedBorderWidth / 2, _focusedBorderWidth / 2,
                 -_focusedBorderWidth / 2, -_focusedBorderWidth / 2);
        QRectF rf(r);
        painter.drawRoundedRect(rf, _focusedBorderWidth, _focusedBorderWidth);
    }

    QWidget::paintEvent(e);
}

/***************************************************************************************************
 * 功能：处理键盘和码盘事件（聚焦切换）
 **************************************************************************************************/
void IWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Left:
        case Qt::Key_Up:
            focusPreviousChild();
            break;

        case Qt::Key_Right:
        case Qt::Key_Down:
            focusNextChild();
            break;

        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (!e->isAutoRepeat())
            {
                emit clicked(this); // 发送按键单击信号
            }
            break;

        default:
            e->ignore();
            return;
    }
}

/***************************************************************************************************
 * 功能：处理键盘和码盘事件（聚焦切换）
 **************************************************************************************************/
void IWidget::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (!e->isAutoRepeat())
            {
                emit released();
                emit released(this); // 发送按键释放信号
            }
            break;

        default:
            e->ignore();
            return;
    }
}

/***************************************************************************************************
 * 功能：处理触摸屏点击按下事件
 **************************************************************************************************/
void IWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        emit clicked(this); // 发送鼠标单击信号
    }
    else
    {
        QWidget::mousePressEvent(e);
    }
}

/***************************************************************************************************
 * 功能：处理触摸屏点击释放事件
 **************************************************************************************************/
void IWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        emit released(this); // 发送鼠标释放信号
    }
    else
    {
        QWidget::mouseReleaseEvent(e);
    }
}

/***************************************************************************************************
 * 功能：更新聚焦框区域
 **************************************************************************************************/
void IWidget::_updateFocusRegion()
{
    _focusRegion = QRegion(0, 0, width(), _focusedBorderWidth);
    _focusRegion += QRegion(0, _focusedBorderWidth, _focusedBorderWidth,
            (height() - 2 * _focusedBorderWidth));
    _focusRegion += QRegion((width() - _focusedBorderWidth), _focusedBorderWidth,
            _focusedBorderWidth, (height() - 2 * _focusedBorderWidth));
    _focusRegion += QRegion(0, (height() - _focusedBorderWidth), width(), _focusedBorderWidth);
}
