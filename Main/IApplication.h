#pragma once
#include <QApplication>


class IApplication : public QApplication
{
    Q_OBJECT
public:
    IApplication(int &argc, char **argv);
    ~IApplication();

#if defined(Q_WS_X11)
    virtual bool x11EventFilter(XEvent *);
#endif

#if defined(Q_WS_QWS)
    bool qwsEventFilter(QWSEvent *e);
#if defined(CONFIG_CAPTURE_SCREEN)
public slots:
    void handleScreenCaptureResult(long result);
#endif
#endif
};
