#pragma once
#include "ErrorLogItem.h"

class ErrorLogInterface
{
public:
    struct Summary {
        int NumOfErrors;                        //number of errors
        int numOfCriticalErrors;                //number of critical errors
        QString mostRecentErrorDate;            //most recent error
        QString mostRecentCriticalErrorDate;    //most recent cirtical error
        QString oldestErrorDate;                //oldest error
        QString lastEraseTimeDate;              //last erase time
        int totalShockCount;                    //total shock count
    };

    virtual ~ErrorLogInterface() {}

    /**
     * @brief count get the curren number of log
     * @return
     */
    virtual int count() = 0;

    /**
     * @brief getLog get an log at specifcic, index zero means the newest
     * @param index
     * @note the the log item should delete by user
     * @return the log item or NULL
     */
    virtual ErrorLogItemBase *getLog(int index) = 0;

    /**
     * @brief append append a log item, the item will be take over by the logger
     * @param item
     */
    virtual void  append(ErrorLogItemBase *item) = 0;

    /**
     * @brief clear clear all the log
     */
    virtual void clear() = 0 ;

    /**
     * @brief getTypeCount get log item number of specific type
     * @param type
     */
    virtual int getTypeCount(unsigned int type)  = 0;

    /**
     * @brief getLatestLog get the latest log of specific type
     * @param type
     * @return
     */
    virtual ErrorLogItemBase *getLatestLog(unsigned int type) = 0;

    /**
     * @brief getSummary get the error log summary
     * @return
     */
    virtual Summary getSummary() = 0;

    /**
     * @brief registerErrorLog register a errorlog object
     * @param interface the new errorLog interface
     * @return the old errorLog interafce
     */
    static ErrorLogInterface *registerErrorLog(ErrorLogInterface *interface);

    /**
     * @brief getErrorLog get the error interface
     * @return the interface or NULL if no any interface
     */
    static ErrorLogInterface *getErrorLog();
};
