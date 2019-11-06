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
#include <QScopedPointer>
#include "Framework/Utility/Utility.h"
#include "ErrorLogInterface.h"

class ErrorLogItemBase;
class ErrorLogPrivate;
class ErrorLog : public ErrorLogInterface
{
public:
    enum
    {
        MaximumLogNum = 1000
    };


    // construction
    static ErrorLog &getInstance();
    ~ErrorLog();

    /**
     * @brief setStorageDir set the error log storage dir
     * @param dir the destinate directory
     * @return true if successfully, otherwise, false
     */
    bool setStorageDir(const QString &dir);

    // get current log count;
    int count();

    // get an log at specific index, index zero means the newest
    // Note: User should delete the return object
    ErrorLogItemBase *getLog(int index);

    // append a log, ErrorLog will delete the item on complete.
    void append(ErrorLogItemBase *item);

    // append log item list, ErrorLog will delete the items on complete.
    void append(const QList<ErrorLogItemBase *> &items);

    // clear all the log
    void clear();

    // get the number of specific log type
    int getTypeCount(unsigned int type);
    // get the most resent log of specific type
    ErrorLogItemBase *getLatestLog(unsigned int type);

    // get the error log summary
    Summary getSummary();

    // periodly run to save data
    void run();

protected:
    const QScopedPointer<ErrorLogPrivate> d_ptr;

private:
    ErrorLog();

    Q_DECLARE_PRIVATE(ErrorLog)
    Q_DISABLE_COPY(ErrorLog)
};
#define errorLog (ErrorLog::getInstance())
