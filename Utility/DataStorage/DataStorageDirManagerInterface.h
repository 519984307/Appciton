/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/6
 **/

#pragma once
#include <QObject>

class DataStorageDirManagerInterface : public QObject
{
    Q_OBJECT
public:
    virtual ~DataStorageDirManagerInterface(){}

    /**
     * @brief registerErrorLog register a errorlog object
     * @param interface the new errorLog interface
     * @return the old errorLog interafce
     */
    static DataStorageDirManagerInterface *registerDataStorageDirManager(DataStorageDirManagerInterface *interface);

    /**
     * @brief getErrorLog get the error interface
     * @return the interface or NULL if no any interface
     */
    static DataStorageDirManagerInterface *getDataStorageDirManager();
    /**
     * @brief newPatient 新建病人信号
     */
    void newPatient();
};

