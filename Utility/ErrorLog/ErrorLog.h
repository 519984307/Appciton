#pragma once
#include <QScopedPointer>
#include "Utility.h"
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


    //construction
    static ErrorLog &getInstance();
    ~ErrorLog();

    //get current log count;
    int count();

    //get an log at specific index, index zero means the newest
    //Note: User should delete the return object
    ErrorLogItemBase *getLog(int index);

    //append a log, ErrorLog will delete the item on complete.
    void append(ErrorLogItemBase *item);

    //append log item list, ErrorLog will delete the items on complete.
    void append(const QList<ErrorLogItemBase *> &items);

    //clear all the log
    void clear();

    //get the number of specific log type
    int getTypeCount(unsigned int type);
    //get the most resent log of specific type
    ErrorLogItemBase *getLatestLog(unsigned int type);

    //get the error log summary
    Summary getSummary();

    //periodly run to save data
    void run();

protected:
    const QScopedPointer<ErrorLogPrivate> d_ptr;

private:
    ErrorLog();

    Q_DECLARE_PRIVATE(ErrorLog)
    Q_DISABLE_COPY(ErrorLog)
};
#define errorLog (ErrorLog::getInstance())
