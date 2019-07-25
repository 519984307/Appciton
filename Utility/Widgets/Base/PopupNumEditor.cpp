/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/16
 **/

#include "PopupNumEditor.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QIcon>
#include "ThemeManager.h"
#include "Utility.h"
#include <QDebug>
#include <QTimer>

class PopupNumEditorPrivate
{
public:
    explicit PopupNumEditorPrivate(PopupNumEditor *const q_ptr)
        : q_ptr(q_ptr),
          valueRect(0, 0, 40, 100),
          isUpPressed(false),
          isDownPressed(false),
          hasBeenPressed(false),
          timer(NULL)
    {}

    // check the mouse in which region
    bool mouseInUpRegion(const QPoint &pos) const;
    bool mouseInDownRegion(const QPoint &pos) const;
    bool mouseInValueRegion(const QPoint &pos) const;

    // increase the value
    void increaseValue();
    // decrease the value
    void decreaseValue();

    // get the value
    QString value() const;

    PopupNumEditor *const q_ptr;

    ItemEditInfo editInfo;
    QRect valueRect;
    bool isUpPressed;
    bool isDownPressed;
    bool hasBeenPressed;

    QTimer *timer;
};

bool PopupNumEditorPrivate::mouseInUpRegion(const QPoint &pos) const
{
    QRect upRegion = q_ptr->rect();
    upRegion.setBottom(q_ptr->rect().height() / 3);

    return upRegion.contains(pos);
}

bool PopupNumEditorPrivate::mouseInDownRegion(const QPoint &pos) const
{
    QRect downRegion = q_ptr->rect();
    downRegion.setTop(q_ptr->rect().height() * 2 / 3);
    return downRegion.contains(pos);
}

bool PopupNumEditorPrivate::mouseInValueRegion(const QPoint &pos) const
{
    QRect valueRegion = q_ptr->rect();
    valueRegion.setTop(q_ptr->rect().height() / 3 + 1);
    valueRegion.setBottom(q_ptr->rect().height() * 2 / 3 - 1);
    return valueRegion.contains(pos);
}

void PopupNumEditorPrivate::increaseValue()
{
    if (editInfo.curValue + editInfo.step <= editInfo.highLimit)
    {
        editInfo.curValue += editInfo.step;
    }
}

void PopupNumEditorPrivate::decreaseValue()
{
    if (editInfo.curValue - editInfo.step >= editInfo.lowLimit)
    {
        editInfo.curValue -= editInfo.step;
    }
}

QString PopupNumEditorPrivate::value() const
{
    if (editInfo.type == ItemEditInfo::VALUE)
    {
        return Util::convertToString(editInfo.curValue, editInfo.scale);
    }
    else if (editInfo.type == ItemEditInfo::LIST)
    {
        return editInfo.list.at(editInfo.curValue);
    }
    return QString();
}

PopupNumEditor::PopupNumEditor()
    : QWidget(NULL, Qt::Popup | Qt::FramelessWindowHint),
      d_ptr(new PopupNumEditorPrivate(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    setFocusPolicy(Qt::StrongFocus);
}

PopupNumEditor::~PopupNumEditor()
{
    delete d_ptr;
}

void PopupNumEditor::setEditValueGeometry(const QRect &r)
{
    if (d_ptr->valueRect == r)
    {
        return;
    }
    d_ptr->valueRect = r;
    updateGeometry();
    update();
}

void PopupNumEditor::setEditInfo(const ItemEditInfo &info)
{
    d_ptr->editInfo = info;
    if (info.type == ItemEditInfo::VALUE)
    {
        if (info.curValue < info.lowLimit || info.curValue > info.highLimit)
        {
            // 如果当前值无效，则设置起始值
            d_ptr->editInfo.curValue = info.startValue;
        }
    }
    update();
}

void PopupNumEditor::paintEvent(QPaintEvent *ev)
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
    QRect upRegion = r;
    upRegion.setBottom(r.height() / 3);
    painter.setClipRect(upRegion);
    if (d_ptr->isUpPressed)
    {
        painter.setBrush(bgColor.darker());
    }
    else if (d_ptr->editInfo.curValue >= d_ptr->editInfo.highLimit)
    {
        painter.setBrush(pal.color(QPalette::Disabled, QPalette::Window));
    }
    else
    {
        painter.setBrush(bgColor);
    }
    painter.drawRoundedRect(upRegion.adjusted(bw / 2, bw / 2, - bw / 2, br + bw), br, br);

    int iconWidth = upRegion.height() > upRegion.width() ? upRegion.width() : upRegion.height();
    const QIcon upIcon = themeManger.getIcon(ThemeManager::IconUp, QSize(iconWidth, iconWidth));
    upIcon.paint(&painter, upRegion);

    QRect downRegion = r;
    downRegion.setTop(r.height() * 2 / 3);
    painter.setClipRect(downRegion);
    if (d_ptr->isDownPressed)
    {
        painter.setBrush(bgColor.darker());
    }
    else if (d_ptr->editInfo.curValue <= d_ptr->editInfo.lowLimit)
    {
        painter.setBrush(pal.color(QPalette::Disabled, QPalette::Window));
    }
    else
    {
        painter.setBrush(bgColor);
    }
    painter.drawRoundedRect(downRegion.adjusted(bw / 2, -br - bw, -bw / 2, -bw / 2), br, br);

    const QIcon downIcon = themeManger.getIcon(ThemeManager::IconDown, QSize(iconWidth, iconWidth));
    downIcon.paint(&painter, downRegion);

    painter.setBrush(pal.color(QPalette::Active, QPalette::Window));


    QRect valueRegion = r;
    valueRegion.setTop(r.height() / 3 + 1);
    valueRegion.setBottom(r.height() * 2 / 3 - 1);
    painter.setClipRect(valueRegion);
    painter.setPen(pen);
    painter.drawRect(valueRegion.adjusted(bw / 2, -bw, -bw / 2, bw));

    painter.setPen(pal.color(QPalette::Inactive, QPalette::WindowText));
    painter.drawText(valueRegion, Qt::AlignCenter, d_ptr->value());
}

QSize PopupNumEditor::sizeHint() const
{
    int width = themeManger.getBorderWidth() * 2 + d_ptr->valueRect.width();
    int height = themeManger.getBorderWidth() * 2 + d_ptr->valueRect.height() * 3;
    return QSize(width, height);
}

void PopupNumEditor::keyPressEvent(QKeyEvent *ev)
{
    d_ptr->hasBeenPressed = true;
    switch (ev->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
        d_ptr->isDownPressed = true;
        update();
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
        d_ptr->isUpPressed = true;
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

void PopupNumEditor::keyReleaseEvent(QKeyEvent *ev)
{
    if (!d_ptr->hasBeenPressed)
    {
        return;
    }
    d_ptr->hasBeenPressed = false;
    switch (ev->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
        d_ptr->isDownPressed = false;
        d_ptr->decreaseValue();
        update();
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
        d_ptr->isUpPressed = false;
        d_ptr->increaseValue();
        update();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit valueChanged(d_ptr->editInfo.curValue);
        this->close();
        break;
    default:
        QWidget::keyReleaseEvent(ev);
        break;
    }
}

void PopupNumEditor::mousePressEvent(QMouseEvent *ev)
{
    d_ptr->hasBeenPressed = true;
    d_ptr->isUpPressed = d_ptr->mouseInUpRegion(ev->pos());
    d_ptr->isDownPressed = d_ptr->mouseInDownRegion(ev->pos());

    update();
    ev->accept();

    if (!d_ptr->isUpPressed && !d_ptr->isDownPressed)
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

void PopupNumEditor::mouseReleaseEvent(QMouseEvent *ev)
{
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

    if (d_ptr->isUpPressed && d_ptr->mouseInUpRegion(ev->pos()))
    {
        d_ptr->increaseValue();
    }
    else if (d_ptr->isDownPressed && d_ptr->mouseInDownRegion(ev->pos()))
    {
        d_ptr->decreaseValue();
    }
    else if (d_ptr->isDownPressed || d_ptr->isUpPressed)
    {
        // press and move out of the region, do nothing
    }
    else
    {
        // click somewhere else
        emit valueChanged(d_ptr->editInfo.curValue);
        close();
    }

    d_ptr->isUpPressed = false;
    d_ptr->isDownPressed = false;
    d_ptr->hasBeenPressed = false;
    update();
    ev->accept();
}

void PopupNumEditor::showEvent(QShowEvent *ev)
{
    QRect r(QPoint(0, 0), d_ptr->valueRect.size());
    QRect rr = this->rect();
    move(r.center() - rr.center() + d_ptr->valueRect.topLeft());
    QWidget::showEvent(ev);
}

void PopupNumEditor::timeOutExec()
{
    if (!d_ptr->hasBeenPressed)
    {
        d_ptr->timer->stop();
        delete d_ptr->timer;
        d_ptr->timer = NULL;
        return;
    }
    if (!d_ptr->isUpPressed && !d_ptr->isDownPressed)
    {
        d_ptr->timer->stop();
        delete d_ptr->timer;
        d_ptr->timer = NULL;
        return;
    }

    if (d_ptr->isUpPressed)
    {
        d_ptr->increaseValue();
    }
    else if (d_ptr->isDownPressed)
    {
        d_ptr->decreaseValue();
    }

    update();

    d_ptr->timer->start(40);
}

