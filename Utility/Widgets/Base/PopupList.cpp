/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/

#include "PopupList.h"
#include "PopupListItem.h"
#include "ScrollArea.h"
#include <QList>
#include <QBoxLayout>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>
#include "ThemeManager.h"
#include <QResizeEvent>
#include "FontManager.h"
#include <QKeyEvent>
#include <QFocusEvent>
#include "SoundManagerInterface.h"

#define DEFAULT_WIDTH 150
#define DEFAULT_HEIGHT  100
#define MAXIMUM_DISPLAY_ITEM 5
#define DEFAULT_GAP 10

class PopupListPrivate
{
public:
    explicit PopupListPrivate(bool concat)
        : widget(NULL),
          itemLayout(NULL),
          scrollArea(NULL),
          curSelectIndex(-1),
          concatToParent(concat),
          popAbove(false),
          maximumDisplayItemNum(MAXIMUM_DISPLAY_ITEM)
    {}

    /**
     * @brief properItemsHeight calculate the proper items height
     * @return
     */
    int properItemsHeight() const;

    QList<PopupListItem *> items;
    QWidget *widget;
    QVBoxLayout *itemLayout;
    ScrollArea *scrollArea;
    int curSelectIndex;
    bool concatToParent;
    bool popAbove;
    QRect globalRect;  // A global rect this popup should pop around
    int maximumDisplayItemNum;
};

int PopupListPrivate::properItemsHeight() const
{
    int height = DEFAULT_HEIGHT;
    int itemCount = items.count();
    if (itemCount > maximumDisplayItemNum)
    {
        itemCount = maximumDisplayItemNum;
    }

    if (itemCount > 0)
    {
        // we consider all the items has the same height
        height = items.at(0)->sizeHint().height() * itemCount;
    }

    return height;
}

PopupList::PopupList(QWidget *parent, bool concatToParent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint),
      d_ptr(new PopupListPrivate(concatToParent && parent != NULL))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    if (parent)
    {
        setFont(fontManager.textFont(parent->font().pixelSize()));
    }
    else
    {
        setFont(fontManager.textFont(themeManager.defaultFontPixSize()));
    }

    QPalette p =  palette();
    themeManager.setupPalette(ThemeManager::ControlPopupList, &p);
    setPalette(p);

    d_ptr->widget = new QWidget();
    d_ptr->itemLayout = new QVBoxLayout(d_ptr->widget);
    d_ptr->itemLayout->setSpacing(0);
    d_ptr->itemLayout->setContentsMargins(0, 0, 0, 0);
    d_ptr->itemLayout->addStretch(1);

    QBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->scrollArea = new ScrollArea();
    d_ptr->scrollArea->setContentsMargins(0, 0, 0, 0);
    d_ptr->scrollArea->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    d_ptr->scrollArea->setFloatbarPolicy(ScrollArea::FloatBarShowForeverWhenNeeded);
    d_ptr->scrollArea->setOverShot(false);
    layout->addWidget(d_ptr->scrollArea);
    int borderRadius = themeManager.getBorderRadius();
    setContentsMargins(borderRadius, borderRadius, borderRadius, borderRadius);
}

PopupList::~PopupList()
{
}

void PopupList::addItemText(const QString &text)
{
    PopupListItem *item = new PopupListItem(text);
    d_ptr->items.append(item);
    d_ptr->itemLayout->addWidget(item);
    connect(item, SIGNAL(clicked(bool)), this, SLOT(onItemSelected()));
}

void PopupList::additemList(const QStringList &list)
{
    QStringList::ConstIterator iter;
    for (iter = list.constBegin(); iter != list.constEnd(); ++iter)
    {
        addItemText(*iter);
    }
}

void PopupList::setCurrentIndex(int index)
{
    if (d_ptr->curSelectIndex == index || index >= d_ptr->items.count())
    {
        return;
    }

    // uncheck selected item
    if (d_ptr->curSelectIndex >= 0)
    {
        d_ptr->items.at(d_ptr->curSelectIndex)->setChecked(false);
    }

    // check selected item
    if (index >= 0)
    {
        d_ptr->items.at(index)->setChecked(true);
    }
    d_ptr->curSelectIndex = index;
}

void PopupList::setCurrentText(const QString &text)
{
    int index = -1;
    for (int i = 0; i < d_ptr->items.size(); ++i)
    {
        if (d_ptr->items.at(i)->text() == text)
        {
            index = i;
            break;
        }
    }

    setCurrentIndex(index);
}

int PopupList::getCurrentIndex() const
{
    return d_ptr->curSelectIndex;
}

void PopupList::setPopAroundRect(const QRect &rect)
{
    d_ptr->globalRect = rect;
}

void PopupList::setMaximumDisplayItem(int num)
{
    d_ptr->maximumDisplayItemNum = num;
}

int PopupList::count() const
{
    return d_ptr->items.count();
}

QSize PopupList::sizeHint() const
{
    QWidget *parent = parentWidget();
    int width = DEFAULT_WIDTH;
    if (parent)
    {
        width = parent->width();
    }

    int height = d_ptr->properItemsHeight();

    QMargins margins = contentsMargins();

    height += margins.top() + margins.bottom();

    return QSize(width, height);
}

void PopupList::showEvent(QShowEvent *e)
{
    d_ptr->scrollArea->setWidget(d_ptr->widget);
    d_ptr->scrollArea->updateFloatBar();

    QWidget::showEvent(e);
    QWidget *parent = parentWidget();

    // find a proper position
    if (parent)
    {
        // set fix width;
        this->setFixedWidth(parent->width());

        QDesktopWidget *desktop = QApplication::desktop();
        QPoint pos =  parent->mapToGlobal(parent->rect().bottomLeft());
        int borderRadius = themeManager.getBorderRadius();

        int screenHeight = desktop->height();

        int popHeight = d_ptr->properItemsHeight() + borderRadius * 2
                + (d_ptr->concatToParent ? 0 : DEFAULT_GAP);
        this->setFixedHeight(popHeight);

        // need gap between when not cancat to the parent widget
        int lowestHeight = pos.y() + popHeight;

        if (lowestHeight < screenHeight)
        {
            // pop under the parent widget
            if (!d_ptr->concatToParent)
            {
                pos.ry() += DEFAULT_GAP;
            }
            else
            {
                pos.ry() -= borderRadius;
            }

            move(pos);
            d_ptr->popAbove = false;
        }
        else
        {
            // pop above the parent widget
            pos = parent->mapToGlobal(parent->rect().topLeft());
            if (d_ptr->concatToParent)
            {
                pos.ry() = pos.ry() - d_ptr->properItemsHeight();
            }
            else
            {
                pos.ry() = pos.ry() - d_ptr->properItemsHeight()
                           - DEFAULT_GAP;
            }

            move(pos);
            d_ptr->popAbove = true;
        }
    }
    else if (!d_ptr->globalRect.isNull())
    {
        QDesktopWidget *desktop = QApplication::desktop();
        QPoint pos =  d_ptr->globalRect.bottomLeft();
        int borderRadius = themeManager.getBorderRadius();

        int screenHeight = desktop->height();
        // need gap between when not cancat to the parent widget
        int lowestHeight = pos.y() + d_ptr->properItemsHeight() + borderRadius * 2;

        if (lowestHeight < screenHeight)
        {
            // pop under the globalRect
            move(pos);
            d_ptr->popAbove = false;
        }
        else
        {
            // pop above the gloablRect
            pos = d_ptr->globalRect.topLeft();
            pos.ry() = pos.ry() - d_ptr->properItemsHeight() - borderRadius * 2;

            move(pos);
            d_ptr->popAbove = true;
        }
    }

    // focus the proper item
    if (d_ptr->items.count())
    {
        if (d_ptr->curSelectIndex >= 0)
        {
            d_ptr->items.at(d_ptr->curSelectIndex)->setFocus();
        }
        else
        {
            d_ptr->items.first()->setFocus();
        }
    }
}

void PopupList::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect r = rect();
    int borderWidth = themeManager.getBorderWidth();
    int borderRadius = themeManager.getBorderRadius();
    int adjustWidth = borderWidth / 2;
    if (d_ptr->concatToParent)
    {
        r.adjust(adjustWidth,
                 d_ptr->popAbove ? adjustWidth : - borderRadius,
                 -adjustWidth,
                 d_ptr->popAbove ? borderRadius : - adjustWidth);
    }
    else
    {
        r.adjust(adjustWidth, adjustWidth, - adjustWidth, - adjustWidth);
    }

    const QPalette &pal = palette();
    QWidget *parent = parentWidget();
    QPen p(pal.brush(parent ? QPalette::Active : QPalette::Inactive, QPalette::Shadow), borderWidth);
    painter.setPen(p);
    painter.setBrush(pal.brush(QPalette::Inactive, QPalette::Window));

    painter.drawRoundedRect(r, borderRadius, borderRadius);
}

void PopupList::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    int borderRadius = themeManager.getBorderRadius();
    d_ptr->scrollArea->setFixedWidth(e->size().width() - borderRadius - borderRadius);
}

void PopupList::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Left:
        break;
    case Qt::Key_Right:
        break;
    case Qt::Key_F4:
        // will show popup when focus, ignore this hey
        break;
    case Qt::Key_Return:
        break;
    default:
        QWidget::keyPressEvent(e);
        break;
    }
}

void PopupList::keyReleaseEvent(QKeyEvent *e)
{
    // should not get here, have not items ??
    switch (e->key())
    {
    case Qt::Key_Left:
        focusPreviousChild();
        break;
    case Qt::Key_Right:
        focusNextChild();
        break;
    case Qt::Key_Return:
        // not items ?
        close();
        break;
    default:
        QWidget::keyPressEvent(e);
        break;
    }
}

bool PopupList::focusNextPrevChild(bool next)
{
    if (d_ptr->items.isEmpty())
    {
        return false;
    }

    PopupListItem *item = qobject_cast<PopupListItem *>(focusWidget());
    int index = d_ptr->items.indexOf(item);
    if (index >= 0)
    {
        index += next ? 1 : -1;
        index += d_ptr->items.count();  // avoid negative value when the item index is 0
        index %= d_ptr->items.count();

        d_ptr->items.at(index)->setFocus();
    }
    else
    {
        index = 0;
        d_ptr->items.at(0)->setFocus();
    }

    int value = d_ptr->items.at(index)->text().toInt();
    emit itemFocusChanged(value);
    emit itemFoucsIndexChanged(index);

    return true;
}

void PopupList::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    // 触屏点击播放按键音
    SoundManagerInterface *sound = SoundManagerInterface::getSoundManager();
    if (sound)
    {
        sound->keyPressTone();
    }
}

void PopupList::onItemSelected()
{
    PopupListItem *item = qobject_cast<PopupListItem *>(sender());
    if (item)
    {
        // get the item index
        int index = d_ptr->items.indexOf(item);
        emit selectItemChanged(index);
        emit selectItemChanged(item->text());
        this->close();
    }
}
