#pragma once
#include "DataExporterBase.h"

class ErrorLogExporter : public DataExporterBase
{
    Q_OBJECT
public:
    ErrorLogExporter();
    void startExport();

private:
    void updateProgress(unsigned char progress);
    bool exportLog(const QString &filename);
    int _currentProgress;

};
