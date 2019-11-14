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
#include <QKeyEvent>
#include <QTimer>
#include <QStyle>

class PopupMoveEditorPrivate
{
public:
    PopupMoveEditorPrivate(PopupMoveEditor *const q_ptr, const QString &text)
        : q_ptr(q_ptr),
          isLeftPressed(false),
          isRightPressed(false),
          hasBeenPressed(false),
          prvRect(0, 0, 40, 100),
          name(text),
          timer(NULL),
          iconSize(themeManager.getAcceptableControlHeight(),
                   themeManager.getAcceptableControlHeight())
    {}

    // check the mouse in which region
    bool mouseInLeftRegion(const QPoint &pos) const;
    bool mouseInRightRegion(const QPoint &pos) const;
    bool mouseInTextRegion(const QPoint &pos) const;

    PopupMoveEditor *const q_ptr;

    bool isLeftPressed;
    bool isRightPressed;
    bool hasBeenPressed;
    QRect prvRect;
    QString name;
    QFont textFont;

    QTimer *timer;
    QSize iconSize;
};

PopupMoveEditor::PopupMoveEditor(const QString &text)
    : QWidget(NULL, Qt::Popup | Qt::FramelessWindowHint),
      d_ptr(new PopupMoveEditorPrivate(this, text))
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
    QRect r = rect();
    int bw = themeManager.getBorderWidth();
    int br = themeManager.getBorderRadius();
    QPen pen(pal.color(QPalette::Inactive, QPalette::Shadow), bw);
    QColor bgColor = pal.color(QPalette::Inactive, QPalette::Window);

    painter.setPen(pen);
    QRect leftRegion = r;
    leftRegion.setRight(r.width() / 3);
    painter.setClipRect(leftRegion);
    if (d_ptr->isLeftPressed)
    {
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
        painter.setBrush(bgColor);
    }
    else
    {
        painter.setBrush(pal.color(QPalette::Disabled, QPalette::Window));
    }
    painter.drawRoundedRect(leftRegion.adjusted(bw / 2, bw / 2, br + bw, - bw / 2), br, br);

    QRect iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignCenter, d_ptr->iconSize, leftRegion);
    painter.drawPixmap(iconRect, themeManager.getPixmap(ThemeManager::IconLeft, d_ptr->iconSize));

    QRect rightRegion = r;
    rightRegion.setLeft(r.width() * 2 / 3);
    painter.setClipRect(rightRegion);
    if (d_ptr->isRightPressed)
    {
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
        painter.setBrush(bgColor);
    }
    else
    {
        painter.setBrush(pal.color(QPalette::Disabled, QPalette::Window));
    }
    painter.drawRoundedRect(rightRegion.adjusted(-br - bw, bw / 2,  -bw / 2, -bw / 2), br, br);

    iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignCenter, d_ptr->iconSize, rightRegion);
    painter.drawPixmap(iconRect, themeManager.getPixmap(ThemeManager::IconRight, d_ptr->iconSize));

    painter.setBrush(pal.color(QPalette::Active, QPalette::Window));

    QRect textRegion = r;
    textRegion.setLeft(r.width() / 3 + 1);
    textRegion.setRight(r.width() * 2 / 3 - 1);
    painter.setClipRect(textRegion);
    painter.drawRect(textRegion.adjusted(- bw, bw / 2, bw, -bw / 2));

    painter.setPen(pal.color(QPalette::Inactive, QPalette::WindowText));
    painter.drawText(textRegion, Qt::AlignCenter, d_ptr->name);
}

QSize PopupMoveEditor::sizeHint() const
{
    int width = themeManager.getBorderWidth() * 2 + d_ptr->prvRect.width() * 3 / 2;
    int height = themeManager.getBorderWidth() * 2 + d_ptr->prvRect.height() * 3 / 2;
    return QSize(width, height);
}

void PopupMoveEditor::keyPressEvent(QKeyEvent *ev)
{
    d_ptr->hasBeenPressed = true;
    switch (ev->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
        d_ptr->isLeftPressed = true;
        update();
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
        d_ptr->isRightPressed = true;
        update();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        break;
    default:
        QWidget::keyReleaseEvent(ev);
        break;
    }
}

void PopupMoveEditor::keyReleaseEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
        d_ptr->isLeftPressed = false;
        emit leftMove();
        update();
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
        d_ptr->isRightPressed = false;
        emit rightMove();
        update();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        this->close();
        break;
    default:
        QWidget::keyReleaseEvent(ev);
        break;
    }
}

void PopupMoveEditor::mousePressEvent(QMouseEvent *ev)
{
    d_ptr->hasBeenPressed = true;
    d_ptr->isLeftPressed = d_ptr->mouseInLeftRegion(ev->pos());
    d_ptr->isRightPressed = d_ptr->mouseInRightRegion(ev->pos());

    update();
    ev->accept();

    if (!d_ptr->isLeftPressed && !d_ptr->isRightPressed)
    {
        return;
    }
    if (d_ptr->timer)
    {
        return;
    }
    d_ptr->timer = new QTimer(this);
    d_ptr->timer->start(400);
    connect(d_ptr->timer, SIGNAL(timeout()), this, SLOT(timeOutExec()));
}

void PopupMoveEditor::mouseReleaseEvent(QMouseEvent *ev)
{
    QWidget::mousePressEvent(ev);
    // 触屏点击播放按键音
    themeManager.playClickSound();
    if (d_ptr->timer)
    {
        d_ptr->timer->stop();
        delete d_ptr->timer;
        d_ptr->timer = NULL;
    }

    if (!d_ptr->hasBeenPressed)
    {
        // don't process this event if have gone through press event
        return;
    }

    if (d_ptr->isLeftPressed && d_ptr->mouseInLeftRegion(ev->pos()))
    {
        emit leftMove();
    }
    else if (d_ptr->isRightPressed && d_ptr->mouseInRightRegion(ev->pos()))
    {
        emit rightMove();
    }
    else if (d_ptr->isLeftPressed || d_ptr->isRightPressed)
    {
        // press and move out of the region, do nothing
    }
    else
    {
        // click somewhere else
        close();
    }

    d_ptr->isRightPressed = false;
    d_ptr->isLeftPressed = false;
    d_ptr->hasBeenPressed = false;
    update();
    ev->accept();
}

void PopupMoveEditor::showEvent(QShowEvent *ev)
{
    move(d_ptr->prvRect.topLeft());
    QWidget::showEvent(ev);
}

void PopupMoveEditor::timeOutExec()
{
    if (!d_ptr->hasBeenPressed)
    {
        d_ptr->timer->stop();
        delete d_ptr->timer;
        d_ptr->timer = NULL;
        return;
    }
    if (!d_ptr->isLeftPressed && !d_ptr->isRightPressed)
    {
        d_ptr->timer->stop();
        delete d_ptr->timer;
        d_ptr->timer = NULL;
        return;
    }

    if (d_ptr->isLeftPressed)
    {
        emit leftMove();
    }
    else if (d_ptr->isRightPressed)
    {
        emit rightMove();
    }

    update();

    d_ptr->timer->start(40);
}

bool PopupMoveEditorPrivate::mouseInLeftRegion(const QPoint &pos) const
{
    QRect leftRegion = q_ptr->rect();
    leftRegion.setRight(q_ptr->rect().width() / 3);

    return leftRegion.contains(pos);
}

bool PopupMoveEditorPrivate::mouseInRightRegion(const QPoint &pos) const
{
    QRect rightRegion = q_ptr->rect();
    rightRegion.setLeft(q_ptr->rect().width() * 2 / 3);

    return rightRegion.contains(pos);
}

bool PopupMoveEditorPrivate::mouseInTextRegion(const QPoint &pos) const
{
    QRect textRegion = q_ptr->rect();
    textRegion.setLeft(q_ptr->rect().width() / 3 + 1);
    textRegion.setRight(q_ptr->rect().width() * 2 / 3 - 1);

    return textRegion.contains(pos);
}
