////////////////////////////////////////////////////////////////////////////////
// 说明：
// 组合框弹出菜单的菜单项
////////////////////////////////////////////////////////////////////////////////
#include <QStylePainter>
#include <QStyleOption>
#include <QKeyEvent>
#include "ComboListItem.h"
#include "ColorManager.h"

/**************************************************************************************************
 * 构造函数。
 * 参数:
 *      flags: 菜单项标识, 是ItemType与RoundedCorner的位组合
 *************************************************************************************************/
ComboListItem::ComboListItem(int flags) : QAbstractButton(NULL) ,
        _flags(flags), _userData(-1), _isChecked(false), _isDrawCheckedMark(true)
{
    QPalette p;
    p.setColor(QPalette::Highlight, colorManager.getHighlight());
    p.setColor(QPalette::Shadow, colorManager.getShadow());
    setPalette(p);

    connect(this, SIGNAL(clicked()), this, SLOT(_itemClicked()));
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
ComboListItem::~ComboListItem()
{
}

/**************************************************************************************************
 * 处理键盘和飞棱事件。
 * 参数:
 *      e: 事件对象
 *************************************************************************************************/
void ComboListItem::keyPressEvent(QKeyEvent *e)
{
    e->ignore();
}

/**************************************************************************************************
 * 处理键盘和飞棱事件。
 * 参数:
 *      e: 事件对象
 *************************************************************************************************/
void ComboListItem::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Space:
            click();
            return;

        default:
            e->ignore();
            return;
    }
}

/**************************************************************************************************
 * 绘图函数。
 *************************************************************************************************/
void ComboListItem::paintEvent(QPaintEvent *e)
{
    if (NULL == e)
    {
        return;
    }

    QStylePainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().color(QPalette::Shadow));

    if (hasFocus())
    {
        painter.setBrush(palette().color(QPalette::Highlight));
    }
    else if (isEnabled())
    {
        painter.setBrush(palette().color(QPalette::Shadow));
    }
    else
    {
        painter.setBrush(palette().alternateBase());
    }

    QRect r;
//    if (_flags & Item_Last)
//    {
//        r = rect().adjusted(0, 0, -1, 0);

//    }
//    else
//    {
//        r = rect().adjusted(0, -1, -1, 0);
//    }
//    r = rect().adjusted(0, -1, -1, 0);
    painter.drawRect(rect());
//    painter.drawLine(rect().topLeft(),rect().bottomLeft());
//    painter.drawLine(rect().topRight(),rect().bottomRight());
//    painter.drawLine(rect().bottomLeft(),rect().bottomRight());

    if (_isChecked && _isDrawCheckedMark)
    {
        QStyleOptionMenuItem opt;
        opt.initFrom(this);
        opt.checkType = QStyleOptionMenuItem::NonExclusive;
        opt.checked = true;
        opt.rect = style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt, this);
        painter.drawPrimitive(QStyle::PE_IndicatorMenuCheckMark, opt);
    }


        painter.drawItemText(rect(), Qt::AlignCenter, palette(), isEnabled(), text(),
                hasFocus() ? QPalette::HighlightedText : QPalette::WindowText);

}

/**************************************************************************************************
 * 处理菜单项点击事件。
 *************************************************************************************************/
void ComboListItem::_itemClicked()
{
    emit clicked(_userData);
}
