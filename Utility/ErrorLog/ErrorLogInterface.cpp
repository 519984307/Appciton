#include "ErrorLogInterface.h"

static ErrorLogInterface *currentIface= NULL;
ErrorLogInterface *ErrorLogInterface::registerErrorLog(ErrorLogInterface *interface)
{
    ErrorLogInterface *old = currentIface;
    currentIface = interface;
    return old;
}

ErrorLogInterface *ErrorLogInterface::getErrorLog()
{
    return currentIface;
}
