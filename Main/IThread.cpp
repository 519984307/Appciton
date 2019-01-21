/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/1/21
 **/

#include "IThread.h"

namespace IUnistd
{
#include "unistd.h"
}

/**************************************************************************************************
 * 功能： 主运行入口。
 *************************************************************************************************/
void IThread::run()
{
    while (1)
    {
        if (_entry != NULL)
        {
            _entry();
        }
        else
        {
            msleep(5000);
        }

        if (_isStop)
        {
            break;
        }
    }
}
#include "Debug.h"
/**************************************************************************************************
 * 功能： 退出。
 *************************************************************************************************/
void IThread::stop(void)
{
    _isStop = true;
debug("%s enter wait\n", qPrintable(_name));
    wait();
    debug("%s exit wait\n", qPrintable(_name));
}

void IThread::sleep(unsigned long d)
{
    using IUnistd::sleep;
    IUnistd::sleep(d);
}

void IThread::msleep(unsigned long d)
{
    using IUnistd::usleep;
    IUnistd::usleep(d * 1000);
}

void IThread::usleep(unsigned long d)
{
    using IUnistd::usleep;
    IUnistd::usleep(d);
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
IThread::IThread(const QString &name, iThreadEntry entry)
{
    _name = name;
    _entry = entry;
    _isStop = false;
}

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
IThread::~IThread()
{
}
