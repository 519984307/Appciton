/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/6
 **/

#pragma once
#include <QApplication>


class IApplication : public QApplication
{
    Q_OBJECT
public:
    IApplication(int &argc, char **argv);   // NOLINT
    ~IApplication();

#if defined(Q_WS_X11)
    virtual bool x11EventFilter(XEvent *);
#endif

#if defined(Q_WS_QWS)
    bool qwsEventFilter(QWSEvent *e);
#endif

#if defined(CONFIG_CAPTURE_SCREEN)
public slots:
    void handleScreenCaptureResult(const QVariant &result);
private:
    /**
     * @brief handleScreenCaptureKeyEvent handle the screen capture key event
     * @param key current press or release key
     * @param isPressed key is pressed or not
     * @return return true if the key is digested, otherwise, false
     */
    bool handleScreenCaptureKeyEvent(Qt::Key key, bool isPressed);
#endif
};
