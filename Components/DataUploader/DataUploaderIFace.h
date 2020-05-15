/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/12/19
 **/

#pragma once


class QHostAddress;
class DataUploaderIface
{
public:
    ~DataUploaderIface() {}

    /**
     * @brief connectToServer connect to server
     * @param addr the server address
     * @param port the server port
     */
    virtual void connectToServer(const QHostAddress &addr, unsigned short port) = 0;

    /**
     * @brief disconnectFromServer disconnect from the remote server
     */
    virtual void disconnectFromServer() = 0;

    /**
     * @brief isConnectedToServer check whether the uploader is connected to the remote server
     * @return true is connected to server, otherwise, false
     */
    virtual bool isConnectedToServer() const = 0;
};

