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
#include <QTextEdit>
#include "LanguageManager.h"
#include <QVBoxLayout>
#include <QScrollBar>
#include <QApplication>
#include <QKeyEvent>

class ErrorLogViewerWindowPrivate
{
public:
    explicit ErrorLogViewerWindowPrivate(ErrorLogItemBase *item = NULL)
        : textEdit(NULL), item(item)
    {}

public:
    QTextEdit *textEdit;
    ErrorLogItemBase *const item;
};
ErrorLogViewerWindow::ErrorLogViewerWindow()
    : Window(), d_ptr(new ErrorLogViewerWindowPrivate())
{
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(500, 400);
    d_ptr->textEdit = new QTextEdit();
    d_ptr->textEdit->setReadOnly(true);
    d_ptr->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    d_ptr->textEdit->setTabChangesFocus(true);
    d_ptr->textEdit->installEventFilter(this);
    d_ptr->textEdit->setFrameStyle(QFrame::NoFrame);
    d_ptr->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    d_ptr->textEdit->setFocusPolicy(Qt::NoFocus);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d_ptr->textEdit);

    setWindowLayout(layout);
}

ErrorLogViewerWindow::ErrorLogViewerWindow(ErrorLogItemBase *item)
    : Window(), d_ptr(new ErrorLogViewerWindowPrivate(item))
{
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(500, 400);
    setWindowTitle(item->name());
    d_ptr->textEdit = new QTextEdit();
    d_ptr->textEdit->setReadOnly(true);
    d_ptr->textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    d_ptr->textEdit->setTabChangesFocus(true);
    d_ptr->textEdit->setFrameStyle(QFrame::NoFrame);
    d_ptr->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    d_ptr->textEdit->setFocusPolicy(Qt::NoFocus);
    QString str;
    QTextStream stream(&str);
    item->outputInfo(stream);
    d_ptr->textEdit->setText(str);
    d_ptr->textEdit->installEventFilter(this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(d_ptr->textEdit);

    setWindowLayout(layout);
}

ErrorLogViewerWindow::~ErrorLogViewerWindow()
{
}

void ErrorLogViewerWindow::setText(const QString &text)
{
    d_ptr->textEdit->setText(text);
}

bool ErrorLogViewerWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if (d_ptr->textEdit == qobject_cast<QObject *>(obj))
    {
        if (ev->type() == QEvent::KeyRelease)
        {
            QKeyEvent *keyEv = static_cast<QKeyEvent *>(ev);
            QScrollBar *scrollBar = d_ptr->textEdit->verticalScrollBar();

            switch (keyEv->key())
            {
            case Qt::Key_Left:
            case Qt::Key_Up:
            {
                if (scrollBar->isVisible() && scrollBar->value() != scrollBar->minimum())
                {
                    scrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
                }
                else
                {
                    focusPreviousChild();
                }
            }
            break;
            case Qt::Key_Right:
            case Qt::Key_Down:
            {
                if (scrollBar->isVisible() && scrollBar->value() != scrollBar->maximum())
                {
                    scrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
                }
                else
                {
                    focusNextChild();
                }
            }
            break;
            }
        }
    }
    return false;
}
