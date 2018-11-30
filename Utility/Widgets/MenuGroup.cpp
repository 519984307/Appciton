/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/18
 **/



#include "MenuGroup.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QBitmap>
#include <QScrollArea>
#include <QScrollBar>
#include <QApplication>
#include "SubMenu.h"
#include "SetWidget.h"
#include "WindowManager.h"
#include "ColorManager.h"
#include "LabelButton.h"
#include "IButton.h"
#include "Debug.h"
#include "MenuWidget.h"
#include "MenuManager.h"
#include "LayoutManager.h"

/**************************************************************************************************
 * 初始化菜单列表项。
 *************************************************************************************************/
void MenuGroup::initMenuList()
{
    setFocusOrder(true);
    _listTable->setCurrentRow(0);
    _listTable->setFocus();
}

/**************************************************************************************************
 * 重新聚焦菜单列表。
 *************************************************************************************************/
void MenuGroup::returnMenuList()
{
    setFocusOrder(true);
    _listTable->setFocus();
}

/**************************************************************************************************
 * 需要更新标题。
 *************************************************************************************************/
void MenuGroup::_titleChanged(void)
{
    SubMenu *m = qobject_cast<SubMenu*>(_subMenus->currentWidget());
    titleLabel->setText(m->desc());
}

/**************************************************************************************************
 * 子菜单页聚焦
 *************************************************************************************************/
void MenuGroup::itemClicked()
{
    SubMenu *m = qobject_cast<SubMenu *>(_scorllArea->widget());

    setFocusOrder(false);
    m->focusFirstItem();
}

/**************************************************************************************************
 * 关闭按钮聚焦
 *************************************************************************************************/
void MenuGroup::_closeBtnSetFoucs()
{
    closeBtnSetFoucs();
}

/**************************************************************************************************
 * 改变滚动区域滚动条的值
 *************************************************************************************************/
void MenuGroup::_changeScrollValue(int value)
{
    QScrollBar *bar = _scorllArea->verticalScrollBar();
    if (1 == value)
    {
        bar->setValue(bar->maximum());
    }
    else
    {
        bar->setValue(0);
    }
}

/**************************************************************************************************
 * 隐藏事件。
 *************************************************************************************************/
void MenuGroup::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
}

/**************************************************************************************************
 * 按键事件。
 *************************************************************************************************/
void MenuGroup::keyPressEvent(QKeyEvent *event)
{
    if (!_closeStrongFocus)
    {
        switch (event->key())
        {
        case Qt::Key_Up:
        case Qt::Key_Left:
            if (Qt::StrongFocus == _listTable->focusPolicy() && closeBtnHasFocus())
            {
                _listTable->setCurrentRow(_listTable->count() - 1);
                _listTable->setFocus();
            }
            else
            {
                if (_return->hasFocus())
                {
                    _changeScrollValue(1);
                }

                focusPreviousChild();
            }
            return;
        case Qt::Key_Down:
        case Qt::Key_Right:
            if (Qt::StrongFocus == _listTable->focusPolicy() && closeBtnHasFocus())
            {
                _listTable->setCurrentRow(0);
                _listTable->setFocus();
            }
            else
            {
                if (closeBtnHasFocus())
                {
                    _changeScrollValue(0);
                }

                focusNextChild();
            }
            return;
        default:
            break;
        }
    }

    QWidget::keyPressEvent(event);
}

/**************************************************************************************************
 * 功能： 改变当前页。
 * 参数：
 *      current: 当前页对象。
 *      previous: 上个页对象。
 *************************************************************************************************/
void MenuGroup::_changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
    {
        current = previous;
    }

    // 重新添加submenu，滚动区域设置新的widget时，原来的会被删除
    QWidget *win = _scorllArea->takeWidget();
    if (NULL != win)
    {
        int index = _listTable->row(previous);
        _subMenus->insertWidget(index, win);
    }

    _subMenus->setCurrentIndex(_listTable->row(current));
    SubMenu *m = qobject_cast<SubMenu *>(_subMenus->currentWidget());

    _scorllArea->setWidget(m);
    titleLabel->setText(m->desc());
}
/**************************************************************************************************
 * 功能： 改变当前页。
 * 参数：
 *      current: 当前页对象。
 *      previous: 上个页对象。
 *************************************************************************************************/
void MenuGroup::changePage(SubMenu *current, SubMenu *previous)
{
    if (current == previous)
    {
        return;
    }
    for (int i = 0; i < _listTable->count(); i++)
    {
        if (current->name() == _listTable->item(i)->text())
        {
            _listTable->setCurrentRow(i);
            break;
        }
    }
}

/**************************************************************************************************
 * 功能：反向操作显现或隐藏。
 *************************************************************************************************/
void MenuGroup::popup(int x, int y)
{
    setFocusOrder(true);
    _listTable->setCurrentRow(0);
    _listTable->setFocus();

    if (x == 0 && y == 0)
    {
        QRect r = layoutManager.getMenuArea();
        x = r.x() + (r.width() - width()) / 2;
        y = r.y() + (r.height() - height());
    }
//    move(x, y);
//    show();
    menuManager.popupWidegt(this, x, y);
}

/**************************************************************************************************
 * 功能： 设置当前子菜单。
 * 参数：
 *      widget：需聚焦的子菜单。
 *************************************************************************************************/
void MenuGroup::setCurrentSubmenu(QWidget *widget)
{
    if (widget)
    {
        _subMenus->setCurrentIndex(2);
    }
}

/**************************************************************************************************
 * 功能： 弹出菜单并聚焦到指定的页面。
 * 参数：
 *      menu：需聚焦的子菜单。
 *************************************************************************************************/
void MenuGroup::popup(SubMenu *menu, int x, int y)
{
    if (menu == NULL)
    {
        return;
    }

    int index = -1;
    // 重新添加submenu，滚动区域设置新的widget时，原来的会被删除
    SubMenu *win = qobject_cast<SubMenu *>(_scorllArea->widget());
    if (NULL != win)
    {
        if (menu != win)
        {
            _scorllArea->takeWidget();
            _subMenus->insertWidget(_listTable->currentRow(), win);
            index = _subMenus->indexOf(menu);
        }
        else
        {
            index = _listTable->currentRow();
        }
    }
    else
    {
        index = _subMenus->indexOf(menu);
    }

    // 聚焦在正确的子菜单。
    _listTable->setCurrentRow(index);
    menu->focusFirstItem();
    setFocusOrder(false);

    if (x == 0 && y == 0)
    {
        QRect r = layoutManager.getMenuArea();
        x = r.x() + (r.width() - width()) / 2;
        y = r.y() + (r.height() - height());
    }

//    menuManager.openWidget(this);
//    menuManager.move(x, y);
//    menuManager.show();
    menuManager.popupWidegt(this, x, y);
}

/**************************************************************************************************
 * 功能：添加子菜单。
 * 参数：
 *      SubMenu: 菜单对象。
 *************************************************************************************************/
void MenuGroup::addSubMenu(SubMenu *subMenu)
{
    // 增加一个列表项。
    QListWidgetItem *listItem = new QListWidgetItem(_listTable);
    listItem->setText(subMenu->name());
    listItem->setTextAlignment(Qt::AlignCenter);
    listItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    listItem->setFont(fontManager.textFont(fontManager.getFontSize(2)));
    listItem->setSizeHint(QSize(0, _listItemHeight));

    // 添加子菜单。
    _subMenus->addWidget(subMenu);

    // 连接菜单关闭信息。
    connect(subMenu, SIGNAL(closeMenu()), this, SLOT(close()));
    connect(subMenu, SIGNAL(titleChanged()), this, SLOT(_titleChanged()));
    connect(subMenu, SIGNAL(scrollAreaChange(int)), this, SLOT(_changeScrollValue(int)));
    connect(subMenu, SIGNAL(returnItemList()), this, SLOT(returnMenuList()));
}

/**************************************************************************************************
 * 设置列表区的宽度。
 *************************************************************************************************/
void MenuGroup::setListWidth(int w)
{
    _listTable->setFixedWidth(w);
}

/**************************************************************************************************
 * 设置滚动区。
 *************************************************************************************************/
void MenuGroup::setScrollAreaSize(int w, int h)
{
    _scorllArea->setFixedSize(w, h);
}

/**************************************************************************************************
 * 设置焦点顺序。
 *************************************************************************************************/
void MenuGroup::setFocusOrder(bool flag)
{
    _closeStrongFocus = false;

    QList<QWidget *> allWidget;
    allWidget.clear();

    if (flag)
    {
        _listTable->setFocusPolicy(Qt::StrongFocus);
        allWidget.append(_listTable);
        allWidget.append(closeBtn);
    }
    else
    {
        _listTable->setFocusPolicy(Qt::NoFocus);
        allWidget.append(_scorllArea);
        allWidget.append(_return);
        allWidget.append(closeBtn);
    }

    int count = allWidget.count();
    for (int i = 0; i < count - 1; ++i)
    {
        setTabOrder(allWidget.at(i), allWidget.at(i + 1));
    }

    allWidget.clear();
}

/**************************************************************************************************
 * 关闭按钮强制聚焦
 *************************************************************************************************/
void MenuGroup::closeStrongFoucs()
{
    _closeBtnSetFoucs();
    _closeStrongFocus = true;
}

void MenuGroup::listTableOrder(bool flag)
{
    if (flag)
    {
        _listTable->setCurrentRow(_listTable->count() - 1);
        _listTable->setFocus();
    }
    else
    {
        _listTable->setCurrentRow(0);
        _listTable->setFocus();
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
MenuGroup::MenuGroup(const QString &name) : MenuWidget(name)
{
    _closeStrongFocus = false;

    _menuName = name;
    _menuDesc = name + "Desc";

    _listWidth = windowManager.getPopWindowWidth() / 4;
    _submenuWidth = windowManager.getPopWindowWidth() - _listWidth - 20;
    _submenuHeight = windowManager.getPopWindowHeight() - 30 - 30 - 20;

    // 创建资源。
    QPalette p;
    p.setColor(QPalette::Foreground, Qt::black);
    setAttribute(Qt::WA_NoSystemBackground);  // draw background in paintEvent

    int fontSize = fontManager.getFontSize(1);

    // 警告栏。
    warn = new QLabel("");
    warn->setAlignment(Qt::AlignCenter);
    warn->setFixedHeight(30);
    warn->setFont(fontManager.textFont(fontManager.getFontSize(3)));
//    QPalette warn_p;
//    warn_p.setColor(QPalette::Foreground, Qt::red);
//    warn->setPalette(warn_p);
    warn->setStyleSheet("color:White;background:red");
    warn->setText(trs("NIBPServiceError"));
    warn->setVisible(false);

    // 列表。
    _listTable = new IListWidget();
    _listTable->setViewMode(QListView::ListMode);
    _listTable->setMovement(QListView::Static);
    _listTable->setSpacing(0);
    _listTable->setFrameStyle(0);
    connect(_listTable, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(_changePage(QListWidgetItem *, QListWidgetItem *)));
    connect(_listTable, SIGNAL(realRelease()), this, SLOT(itemClicked()));
    connect(_listTable, SIGNAL(exitList()), this, SLOT(_closeBtnSetFoucs()));
    _listTable->setCurrentRow(0);
    _listTable->setFocus();
    _listTable->setFixedWidth(_listWidth);

    // 子菜单容器。
    _subMenus = new QStackedWidget();
    _scorllArea = new QScrollArea();
    _scorllArea->setFocusPolicy(Qt::NoFocus);
    _scorllArea->setFrameStyle(QFrame::NoFrame);
    _scorllArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scorllArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _scorllArea->setContentsMargins(0, 0, 0, 0);
    _scorllArea->setFixedSize(_submenuWidth, _submenuHeight);

    _return = new IButton("");
    _return->setEnableKeyAction(false);
    _return->setPicture(QImage("/usr/local/nPM/icons/leftArrow.png"));
    _return->setFixedHeight(_titleBarHeight);
    _return->setFont(fontManager.textFont(fontSize, false));
    _return->setBorderEnabled(true);
    connect(_return, SIGNAL(realReleased()), this, SLOT(returnMenuList()));

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0, 0, 12, 5);
    vlayout->setSpacing(0);
    vlayout->addWidget(_scorllArea);
    vlayout->addWidget(_return, 0, Qt::AlignBottom | Qt::AlignRight);

    // 布局。
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 5, 0, 0);
    hLayout->setSpacing(0);
    hLayout->addWidget(_listTable);
    hLayout->addLayout(vlayout);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(_borderWidth, 0, _borderWidth, _borderWidth);
    vLayout->setSpacing(0);
    vLayout->addWidget(warn);
    vLayout->addLayout(hLayout);

    mainLayout->addLayout(vLayout);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
MenuGroup::~MenuGroup()
{
    delete _subMenus;
}
