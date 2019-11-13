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
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QTimer>
#include <QGridLayout>
#include <QLabel>
#include <QSignalMapper>

#define BUTTON_WIDTH 120
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 230

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

MessageBox::MessageBox(const QString &title, const QString &text, bool btn, bool wordWrap)
    : d_ptr(new MessageBoxPrivate)
{
    setWindowTitle(title);

    QGridLayout *layout = new QGridLayout();
    layout->setMargin(10);

    d_ptr->index++;
    QLabel *label = new QLabel(trs(text));
    if (wordWrap == true)
    {
        label->setWordWrap(true);
    }
    layout->addWidget(label, 1, 0, Qt::AlignCenter);
    d_ptr->index++;

    QHBoxLayout *hl = new QHBoxLayout;
    Button *button = new Button(trs("Yes"));
    button->setFixedWidth(BUTTON_WIDTH);
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
    d_ptr->yesKey = button;

    if (btn)
    {
        button = new Button(trs("No"));
        button->setFixedWidth(BUTTON_WIDTH);
        button->setButtonStyle(Button::ButtonTextOnly);
        hl->addWidget(button);
        connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
        d_ptr->noKey = button;
    }

    layout->addLayout(hl, 3, 0);
    layout->setRowStretch(4, 0);
    d_ptr->layout = layout;
    setWindowLayout(layout);

    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

MessageBox::MessageBox(const QString &title, const QPixmap &icon, const QString &text, bool btn, bool wordWrap)
    : d_ptr(new MessageBoxPrivate)
{
    setWindowTitle(title);

    QGridLayout *layout = new QGridLayout();
    layout->setMargin(10);

    d_ptr->index++;
    QLabel *label = new QLabel();
    label->setPixmap(icon);
    label->setScaledContents(true);
    label->setWordWrap(true);
    layout->addWidget(label, 1, 0, Qt::AlignCenter);
    d_ptr->index++;

    label = new QLabel(trs(text));
    if (wordWrap == true)
    {
        label->setWordWrap(true);
    }
    layout->addWidget(label, 1, 1,  Qt::AlignCenter);
    d_ptr->index++;

    QHBoxLayout *hl = new QHBoxLayout;
    Button *button = new Button(trs("Yes"));
    button->setFixedWidth(BUTTON_WIDTH);
    button->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(button);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
    d_ptr->yesKey = button;

    if (btn)
    {
        button = new Button(trs("No"));
        button->setFixedWidth(BUTTON_WIDTH);
        button->setButtonStyle(Button::ButtonTextOnly);
        hl->addWidget(button);
        connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
        d_ptr->noKey = button;
    }

    layout->addLayout(hl, 2, 0, 1, 2);
    d_ptr->layout = layout;
    setWindowLayout(layout);

    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

MessageBox::MessageBox(const QString &title, const QString &text, const QStringList &btnNameList, bool wordWrap)
    : d_ptr(new MessageBoxPrivate)
{
    setWindowTitle(title);

    QGridLayout *layout = new QGridLayout();
    layout->setMargin(10);

    d_ptr->index++;
    QLabel *label = new QLabel(trs(text));
    if (wordWrap == true)
    {
        label->setWordWrap(true);
    }
    layout->addWidget(label, 1, 0,  Qt::AlignCenter);
    d_ptr->index++;

    int index = 0;
    QHBoxLayout *hl = new QHBoxLayout;
    QSignalMapper *signalMapper = new QSignalMapper(this);
    foreach(QString btnName, btnNameList)
    {
        Button *key = new Button(trs(btnName));
        key->setFixedWidth(BUTTON_WIDTH);
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

    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
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
    Dialog::showEvent(ev);


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


