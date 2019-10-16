/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/


#include "ComboBox.h"
#include "PopupList.h"
#include <QPointer>
#include <QKeyEvent>
#include <QPainter>
#include "ThemeManager.h"
#include "SoundManager.h"

#define DEFAULT_HEIGHT (themeManger.getAcceptableControlHeight())

class ComboBoxPrivate
{
public:
    ComboBoxPrivate()
        : isPopup(false),
          popupList(NULL)
    {}

    bool isPopup;
    QPointer<PopupList> popupList;
};

ComboBox::ComboBox(QWidget *parent)
    : QComboBox(parent), d_ptr(new ComboBoxPrivate)
{
    QPalette pal = palette();
    themeManger.setupPalette(ThemeManager::ControlComboBox, pal);
    setPalette(pal);
}

ComboBox::~ComboBox()
{
}

void ComboBox::showPopup()
{
    if (count() <= 0)
    {
        return;
    }

    if (!d_ptr->popupList)
    {
        d_ptr->popupList = new PopupList(this);

        for (int i = 0; i < count(); i++)
        {
            d_ptr->popupList->addItemText(itemText(i));
        }
        d_ptr->popupList->setCurrentIndex(currentIndex());
        connect(d_ptr->popupList.data(), SIGNAL(destroyed(QObject *)), this, SLOT(onPopupDestroyed()));
        connect(d_ptr->popupList.data(), SIGNAL(selectItemChanged(int)), this, SLOT(setCurrentIndex(int)));
        connect(d_ptr->popupList.data(), SIGNAL(itemFocusChanged(int)), this, SIGNAL(itemFocusChanged(int)));
        connect(d_ptr->popupList.data(), SIGNAL(itemFoucsIndexChanged(int)), this, SIGNAL(itemFoucsIndexChanged(int)));
    }

    d_ptr->popupList->show();
}

QSize ComboBox::sizeHint() const
{
    QSize s = QComboBox::sizeHint();
    s.setHeight(DEFAULT_HEIGHT);
    return s;
}

void ComboBox::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Left:
        break;
    case Qt::Key_Right:
        break;
    case Qt::Key_F4:
        // will show popup when focus, ignore this hey
        break;
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Return:
        break;
    default:
        QComboBox::keyPressEvent(ev);
        break;
    }
}

void ComboBox::keyReleaseEvent(QKeyEvent *ev)
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
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_F4:
        break;
    default:
        QComboBox::keyPressEvent(ev);
        break;
    }
}

void ComboBox::hideEvent(QHideEvent *ev)
{
    Q_UNUSED(ev)
    if (d_ptr->popupList)
    {
        d_ptr->popupList->close();
    }
}

void ComboBox::paintEvent(QPaintEvent *ev)
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
    painter.drawText(contentsRect(), Qt::AlignCenter, currentText());
}

void ComboBox::mousePressEvent(QMouseEvent *ev)
{
    QComboBox::mousePressEvent(ev);
    // 触屏点击播放按键音
    soundManager.keyPressTone();
}

void ComboBox::onPopupDestroyed()
{
    hidePopup();
    if (count())
    {
        emit activated(currentIndex());
        emit activated(currentText());
    }
}
