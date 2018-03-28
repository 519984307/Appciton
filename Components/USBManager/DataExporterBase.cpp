#include "DataExporterBase.h"
#include "USBManager.h"

void DataExporterBase::exportComplete(DataExporterBase::ExportStatus status) {
    usbManager.stopWriteUSBDisk();
    emit exportFinished(status);
}

void DataExporterBase::startExport()
{
    usbManager.startWriteUSBDisk();
    emit exportBegin();
}
