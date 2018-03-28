#pragma once
#include <QScopedPointer>
#include "StorageManager.h"
#include "Utility.h"

class ErrorLogItemBase;
class ErrorLogPrivate;
class ErrorLog
{
public:
    enum
    {
        MaximumLogNum = 1000
    };

    struct Summary {
        int NumOfErrors;                     //number of errors
        int numOfCriticalErrors;            //number of critical errors
        QString mostRecentErrorDate;           //most recent error
        QString mostRecentCriticalErrorDate;  //most recent cirtical error
        QString oldestErrorDate;               //oldest error
        QString lastEraseTimeDate;             //last erase time
        int totalShockCount;                //total shock count
    };

    //construction
    static ErrorLog &construction()
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ErrorLog;
        }
        return *_selfObj;
    }
    static ErrorLog *_selfObj;
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
#define errorLog (ErrorLog::construction())
#define deleteErrorLog() (delete ErrorLog::_selfObj)
