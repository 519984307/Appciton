/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/1/21
 **/


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
    static void sleep(unsigned long d);
    static void msleep(unsigned long d);
    static void usleep(unsigned long d);

    // 构造与析构。
    IThread(const QString &name, iThreadEntry entry);
    virtual ~IThread();

protected:
    virtual void run(void);
    bool _isStop;
    iThreadEntry _entry;
    QString _name;
};
