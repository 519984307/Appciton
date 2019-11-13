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
#include <QStyle>

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
    setIconSize(QSize(32, 32));
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
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
    }
    else if (!isEnabled())
    {
        pen.setColor(pal.color(QPalette::Disabled, QPalette::Shadow));
    }

    painter.setPen(pen);
    painter.setBrush(bgColor);

    painter.drawRoundedRect(r.adjusted(bw / 2, bw / 2, -bw / 2, -bw/2),
                            br, br);
    QRect leftRegion = r;
    leftRegion.setRight(r.width() / 4);
    QRect iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignCenter, iconSize(), leftRegion);
    painter.drawPixmap(iconRect, themeManger.getPixmap(ThemeManager::IconLeft, iconSize()));

    QRect rightRegion = r;
    rightRegion.setLeft(r.width() * 3 / 4);
    iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignCenter, iconSize(), rightRegion);
    painter.drawPixmap(iconRect, themeManger.getPixmap(ThemeManager::IconRight, iconSize()));

    QRect textRect = r;
    textRect.adjust(r.width() / 4, bw , - r.width() / 4, - bw);
    painter.fillRect(textRect, pal.color(QPalette::Inactive, QPalette::Window));
    painter.setPen(pal.color(QPalette::Inactive, QPalette::WindowText));
    painter.drawText(textRect, Qt::AlignCenter, d_ptr->name);
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
        QFontInfo fontinfo(font());
        editor->setFont(QFont(fontinfo.family(), fontinfo.pixelSize(), fontinfo.weight(), fontinfo.italic()));
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
    QAbstractButton::mousePressEvent(ev);
    QRect vrect = rect();
    QRect rect(this->mapToGlobal(vrect.topLeft()),
                this->mapToGlobal(vrect.bottomRight()));
    PopupMoveEditor *editor = new PopupMoveEditor(d_ptr->name);
    editor->setEditorGeometry(rect);
    QFontInfo fontinfo(font());
    editor->setFont(QFont(fontinfo.family(), fontinfo.pixelSize(), fontinfo.weight(), fontinfo.italic()));
    editor->setPalette(palette());
    connect(editor, SIGNAL(leftMove()), this, SIGNAL(leftMove()));
    connect(editor, SIGNAL(rightMove()), this, SIGNAL(rightMove()));
    editor->show();
}
