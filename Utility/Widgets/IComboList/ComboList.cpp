#include <QPainter>
#include <QKeyEvent>
#include "ComboList.h"
#include "ComboListPopup.h"
#include "ColorManager.h"

/**************************************************************************************************
 * 构造函数。
 * 参数:
 *      parent: 父控件
 *************************************************************************************************/
ComboList::ComboList(QWidget *parent)
        : QComboBox(parent), _isPopup(false), _radius(10), _popup(NULL), _drawCheckMark(true)
{
    QPalette p;
    p.setColor(QPalette::Highlight, colorManager.getHighlight());
    p.setColor(QPalette::Shadow, colorManager.getShadow());
    p.setColor(QPalette::WindowText, Qt::black);
    p.setColor(QPalette::HighlightedText, Qt::white);
    p.setColor(QPalette::ToolTipBase, QColor(146, 208, 80));

    setPalette(p);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 设置能接受焦点
    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(_popup);
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
ComboList::~ComboList()
{
    if (_popup)
    {
        delete _popup;
        _popup = NULL;
    }
}

/**************************************************************************************************
 * 显示弹出菜单。
 *************************************************************************************************/
void ComboList::showPopup()
{
    if (count() <= 0)
    {
        return;
    }

    if (!_popup)
    {
        _popup = new ComboListPopup(this);
        _popup->setFont(((QWidget*)parent())->font());
        connect(_popup, SIGNAL(destroyed()), this, SLOT(_popupDestroyed()));
        _popup->setItemDrawMark(_drawCheckMark);
    }

    _popup->show();


    // 下拉菜单弹出
    _isPopup = true;

}

/**************************************************************************************************
 * 设置边框倒角。
 *************************************************************************************************/
void ComboList::setBorderRadius(double radius)
{
    _radius = radius;
    // 设置弹出菜单的倒角
    if (NULL != _popup)
    {
        _popup->setBorderRadius(_radius);
    }
}

/**************************************************************************************************
 * 设置Item文本。
 *************************************************************************************************/
void ComboList::setItemText(int index, const QString &text)
{
    QComboBox::setItemText(index, text);

    if (NULL != _popup)
    {
        _popup->updateItemText(index, text);
    }
}

/**************************************************************************************************
 * 设置后缀。
 *************************************************************************************************/
void ComboList::setSuffix(const QString &suffix)
{
    _suffix = suffix;
}

/**************************************************************************************************
 * 获取后缀。
 *************************************************************************************************/
QString ComboList::getSuffix()
{
    return _suffix;
}

void ComboList::setDrawCheckMark(bool flag)
{
    _drawCheckMark = flag;
    if(_popup)
    {
        _popup->setItemDrawMark(flag);
    }
}

/**************************************************************************************************
 * 处理键盘和飞棱事件: 回车, 空格或飞棱按下时弹出菜单。
 *************************************************************************************************/
void ComboList::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Space:
        case Qt::Key_Left:
        case Qt::Key_Up:
        case Qt::Key_Right:
        case Qt::Key_Down:
            QComboBox::keyPressEvent(e);
            return;
        default://当有焦点时，会响应F4软按键弹出下拉列表
            break;
    }
}

/**************************************************************************************************
 * 处理键盘和飞棱事件: 回车, 空格或飞棱按下时弹出菜单。
 *************************************************************************************************/
void ComboList::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            showPopup();
            break;
        default:
            break;
    }

    QComboBox::keyReleaseEvent(e);
}

/**************************************************************************************************
 * 隐藏控件时, 强制关闭下拉菜单。
 *************************************************************************************************/
void ComboList::hideEvent(QHideEvent */*e*/)
{
    if (_popup)
    {
        _popup->close();
    }
}

/**************************************************************************************************
 * 绘图函数。
 *************************************************************************************************/
void ComboList::paintEvent(QPaintEvent */*e*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 填充背景
    painter.setPen(Qt::black);
    if (hasFocus())
    {
        // 聚焦
        painter.setBrush(palette().highlight()); // 亮色
    }
    else if (_isPopup)
    {
        // 弹出下拉框
        painter.setBrush(palette().toolTipBase()); //绿色
    }
    else if (!isEnabled())
    {
        painter.setBrush(palette().shadow());   // 暗色
    }
    else
    {
        painter.setBrush(Qt::white); // 无画刷
    }
    painter.drawRoundedRect(rect(), _radius, _radius);

    // 画底线/边线
    if (!_isPopup)
    {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(palette().color(QPalette::WindowText));
        painter.drawRoundedRect(rect(), _radius, _radius);
    }
    else
    {
        if (_popup->isPopupUp())
        {
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setPen(palette().color(QPalette::WindowText));
            QRect r = rect().adjusted(0, -_radius, 0, 0);
            painter.drawRoundedRect(r, _radius, _radius);
        }
        else
        {
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setPen(palette().color(QPalette::WindowText));
            QRect r = rect().adjusted(0, 0, 0, _radius);
            painter.drawRoundedRect(r, _radius, _radius);
        }
    }

    // 输出文字
    painter.setPen(hasFocus() ?
            palette().color(QPalette::HighlightedText) :
            palette().color(QPalette::WindowText));
    painter.drawText(rect(), Qt::AlignCenter, currentText() + _suffix);
}

/**************************************************************************************************
 * 弹出关闭时自动销毁, 清空对它的引用。
 *************************************************************************************************/
void ComboList::_popupDestroyed()
{
    _popup = NULL;
    hidePopup();

    // 下拉菜单隐藏
    _isPopup = false;
}
