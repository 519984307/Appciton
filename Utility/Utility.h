#pragma once
#include <QThread>
#include <QImage>
#include <QString>
#include <QVariant>

namespace Util {
#if defined(CONFIG_CAPTURE_SCREEN) && defined(Q_WS_QWS)
    //capture the screen
    QImage captureScreen();
#endif

    //WorkerThread, use to handle time consuming jobs
    typedef long (*WorkerThreadEntry)(const QVariant &para);
    class WorkerThread : public QThread
    {
        Q_OBJECT
    public:
        WorkerThread(WorkerThreadEntry entry, const QVariant &para)
            :entry(entry), para(para)
        {
            connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
        }

        ~WorkerThread()
        {

        }

        void msleep(unsigned ms)
        {
            QThread::msleep(ms);
        }

    signals:
        void resultReady(long result);

    protected:
        void run()
        {
            if(entry)
            {
                emit resultReady(entry(para));
            }
        }

    private:
        WorkerThreadEntry entry;
        QVariant para;
    };

    //run lenght encode
    bool RunLengthEncode(const char *src, int srclen, char *dest, int *destlen);

    //run lenght decode
    bool RunLengthDecode(const char *src, int srclen, char *dest, int *destlen);

    char *strlcpy(char *dest, const char *src, size_t n);

    //remove directory
    void deleteDir(const QString &path);

    //get the system boot time sync power up, in units of ms
    quint64 systemBootTime();

    //non-block waiting, start an eventloop inside and this function should be invoked in main thread.
    void waitInEventLoop(int ms);

    //set up the system log, route the debug message to /var/log/messages
    void setupSysLog();
}
