/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/30
 **/

#include "DropList.h"
#include "PopupList.h"
#include "ThemeManager.h"
#include <QPointer>
#include <QKeyEvent>
#include <QPainter>
#include "SoundManagerInterface.h"

#define DEFAULT_HEIGHT (themeManger.getAcceptableControlHeight())

class DropListPrivate
{
public:
    DropListPrivate()
        : isPopup(false), popupList(NULL),
          curIndex(0)
    {}

    bool isPopup;
    QPointer<PopupList> popupList;
    QStringList strList;
    int curIndex;
};

DropList::DropList(const QString &text, QWidget *parent)
    : QAbstractButton(parent),
      d_ptr(new DropListPrivate())
{
    setText(text);
    QPalette pal = palette();
    themeManger.setupPalette(ThemeManager::ControlComboBox, &pal);
    setPalette(pal);

    connect(this, SIGNAL(released()), this, SLOT(showPopup()));
}

DropList::~DropList()
{
}

void DropList::addItem(const QString &text)
{
    d_ptr->strList.append(text);
}

void DropList::addItems(const QStringList &textList)
{
    d_ptr->strList = textList;
}

void DropList::showPopup()
{
    if (d_ptr->strList.count() <= 0)
    {
        return;
    }

    if (!d_ptr->popupList)
    {
        d_ptr->popupList = new PopupList(this);

        for (int i = 0; i < d_ptr->strList.count(); i++)
        {
            d_ptr->popupList->addItemText(d_ptr->strList.at(i));
        }
        d_ptr->popupList->setCurrentIndex(d_ptr->curIndex);
        connect(d_ptr->popupList.data(), SIGNAL(selectItemChanged(int)), this, SLOT(setCurrentIndex(int)));
    }

    d_ptr->popupList->show();
}

QSize DropList::sizeHint() const
{
    QSize s = QAbstractButton::sizeHint();
    s.setHeight(DEFAULT_HEIGHT);
    return s;
}

void DropList::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_Enter:
    case Qt::Key_Return:
        break;
    default:
        QAbstractButton::keyPressEvent(ev);
        break;
    }
}

void DropList::keyReleaseEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Left:
        focusPreviousChild();
        break;
    case Qt::Key_Right:
        focusNextChild();
        break;
    case Qt::Key_Return:
        showPopup();
        break;
    case Qt::Key_F4:
        break;
    default:
        QAbstractButton::keyPressEvent(ev);
        break;
    }
}

void DropList::hideEvent(QHideEvent *ev)
{
    Q_UNUSED(ev)
    if (d_ptr->popupList)
    {
        d_ptr->popupList->close();
    }
}

void DropList::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QPalette &pal = palette();

    QColor borderColor;
    QColor bgColor;
    QColor textColor;
    if (hasFocus())
    {
        borderColor = pal.color(QPalette::Active, QPalette::Shadow);
        textColor = pal.color(QPalette::Inactive, QPalette::WindowText);
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
    }
    else if (d_ptr->popupList)
    {
        borderColor = pal.color(QPalette::Active, QPalette::Shadow);
        textColor = pal.color(QPalette::Active, QPalette::WindowText);
        bgColor = pal.color(QPalette::Active, QPalette::Window);
    }
    else if (!isEnabled())
    {
        borderColor = pal.color(QPalette::Disabled, QPalette::Shadow);
        textColor = pal.color(QPalette::Disabled, QPalette::WindowText);
        bgColor = pal.color(QPalette::Disabled, QPalette::Window);
    }
    else
    {
        borderColor = pal.color(QPalette::Inactive, QPalette::Shadow);
        textColor = pal.color(QPalette::Inactive, QPalette::WindowText);
        bgColor = pal.color(QPalette::Inactive, QPalette::Window);
    }

    QRect r = contentsRect();
    int borderWidth = themeManger.getBorderWidth();
    int radius = themeManger.getBorderRadius();
    r.adjust(borderWidth / 2, borderWidth / 2, -borderWidth / 2, -borderWidth / 2);
    QPen pen(borderColor, borderWidth);
    painter.setPen(pen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(r, radius, radius);

    painter.setPen(textColor);
    painter.drawText(contentsRect(), Qt::AlignCenter, text());
}

void DropList::mousePressEvent(QMouseEvent *e)
{
    QAbstractButton::mousePressEvent(e);
    // 触屏点击播放按键音
    SoundManagerInterface *sound = SoundManagerInterface::getSoundManager();
    if (sound)
    {
        sound->keyPressTone();
    }
}

void DropList::setCurrentIndex(int index)
{
    if (index >= d_ptr->strList.count() || d_ptr->curIndex == index)
    {
        return;
    }

    d_ptr->curIndex = index;
    emit currentIndexChanged(index);
}
