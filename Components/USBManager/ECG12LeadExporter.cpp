#include "ECG12LeadExporter.h"
#include <QDateTime>
#include <USBManager.h>
#include <QFile>
#include "Debug.h"
#include <QDir>

ECG12LeadExporter::ECG12LeadExporter(ECG12LeadFileBuilderBase *fileBuilder)
    :DataExporterBase(), _fileBuilder(fileBuilder), _curStatus(Success)
{
    if(_fileBuilder)
    {
        _fileBuilder->setParent(this);
        connect(_fileBuilder, SIGNAL(oneFileComplete(QString,QByteArray)), this, SLOT(onFileBuild(QString,QByteArray)));
        connect(_fileBuilder, SIGNAL(buildFinished(bool)), this, SLOT(onFileBuildFinished(bool)));
    }
}

void ECG12LeadExporter::startExport()
{
    if(_fileBuilder == NULL || !usbManager.isUSBExist())
    {
        exportComplete(Failed);
    }
    else
    {
        DataExporterBase::startExport();
        _fileBuilder->startBuilding();
    }
}

/***************************************************************************************************
 * Note: The build process contain 2 phases, the first phase is build, sencond phase is write file to
 *       usb disk.
 **************************************************************************************************/
void ECG12LeadExporter::onFileBuild(const QString &name, const QByteArray &content)
{
    //save file to usb

    if(!usbManager.isUSBExist())
    {
        _curStatus = Disconnect;
        _fileBuilder->cancelBuilding();
        return;
    }

    if(_requestCancel)
    {
        _curStatus = Cancel;
        _fileBuilder->cancelBuilding();
        return;
    }

    QString filepath = name;
    int index = name.lastIndexOf("/");
    QString dirStr = filepath.left(index);
    QDir dir(dirStr);

    if (!dir.exists())
    {
        dir.mkpath(dirStr);
    }

    //content is build
    updateProgress(100 * (2 * _fileBuilder->filesProcess() - 1) / (2 * _fileBuilder->filesToBuild()));

    bool failed = false;

    QFile file(filepath);
    if(file.open(QIODevice::WriteOnly))
    {
        if(file.write(content) == content.length())
        {
            file.flush();
        }
        else
        {
            failed = true;
        }
    }
    else
    {
        failed = true;
        qDebug()<<"Open file Failed"<<file.errorString();
    }

    //check failure
    if(failed)
    {
        //Fail on usb disconnect
        if(!usbManager.checkStatus())
        {
            _curStatus = Disconnect;
            _fileBuilder->cancelBuilding();
        }

        int freeSpace = usbManager.getUSBFreeSize();
        if(freeSpace == -1) //error happen while calculate free space
        {
            //At most case, this error is cause by disconnection
            _curStatus = Disconnect;
            _fileBuilder->cancelBuilding();
        }
        else if(freeSpace <= 4) //more than 4KB space left
        {
            //no enough space
            _curStatus = NoSpace;
            _fileBuilder->cancelBuilding();
        }

        //other status, continue process
    }
    else
    {
        //file is export
        updateProgress(100 * _fileBuilder->filesProcess()  /  _fileBuilder->filesToBuild());

    }
}

void ECG12LeadExporter::onFileBuildFinished(bool transfer)
{
    if(transfer)
    {
        //build file complete
        if(_curStatus == Success)
        {
            updateProgress(100);
        }
        exportComplete(_curStatus);
    }
    else
    {
        if (_requestCancel)
        {
            exportComplete(Cancel);
        }
        else
        {
            exportComplete(_curStatus);
        }
    }
}

void ECG12LeadExporter::cancelExport()
{
    DataExporterBase::cancelExport();
    _fileBuilder->cancelBuilding();

    return;
}

void ECG12LeadExporter::updateProgress(unsigned char progress)
{
    if (!_requestCancel)
    {
        emit progressChanged(progress);
    }
}
