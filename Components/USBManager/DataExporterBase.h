#pragma once
#include <QObject>
#include <QMetaType>

class DataExporterBase : public QObject
{
    Q_OBJECT
public:
    enum ExportStatus {
        Success,            //export finished successfully
        Failed,             //export failed
        Cancel,             //cancel by userl
        Disconnect,         //U disk disconnect
        NoSpace,            //U disk has no enough space
    };
    DataExporterBase() : _requestCancel(false){}

signals:
    void progressChanged(unsigned char process);
    void exportFinished(DataExporterBase::ExportStatus staus);
    void exportBegin();

public slots:
    virtual void cancelExport() {_requestCancel = true;}
    virtual void startExport();

protected:
    virtual void exportComplete(DataExporterBase::ExportStatus status);

protected:
    bool _requestCancel;
};

Q_DECLARE_METATYPE(DataExporterBase::ExportStatus);
