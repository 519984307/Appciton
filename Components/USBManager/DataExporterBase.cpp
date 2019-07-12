/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/7/12
 **/

#include "DataExporterBase.h"
#include "USBManager.h"

void DataExporterBase::exportComplete(DataExporterBase::ExportStatus status) {
    if (status == NoSpace)
    {
        usbManager.stopWriteUSBDisk();
    }
    emit exportFinished(status);
}

void DataExporterBase::startExport()
{
    usbManager.startWriteUSBDisk();
    emit exportBegin();
}
