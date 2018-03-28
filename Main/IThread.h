#pragma once
#include <QThread>
#include <QTimer>

typedef void (*iThreadEntry) (void);
class IThread : public QThread
{
    Q_OBJECT

public:
    // 停止。
    void stop(void);

    // 休眠。
    static void sleep(unsigned long d) { QThread::sleep(d); }
    static void msleep(unsigned long d) { QThread::msleep(d); }
    static void usleep(unsigned long d) { QThread::usleep(d); }

    // 构造与析构。
    IThread(const QString &name, iThreadEntry entry);
    virtual ~IThread();

protected:
    virtual void run(void);
    bool _isStop;
    iThreadEntry _entry;
    QString _name;
};
