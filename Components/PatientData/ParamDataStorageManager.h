/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/9/5
 **/

#pragma once
#include "StorageManager.h"
#include "ParamDataDefine.h"

#define TREND_FLAG_NORMAL   (1)
#define TREND_FLAG_ALARM    (1 << 1)
#define TREND_FLAG_NIBP     (1 << 2)

class ParamDataStorageManagerPrivate;
class ParamDataStorageManager: public StorageManager
{
public:
    struct ParamBuf
    {
        ParamDataItem item;
        ParamItem paramItem[SUB_PARAM_NR];
    };

    static ParamDataStorageManager &construction(void);
    ~ParamDataStorageManager();

    // 主运行函数, perform the 30 seconds intreval storage, multi-thread unsafe
    void mainRun(unsigned t);

    // 添加NIBP数据, called immediately after nibp measurement, multi-thread unsafe
    void addNIBPData(unsigned t);

    // 添加报警时的趋势数据, called after parameter alarm, multi-thread unsafe
    void addAlarmData(unsigned t, ParamID id);

    // 存储函数
    void run();

    // get the last store param data
    void getRecentParamData(ParamBuf &parambuf);    /* NOLINT */

protected:
    /* override */
    void newPatientHandle();

private:
    Q_DECLARE_PRIVATE(ParamDataStorageManager)
    ParamDataStorageManager();

    virtual void createDir();
};

#define paramDataStorageManager (ParamDataStorageManager::construction())
#define deleteParamDataStorageManager() (delete &paramDataStorageManager)   /* NOLINT */
