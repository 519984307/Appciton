/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/4
 **/

#include "PopupMoveEditor.h"
#include <QPainter>
#include "ThemeManager.h"
#include <QIcon>

class PopupMoveEditorPrivate
{
public:
    PopupMoveEditorPrivate()
        : isLeftPressed(false),
          isRightPressed(false),
          prvRect(0, 0, 100, 40),
          name("Paging")
    {}

    bool isLeftPressed;
    bool isRightPressed;
    QRect prvRect;
    QString name;
};

PopupMoveEditor::PopupMoveEditor()
    : QWidget(NULL, Qt::Popup | Qt::FramelessWindowHint),
      d_ptr(new PopupMoveEditorPrivate())
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    setFocusPolicy(Qt::StrongFocus);
}

PopupMoveEditor::~PopupMoveEditor()
{
}

void PopupMoveEditor::setEditorGeometry(const QRect &r)
{
    if (d_ptr->prvRect == r)
    {
        return;
    }
    d_ptr->prvRect = r;
    updateGeometry();
    update();
}

void PopupMoveEditor::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QPalette &pal = palette();
    QRect r = d_ptr->prvRect.adjusted(width() / 2, height() / 2, 0, 0);
    int bw = themeManger.getBorderWidth();
    int br = themeManger.getBorderRadius();
    QPen pen(pal.color(QPalette::Inactive, QPalette::Shadow), bw);
    QColor bgColor = pal.color(QPalette::Inactive, QPalette::Window);

    painter.setPen(pen);
    QRect leftRegion = r;
    leftRegion.setRight(r.width() / 4);
    painter.setClipRect(leftRegion);
    if (d_ptr->isLeftPressed)
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
    if (d_ptr->isRightPressed)
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

QSize PopupMoveEditor::sizeHint() const
{
}

void PopupMoveEditor::keyPressEvent(QKeyEvent *ev)
{
}

void PopupMoveEditor::keyReleaseEvent(QKeyEvent *ev)
{
}

void PopupMoveEditor::mousePressEvent(QMouseEvent *ev)
{
}

void PopupMoveEditor::mouseReleaseEvent(QMouseEvent *ev)
{
}

void PopupMoveEditor::showEvent(QShowEvent *ev)
{
    QRect r(QPoint(0, 0), d_ptr->prvRect.size());
    QRect rr = this->rect();
    move(r.center() - rr.center() + d_ptr->prvRect.topLeft());
    QWidget::showEvent(ev);
}
