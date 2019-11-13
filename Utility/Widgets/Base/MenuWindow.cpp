/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/


#include "MenuWindow.h"
#include <QBoxLayout>
#include <QStackedWidget>
#include "MenuSidebar.h"
#include "MenuSidebarItem.h"
#include "WindowManager.h"
#include "Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QList>
#include <QPainter>
#include "ThemeManager.h"

class MenuWindowPrivate
{
public:
    MenuWindowPrivate()
        : sidebar(NULL),
          stackWidget(NULL),
          retBtn(NULL),
          rightLayout(NULL),
          menuPath(""),
          isBelongToLeftWidget(false)
    {
    }

    MenuSidebar *sidebar;
    QStackedWidget *stackWidget;
    Button *retBtn;
    QBoxLayout *rightLayout;
    QString menuPath;
    bool isBelongToLeftWidget;  // 聚焦点否属于左边菜单
};

MenuWindow::MenuWindow()
    : Dialog(), d_ptr(new MenuWindowPrivate())
{
    setWindowTitle("MenuWindow");
    resize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setWindowLayout(layout);

    d_ptr->sidebar = new MenuSidebar();
    d_ptr->sidebar->setFixedWidth(200);
    layout->addWidget(d_ptr->sidebar);

    d_ptr->rightLayout = new QVBoxLayout();
    d_ptr->rightLayout->setContentsMargins(0, 0, 0, 0);
    d_ptr->rightLayout->setSpacing(0);
    layout->addLayout(d_ptr->rightLayout);

    d_ptr->stackWidget = new QStackedWidget();
    d_ptr->rightLayout->addWidget(d_ptr->stackWidget, 1);

    QFrame *line = new QFrame;
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setLineWidth(0);
    line->setMidLineWidth(1);
    d_ptr->rightLayout->addWidget(line);

    connect(d_ptr->sidebar, SIGNAL(selectItemChanged(int)), this, SLOT(onSelectItemChanged(int)));
    connect(d_ptr->sidebar, SIGNAL(visiableItemChanged(int)), this, SLOT(onVisiableItemChanged(int)));
}

MenuWindow::~MenuWindow()
{
}

void MenuWindow::addMenuContent(MenuContent *menu)
{
    d_ptr->sidebar->addItem(menu->name());
    menu->doLayout();

    ScrollArea *scrollArea = new ScrollArea;
    scrollArea->setWidget(menu);
    scrollArea->setFloatbarPolicy(ScrollArea::FloatBarShowForeverWhenNeeded);
    d_ptr->stackWidget->addWidget(scrollArea);

    if (d_ptr->stackWidget->count() == 1)
    {
        // set the first Item
        d_ptr->sidebar->setChecked(menu->name());
    }
}

void MenuWindow::popup(const QString &menuName, const QVariant &param,
                       const WindowManager::ShowBehaviorFlags &showFlags)
{
    d_ptr->sidebar->setChecked(menuName);

    // find current menucontent and set the show param
    ScrollArea *area = qobject_cast<ScrollArea *>(d_ptr->stackWidget->currentWidget());
    if (area)
    {
        MenuContent *content = qobject_cast<MenuContent *>(area->widget());
        if (content)
        {
            content->setShowParam(param);
        }
    }

    windowManager.showWindow(this, showFlags);
}

void MenuWindow::setWindowTitlePrefix(const QString &prefix)
{
    QString strPath = prefix;
    QStringList pathList = strPath.split('-');

    d_ptr->menuPath.clear();
    if (pathList.count() < 2)
    {
        return;
    }
    d_ptr->menuPath  = trs(pathList.at(0));
    d_ptr->menuPath += "-";
    d_ptr->menuPath += trs(pathList.at(1));

    update();
}

void MenuWindow::focusFirstMenuItem()
{
    MenuSidebar *bar = d_ptr->sidebar;
    MenuSidebarItem *barItem = bar->itemAt(0);
    if (bar && barItem)
    {
        bar->setChecked(barItem->text());
        barItem->setFocus();
    }
}

void MenuWindow::ensureWidgetVisiable(QWidget *w)
{
    ScrollArea *area = qobject_cast<ScrollArea *>(d_ptr->stackWidget->currentWidget());
    if (area && area->isAncestorOf(w))
    {
        area->ensureWidgetVisible(w);
    }
}

bool MenuWindow::focusNextPrevChild(bool next)
{
    /*
     *  when the focus switch back to the menusidebar, we need to make menusidebar
     *  focus on the checked item.
     */

    QWidget *cur = focusWidget();

    bool isBelongToSideBar = d_ptr->sidebar->isAncestorOf(cur);
    Button *closeBtn = getCloseBtn();

    // 判断当前聚焦菜单是属于左侧还是右侧
    if (isBelongToSideBar)
    {
        d_ptr->isBelongToLeftWidget = true;
    }
    else if (closeBtn != cur)
    {
        d_ptr->isBelongToLeftWidget = false;
    }

    // 判断当前聚焦是否属于左侧聚焦区域
    if (d_ptr->isBelongToLeftWidget)
    {
        int index = d_ptr->sidebar->indexOf(qobject_cast<MenuSidebarItem *>(cur));
        int count = d_ptr->sidebar->itemCount();

        // 当聚焦在左侧首个聚焦点，且飞梭向前移动聚焦点时，强制进入关闭按钮聚焦区域
        if (index == 0 && !next)
        {
            closeBtn->setFocus();
            return true;
        }
        // 当聚焦在左侧末尾那个聚焦点，且飞梭向后移动聚焦点时，强制进入关闭按钮聚焦区域
        else if (index == count - 1 && next)
        {
            closeBtn->setFocus();
            return true;
        }

        if (cur == closeBtn)
        {
            // the focus is at the close button currently
            int newIndex = 0;
            if (!next)
            {
                newIndex =  count - 1;
            }
            MenuSidebarItem *item = d_ptr->sidebar->itemAt(newIndex);
            item->setFocus(next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
            return true;
        }
        else if (index >= 0)
        {
            int newIndex = 0;
            if (next)
            {
                newIndex = (index + 1) % count;
            }
            else
            {
                newIndex = (index + count -  1) % count;
            }
            MenuSidebarItem *item = d_ptr->sidebar->itemAt(newIndex);
            item->setFocus(next ? Qt::TabFocusReason : Qt::BacktabFocusReason);
        }
        return true;
    }

    ScrollArea *area = qobject_cast<ScrollArea *>(d_ptr->stackWidget->currentWidget());
    QWidget *w = NULL;
    if (next)
    {
        w = cur->nextInFocusChain();
        // find previous focus widget in the focus chain
        while (w && cur != w)
        {
            if (w->isEnabled()
                    && (w->focusPolicy() & Qt::TabFocus)
                    && w->isVisibleTo(this)
                    && !d_ptr->sidebar->isAncestorOf(w))
            {
                break;
            }
            w = w->nextInFocusChain();
        }

        if (w)
        {
            w->setFocus(Qt::TabFocusReason);
            if (area && area->isAncestorOf(w))
            {
                area->ensureWidgetVisible(w);
            }
            return true;
        }
    }
    else
    {
        w = cur->previousInFocusChain();

        // find previous focus widget in the focus chain
        while (w && cur != w)
        {
            if (w->isEnabled()
                    && (w->focusPolicy() & Qt::TabFocus)
                    && w->isVisibleTo(this)
                    && !d_ptr->sidebar->isAncestorOf(w))
            {
                break;
            }
            w = w->previousInFocusChain();
        }

        if (w)
        {
            w->setFocus(Qt::BacktabFocusReason);
            if (area && area->isAncestorOf(w))
            {
                area->ensureWidgetVisible(w);
            }

            return true;
        }
    }

    return Dialog::focusNextPrevChild(next);
}

void MenuWindow::showEvent(QShowEvent *ev)
{
    /**
     * Here is a workround, we want the return button as the last widget to has focus in the window,
     * But the Qt's tab order is based on the construction order, so we make the return button create
     * at the last, after all the menu content is added
     */
    if (d_ptr->retBtn == NULL)
    {
        QHBoxLayout *bottomLayout = new QHBoxLayout;
        bottomLayout->setContentsMargins(4, 4, 8, 4);
        bottomLayout->addStretch(1);

        QSize iconSize(32, 32);
        QIcon icon = themeManger.getIcon(ThemeManager::IconReturn, iconSize);
        d_ptr->retBtn = new Button(QString(), icon);
        d_ptr->retBtn->setButtonStyle(Button::ButtonIconOnly);
        d_ptr->retBtn->setIconSize(iconSize);
        d_ptr->retBtn->setMinimumWidth(100);
        bottomLayout->addWidget(d_ptr->retBtn);
        d_ptr->rightLayout->addLayout(bottomLayout);
        connect(d_ptr->retBtn, SIGNAL(clicked()), this, SLOT(onReturnBtnClick()));
    }

    Dialog::showEvent(ev);

    // 强制保证聚焦在子菜单内部
    QWidget *cur = focusWidget();
    if (getCloseBtn() == cur)
    {
        focusNextPrevChild(true);
    }
}

void MenuWindow::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);
    QRectF r = contentsRect();
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);  // 抗锯齿
    QPainterPath painterPath;
    painterPath.addRoundedRect(r, themeManger.getBorderRadius(), themeManger.getBorderRadius());
    // 下部直角
    int bottomHeight = r.height() - themeManger.getBorderRadius();
    QRectF bottomRect = r.adjusted(0, bottomHeight, 0, 0);
    painterPath.addRect(bottomRect);
    painterPath.setFillRule(Qt::WindingFill);

    p.fillPath(painterPath, palette().background());
}

void MenuWindow::onSelectItemChanged(int index)
{
    Q_ASSERT(index < d_ptr->stackWidget->count());

    d_ptr->stackWidget->setCurrentIndex(index);

    ScrollArea *area = qobject_cast<ScrollArea *>(d_ptr->stackWidget->currentWidget());
    if (area)
    {
        d_ptr->stackWidget->setFocusProxy(area);
        MenuContent *content = qobject_cast<MenuContent *>(area->widget());
        if (content)
        {
            if (d_ptr->menuPath.isEmpty())
            {
                setWindowTitle(content->description());
            }
            else
            {
                QString windowTitle = d_ptr->menuPath;
                windowTitle += "-";
                windowTitle += content->description();
                setWindowTitle(windowTitle);
            }
            content->setFocus(Qt::TabFocusReason);
            if (area->isAncestorOf(content->focusWidget()))
            {
                area->ensureWidgetVisible(content->focusWidget());
            }
        }
    }
}

void MenuWindow::onVisiableItemChanged(int index)
{
    Q_ASSERT(index < d_ptr->stackWidget->count());

    d_ptr->stackWidget->setCurrentIndex(index);

    ScrollArea *area = qobject_cast<ScrollArea *>(d_ptr->stackWidget->currentWidget());
    if (area)
    {
        d_ptr->stackWidget->setFocusProxy(area);
        MenuContent *content = qobject_cast<MenuContent *>(area->widget());
        if (content)
        {
            if (d_ptr->menuPath.isEmpty())
            {
                setWindowTitle(content->description());
            }
            else
            {
                QString windowTitle = d_ptr->menuPath;
                windowTitle += "-";
                windowTitle += content->description();
                setWindowTitle(windowTitle);
            }

            // find the first widget and make it visiable
            QList<QWidget*> childlist = content->findChildren<QWidget *>();
            if (!childlist.isEmpty())
            {
                area->ensureWidgetVisible(childlist.first());
            }
        }
    }
}

void MenuWindow::onReturnBtnClick()
{
    // set focus back to the menubar
    QWidget *f = d_ptr->sidebar->getChecked();
    if (f)
    {
        f->setFocus();
    }
}
