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
#include "Dialog.h"
#include "FontManager.h"
#include <QTimer>
#include "TopBarWidget.h"
#include "LayoutManager.h"

#define WINDOW_MARGINS 3

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
    QList<QPointer<Dialog> > windowStacks;
    QTimer *timer;              // timer to auto close the windows
    QWidget *demoWidget;
    Dialog *curWindow;

    /**
     * @brief menuProperPos 菜单显示合适的位置
     * @param w
     * @return
     */
    QPoint menuProperPos(Dialog *w);
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
        WindowManagerInterface *old = registerWindowManager(instance);
        if (old)
        {
            delete old;
        }
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

QPoint WindowManagerPrivate::menuProperPos(Dialog *w)
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
        if (r.width() >= w->width())
        {
            // 菜单将靠右上显示
            r.adjust(r.width() - w->width(), 0, 0, 0);
        }
        else
        {
            // 显示不全时，遮挡参数区
            r.adjust(WINDOW_MARGINS, 0, 0, 0);
        }
        if (r.height() < w->height())
        {
            // 显示不全时，遮挡第一道波形
            r.adjust(0, r.height() - w->height(), 0, 0);
        }
        globalTopLeft = r.topLeft();
        if (windowStacks.count() > 1)
        {
            // 二级以上的菜单在一级菜单区域中居中显示
            Dialog *win = windowStacks.at(0);
            QPoint tmp = globalTopLeft - QPoint((win->width() - w->width()) / 2, -(win->height() - w->height()) / 2);
            if (tmp.rx() + w->width() <= r.topRight().rx())
            {
                globalTopLeft = tmp;
            }
        }
    }
    return globalTopLeft;
}

void WindowManager::showWindow(Dialog *w, ShowBehavior behaviors)
{
    if (w == NULL)
    {
        qWarning() << "Invalid window";
        return;
    }

    d_ptr->curWindow = w;

    Dialog *top = topWindow();

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
        while (!d_ptr->windowStacks.isEmpty())
        {
            Dialog *p = d_ptr->windowStacks.last();
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

    QPointer<Dialog> newP = w;
    // remove the window in the stack if it's already exist.
    QList<QPointer<Dialog> >::Iterator iter = d_ptr->windowStacks.begin();
    for (; iter != d_ptr->windowStacks.end(); ++iter)
    {
        if (iter->data() == w)
        {
            d_ptr->windowStacks.erase(iter);
            break;
        }
    }

    // 添加窗口定时器是否启动属性
    bool timerStart = true;
    if (behaviors & ShowBehaviorNoAutoClose)
    {
        timerStart = false;
    }
    newP->setProperty("TimerStart", qVariantFromValue(timerStart));

    d_ptr->windowStacks.append(newP);
    connect(w, SIGNAL(windowHide(Dialog *)), this, SLOT(onWindowHide(Dialog *)), Qt::DirectConnection);

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

Dialog *WindowManager::topWindow()
{
    // find top window
    QPointer<Dialog> top;
    while (!d_ptr->windowStacks.isEmpty())
    {
        top = d_ptr->windowStacks.last();
        if (top)
        {
            break;
        }
        else
        {
            d_ptr->windowStacks.takeLast();
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
        pal.setColor(QPalette::Window, QColor(0xc0, 0xc0, 0xc0, 0xb0));   // 灰色 透明度百分六十左右
        pal.setColor(QPalette::WindowText, QColor(0xff, 0xff, 0xff, 0xb0));  // 白色 透明度百分六十左右
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
    if (d_ptr->windowStacks.isEmpty())
    {
        return false;
    }

    if (ev->type() == QEvent::Show)
    {
        bool timerStart = true;
        QList<QPointer<Dialog> >::Iterator iter = d_ptr->windowStacks.begin();
        for (; iter != d_ptr->windowStacks.end(); ++iter)
        {
            if (iter->data() == d_ptr->curWindow)
            {
                timerStart = d_ptr->curWindow->property("TimerStart").toBool();
                break;
            }
        }
        if (timerStart == true)
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

    bool dialogStatus = false;
    // close the window in the window stack
    while (!d_ptr->windowStacks.isEmpty())
    {
        Dialog *p = d_ptr->windowStacks.last();
        if (p)
        {
            p->close();
            dialogStatus = true;
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

    if (dialogStatus == true)
    {  // send the signal when the dialgs's status changes
        emit allDialogsStatusChanged();
    }

    QApplication::processEvents(QEventLoop::AllEvents);
}

void WindowManager::onWindowHide(Dialog *w)
{
    // find top window
    Dialog *top = topWindow();

    if (top == w)
    {
        // remove the window,
        d_ptr->windowStacks.takeLast();
        disconnect(w, SIGNAL(windowHide(Dialog *)), this, SLOT(onWindowHide(Dialog *)));

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
    else if (d_ptr->windowStacks.isEmpty())
    {
        disconnect(w, SIGNAL(windowHide(Dialog *)), this, SLOT(onWindowHide(Dialog *)));
        d_ptr->timer->stop();
    }

    QApplication::processEvents(QEventLoop::AllEvents);
}
