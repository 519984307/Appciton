/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/5
 **/

#pragma once
#include <QObject>
#include <QMutex>

class QSocketNotifier;

enum FlowControl
{
    FlOW_CTRL_NONE,
    FlOW_CTRL_HARD,
    FlOW_CTRL_SOFT
};
//串口属性定义
struct UartAttrDesc
{
    UartAttrDesc(int b = 9600, char d = 8, char p = 'N', char s = 1,
                 unsigned char v = 0, FlowControl f = FlOW_CTRL_NONE, bool nonblk = true)
        :baud(b), dataLen(d), stopLen(s), parity(p), vmin(v), flowCtrl(f), nonBlocking(nonblk)
    {
    }

    int baud;      // 波特率。
    char dataLen;  // 数据长度。
    char stopLen;  // 停止位长度。
    char parity;   // 奇偶校验, 'O'奇校验，‘E’偶校验，‘N’无校验。
    unsigned char vmin;    // 数据端口有效的最小数据个数。
    FlowControl flowCtrl;  // 数据流控制
    bool nonBlocking;      // is non-blocking or not
};

class Uart : public QObject
{
    Q_OBJECT

public:
    // 初始化数据端口，派生类调用。
    virtual bool initPort(const QString &port, const UartAttrDesc &desc, bool needNotify = true);

    /**
     * @brief updateBaudrate upgrade the uart port setting
     * @param baudrate the new baudrate
     */
    void updateSetting(const UartAttrDesc &desc);

    // 写数据。
    virtual int write(const unsigned char buff[], int len);

    // 读数据。
    virtual int read(unsigned char *buff, int nbytes);

    // wait until all output data is send
    virtual void sync();

    // get the file descriptor
    virtual int fd()
    {
        return _fd;
    }

    //关闭串口
    virtual void closePort(void);

    explicit Uart(QObject *parent = NULL);
    virtual ~Uart();

signals:
    void activated(int socket);

protected:
    QString _port;
    int _fd;  //串口的文件描述符
    QSocketNotifier *_notifier;

private:
    QMutex _mutex;
};
