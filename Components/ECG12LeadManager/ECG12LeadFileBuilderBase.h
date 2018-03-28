#pragma once
#include <QObject>
#include <QByteArray>

class ECG12LeadFileBuilderBase : public QObject
{
    Q_OBJECT
public:
    enum Type {
        PDF,
        JSON,
    };
    ECG12LeadFileBuilderBase() : _requestCancel(false){}

    virtual Type type() const = 0;
    //get the number of files to build
    virtual int filesToBuild() {return 0;}
    //get the number of files that has been processed
    virtual int filesProcess() {return 0;}

signals:
    //building complete signal, emit event build failed
    void buildFinished(bool transfer);
    //one file complete signal
    void oneFileComplete(const QString &name, const QByteArray &fileData);

public slots:
    virtual void cancelBuilding() { _requestCancel = true;}
    virtual void startBuilding() = 0;

protected:
    bool _requestCancel;
};
