/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/10
 **/

#include "ErrorLogViewerWindow.h"
#include "LanguageManager.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QKeyEvent>
#include "ScrollArea.h"
#include <QScrollBar>
#include <QLabel>
#include <qdebug.h>

class ErrorLogViewerWindowPrivate
{
public:
    explicit ErrorLogViewerWindowPrivate(ErrorLogItemBase *item = NULL)
        : text(NULL), item(item), area(NULL)
    {}

public:
    QLabel *text;
    ErrorLogItemBase *const item;
    ScrollArea *area;
};
ErrorLogViewerWindow::ErrorLogViewerWindow()
    : Dialog(), d_ptr(new ErrorLogViewerWindowPrivate())
{
    setFixedSize(500, 400);
    d_ptr->text = new QLabel();
    d_ptr->text->setWordWrap(true);

    d_ptr->area = new ScrollArea();
    d_ptr->area->setWidget(d_ptr->text);
    d_ptr->area->setAcceptDrops(true);
    d_ptr->area->setContentsMargins(10, 10, 10, 10);
    d_ptr->area->setFloatbarPolicy(ScrollArea::FloatBarShowForeverWhenNeeded);
    d_ptr->area->setScrollDirection(ScrollArea::ScrollVertical);
    d_ptr->area->installEventFilter(this);
    d_ptr->area->setFocusPolicy(Qt::StrongFocus);
    d_ptr->area->verticalScrollBar()->setMaximum(d_ptr->text->height());

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d_ptr->area);
    layout->addStretch();
    setWindowLayout(layout);
}

ErrorLogViewerWindow::ErrorLogViewerWindow(ErrorLogItemBase *item)
    : Dialog(), d_ptr(new ErrorLogViewerWindowPrivate(item))
{
    setFixedSize(500, 400);
    setWindowTitle(item->name());
    d_ptr->text = new QLabel();
    d_ptr->text->setWordWrap(true);
    QString str;
    QTextStream stream(&str);
    item->outputInfo(stream);
    d_ptr->text->setText(str);

    d_ptr->area = new ScrollArea();
    d_ptr->area->setContentsMargins(10, 10, 10, 10);
    d_ptr->area->setWidget(d_ptr->text);
    d_ptr->area->setAcceptDrops(true);
    d_ptr->area->setOverShot(true);
    d_ptr->area->setFloatbarPolicy(ScrollArea::FloatBarShowForeverWhenNeeded);
    d_ptr->area->setScrollDirection(ScrollArea::ScrollVertical);
    d_ptr->area->installEventFilter(this);
    d_ptr->area->setFocusPolicy(Qt::StrongFocus);
    d_ptr->area->verticalScrollBar()->setMaximum(d_ptr->text->height());

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d_ptr->area);
    layout->addStretch();

    setWindowLayout(layout);
}

ErrorLogViewerWindow::~ErrorLogViewerWindow()
{
}

void ErrorLogViewerWindow::setText(const QString &text)
{
    d_ptr->text->setText(text);
}

bool ErrorLogViewerWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == d_ptr->area)
    {
        qDebug() << ev->type();
        if (ev->type() == QEvent::KeyRelease)
        {
            bool ret = false;
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(ev);
            static int viewPortPos = 0;
            if (keyEvent->key() == Qt::Key_Right)
            {
                // scroll down
                if (d_ptr->area->verticalScrollBar()->value() != d_ptr->area->verticalScrollBar()->maximum())
                {
                    // 当滑动未到底部时，则拦截该事件
                    ret = true;
                    viewPortPos += d_ptr->area->viewport()->height();
                }
            }
            else if (keyEvent->key() == Qt::Key_Left)
            {
                // scroll up
                if (d_ptr->area->verticalScrollBar()->value() != d_ptr->area->verticalScrollBar()->minimum())
                {
                    // 当滑动未到顶部时，则拦截该事件
                    ret = true;
                    viewPortPos -= d_ptr->area->viewport()->height();
                }
            }
            d_ptr->area->verticalScrollBar()->setValue(viewPortPos);
            return ret;
        }
        else if (ev->type() == QEvent::FocusIn)
        {
            // 如果无需滑动显示下一页，则直接聚焦下一个焦点
            if (d_ptr->area->verticalScrollBar()->maximum() == d_ptr->area->verticalScrollBar()->minimum())
            {
                focusNextChild();
            }
        }
        return false;
    }
    return Dialog::eventFilter(obj, ev);
}
