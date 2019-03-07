/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/3/7
 **/

#include "mythread.h"
/*************************************************************************************************
 * 构造
 * **********************************************************************************************/
MyThread::MyThread(threadEntry entry) : QThread(), _threadEntry(entry), _isStop(false)
{
}

/*************************************************************************************************
 * run
 * **********************************************************************************************/
void MyThread::run()
{
    while (1)
    {
        if (NULL != _threadEntry)
        {
            _threadEntry();
        }

        if (_isStop)
        {
            break;
        }

        msleep(500);
    }
}

/*************************************************************************************************
 * stop
 * **********************************************************************************************/
void MyThread::stop()
{
    _isStop = true;
}
