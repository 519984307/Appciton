#pragma once
#include "DataExporterBase.h"
#include <QStringList>
#include "ECG12LeadFileBuilderBase.h"

class ECG12LeadExporter : public DataExporterBase
{
    Q_OBJECT
public:
    ECG12LeadExporter(ECG12LeadFileBuilderBase *fileBuilder);
    void startExport();

public slots:
    void cancelExport();

private slots:
    void onFileBuild(const QString &name, const QByteArray &content);
    void onFileBuildFinished(bool transfer);

private:
    void updateProgress(unsigned char progress);
    ECG12LeadFileBuilderBase *_fileBuilder;
    ExportStatus _curStatus;
};
