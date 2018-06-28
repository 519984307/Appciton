#include "MenuSidebar.h"
#include <QList>
#include "MenuSidebarItem.h"
#include <QBoxLayout>
#include <QResizeEvent>
#include <QSpacerItem>
#include <qscrollareakineticscroller.h>
#include "FloatScrollBar.h"
#include <QDebug>

#define PREFER_ITEM_HEIGHT 40
#define DEFAULT_ITEM_NUM 8

class MenuSidebarPrivate
{
public:
    MenuSidebarPrivate(MenuSidebar * const q_ptr)
        : q_ptr(q_ptr),
          viewPort(NULL),
          scroller(NULL),
          itemLayout(NULL),
          curSelectIndex(-1),
          currentPage(0),
          itemPerPage(DEFAULT_ITEM_NUM)
    {}

    void updateItems(int itemNum);
    void reloadContent();
    MenuSidebar * const q_ptr;
    QWidget *viewPort;
    QScrollAreaKineticScroller *scroller;
    FloatScrollBar *scrollBar;
    QStringList itemTextList;
    QList<MenuSidebarItem*> itemList;
    QVBoxLayout *itemLayout;
    int curSelectIndex;
    int currentPage;
    int itemPerPage;
};

void MenuSidebarPrivate::updateItems(int itemNum)
{
    if(itemList.count() < itemNum)
    {
        while(itemList.count() < itemNum)
        {
            MenuSidebarItem *item = new MenuSidebarItem();
            itemLayout->insertWidget(itemList.count(), item);
            itemList.append(item);
            QObject::connect(item, SIGNAL(clicked(bool)), q_ptr, SLOT(onItemClicked()));
        }
    }
    else
    {
        while(itemList.count() > itemNum)
        {
            MenuSidebarItem *item = itemList.takeLast();
            itemLayout->removeWidget(item);
            item->setVisible(false);
            item->deleteLater();
        }
    }

    itemPerPage = itemNum;
}

void MenuSidebarPrivate::reloadContent()
{
    for(int i=0; i< itemList.count(); i++)
    {
        int index = itemPerPage * currentPage + i;
        if(i >= itemTextList.count())
        {
            itemList.at(i)->setVisible(false);
        }
        else
        {
            itemList.at(i)->setText(itemTextList.at(index));
            itemList.at(i)->setVisible(true);
            itemList.at(i)->blockSignals(true);
            if(index == curSelectIndex)
            {
                itemList.at(i)->setChecked(true);
            }
            else
            {
                itemList.at(i)->setChecked(false);
            }
            itemList.at(i)->blockSignals(false);
        }
    }
}

MenuSidebar::MenuSidebar(QWidget *parent)
    :QScrollArea(parent),
      d_ptr(new MenuSidebarPrivate(this))
{
    setAutoFillBackground(true);

    d_ptr->viewPort = new QWidget();

    d_ptr->itemLayout = new QVBoxLayout(d_ptr->viewPort);
    d_ptr->itemLayout->setSpacing(0);
    d_ptr->itemLayout->setContentsMargins(0,0,0,0);
    d_ptr->itemLayout->addStretch(1);

    d_ptr->scroller = new QScrollAreaKineticScroller();
    d_ptr->scroller->setWidget(this);
    d_ptr->scroller->setScrollMetric(QKineticScroller::OvershootMaximumDistance, qVariantFromValue(QPointF(0, 0.02)));
    d_ptr->scroller->setScrollMetric(QKineticScroller::OvershootSpringConstant, qVariantFromValue(80));

    d_ptr->scrollBar = new FloatScrollBar();
    d_ptr->scrollBar->setWidget(this);

    QPalette pal = d_ptr->viewPort->palette();
    QColor bgColor = pal.color(QPalette::Button);
    bgColor = bgColor.darker(120);
    pal.setColor(QPalette::Window, bgColor);
    pal.setColor(QPalette::Button, bgColor);
    d_ptr->viewPort->setPalette(pal);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);
    setFrameStyle(QFrame::NoFrame);
}

MenuSidebar::~MenuSidebar()
{

}

void MenuSidebar::addItem(const QString &text)
{
    if(d_ptr->itemTextList.contains(text))
    {
        return;
    }

    d_ptr->itemTextList.append(text);

    MenuSidebarItem *item = new MenuSidebarItem();
    item->setText(text);
    item->setFixedHeight(PREFER_ITEM_HEIGHT);
    d_ptr->itemLayout->insertWidget(d_ptr->itemList.count(), item);
    d_ptr->itemList.append(item);
    QObject::connect(item, SIGNAL(clicked(bool)), this, SLOT(onItemClicked()));
}

QSize MenuSidebar::sizeHint() const
{
    return QSize(180, PREFER_ITEM_HEIGHT * d_ptr->itemPerPage);
}

void MenuSidebar::resizeEvent(QResizeEvent *ev)
{
    QWidget::resizeEvent(ev);
}

void MenuSidebar::showEvent(QShowEvent *ev)
{
    setWidget(d_ptr->viewPort);
    QWidget::showEvent(ev);
}

void MenuSidebar::scrollContentsBy(int dx, int dy)
{
    QScrollArea::scrollContentsBy(dx, dy);
    d_ptr->scrollBar->move(d_ptr->scrollBar->x(), d_ptr->scrollBar->y() + dy);
    d_ptr->scrollBar->setVisible(true);
}

void MenuSidebar::onItemClicked()
{
    MenuSidebarItem *item = qobject_cast<MenuSidebarItem*>(sender());
    if(item)
    {
        if(item->isChecked())
        {
            //current item, do nothing
            return;
        }

        if(d_ptr->curSelectIndex >= 0)
        {
            //uncheck the previous item
            MenuSidebarItem *lastCheckItem = d_ptr->itemList.at(d_ptr->curSelectIndex);
            lastCheckItem->setChecked(false);
        }

        d_ptr->curSelectIndex = d_ptr->itemList.indexOf(item);
        item->setChecked(true);

        emit selectItemChanged(item->text());
    }
}

