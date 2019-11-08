/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/5/29
 **/
#include "ErrorLogExporter.h"
#include "Framework/ErrorLog/ErrorLog.h"
#include "Framework/ErrorLog/ErrorLogItem.h"
#include "USBManager.h"
#include "IConfig.h"
#include <QFile>
#include <QDateTime>
#include <QApplication>
#include "Framework/Language/LanguageManager.h"
#include "Framework/Utility/Utility.h"
#include "Debug.h"

ErrorLogExporter::ErrorLogExporter()
    : DataExporterBase(), _currentProgress(0)
{
}

void ErrorLogExporter::startExport()
{
    DataExporterBase::startExport();

    QString exportNamePrefix = usbManager.getUdiskMountPoint();
    exportNamePrefix += "/ErrorLog";
    QString serialNum;
    machineConfig.getStrValue("SerialNumber", serialNum);
    exportNamePrefix += serialNum;
    quint32 identifier = 10;
    QString filename;
    do
    {
        filename = exportNamePrefix + QString::number(identifier, 36).toUpper() + ".log";
        identifier++;
        if (identifier % 36 == 0)
        {
            identifier = (identifier - 1) * 36 + 10;
        }
    }while (QFile::exists(filename));

    if (exportLog(filename))
    {
        exportComplete(Success);
    }
    else
    {
        // request cancel
        if (_requestCancel)
        {
            exportComplete(Cancel);
            return;
        }

        // Fail on usb disconnect
        if (!usbManager.checkStatus())
        {
            exportComplete(Disconnect);
            return;
        }

        int freeSpace = usbManager.getUSBFreeSize();
        if (freeSpace == -1)  // error happen while calculate free space
        {
            // At most case, this error is cause by disconnection
            exportComplete(Disconnect);
            return;
        }
        else if (freeSpace <= 4)  // more than 4KB space left
        {
            // no enough space
            exportComplete(NoSpace);
            return;
        }

        // otherwise, failed
        exportComplete(Failed);
    }
}

void ErrorLogExporter::updateProgress(unsigned char progress)
{
    if (progress == _currentProgress)
    {
        return;
    }

    _currentProgress = progress;
    emit progressChanged(progress);
}

#define TEXT_ENDL "\r\n"
bool ErrorLogExporter::exportLog(const QString &filename)
{
    if (filename.isEmpty() || !usbManager.isUSBExist())
    {
        return false;
    }

    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream stream(&file);
        int index = 0;
        int total = errorLog.count();
        ErrorLog::Summary summary = errorLog.getSummary();
        stream << trs("ErrorLogExportAt") << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << TEXT_ENDL;
        stream << trs("NumberOfErrors") << summary.NumOfErrors << TEXT_ENDL;
        stream << trs("NumberOfCriticalFaults") << summary.numOfCriticalErrors << TEXT_ENDL;
        stream << trs("MostRecentError") << summary.mostRecentErrorDate << TEXT_ENDL;
        stream << trs("MostRecentCriticalFault") << summary.mostRecentCriticalErrorDate << TEXT_ENDL;
        stream << trs("OldestError") << summary.oldestErrorDate << TEXT_ENDL;
        unsigned int num = 0;
        systemConfig.getNumValue("ErrorLogEraseTime", num);
        if (num > 0)
        {
            stream << trs("LastEraseTime") << QDateTime::fromTime_t(num).toString("yyyy-MM-dd HH:mm:ss")
                   << TEXT_ENDL;
        }
        stream << TEXT_ENDL;

        while (index < total)
        {
            ErrorLogItemBase *item = NULL;
            updateProgress(100 * (index + 1) / total);

            stream << "Log(" << index + 1 << "/" << total << ")" << TEXT_ENDL;
            item = errorLog.getLog(index);
            if (item)
            {
                item->outputInfo(&stream);
                stream << TEXT_ENDL;
                delete item;
            }
            else
            {
                qdebug("get log index %d failed", index);
            }
            index++;

            Util::waitInEventLoop(1);

            if (_requestCancel)
            {
                file.remove();
                return false;
            }
            else if (stream.status() == QTextStream::WriteFailed)
            {
                qdebug("Write %s Failed!", qPrintable(filename));
                file.remove();
                return false;
            }
            else if (!usbManager.isUSBExist())
            {
                file.remove();
                return false;
            }
        }

        stream.flush();
        file.close();
        if (stream.status() == QTextStream::WriteFailed)
        {
            qdebug("Write %s Failed!", qPrintable(filename));
            file.remove();
            return false;
        }
        else
        {
            // the log is empty
            if (total == 0)
            {
                updateProgress(100);
            }

            return true;
        }
    }
    else
    {
        return false;
    }
}
