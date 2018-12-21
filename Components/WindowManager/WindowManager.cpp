/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/12
 **/

#include "Debug.h"
#include <QList>
#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include "IWidget.h"
#include "WindowManager.h"
#include "SoftKeyManager.h"
#include "IConfig.h"
#include "SystemModeBarWidget.h"
#include "TrendWidget.h"
#include "CO2Param.h"
#include "RESPParam.h"
#include "ECGParam.h"
#include "ECGSymbol.h"
#include "MainMenuWindow.h"
#include "ConfigManagerMenuWindow.h"
#include "UserMaintainMenuWindow.h"
#include "FactoryMaintainMenuWindow.h"
#include "ConfigEditMenuWindow.h"
#include "NIBPRepairMenuWindow.h"
#include <QApplication>
#include "Window.h"
#include "FontManager.h"
#include <QTimer>
#include "TopBarWidget.h"
#include "LayoutManager.h"

struct WindowStacksInfo{
    WindowStacksInfo()
       : timerStart(true)
    {}
    QPointer<Window> window;
    bool timerStart;
};

class WindowManagerPrivate
{
public:
    explicit WindowManagerPrivate(WindowManager * const q_ptr)
        :q_ptr(q_ptr),
          timer(NULL),
          demoWidget(NULL)
        , curWindow(NULL)
    {}

    WindowManager * const q_ptr;
    QList<WindowStacksInfo> windowStacksInfo;
    QTimer *timer;              // timer to auto close the windows
    QWidget *demoWidget;
    Window *curWindow;
    /**
     * @brief menuProperPos 菜单显示合适的位置
     * @param w
     * @return
     */
    QPoint menuProperPos(Window *w);
};

void WindowManager::onLayoutChanged()
{
    if (d_ptr->demoWidget && d_ptr->demoWidget->isVisible())
    {
        d_ptr->demoWidget->raise();
    }
}

/***************************************************************************************************
 * 功能： 获取弹出菜单宽度
 **************************************************************************************************/
int WindowManager::getPopWindowWidth()
{
    return 710;
}

/***************************************************************************************************
 * 功能： 获取弹出菜单高度
 **************************************************************************************************/
int WindowManager::getPopWindowHeight()
{
    return 530;
}

/***************************************************************************************************
 * 功能：构造函数
 **************************************************************************************************/
#if defined(Q_WS_QWS)
WindowManager::WindowManager() : QWidget(NULL, Qt::FramelessWindowHint), d_ptr(new WindowManagerPrivate(this))
#else
WindowManager::WindowManager() : QWidget(), d_ptr(new WindowManagerPrivate(this))
#endif
{
    // 设置调色
    QPalette p;
    p.setColor(QPalette::Background, Qt::black);
    p.setColor(QPalette::Foreground, Qt::white);
    setPalette(p);

    d_ptr->timer = new QTimer(this);
    d_ptr->timer->setSingleShot(true);
    d_ptr->timer->setInterval(60 * 1000);    // 60s
    connect(d_ptr->timer, SIGNAL(timeout()), this, SLOT(closeAllWidows()));

    qApp->installEventFilter(this);

    connect(&layoutManager, SIGNAL(layoutChanged()), this, SLOT(onLayoutChanged()));
    QTimer::singleShot(0, this, SLOT(show()));
}

/***************************************************************************************************
 * 功能：析构函数
 **************************************************************************************************/
WindowManager &WindowManager::getInstance()
{
    static WindowManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new WindowManager();
    }
    return *instance;
}

WindowManager::~WindowManager()
{
    // 清除窗体
    QList<IWidget *> winList = _winMap.values();
    foreach(IWidget *w, winList)
    {
        if (w && !w->parent())
        {
            delete w;   // WindowManager的所有子窗体会自动析构，因此不需要手动析构。
            w = NULL;
        }
    }

    _winMap.clear();
}

QPoint WindowManagerPrivate::menuProperPos(Window *w)
{
    // move the proper position
    QRect r = layoutManager.getMenuArea();
    QPoint globalTopLeft;
    if (layoutManager.getUFaceType() == UFACE_MONITOR_BIGFONT)
    {
        // 菜单居中显示
        globalTopLeft = r.center() - w->rect().center();
    }
    else
    {
        // 菜单将靠右上显示
        r.adjust(r.width() - w->width(), 0, 0, 0);
        if (r.height() < w->height())
        {
            // 显示不全时，遮挡第一道波形
            r.adjust(0, r.height() - w->height(), 0, 0);
        }
        globalTopLeft = r.topLeft();
        if (windowStacksInfo.count() > 1)
        {
            // 二级以上的菜单在一级菜单区域中居中显示
            Window *win = windowStacksInfo.at(0).window;
            QPoint tmp = globalTopLeft - QPoint((win->width() - w->width()) / 2, -(win->height() - w->height()) / 2);
            if (tmp.rx() + w->width() <= r.topRight().rx())
            {
                globalTopLeft = tmp;
            }
        }
    }
    return globalTopLeft;
}

void WindowManager::showWindow(Window *w, ShowBehavior behaviors)
{
    if (w == NULL)
    {
        qWarning() << "Invalid window";
        return;
    }

    d_ptr->curWindow = w;

    Window *top = topWindow();

    if (top == w)
    {
        if (behaviors & ShowBehaviorCloseIfVisiable)
        {
            // close will emit windowHide signal, so the window will remove from the stack after calling
            // the window hide slot
            w->close();
        }
        else
        {
            w->activateWindow();
        }
        return;
    }


    if (behaviors & ShowBehaviorCloseOthers)
    {
        while (!d_ptr->windowStacksInfo.isEmpty())
        {
            Window *p = d_ptr->windowStacksInfo.last().window;
            if (p)
            {
                p->close();
            }
        }
        // when got here, no any top window
        top = NULL;
    }

    if (top)
    {
        // will not enter here if the behaviro is close others
        if (behaviors & ShowBehaviorHideOthers)
        {
            // don't emit windowHide signal
            top->blockSignals(true);
            top->hide();
            top->blockSignals(false);
        }
        else
        {
            top->showMask(true);
        }
    }

    QPointer<Window> newP = w;
    // remove the window in the stack if it's already exist.
    WindowStacksInfo stackInfo;
    for (int i = 0; i < d_ptr->windowStacksInfo.count(); i++)
    {
        stackInfo = d_ptr->windowStacksInfo.at(i);
        if (newP == stackInfo.window)
        {
            d_ptr->windowStacksInfo.removeAt(i);
            break;
        }
    }
    stackInfo.window = newP;
    if (behaviors & ShowBehaviorNoAutoClose)
    {
        stackInfo.timerStart = false;
    }
    else
    {
        stackInfo.timerStart = true;
    }
    d_ptr->windowStacksInfo.append(stackInfo);
    connect(w, SIGNAL(windowHide(Window *)), this, SLOT(onWindowHide(Window *)), Qt::DirectConnection);

    if (behaviors & ShowBehaviorModal)
    {
        // 在显示模态窗口之前移动合适的位置
        w->move(d_ptr->menuProperPos(w));
        w->exec();
    }
    else
    {
        w->show();
        // 在show之后可以根据自适应的窗口大小移动位置
        w->move(d_ptr->menuProperPos(w));
        w->activateWindow();
    }
}

Window *WindowManager::topWindow()
{
    // find top window
    QPointer<Window> top;
    while (!d_ptr->windowStacksInfo.isEmpty())
    {
        top = d_ptr->windowStacksInfo.last().window;
        if (top)
        {
            break;
        }
        else
        {
            d_ptr->windowStacksInfo.takeLast();
        }
    }
    return top.data();
}

void WindowManager::showDemoWidget(bool flag)
{
    if (d_ptr->demoWidget == NULL)
    {
        // demo widget no exist yet, create one
        QLabel *l = new QLabel(trs("DEMO"), this);
        l->setAutoFillBackground(true);
        QPalette pal = l->palette();
        pal.setColor(QPalette::Window, Qt::gray);
        pal.setColor(QPalette::WindowText, Qt::white);
        l->setPalette(pal);
        l->setFont(fontManager.textFont(64));
        d_ptr->demoWidget = l;
        d_ptr->demoWidget->move(430, 100);
        d_ptr->demoWidget->setFixedSize(l->sizeHint());
    }

    if (flag)
    {
        d_ptr->demoWidget->show();
        d_ptr->demoWidget->raise();
    }
    else
    {
        d_ptr->demoWidget->lower();
        d_ptr->demoWidget->hide();
    }
}

bool WindowManager::eventFilter(QObject *obj, QEvent *ev)
{
    Q_UNUSED(obj)
    if (d_ptr->windowStacksInfo.isEmpty())
    {
        return false;
    }

    if (ev->type() == QEvent::Show)
    {
        bool start = true;
        for (int i = 0; i < d_ptr->windowStacksInfo.count(); i++)
        {
            WindowStacksInfo stackInfo = d_ptr->windowStacksInfo.at(i);
            if (d_ptr->curWindow == stackInfo.window)
            {
                start = stackInfo.timerStart;
                break;
            }
        }
        if (start == true)
        {
            d_ptr->timer->start();
        }
        else
        {
            d_ptr->timer->stop();
        }
    }

    if ((ev->type() == QEvent::KeyPress) || (ev->type() == QEvent::MouseButtonPress))
    {
        // reactive the timer
        if (d_ptr->timer->isActive())
        {
            d_ptr->timer->start();
        }
    }

    return false;
}


void WindowManager::closeAllWidows()
{

    // close the popup widget
    QWidget *popup = NULL;
    while ((popup = QApplication::activePopupWidget()))
    {
        popup->close();
    }

    // close the window in the window stack
    while (!d_ptr->windowStacksInfo.isEmpty())
    {
        Window *p = d_ptr->windowStacksInfo.last().window;
        if (p)
        {
            p->close();
        }
    }

    // close the active window
    QWidget *activeWindow = NULL;
    while ((activeWindow = QApplication::activeWindow()))
    {
        if (activeWindow == this)
        {
            break;
        }
        activeWindow->close();
    }
    d_ptr->timer->stop();
}

void WindowManager::onWindowHide(Window *w)
{
    // find top window
    Window *top = topWindow();

    if (top == w)
    {
        // remove the window,
        d_ptr->windowStacksInfo.takeLast();
        disconnect(w, SIGNAL(windowHide(Window *)), this, SLOT(onWindowHide(Window *)));

        // show the previous window
        top = topWindow();
        if (top)
        {
            top->showMask(false);
            d_ptr->curWindow = top;
            top->show();
        }
        else
        {
            d_ptr->timer->stop();
        }
    }
    else if (d_ptr->windowStacksInfo.isEmpty())
    {
        disconnect(w, SIGNAL(windowHide(Window *)), this, SLOT(onWindowHide(Window *)));
        d_ptr->timer->stop();
    }
}
