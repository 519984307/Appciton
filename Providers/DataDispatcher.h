/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/4
 **/

#pragma once
#include <QObject>

class DataDispatcherPrivate;
class Provider;
class DataDispatcher : public QObject
{
    Q_OBJECT
public:
    enum PacketType
    {
        PACKET_TYPE_INVALID = -1,    // invalid packet type
        PACKET_TYPE_CTRL = 0xF0,     // control packet type
        PACKET_TYPE_T5 = 0xF1,       // 温度
        PACKET_TYPE_S5 = 0xF2        // 血氧
    };

    enum PacketPortBaudrate
    {
        BAUDRATE_9600   = 0x10,
        BAUDRATE_19200  = 0x11,
        BAUDRATE_28800  = 0x12,
        BAUDRATE_38400  = 0x13,
        BAUDRATE_57600  = 0x14,
        BAUDRATE_115200 = 0x15,
    };

    struct DispatchInfo
    {
        DispatchInfo(): dispatcher(NULL), packetType(PACKET_TYPE_INVALID) {}
        DataDispatcher *dispatcher;
        PacketType packetType;
    };


public:
    explicit DataDispatcher(const QString &name, QObject *parent = NULL);
    virtual ~DataDispatcher();

    /**
     * @brief getName get the dispatcher name
     * @return  the name
     */
    QString getName() const;

    /**
     * @brief connectProvider connect data dispatcher to the provider
     * @param type
     * @param provider
     */
    void connectProvider(PacketType type, Provider *provider);

    /**
     * @brief sendData send data through the data dispatcher
     * @param buff ther data buffer
     * @param len the buffer length
     * @return the length of data have beed send
     */
    int sendData(PacketType type, const unsigned char *buff, int len);

    /**
     * @brief addDataDispatcher add a data dispatcher to the system
     * @param dispatcher the new added dispatcher
     */
    static void addDataDispatcher(DataDispatcher *dispatcher);

    /**
     * @brief getDataDispatcher get a data dispathcer from the system base on the name
     * @param name the dispatcher name
     * @return  the dispatcher object or null is not exist
     */
    static DataDispatcher *getDataDispatcher(const QString &name);

    /**
     * @brief resetPacketPort reset the specific packet port
     * @param type the port of specific packet type
     * @return true if command send sucessfully, otherwise, false
     */
    bool resetPacketPort(PacketType type);

    /**
     * @brief setPacketPortBaudrate set the specific port baudrate
     * @param type the port packet type
     * @param baud the port baudrate
     * @return true if command send successfully, otherwise, false
     */
    bool setPacketPortBaudrate(PacketType type, PacketPortBaudrate baud);

private slots:
    // 接收数据
    void dataArrived();

private:
    DataDispatcherPrivate *const d_ptr;
};
