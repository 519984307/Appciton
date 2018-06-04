#pragma once
#include <QKeyEvent>
#include <QButtonGroup>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include "PopupWidget.h"
#include "IButton.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "SystemManager.h"
#include "WindowManager.h"
#include <QPixmap>
#include <QSignalMapper>
#include "ColorManager.h"

class IMessageBox : public PopupWidget
{
    Q_OBJECT

public:

    explicit IMessageBox(): PopupWidget()
    {

    }

    explicit IMessageBox(const QString &title, const QString &text, bool btn = true): PopupWidget()
    {
        timeoutHook = NULL;
        hideExitButton();
        setTitleBarText(title);

        int fontSize = fontManager.getFontSize(1);

        QLabel *l = new QLabel(text);
        l->setFont(fontManager.textFont(fontSize));
        l->setAlignment(Qt::AlignHCenter);
        l->setWordWrap(true);

        QHBoxLayout *hlayout = new QHBoxLayout();
        hlayout->setMargin(0);
        hlayout->setSpacing(10);
        hlayout->addStretch();

        _YesKey = new IButton(trs("Yes"));
        _YesKey->setBorderEnabled(true);
        _YesKey->setBorderColor(QColor(120, 120, 120));
        _YesKey->setFont(fontManager.textFont(fontSize));
        connect(_YesKey, SIGNAL(realReleased()), this, SLOT(_yesReleased()));
        hlayout->addWidget(_YesKey);

        if (btn)
        {
            IButton *key = new IButton(trs("No"));
            key->setBorderEnabled(true);
            key->setBorderColor(QColor(120, 120, 120));
            key->setFont(fontManager.textFont(fontSize));
            connect(key, SIGNAL(realReleased()), this, SLOT(_noReleased()));
            hlayout->addWidget(key);
        }

        hlayout->addStretch();

        QVBoxLayout *vlayout = new QVBoxLayout();
        vlayout->setContentsMargins(10, 1, 10, 4);
        vlayout->setSpacing(4);
        vlayout->addWidget(l, 0, Qt::AlignVCenter);
        vlayout->addLayout(hlayout);

        contentLayout->addLayout(vlayout);

        _enableTimer = false;
        _interval = 1000;
        _timer = new QTimer();
        connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOut()));

        QDesktopWidget *w = QApplication::desktop();
        setFixedSize(w->width() / 2, w->height() / 3);
    }


    explicit IMessageBox(const QString &title, const QPixmap &icon, const QString &text, bool btn = true): PopupWidget()
    {

        timeoutHook = NULL;
        hideExitButton();
        setTitleBarText(title);


        QHBoxLayout *firstLayout = new QHBoxLayout();
        firstLayout->setSpacing(8);

        QLabel *iconlabel = new QLabel();
        iconlabel->setPixmap(icon);
        iconlabel->setScaledContents(true);
        firstLayout->addWidget(iconlabel, 0, Qt::AlignVCenter);

        int fontSize = fontManager.getFontSize(1);
        QLabel *l = new QLabel(text);
        l->setFont(fontManager.textFont(fontSize));
        l->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        l->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
        l->setWordWrap(true);
        firstLayout->addWidget(l, 0, Qt::AlignVCenter);


        QHBoxLayout *hlayout = new QHBoxLayout();
        hlayout->setMargin(0);
        hlayout->setSpacing(10);
        hlayout->addStretch();

        _YesKey = new IButton(trs("Yes"));
        _YesKey->setBorderEnabled(true);
        _YesKey->setBorderColor(QColor(120, 120, 120));
        _YesKey->setFont(fontManager.textFont(fontSize));
        connect(_YesKey, SIGNAL(realReleased()), this, SLOT(_yesReleased()));
        hlayout->addWidget(_YesKey);

        if (btn)
        {
            IButton *key = new IButton(trs("No"));
            key->setBorderEnabled(true);
            key->setBorderColor(QColor(120, 120, 120));
            key->setFont(fontManager.textFont(fontSize));
            connect(key, SIGNAL(realReleased()), this, SLOT(_noReleased()));
            hlayout->addWidget(key);
        }

        hlayout->addStretch();

        QVBoxLayout *vlayout = new QVBoxLayout();
        vlayout->setContentsMargins(10, 4, 10, 4);
        vlayout->setSpacing(4);
        vlayout->addLayout(firstLayout);
        vlayout->addLayout(hlayout);

        contentLayout->addLayout(vlayout);

        _enableTimer = false;
        _interval = 1000;
        _timer = new QTimer();
        connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOut()));

#if defined(Q_WS_QWS)
        QDesktopWidget *w = QApplication::desktop();
        setFixedSize(w->width() / 2, w->height() / 3);
#endif
    }
    explicit IMessageBox(const QString &title, const QString &text, const QStringList &btnNameList ): PopupWidget(), _YesKey(NULL)
    {
        timeoutHook = NULL;
        hideExitButton();
        setTitleBarText(title);

        int fontSize = fontManager.getFontSize(1);
        QLabel *l = new QLabel(text);
        l->setFont(fontManager.textFont(fontSize));
        l->setAlignment(Qt::AlignHCenter);
        l->setWordWrap(true);

        QHBoxLayout *hlayout = new QHBoxLayout();
        hlayout->setMargin(0);
        hlayout->setSpacing(10);
        hlayout->addStretch();

        int index = 0;
        QSignalMapper *signalMapper = new QSignalMapper(this);
        foreach(QString btnName, btnNameList)
        {
            IButton *key = new IButton(btnName);
            key->setBorderEnabled(true);
            key->setBorderColor(QColor(120, 120, 120));
            key->setFont(fontManager.textFont(fontSize));
            hlayout->addWidget(key);
            connect(key, SIGNAL(realReleased()), signalMapper, SLOT(map()));
            signalMapper->setMapping(key, index);
            index++;
        }

        connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(done(int)));
        hlayout->addStretch();


        QVBoxLayout *vlayout = new QVBoxLayout();
        vlayout->setMargin(4);
        vlayout->setSpacing(4);
        vlayout->addWidget(l, 0, Qt::AlignVCenter);
        vlayout->addLayout(hlayout);

        contentLayout->addLayout(vlayout);

        _enableTimer = false;
        _interval = 1000;
        _timer = new QTimer();
        connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOut()));

#if defined(Q_WS_QWS)
        QDesktopWidget *w = QApplication::desktop();
        setFixedSize(w->width() / 2, w->height() / 3);
#endif
    }

    ~IMessageBox()
    {
        if (NULL != _timer)
        {
            delete _timer;
            _timer = NULL;
        }
    }

public:
    void enableTimer(bool enable, int interval)
    {
        _enableTimer = enable;
        _interval = interval;
    }

    void setYesKeyVisible(bool enable)
    {
        _YesKey->setVisible(enable);
    }

    void getFunction(bool fun(void))
    {
        timeoutHook = fun;
    }

    void setYesBtnTxt(const QString &txt)
    {
        if (_YesKey)
        {
            _YesKey->setText(txt);
        }
    }

protected:
    virtual void exit(void)
    {
        done(0);
    }

    //void keyPressEvent(QKeyEvent *e)
    //{
        //e->ignore();
    //}

    void showEvent(QShowEvent *event)
    {
        PopupWidget::showEvent(event);

        if(_YesKey)
        {
            _YesKey->setFocus();
        }
        if (_enableTimer)
        {
            if (NULL != _timer)
            {
                if(_interval>0)
                {
                    _timer->start(_interval);
                }
            }
        }
    }

private slots:
    void _yesReleased(void)
    {
        done(1);
    }

    void _noReleased(void)
    {
        done(0);
    }

    void _timeOut(void)
    {
        if(timeoutHook != NULL)
        {
            if(timeoutHook())
            {
                _timer->stop();
                done(1);
            }
        }
        else
        {
            done(0);
        }
    }

private:
    QTimer *_timer;
    QTime *a;
    bool _enableTimer;
    int _interval;

    typedef bool (*timeOutHook)(void);
    timeOutHook timeoutHook;

    IButton *_YesKey;
    IButton *_AllYesKey;
    IButton *_AllNoKey;
    IButton *_NoKey;


};
