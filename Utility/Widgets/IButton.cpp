#include "IButton.h"
#include "ColorManager.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include "Debug.h"

void IButton::_onClicked(void)
{
    emit clicked(_id);
    emit clicked(text());
}

/***************************************************************************************************
 * 功能：设置是否显示边框。
 **************************************************************************************************/
void IButton::setBorderEnabled(bool border, bool isAntialiasing)
{
    _isBorderEnabled = border;
    if (!_isBorderEnabled)
    {
        _isAntialiasingWhenNoBorder = isAntialiasing;
    }
}

/***************************************************************************************************
 * 功能：设置边框的颜色。
 **************************************************************************************************/
void IButton::setBorderColor(QColor color)
{
    _borderColor = color;
}

/***************************************************************************************************
 * 功能：设置字体的颜色。
 **************************************************************************************************/
void IButton::setTextColor(QColor color)
{
    _textColor = color;
}

/***************************************************************************************************
 * 功能：设置不活动字体的颜色。
 **************************************************************************************************/
void IButton::setInactivityTextColor(QColor color)
{
    _inactivityTxtColor = color;
}

/***************************************************************************************************
 * 功能：设置高亮字体的颜色。
 **************************************************************************************************/
void IButton::setHighLightTextColor(QColor color)
{
    _highlightTxtColor = color;
}

/***************************************************************************************************
 * 功能：设置关闭按钮的背景颜色。
 **************************************************************************************************/
void IButton::setBackgroundColor(QColor color)
{
    QPalette p = this->palette();
    p.setColor(QPalette::Background, color);
    setPalette(p);
}

/***************************************************************************************************
 * 功能：设置按钮的不活动颜色。
 **************************************************************************************************/
void IButton::setShdowColor(QColor color)
{
    QPalette p = this->palette();
    p.setColor(QPalette::Shadow, color);
    setPalette(p);
}

/***************************************************************************************************
 * 功能：设置边框倒角。
 **************************************************************************************************/
void IButton::setBorderRadius(double radius)
{
    _radius = radius;
}

/***************************************************************************************************
 * 功能：设置各种背景图片。
 **************************************************************************************************/
void IButton::setPicture(const QImage &pic)
{
    _picture = pic;
    update();
}

/***************************************************************************************************
 * function:check picture is null。
 **************************************************************************************************/
bool IButton::isPictureNull()
{
    return _picture.isNull();
}

/***************************************************************************************************
 * 功能：设置文本的对齐方式。
 **************************************************************************************************/
void IButton::setAlignment(Qt::Alignment align)
{
    _textAlign = align;
    update();
}

/***************************************************************************************************
 * 功能：设置图片的对齐方式。
 **************************************************************************************************/
void IButton::setPicAlignment(Qt::Alignment align)
{
    _picAlign = align;
}

/***************************************************************************************************
 * 功能：设置响应按键。
 **************************************************************************************************/
void IButton::setEnableKeyAction(bool flag)
{
    _keyActionEnable = flag;
}

/***************************************************************************************************
 * 功能：设置背景的更改开关。
 **************************************************************************************************/
//void IButton::setBackgroundEnabled(bool enabled)
//{
//    update();
//}

/***************************************************************************************************
 * 功能：设置ID号。
 **************************************************************************************************/
void IButton::setId(int id)
{
    _id = id;
}

/***************************************************************************************************
 * 功能：返回ID号。
 **************************************************************************************************/
int IButton::id(void)
{
    return _id;
}

/***************************************************************************************************
 * 功能：响应重绘事件。
 **************************************************************************************************/
void IButton::paintEvent(QPaintEvent */*e*/)
{
    _paintButton();
    _paintPicture();
    _paintText();
}

/***************************************************************************************************
 * 功能：绘制按钮的轮廓和背景。如果设置了需要绘制的图像，选中效果会表现在图像透明部分。
 **************************************************************************************************/
void IButton::_paintButton()
{
    QPainter painter(this);

    // 设置画笔
    if (_isBorderEnabled)
    {
        // 有边框
        painter.setPen(QPen(_borderColor));
        painter.setRenderHint(QPainter::Antialiasing, true);
    }
    else
    {
        painter.setPen(Qt::NoPen);
        if (hasFocus())
        {
            painter.setRenderHint(QPainter::Antialiasing, true);
        }
        else
        {
            painter.setRenderHint(QPainter::Antialiasing, _isAntialiasingWhenNoBorder);
        }
    }

    // 设置画刷
    painter.setBrush(Qt::NoBrush); // 无画刷
    if (isEnabled() && hasFocus())
    {
        painter.setBrush(palette().highlight());    // 亮色
    }
    else if (isEnabled())
    {
        painter.setBrush(palette().background());  // 普通颜色
    }
    else
    {
        painter.setBrush(palette().shadow());      // 普通颜色
    }

    // 开始绘制按钮背景
    painter.drawRoundedRect(rect(), _radius, _radius);
}

/***************************************************************************************************
 * 功能：如果设置了背景图，进行绘制。
 **************************************************************************************************/
void IButton::_paintPicture()
{
    if (_picture.isNull())
    {
        return;
    }

    QPainter painter(this);
    QRect r = _picture.rect();

    if (rect().width() > r.width())
    {
        if (_picAlign & Qt::AlignRight)
        {
            r.moveRight(rect().right() - 5);
        }
        else if (_picAlign & Qt::AlignLeft)
        {
            r.moveLeft(rect().left() + 5);
        }
        else if (_picAlign & Qt::AlignHCenter)
        {
            r.moveLeft((rect().width() - r.width()) / 2);
        }
    }

    if (rect().height() > r.height())
    {
        if (_picAlign & Qt::AlignTop)
        {
            r.moveTop(rect().top() + 5);
        }
        else if (_picAlign & Qt::AlignBottom)
        {
            r.moveBottom(rect().bottom() - 5);
        }
        else if (_picAlign & Qt::AlignVCenter)
        {
            r.moveTop((rect().height() - r.height()) / 2);
        }
    }

    painter.drawImage(r, _picture);
}

/***************************************************************************************************
 * 功能：绘制按钮上的文字。
 **************************************************************************************************/
void IButton::_paintText()
{
    // TODO:
    // 设置字体

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(_textColor);

    if (isEnabled() && hasFocus())
    {
        painter.setPen(_highlightTxtColor);    // 亮色
    }
    else if (isEnabled())
    {
        painter.setPen(_textColor);  // 普通颜色
    }
    else
    {
        painter.setPen(_inactivityTxtColor);      // 普通颜色
    }

    // 这里需要人为设置一下垂直方向的居中对齐
    painter.drawText(rect(), _textAlign, text());
}

/***************************************************************************************************
 * 功能：响应鼠标进入事件。
 **************************************************************************************************/
//void IButton::enterEvent(QEvent *e)
//{
//    if (NULL == e)
//    {
//        return;
//    }

//    QPushButton::enterEvent(e);
//}

/***************************************************************************************************
 * 功能：响应鼠标退出事件。
 **************************************************************************************************/
//void IButton::leaveEvent(QEvent *e)
//{
//    if (NULL == e)
//    {
//        return;
//    }

//    QPushButton::leaveEvent(e);
//}

/***************************************************************************************************
 * 功能：处理键盘和飞棱事件。
 **************************************************************************************************/
void IButton::keyPressEvent(QKeyEvent *e)
{
    if (!_keyActionEnable)
    {
        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        {
            _isKeyPressed = true;
        }
        e->ignore();
        return;
    }

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
            _isKeyPressed = true;
        default:
            e->ignore();
            break;
    }
}

/***************************************************************************************************
 * 功能：处理键盘和飞棱事件。
 **************************************************************************************************/
void IButton::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        {
            if (!e->isAutoRepeat())
            {
                if(_isKeyPressed == true)
                {
                    emit realReleased();
                }
            }
            _isKeyPressed = false;
            break;
        }
        default:
            break;
    }

    QPushButton::keyReleaseEvent(e);
}

/***************************************************************************************************
 * 功能：触摸屏点击或按键按下事件。
 **************************************************************************************************/
void IButton::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        emit realReleased(); // 发出点击消息
    }

    QPushButton::mouseReleaseEvent(e);
}

/***************************************************************************************************
 * 功能：构造函数。
 **************************************************************************************************/
IButton::IButton(const QString &text, QWidget *parent) : QPushButton(text, parent),
        _radius(4.0), _isBorderEnabled(false), _isAntialiasingWhenNoBorder(false),
        _keyActionEnable(true),_isKeyPressed(false), _borderColor(Qt::black),
        _textColor(Qt::black), _inactivityTxtColor(Qt::black), _highlightTxtColor(Qt::black),
        _textAlign(Qt::AlignCenter), _picAlign(Qt::AlignCenter)
{
    QPalette p;
    p.setColor(QPalette::Highlight, colorManager.getHighlight());
    p.setColor(QPalette::Shadow, colorManager.getShadow());
    p.setColor(QPalette::Background, Qt::white);
    setPalette(p);

    // 设置能接受焦点
    this->setFocusPolicy(Qt::StrongFocus);
    connect(this, SIGNAL(clicked()), this, SLOT(_onClicked()));
}

/***************************************************************************************************
 * 功能：析构函数。
 **************************************************************************************************/
IButton::~IButton()
{
}
