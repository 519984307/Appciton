/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/5
 **/

#include "MoveButton.h"
#include <QPainter>
#include "ThemeManager.h"
#include <QIcon>
#include <QKeyEvent>
#include "PopupMoveEditor.h"

class MoveButtonPrivate
{
public:
    explicit MoveButtonPrivate(QString text)
        : name(text)
    {}

    QString name;
};

MoveButton::MoveButton(const QString &text)
    : Button(text), d_ptr(new MoveButtonPrivate(text))
{
}

MoveButton::~MoveButton()
{
}

void MoveButton::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QPalette &pal = palette();
    QRect r = rect();
    int bw = themeManger.getBorderWidth();
    int br = themeManger.getBorderRadius();
    QPen pen(pal.color(QPalette::Inactive, QPalette::Shadow), bw);
    QColor bgColor = pal.color(QPalette::Inactive, QPalette::Window);

    painter.setPen(pen);
    QRect leftRegion = r;
    leftRegion.setRight(r.width() / 4);
    painter.setClipRect(leftRegion);
    if (hasFocus())
    {
        painter.setBrush(bgColor.darker());
    }
    else
    {
        painter.setBrush(pal.color(QPalette::Disabled, QPalette::Window));
    }
    painter.drawRoundedRect(leftRegion.adjusted(bw / 2, bw / 2, br + bw, - bw / 2), br, br);

    const QIcon &leftIcon = themeManger.getIcon(ThemeManager::IconLeft);
    leftIcon.paint(&painter, leftRegion);

    QRect rightRegion = r;
    rightRegion.setLeft(r.width() * 3 / 4);
    painter.setClipRect(rightRegion);
    if (hasFocus())
    {
        painter.setBrush(bgColor.darker());
    }
    else
    {
        painter.setBrush(pal.color(QPalette::Disabled, QPalette::Window));
    }
    painter.drawRoundedRect(rightRegion.adjusted(-br - bw, bw / 2,  -bw / 2, -bw / 2), br, br);

    const QIcon &rightIcon = themeManger.getIcon(ThemeManager::IconRight);
    rightIcon.paint(&painter, rightRegion);

    painter.setBrush(pal.color(QPalette::Active, QPalette::Window));
    QRect textRegion = r;
    textRegion.setLeft(r.width() / 4 + 1);
    textRegion.setRight(r.width() * 3 / 4 - 1);
    painter.setClipRect(textRegion);
    painter.drawRect(textRegion.adjusted(- bw, bw / 2, bw, -bw / 2));

    painter.setPen(pal.color(QPalette::Inactive, QPalette::WindowText));
    painter.drawText(textRegion, Qt::AlignCenter, d_ptr->name);
}

void MoveButton::keyPressEvent(QKeyEvent *ev)
{
    Button::keyPressEvent(ev);
}

void MoveButton::keyReleaseEvent(QKeyEvent *ev)
{
    switch (ev->key())
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
    {
        PopupMoveEditor *editor = new PopupMoveEditor();
        editor->setEditorGeometry(rect());
        editor->setPalette(palette());
        editor->show();
        break;
    }
    default:
        Button::keyReleaseEvent(ev);
        break;
    }
}
