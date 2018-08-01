/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/25
 **/
#include "MessageBox.h"
#include "Button.h"
#include <QTimer>
#include <QGridLayout>
#include <QLabel>
#include <QSignalMapper>
#include "LanguageManager.h"
#include <QApplication>
#include <QDesktopWidget>

class MessageBoxPrivate
{
public:
    MessageBoxPrivate();
    typedef bool (*TimeOutHook)(void);
    TimeOutHook timeOutHook;
    QTimer *timer;
    bool isEnableTimer;
    int interval;
    int index;

    Button *yesKey;
    Button *noKey;

    QGridLayout *layout;
};

MessageBoxPrivate::MessageBoxPrivate()
    : timeOutHook(NULL),
      timer(NULL),
      isEnableTimer(false),
      interval(0),
      index(0),
      yesKey(NULL),
      noKey(NULL),
      layout(NULL)
{
}

MessageBox::~MessageBox()
{
    delete d_ptr;
}

MessageBox::MessageBox(const QString &title, const QString &text, bool btn)
    : d_ptr(new MessageBoxPrivate)
{
    setWindowTitle(title);

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    d_ptr->index++;
    QLabel *label = new QLabel(trs(text));
    layout->addWidget(label, 1, 0, Qt::AlignCenter);
    d_ptr->index++;

    QHBoxLayout *hl = new QHBoxLayout;
    Button *button = new Button(trs("Yes"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
    d_ptr->yesKey = button;

    if (btn)
    {
        button = new Button(trs("No"));
        button->setButtonStyle(Button::ButtonTextOnly);
        hl->addWidget(button);
        connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
        d_ptr->noKey = button;
    }

    layout->addLayout(hl, 3, 0);
    layout->setRowStretch(4, 0);
    d_ptr->layout = layout;
    setWindowLayout(layout);

    QDesktopWidget *w = QApplication::desktop();
    setFixedSize(w->width() / 4, w->height() / 4);
}

MessageBox::MessageBox(const QString &title, const QPixmap &icon, const QString &text, bool btn)
    : d_ptr(new MessageBoxPrivate)
{
    setWindowTitle(title);

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    d_ptr->index++;
    QLabel *label = new QLabel();
    label->setPixmap(icon);
    label->setScaledContents(true);
    layout->addWidget(label, 1, 0, Qt::AlignCenter);
    d_ptr->index++;

    label = new QLabel(trs(text));
    layout->addWidget(label, 2, 0,  Qt::AlignCenter);
    d_ptr->index++;

    QHBoxLayout *hl = new QHBoxLayout;
    Button *button = new Button(trs("Yes"));
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
    d_ptr->yesKey = button;

    if (btn)
    {
        button = new Button(trs("No"));
        button->setButtonStyle(Button::ButtonTextOnly);
        hl->addWidget(button);
        connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
        d_ptr->noKey = button;
    }

    layout->addLayout(hl, 4, 0);
    layout->setRowStretch(5, 0);
    d_ptr->layout = layout;
    setWindowLayout(layout);

    QDesktopWidget *w = QApplication::desktop();
    setFixedSize(w->width() / 4, w->height() / 4);
}

MessageBox::MessageBox(const QString &title, const QString &text, const QStringList &btnNameList)
    : d_ptr(new MessageBoxPrivate)
{
    setWindowTitle(title);

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    d_ptr->index++;
    QLabel *label = new QLabel(trs(text));
    layout->addWidget(label, 1, 0,  Qt::AlignCenter);
    d_ptr->index++;

    int index = 0;
    QHBoxLayout *hl = new QHBoxLayout;
    QSignalMapper *signalMapper = new QSignalMapper(this);
    foreach(QString btnName, btnNameList)
    {
        Button *key = new Button(trs(btnName));
        key->setMinimumWidth(100);
        key->setButtonStyle(Button::ButtonTextOnly);
        hl->addWidget(key);
        connect(key, SIGNAL(released()), signalMapper, SLOT(map()));
        signalMapper->setMapping(key, index);
        index++;
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(done(int)));
    layout->addLayout(hl, 3, 0);
    layout->setRowStretch(4, 0);
    d_ptr->layout = layout;
    setWindowLayout(layout);

//    QDesktopWidget *w = QApplication::desktop();
//    setFixedSize(w->width() / 4, w->height() / 4);
}

void MessageBox::onBtnReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (d_ptr->yesKey == button)
    {
        done(1);
    }
    else if (d_ptr->noKey == button)
    {
        done(0);
    }
}

void MessageBox::timeOut()
{
    if (!d_ptr->timeOutHook)
    {
        done(0);
        return;
    }
    if (d_ptr->timeOutHook())
    {
        d_ptr->timer->stop();
        done(1);
        return;
    }
}

void MessageBox::exit()
{
    done(0);
}

void MessageBox::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);


    if (d_ptr->yesKey)
    {
        d_ptr->yesKey->setMinimumWidth(100);
        d_ptr->yesKey->setMaximumWidth(200);
    }
    if (d_ptr->noKey)
    {
        d_ptr->noKey->setMinimumWidth(100);
        d_ptr->noKey->setMaximumWidth(200);
    }

    if (d_ptr->layout)
    {
        d_ptr->layout->setRowMinimumHeight(d_ptr->index, 30);
        d_ptr->layout->setRowMinimumHeight(0, 15);
    }

    if (d_ptr->yesKey)
    {
        d_ptr->yesKey->setFocus();
    }
    if (!d_ptr->isEnableTimer)
    {
        return;
    }
    if (!d_ptr->timer)
    {
        return;
    }
    if (d_ptr->interval > 0)
    {
        d_ptr->timer->start(d_ptr->interval);
    }
}


