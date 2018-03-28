////////////////////////////////////////////////////////////////////////////////
// 说明：
// 组合框控件的弹出菜单
////////////////////////////////////////////////////////////////////////////////
#include <QVBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include "ComboListPopup.h"
#include "ComboList.h"
#include "ComboListItem.h"
#include "ColorManager.h"
#include "IDropList.h"
#include "Debug.h"

#include <QScrollArea>
#include <QScrollBar>

ComboListPopup *ComboListPopup::_current = NULL;

/**************************************************************************************************
 * 构造函数。
 * 参数:
 *      combo: 关联的组合框控件对象
 *************************************************************************************************/
ComboListPopup::ComboListPopup(QWidget *parent, int type, int count, int current) :
    QWidget(NULL, Qt::Popup | Qt::FramelessWindowHint), _parent(parent), _type(type),
    _index(-1), _boardWidth(1), _radius(3.0), _boardColor(Qt::black), _count(count),
    _curIndex(current), _isPopup(false), _isErrorClose(true)
{
    _itemTxt.clear();

    QPalette p;
    p.setColor(QPalette::Highlight, colorManager.getHighlight());
    p.setColor(QPalette::Window, colorManager.getShadow());
    setPalette(p);

    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);

    int itemCount = _getCount();
    int Height = parent->height();
    if (itemCount < _pageSize)
    {
        Height = (parent->height() * itemCount);
    }
    else
    {
        Height = (parent->height() * 6);
    }

    //创建ListWidget
    _listwidget = new CombolistWidget();
    _listwidget->setSpacing(0);
    _listwidget->setFrameStyle(QFrame::NoFrame);
    _listwidget->setAutoScroll(true);
    _listwidget->setAutoScrollMargin(1);
    _listwidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{width:10px;"
                                                    "background:rgba(255,255,255,100%);"
                                                    "border:0px solid white;"
                                                    "border-radius:4px;}"
                                                    "QScrollBar::handle:vertical{border-radius:4px;"
                                                    "background:rgb(230, 230, 230);"
                                                    "border:1px solid rgb(140, 140, 140);}"
                                                    "QScrollBar::add-line:vertical{height:0px}"
                                                    "QScrollBar::sub-line:vertical{height:0px}");

    _listwidget->setResizeMode(QListWidget::Fixed);
    _listwidget->setFixedWidth(parent->width() - 2 - 2 * _boardWidth);
    _listwidget->setFixedHeight(Height);

    //添加Item
    for (int n = 0; (n < itemCount); n++)
    {
        //最后一个Item为圆角
        int flags = ComboListItem::Item_First;
        if (n == (itemCount - 1))
        {
            flags |= (ComboListItem::Item_Last);
        }
        //添加Item
        ComboListItem *comboListItem = new ComboListItem(flags);
        connect(comboListItem, SIGNAL(clicked(int)), this, SLOT(_itemClicked(int)));
        _items.append(comboListItem);

        //添加QListWidgetItem
        QListWidgetItem *button_item = new QListWidgetItem();
        button_item->setTextAlignment(Qt::AlignCenter);
        button_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        button_item->setSizeHint(QSize(0, _parent->height()));
        _listwidget->addItem(button_item);

        //实现替换，自定义item
        _listwidget->setItemWidget(button_item,comboListItem);

    }

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setMargin(1);
    vbox->setSpacing(0);
    vbox->addWidget(_listwidget,0,Qt::AlignCenter);
    setLayout(vbox);
    _borderheight = _boardWidth * 2 + 2;
    setFixedSize(parent->width(), Height + 2 * _boardWidth + 2);
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
ComboListPopup::~ComboListPopup()
{
    _itemTxt.clear();
}

/**************************************************************************************************
 * 设置边框倒角。
 *************************************************************************************************/
void ComboListPopup::setBorderRadius(double radius)
{
    _radius = radius;
}

/**************************************************************************************************
 * 设置边框倒角。
 *************************************************************************************************/
void ComboListPopup::setBorderWidth(int width)
{
    _listwidget->setFixedWidth(_parent->width() - 2 * width - 2);
    setFixedHeight(this->height() + (width - _boardWidth) * 2);
    _boardWidth = width;
    _borderheight = _boardWidth * 2 + 2;
}

/**************************************************************************************************
 * 设置是否画选中标记。
 *************************************************************************************************/
void ComboListPopup::setItemDrawMark(bool drawMark)
{
    int count = _items.count();
    ComboListItem *item = NULL;
    for (int i = 0; i < count; ++i)
    {
        item = _items.at(i);
        if (NULL != item)
        {
            item->setDrawCheckedMark(drawMark);
        }
    }
}

/**************************************************************************************************
 * 设置子项名称。
 *************************************************************************************************/
void ComboListPopup::addItemText(const QString &txt)
{
    if (_type != POPUP_TYPE_USER)
    {
        return;
    }

    _itemTxt.append(txt);
}

/**************************************************************************************************
 * 更新子项名称。
 *************************************************************************************************/
void ComboListPopup::updateItemText(int index, const QString &txt)
{
    if (index > _items.count() || index < 0)
    {
        return;
    }

    if (_type == POPUP_TYPE_USER)
    {
        _itemTxt[index] = txt;
    }

    //添加Item的文字
    ComboListItem *comboListItem = _items.at(index);
    comboListItem->setText(txt);
    comboListItem->update();
}

/**************************************************************************************************
 * 获取当前索引。
 *************************************************************************************************/
int ComboListPopup::getCurIndex()
{
    if (_type != POPUP_TYPE_USER)
    {
        return -1;
    }

    if (_isErrorClose)
    {
        return -1;
    }

    return _curIndex;
}

bool ComboListPopup::eventFilter(QObject *obj, QEvent *ev)
{
    ComboList *cmblist = qobject_cast<ComboList *>(obj);
    if(cmblist)
    {
        if(ev->type() == QEvent::KeyPress)
        {
            return true;
        }

    }
    return false;
}

void ComboListPopup::updatePos()
{
    if(!isVisible() || _parent == NULL)
    {
        return;
    }

    int y;
    int itemCount = _getCount();

    if (itemCount == 0)
    {
        y = 0;
    }
    else if (itemCount <= _pageSize)
    {
        y = _items.at(0)->height() * itemCount;
    }
    else
    {
        y = _items.at(0)->height() * _pageSize;
    }

    QDesktopWidget *desktop = QApplication::desktop();
    QPoint pos = _parent->mapToGlobal(_parent->rect().bottomLeft());
    int y1 = desktop->height();
    int y2 = pos.y() + y;

    if ((y2 + _borderheight) < y1)
    {
        move(pos + QPoint(0, 1));
    }
    else
    {
        move(_parent->mapToGlobal(_parent->rect().topLeft()) - QPoint(0, y + _borderheight));
    }
}


/**************************************************************************************************
 * 处理键盘和飞棱事件, 切换聚集或菜单项翻页。
 *************************************************************************************************/
void ComboListPopup::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Escape:
        _isErrorClose = false;
        close();
        return;
    default:
        break;
    }
}

/**************************************************************************************************
 * 功能:显示
 *************************************************************************************************/
void ComboListPopup::showEvent(QShowEvent *e)
{
    _current = this;

    _showIndex(_getCurIndex());
    _relocate();

    QWidget::showEvent(e);
}

/**************************************************************************************************
 * 功能:隐藏
 *************************************************************************************************/
void ComboListPopup::hideEvent(QHideEvent *e)
{
    _current = NULL;
    QWidget::hideEvent(e);
}

/**************************************************************************************************
 * 绘图函数。
 *************************************************************************************************/
void ComboListPopup::paintEvent(QPaintEvent */*e*/)
{
    //外框体
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QRect r;
    painter.setPen(QPen(_boardColor, _boardWidth));
    painter.setBrush(palette().background());
    if (isPopupUp())
    {
        QPainterPath clipPath;
        clipPath.addRoundedRect(rect(), _radius, _radius);
        painter.setClipPath(clipPath);
        painter.drawRect(rect());
    }
    else
    {
        r = rect().adjusted(0, -_radius, 0, 0);
        painter.drawRoundedRect(r, _radius, _radius);
    }
}

/**************************************************************************************************
 * 处理菜单项点击: 指定当前项并关闭菜单。
 * 参数:
 *      index: 组合框中的选项索引。
 *************************************************************************************************/
void ComboListPopup::_itemClicked(int index)
{
    _isErrorClose = false;
    close();
    _setCurIndex(index);
}

/**************************************************************************************************
 * 保证指定选项可见。
 * 参数:
 *      index: 组合框中的选项索引
 *      isFocus: 是否聚焦
 *************************************************************************************************/
void ComboListPopup::_showIndex(int index)
{
    _index = qBound(0, index, _getCount());
    int i = _index;

    _listwidget->setFocus();

    for (int n = 0; n < _getCount(); n++)
    {
        //添加Item的文字
        ComboListItem *comboListItem = _items.at(n);
        comboListItem->setUserData(n);
        comboListItem->setChecked(_getCurIndex() == n);
        comboListItem->setText(_getItemTxt(n));
    }

    //设置当前选项
    QListWidgetItem *listItem = _listwidget->item(i);
    _listwidget->setCurrentItem(listItem);
}

/**************************************************************************************************
 * 调整下拉菜单位置。
 *************************************************************************************************/
void ComboListPopup::_relocate()
{
    int y;
    int itemCount = _getCount();

    if (itemCount == 0)
    {
        y = 0;
    }
    else if (itemCount <= _pageSize)
    {
        y = _items.at(0)->height() * itemCount;
    }
    else
    {
        y = _items.at(0)->height() * _pageSize;
    }

    QDesktopWidget *desktop = QApplication::desktop();
    QPoint pos = _parent->mapToGlobal(_parent->rect().bottomLeft());
    int y1 = desktop->height();
    int y2 = pos.y() + y;

    if ((y2 + _borderheight) < y1)
    {
        move(pos + QPoint(0, 1));
    }
    else
    {
        move(_parent->mapToGlobal(_parent->rect().topLeft()) - QPoint(0, y + _borderheight));
        popupUp(true);
    }
}

/**************************************************************************************************
 * 获取当前项。
 *************************************************************************************************/
int ComboListPopup::_getCurIndex()
{
    if (NULL == _parent)
    {
        return 0;
    }

    switch (_type)
    {
    case POPUP_TYPE_COMBOLIST:
    {
        ComboList *list = (ComboList*)_parent;
        return list->currentIndex();
    }
    case POPUP_TYPE_DROPLIST:
    {
        IDropList *list = (IDropList*)_parent;
        return list->currentIndex();
    }
    default:
        return _curIndex;
    }


}

/**************************************************************************************************
 * 获取列表总数。
 *************************************************************************************************/
int ComboListPopup::_getCount()
{
    if (NULL == _parent)
    {
        return 0;
    }

    switch (_type)
    {
    case POPUP_TYPE_COMBOLIST:
    {
        ComboList *list = (ComboList*)_parent;
        return list->count();
    }
    case POPUP_TYPE_DROPLIST:
    {
        IDropList *list = (IDropList*)_parent;
        return list->count();
    }
    default:
        return _count;
    }
}

/**************************************************************************************************
 * 获取指定项的字符串。
 *************************************************************************************************/
QString ComboListPopup::_getItemTxt(int index)
{
    if (NULL == _parent)
    {
        return QString("");
    }

    switch (_type)
    {
    case POPUP_TYPE_COMBOLIST:
    {
        ComboList *list = (ComboList*)_parent;
        return list->itemText(index);
    }
    case POPUP_TYPE_DROPLIST:
    {
        IDropList *list = (IDropList*)_parent;
        return list->itemText(index);
    }
    default:
        if (index >= _itemTxt.count())
        {
            return QString("");
        }
        else
        {
            return _itemTxt.at(index);
        }
    }
}

/**************************************************************************************************
 * 设置当前项。
 *************************************************************************************************/
void ComboListPopup::_setCurIndex(int index)
{
    if (NULL == _parent)
    {
        return ;
    }

    switch (_type)
    {
    case POPUP_TYPE_COMBOLIST:
    {
        ComboList *list = dynamic_cast<ComboList*>(_parent);
        if (NULL != list)
        {
            return list->setCurrentIndex(index);
        }
        break;
    }
    case POPUP_TYPE_DROPLIST:
    {
        IDropList *list = dynamic_cast<IDropList*>(_parent);
        if (NULL != list)
        {
            return list->setCurrentIndex(index);
        }
        break;
    }
    default:
        _curIndex = index;
        return;
    }
}
