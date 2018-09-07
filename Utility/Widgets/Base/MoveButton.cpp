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
#include "FontManager.h"

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
    QColor bgColor = pal.color(QPalette::Inactive, QPalette::Window);
    QPen pen(pal.color(QPalette::Inactive, QPalette::Shadow), bw);
    if (hasFocus())
    {
        pen.setColor(pal.color(QPalette::Active, QPalette::Shadow));
    }
    else if (!isEnabled())
    {
        pen.setColor(pal.color(QPalette::Disabled, QPalette::Shadow));
    }
    painter.setPen(pen);
    QRect leftRegion = r;
    leftRegion.setRight(r.width() / 4);
    painter.setClipRect(leftRegion);
    if (hasFocus())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
        painter.setBrush(bgColor);
        //        textColor = pal.color(QPalette::Active, QPalette::HighlightedText);
//        icoMode = QIcon::Active;
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
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
        painter.setBrush(bgColor);
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
        QRect vrect = rect();
        QRect rect(this->mapToGlobal(vrect.topLeft()),
                    this->mapToGlobal(vrect.bottomRight()));
        PopupMoveEditor *editor = new PopupMoveEditor(d_ptr->name);
        editor->setEditorGeometry(rect);
        editor->setFont(fontManager.textFont(this->font().pixelSize()));
        editor->setPalette(palette());
        connect(editor, SIGNAL(leftMove()), this, SIGNAL(leftMove()));
        connect(editor, SIGNAL(rightMove()), this, SIGNAL(rightMove()));
        editor->show();
        break;
    }
    default:
        Button::keyReleaseEvent(ev);
        break;
    }
}

void MoveButton::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    QRect vrect = rect();
    QRect rect(this->mapToGlobal(vrect.topLeft()),
                this->mapToGlobal(vrect.bottomRight()));
    PopupMoveEditor *editor = new PopupMoveEditor(d_ptr->name);
    editor->setEditorGeometry(rect);
    editor->setFont(fontManager.textFont(this->font().pixelSize()));
    editor->setPalette(palette());
    connect(editor, SIGNAL(leftMove()), this, SIGNAL(leftMove()));
    connect(editor, SIGNAL(rightMove()), this, SIGNAL(rightMove()));
    editor->show();
}
