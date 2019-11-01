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
#include <QThread>
#include <QString>
#include <QVariant>

class QFile;
class QPixmap;
class QImage;
namespace Util
{
// WorkerThread, use to handle time consuming jobs
typedef QVariant (*WorkerThreadEntry)(const QVariant &para);
class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(WorkerThreadEntry entry, const QVariant &para)
        : entry(entry), para(para)
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
    void resultReady(const QVariant &result);

protected:
    void run()
    {
        if (entry)
        {
            emit resultReady(entry(para));
        }
    }

private:
    WorkerThreadEntry entry;
    QVariant para;
};

// run lenght encode
bool RunLengthEncode(const char *src, int srclen, char *dest, int *destlen);

// run lenght decode
bool RunLengthDecode(const char *src, int srclen, char *dest, int *destlen);

// strlcpy safe version, the dest[n-1] will always be '\0'
char *strlcpy(char *dest, const char *src, size_t n);

// convert a value to a string base on the value and the scale
// if the scale == 1, return a integer string,
// otherwise, return a string contains the decimal part
QString convertToString(int value, int scale);

// remove directory
void deleteDir(const QString &path);

// get the system boot time sync power up, in units of ms
quint64 systemBootTime();

// non-block waiting, start an eventloop inside and this function should be invoked in main thread.
void waitInEventLoop(int ms);

// set up the system log, route the debug message to /var/log/messages
void setupSysLog();

// generate the kernel logo file
bool generateKernelLogo(QFile *logoFile, const QImage &logoImage);

}   // namespace Util
