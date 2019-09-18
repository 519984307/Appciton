/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/3/7
 **/
#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>

typedef void(*threadEntry)(void);

class MyThread : public QThread
{
public:
    explicit MyThread(threadEntry entry);

    virtual void run();

    void stop();
private:
    threadEntry _threadEntry;
    bool _isStop;
};


#endif  // MYTHREAD_H
