/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/22
 **/

#include <QShortcut>
#include <QStyleFactory>
#include "FontManager.h"
#include "IApplication.h"
#include "SystemManager.h"
#include "KeyActionManager.h"
#include "Debug.h"
#include "USBManager.h"
#include "SoundManagerInterface.h"
#include "MessageBox.h"

#if defined(CONFIG_CAPTURE_SCREEN)
#include <QDateTime>
#include "Utility.h"
#include "LanguageManager.h"
#include "WindowManager.h"
#endif

/**************************************************************************************************
 * 功能：筛选事件，在X11环境中，通过x11EventFilter()来获取全局的事件
 * 参数:
 *      e: 事件参数
 *************************************************************************************************/
#if defined(Q_WS_X11)
#include <X11/Xlib.h>
bool IApplication::x11EventFilter(XEvent *e)
{
    // 关闭键盘事件重复发送特性。
//    ::XAutoRepeatOff(e->xany.display);

#if defined(CONFIG_CAPTURE_SCREEN)
    if (e->type == KeyPress || e->type == KeyRelease)
    {
        Qt::Key key = Qt::Key_unknown;
        if (e->xkey.keycode == 67)
        {
            key = Qt::Key_F1;
        }
        else if (e->xkey.keycode == 36)
        {
            key = Qt::Key_Return;
        }

        if (handleScreenCaptureKeyEvent(key, e->type == KeyPress))
        {
            // screen capture key is pressed and handle
            return true;
        }
    }
#endif

    if (e->type == KeyRelease)
    {
        switch (e->xkey.keycode)
        {
        case 67:                      // F1
            keyActionManager.handleKeyAction(KEY_F1_RELEASED);
            break;

        case 68:                      // F2
            keyActionManager.handleKeyAction(KEY_F2_RELEASED);
            break;

        case 69:                      // F3
            keyActionManager.handleKeyAction(KEY_F3_RELEASED);
            break;

        case 70:                      // F4
            keyActionManager.handleKeyAction(KEY_F4_RELEASED);
            break;

        case 71:                      // F5
            keyActionManager.handleKeyAction(KEY_F5_RELEASED);
            break;

        case 72:                      // F6
            keyActionManager.handleKeyAction(KEY_F6_RELEASED);
            break;

        case 73:                      // F7
            keyActionManager.handleKeyAction(KEY_F7_RELEASED);
            break;

        case 74:                      // F8
            keyActionManager.handleKeyAction(KEY_F8_RELEASED);
            break;

        case 75:                      // F9
            keyActionManager.handleKeyAction(KEY_F9_RELEASED);
            break;

        case 24:                      // Q
            keyActionManager.handleKeyAction(KEY_Q_RELEASED);
            break;

        case 25:                      // W
            keyActionManager.handleKeyAction(KEY_W_RELEASED);
            break;

        case 26:                      // E
            keyActionManager.handleKeyAction(KEY_E_RELEASED);
            break;

        case 27:                      // R
            keyActionManager.handleKeyAction(KEY_R_RELEASED);
            break;

        case 28:                      // T
            keyActionManager.handleKeyAction(KEY_T_RELEASED);
            break;

        default:
            break;
        }
    }
    else if (e->type == KeyPress)
    {
        switch (e->xkey.keycode)
        {
        case 67:                      // F1
            keyActionManager.handleKeyAction(KEY_F1_PRESSED);
            break;

        case 68:                      // F2
            keyActionManager.handleKeyAction(KEY_F2_PRESSED);
            break;

        case 69:                      // F3
            keyActionManager.handleKeyAction(KEY_F3_PRESSED);
            break;

        case 70:                      // F4
            keyActionManager.handleKeyAction(KEY_F4_PRESSED);
            break;

        case 71:                      // F5
            keyActionManager.handleKeyAction(KEY_F5_PRESSED);
            break;

        case 72:                      // F6
            keyActionManager.handleKeyAction(KEY_F6_PRESSED);
            break;

        case 73:                      // F7
            keyActionManager.handleKeyAction(KEY_F7_PRESSED);
            break;

        case 74:                      // F8
            keyActionManager.handleKeyAction(KEY_F8_PRESSED);
            break;

        case 75:                      // F9
            keyActionManager.handleKeyAction(KEY_F9_PRESSED);
            break;

        default:
            break;
        }
    }

    return QApplication::x11EventFilter(e);
}
#endif

#ifdef CONFIG_CAPTURE_SCREEN
static long capture(const QVariant &para)
{
    QString filepath = para.toString();
    if (filepath.isEmpty())
    {
        return 0;
    }
    QImage *screen = new QImage(Util::captureScreen());

    if (screen->isNull() || (!screen->save(filepath)))
    {
        delete screen;
        return 0;
    }
    else
    {
        // Note: screen is deleted in slot
        return (long) screen;
    }
}

void IApplication::handleScreenCaptureResult(long result)
{
    if (result)
    {
        QImage *image = reinterpret_cast<QImage *>(result);
        MessageBox msgBox(trs("ScreenCapture"), QPixmap::fromImage(*image).scaled(
                              150, 90, Qt::IgnoreAspectRatio, Qt::SmoothTransformation), trs("CaptureSuccess"), false);

        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | flags);
        windowManager.showWindow(&msgBox,
                                 WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
        delete image;
    }
    else
    {
        MessageBox msgBox(trs("ScreenCapture"), trs("CaptureFail"), false);
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | flags);
        windowManager.showWindow(&msgBox, WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
    }
}


#define MENU_KEY Qt::Key_F4

bool IApplication::handleScreenCaptureKeyEvent(Qt::Key key, bool isPressed)
{
    // To peform a screen caputure, must meet the following conditions:
    // 1. Menu Key and Enter key Press at the same time
    // 2. USB disk is connected
    // 3. only one capture action during one second
    static bool returnKeyPress = false;
    static bool menuKeyPress = false;
    static QDateTime lastDatetime;
    static bool isCaptureProcess = false;
    if (key == MENU_KEY)
    {
        menuKeyPress = isPressed;
        if (isCaptureProcess && (!menuKeyPress && returnKeyPress))
        {
            // filter this event
            return true;
        }
    }
    if (key == Qt::Key_Return)
    {
        returnKeyPress = isPressed;
        if (isCaptureProcess && (!returnKeyPress && menuKeyPress))
        {
            // filter this event
            return true;
        }
    }
    if (returnKeyPress && menuKeyPress && usbManager.isUSBExist())
    {
        QDateTime curDt = QDateTime::currentDateTime();
        if (curDt != lastDatetime)
        {
            QString imageFilename = QString("%1/%2.png")
                                    .arg(usbManager.getUdiskMountPoint())
                                    .arg(curDt.toString("yyyyMMddhhmmss"));

#ifdef Q_WS_X11
            isCaptureProcess = true;
            handleScreenCaptureResult(capture(imageFilename));
#else
            Util::WorkerThread *workerThread = new Util::WorkerThread(capture, imageFilename);
            connect(workerThread, SIGNAL(resultReady(long)), this, SLOT(handleScreenCaptureResult(long)));
            lastDatetime = curDt;
            workerThread->start();
            isCaptureProcess = true;
#endif
            return true;
        }
    }
    else if (isCaptureProcess && (!returnKeyPress && !menuKeyPress))
    {
        // return_key and menu_key is released, exit capture process
        // still need to filter this key event
        isCaptureProcess = false;
        keyActionManager.reset();
        return true;
    }

    return false;
}
#endif

#if defined(Q_WS_QWS)
#include <QWSEvent>


/**************************************************************************************************
 * 功能：筛选事件，在嵌入式Linux环境中，必须要通过qwsEventFilter()来获取全局的事件
 * 参数:
 *      e: 事件参数
 *************************************************************************************************/
bool IApplication::qwsEventFilter(QWSEvent *e)
{
    if (e->type == QWSEvent::Key)
    {
        QWSKeyEvent *keyEvent = static_cast<QWSKeyEvent *>(e);

#ifdef CONFIG_CAPTURE_SCREEN
        if (handleScreenCaptureKeyEvent(static_cast<Qt::Key>(keyEvent->simpleData.keycode),
                                        keyEvent->simpleData.is_press))
        {
            // screen capture key is pressed and handle
            return true;
        }
#endif


        if (!keyEvent->simpleData.is_press)   // release事件。
        {
            switch (keyEvent->simpleData.keycode)
            {
            case Qt::Key_F1:
                keyActionManager.handleKeyAction(KEY_F1_RELEASED);
                break;

            case Qt::Key_F2:
                keyActionManager.handleKeyAction(KEY_F2_RELEASED);
                break;

            case Qt::Key_F3:
                keyActionManager.handleKeyAction(KEY_F3_RELEASED);
                break;

            case Qt::Key_F4:
                keyActionManager.handleKeyAction(KEY_F4_RELEASED);
                break;

            case Qt::Key_F5:
                keyActionManager.handleKeyAction(KEY_F5_RELEASED);
                break;

            case Qt::Key_F6:
                keyActionManager.handleKeyAction(KEY_F6_RELEASED);
                break;

            case Qt::Key_F7:
                keyActionManager.handleKeyAction(KEY_F7_RELEASED);
                break;

            case Qt::Key_F8:
                keyActionManager.handleKeyAction(KEY_F8_RELEASED);
                break;

            case Qt::Key_F9:
                keyActionManager.handleKeyAction(KEY_F9_RELEASED);
                break;

            default:
                break;
            }
        }
        else if (keyEvent->simpleData.is_press)   // press事件。
        {
            if ((keyEvent->simpleData.keycode >= Qt::Key_F1 && keyEvent->simpleData.keycode <= Qt::Key_F9)
                    || keyEvent->simpleData.keycode == Qt::Key_Return
                    || keyEvent->simpleData.keycode == Qt::Key_Enter)
            {
                // 面板9个按键和飞梭按键播放按键音
                SoundManagerInterface *sound = SoundManagerInterface::getSoundManager();
                if (sound)
                {
                    sound->keyPressTone();
                }
            }
            switch (keyEvent->simpleData.keycode)
            {
            case Qt::Key_F1:
                keyActionManager.handleKeyAction(KEY_F1_PRESSED);
                break;

            case Qt::Key_F2:
                keyActionManager.handleKeyAction(KEY_F2_PRESSED);
                break;

            case Qt::Key_F3:
                keyActionManager.handleKeyAction(KEY_F3_PRESSED);
                break;

            case Qt::Key_F4:
                keyActionManager.handleKeyAction(KEY_F4_PRESSED);
                break;

            case Qt::Key_F5:
                keyActionManager.handleKeyAction(KEY_F5_PRESSED);
                break;

            case Qt::Key_F6:
                keyActionManager.handleKeyAction(KEY_F6_PRESSED);
                break;

            case Qt::Key_F7:
                keyActionManager.handleKeyAction(KEY_F7_PRESSED);
                break;

            case Qt::Key_F8:
                keyActionManager.handleKeyAction(KEY_F8_PRESSED);
                break;

            case Qt::Key_F9:
                keyActionManager.handleKeyAction(KEY_F9_PRESSED);
                break;
            default:
                break;
            }
        }
    }

    return QApplication::qwsEventFilter(e);
}
#endif

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IApplication::IApplication(int &argc, char **argv) : QApplication(argc, argv)
{
    setFont(fontManager.defaultTextFont());

#if defined(Q_WS_QWS)
    // Qt内建立的风格有：windows，plastique，windowsce，windowsmobile，windowsxp，windowsvista，
    // motif，cde，s60，cleanlooks，gtk，gtk+，macintosh，但编译Qt库时没有全部编译进去。
    QApplication::setStyle(QStyleFactory::create("cleanlooks"));
#endif
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IApplication::~IApplication()
{
}
