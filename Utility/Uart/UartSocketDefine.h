/**
 ** This file is part of the SocketUart project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/9/26
 **/

#ifndef UARTSOCKETDEFINE_H
#define UARTSOCKETDEFINE_H

/*
 *  Data frame is in follow format:
 *  +-------------+---------------+------------+--------------+------------+
 *  | Frame Head  |  Frame Length | Frame Type | Data payload | CheckSum   |
 *  |             |               |            |              |            |
 *  |   1 Byte    |    2 byte     |   1 byte   |  0~N bytes   |  1 btye    |
 *  +-------------+---------------+------------+--------------+------------+
 *  Note:
 *  1. The frame head is 0x01.
 *  2. The checksum is the CRC8 value from Frame Head to the end of Data payload
 *  3. If 0x01 located at the fields except Frame Head, the 0x01 will send twice
 */

#define UARTSOCKET_FRAME_HEAD   0x01
#define UARTSOCKET_MAX_FRAME_LEN    37
#define UARTSOCKET_MIN_FRAME_LEN    5

typedef enum {
    /*
     * type : open port command
     * Data payload:
     *  +------------------+
     *  | uart device path |
     *  |     32 bytes     |
     *  +------------------+
     */
    UARTSOCKET_CTRL_CMD_OPEN_PORT = 0x80,
    /*
     * type : respone of open port command
     * Data payload:
     *  +--------------------------------------+
     *  |     Result    | TCP port of the uart |
     *  |     1 byte    |                      |
     *  | 1 for success |        2 bytes       |
     *  |  0 for fail   |                      |
     *  +--------------------------------------+
     */
    UARTSOCKET_CTRL_RSP_OPEN_PORT = 0x81,   /* response of open port */
    /*
     * type : configurate uart port command
     * Data payload:
     * +-------------+----------+---------+------------+------------+--------+--------------------------+
     * | uart server | baudrate | datalen |   parity   | stop bytes |  vmin  |       flow contorl       |
     * |     port    | 4 bytes  |  1 byte |   1 btye   |  1 btye    | 1 byte |          1 byte          |
     * |   2 bytes   |          |   8/9   | 'O'/'E'/'N'|  1 / 2     | 1~255  | 0/1/2 for None/Hard/Soft |
     * +-------------+----------+---------+------------+------------+--------+--------------------------+
     *
     */
    UARTSOCKET_CTRL_CMD_CONF_PORT = 0x82,   /* config port command */
    /*
     * type : response of configurate uart port
     * Data payload:
     * +---------------+
     * |    Result     |
     * |    1 byte     |
     * | 1 for success |
     * |  0 for fail   |
     * +---------------+
     */
    UARTSOCKET_CTRL_RSP_CONF_PORT = 0x83,   /* response of config port */
} UartSocketFrameType;


#endif  // UARTSOCKETDEFINE_H
