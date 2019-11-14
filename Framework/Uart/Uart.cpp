/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/5
 **/

#include "Uart.h"
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <QSocketNotifier>
#include <QDebug>

/**************************************************************************************************
 * 功能： 转换波特率
 * 参数：
 *      baud：波特率；
 * 返回：
 *      设置需要的宏定义。
 *************************************************************************************************/
static int _getBaud(int baud)
{
    int ret = B9600;

    switch (baud)
    {
    case 4800:
        ret = B4800;
        break;
    case 9600:
        ret = B9600;
        break;
    case 19200:
        ret = B19200;
        break;
    case 38400:
        ret = B38400;
        break;
    case 57600:
        ret = B57600;
        break;
    case 115200:
        ret = B115200;
        break;
    case 230400:
        ret = B230400;
        break;
    case 460800:
        ret = B460800;
        break;
    case 500000:
        ret = B500000;
        break;
    case 576000:
        ret = B576000;
        break;
    case 921600:
        ret = B921600;
        break;
    case 1000000:
        ret = B1000000;
        break;
    case 1152000:
        ret = B1152000;
        break;
    case 1500000:
        ret = B1500000;
        break;
    case 2000000:
        ret = B2000000;
        break;
    case 2500000:
        ret = B2500000;
        break;
    case 3000000:
        ret = B3000000;
        break;
    case 3500000:
        ret = B3500000;
        break;
    case 4000000:
        ret = B4000000;
        break;
    default:
        ret = B9600;
        break;
    }

    return ret;
}

/**************************************************************************************************
 * 功能： 转换数据长度
 * 参数：
 *      len：数据长度；
 * 返回：
 *      设置需要的宏定义。
 *************************************************************************************************/
static int _getDataLen(int len)
{
    int ret = CS8;

    switch (len)
    {
    case 5:
        ret = CS5;
        break;
    case 6:
        ret = CS6;
        break;
    case 7:
        ret = CS7;
        break;
    case 8:
        ret = CS8;
        break;
    default:
        ret = CS8;
        break;
    }

    return ret;
}

/**************************************************************************************************
 * 功能： 设置端口的属性。
 * 参数：
 *      fd：文件描述符；
 *      attr：属性。
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
static bool _settingFD(int fd, const UartAttrDesc &attr)
{
    int ret;
    // 定义终端端口属性变量
    struct termios term;
    // 读取fd的属性状态到term
    ret = tcgetattr(fd, &term);
    if (ret != 0)
    {
        close(fd);
        qDebug("tcgetattr failed.");
        return false;
    }

    // Raw mode
    //将终端设置为原始状态
    cfmakeraw(&term);

    // Ignore modem control lines and Enable receiver
    //设置终端控制模式
    term.c_cflag |= (CLOCAL | CREAD);

    // Vmin
    term.c_cc[VMIN] = attr.vmin;
    term.c_cc[VTIME] = 0;

    // Setting baud rate
    ret = cfsetspeed(&term, _getBaud(attr.baud));
    if (ret != 0)
    {
        close(fd);
        qDebug("cfsetspeed failed");
        return false;
    }

    // Seting data length
    term.c_cflag &= ~CSIZE;
    term.c_cflag |= _getDataLen(attr.dataLen);

    // Setting length of stop bit
    switch (attr.stopLen)
    {
    case 1:
        term.c_cflag &= ~CSTOPB;
        break;
    case 2:
        term.c_cflag |= CSTOPB;
        break;
    default:
        break;
    }

    // Setting Parity
    term.c_iflag &= ~(PARMRK | INPCK);
    term.c_iflag |= IGNPAR;
    switch (attr.parity)
    {
    case 'o':
    case 'O':  // Odd
        term.c_cflag |= (PARENB | PARODD);
        break;
    case 'e':
    case 'E':  // Even
        term.c_cflag &= ~PARODD;
        term.c_cflag |= PARENB;
        break;
    case 'n':
    case 'N':  // None
        term.c_cflag &= ~PARENB;
        break;
    default:
        break;
    }

    // Flow control
    switch (attr.flowCtrl)
    {
    case FlOW_CTRL_HARD:
        term.c_cflag |= CRTSCTS;
        term.c_iflag &= ~(IXON | IXOFF | IXANY);
        break;
    case FlOW_CTRL_SOFT:
        term.c_cflag &= ~CRTSCTS;
        term.c_iflag |= IXON | IXOFF | IXANY;
        break;
    case FlOW_CTRL_NONE:
    default:
        term.c_cflag &= ~CRTSCTS;
        term.c_iflag &= ~(IXON | IXOFF | IXANY);
        break;
    }

    // Flush data
    ret = tcflush(fd, TCIOFLUSH);
    if (ret != 0)
    {
        close(fd);
        qDebug("tcflush failed");
        return false;
    }

    // Changes take effect immediately
    ret = tcsetattr(fd, TCSANOW, &term);
    if (ret != 0)
    {
        close(fd);
        qDebug("tcsetattr failed");
        return false;
    }

    return true;
}


#define FAIL_RETRY_LIMIT 100
/**************************************************************************************************
 * 功能： 向端口写数据。
 * 参数：
 *      buff： 数据。
 *      len： 数据长度。
 * 返回：
 *      实际发送的数据长度。
 *************************************************************************************************/
int Uart::write(const unsigned char buff[], int len)
{
    if ((_fd < 0) || (buff == NULL))
    {
        return 0;
    }

    QMutexLocker locker(&_mutex);
    int remain = len;
    int retry = 0;
    const unsigned char *current = buff;

    // 确保数据能够全部写往端口。
    while (remain > 0)
    {
        int ret = TEMP_FAILURE_RETRY(::write(_fd, current, remain));
        if (ret == -1)
        {
            if (errno == EAGAIN)
            {
                if (++retry <= FAIL_RETRY_LIMIT)
                {
                    continue;
                }
            }

            QString errorStr = strerror(errno);
            qDebug() << _port << errorStr;
            return (len - remain);
        }
        ::fsync(_fd);
        current += ret;
        remain -= ret;
        retry = 0;
    }
    return len;
}

/**************************************************************************************************
 * 功能： 读数据。
 * 参数：
 *      buf：带回数据；
 *      nbytes：读取的数据个数。
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
int Uart::read(unsigned char *buff, int nbytes)
{
    if (NULL == buff)
    {
        return 0;
    }

    return ::read(_fd, buff, nbytes);
}

/***************************************************************************************************
 * waits untill all output is send
 **************************************************************************************************/
void Uart::sync()
{
    ::tcdrain(_fd);
}

/**************************************************************************************************
 * 功能： 关闭串口。
 *************************************************************************************************/
void Uart::closePort(void)
{
    if (_notifier)
    {
        delete _notifier;
    }

    if (_fd != -1)
    {
        close(_fd);
    }
}

/**************************************************************************************************
 * 功能： 初始化数据端口。
 * 参数：
 *      port：端口地址；
 *      desc：端口的设置参数。
 * 返回：
 *      true，成功；false，失败。
 *************************************************************************************************/
bool Uart::initPort(const QString &port, const UartAttrDesc &desc, bool needNotify)
{
    _port = port;
    int flags = O_RDWR | O_NOCTTY | O_CLOEXEC;
    if (desc.nonBlocking)
    {
        flags |= O_NONBLOCK;
    }

    if ((_fd = open(qPrintable(port), flags)) == -1)
    {
        qDebug("open %s failed.", qPrintable(port));
        return false;
    }

    //设置终端串口的属性，例如波特率、停止位、优先级等
    if (!_settingFD(_fd, desc))
    {
        qDebug("%s set attribute failed", qPrintable(port));
        close(_fd);
        _fd = -1;
        return false;
    }

    if (needNotify)
    {
        _notifier = new QSocketNotifier(_fd, QSocketNotifier::Read);
        connect(_notifier, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
        _notifier->setEnabled(true);
    }

    return true;
}

void Uart::updateSetting(const UartAttrDesc &desc)
{
    if (_fd != -1)
    {
        _settingFD(_fd, desc);
    }
}

/**************************************************************************************************
 * 功能： 构造。
 *************************************************************************************************/
Uart::Uart(QObject *parent)
    : QObject(parent)
{
    _fd = -1;
    _notifier = NULL;
}

/**************************************************************************************************
 * 功能： 析构。
 *************************************************************************************************/
Uart::~Uart()
{
    if (_notifier)
    {
        delete _notifier;
    }

    if (_fd != -1)
    {
        tcflush(_fd, TCIOFLUSH);
        close(_fd);
    }
}
