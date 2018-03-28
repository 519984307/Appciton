#include <QApplication>
#include "ECG12LeadManager.h"
#include "ECG12LDataAcquire.h"
#include "ECG12LDataPrint.h"
#include "ECG12LDataMessNote.h"
#include "EmailAddressLayout.h"
#include "WindowManager.h"
#include "MenuManager.h"
#include "ECG12LSnapShotReview.h"
#include "IMessageBox.h"
#include "TimeDate.h"
#include "WaveformCache.h"
#include "DataStorageDirManager.h"
#include "IConfig.h"
#include "NetworkManager.h"
#include "MailManager.h"
#include <QScopedPointer>
#include "USBManager.h"
#include "ECG12LeadPdfBuilder.h"
#include "Utility.h"
#include "ECGAlarm.h"

ECG12LeadManager *ECG12LeadManager::_selfObj = NULL;

/**************************************************************************************************
 * 构造
 *************************************************************************************************/
ECG12LeadManager::ECG12LeadManager():QObject()
    , _exportUsbWidget(NULL), _exportWifiWidget(NULL), _transferType(USB_RESCUE),
    _isTriggerTransfer(false), _isTransfer(false), _data(NULL)
{
    _exportUsbWidget = new ExportDataWidget(EXPORT_ECG12LEAD_DATA_BY_USB);
    _exportWifiWidget = new ExportDataWidget(EXPORT_ECG12LEAD_DATA_BY_WIFI);

    connect(&ecg12LDataAcquire, SIGNAL(acquire12LDataFinish()), this, SLOT(acquire12LDataProcess()));
    connect(&ecg12LDataAcquire, SIGNAL(acquire12LDataTransfer(int)), this, SLOT(acquireDataTransfer(int)));
    connect(&ecg12LDataAcquire, SIGNAL(acquire12LDataPrint()), this, SLOT(acquireDataPrint()));

    connect(&ecg12LSnapShotReview, SIGNAL(snapShotReviewPrint(int)), this, SLOT(snapShotprint(int)));
    connect(&ecg12LSnapShotReview, SIGNAL(snapShotReviewTransfer(int)), this, SLOT(snapShotTransfer(int)));

    connect(&mailAddressWidget, SIGNAL(startToSend()), this, SLOT(startToSendEmail()));

    //USB
    connect(_exportUsbWidget, SIGNAL(cancel()), &usbManager, SLOT(cancelExport()));
    connect(_exportUsbWidget, SIGNAL(cancel()), this, SLOT(transfercancel()));
    connect(&usbManager, SIGNAL(exportProcessChanged(unsigned char)), _exportUsbWidget, SLOT(setBarValue(unsigned char)));

    //WIFI
    connect(_exportWifiWidget, SIGNAL(cancel()), &mailManager, SLOT(cancelExport()));
    connect(_exportWifiWidget, SIGNAL(cancel()), this, SLOT(transfercancel()));
    connect(&mailManager, SIGNAL(exportProcessChanged(unsigned char)), _exportWifiWidget, SLOT(setBarValue(unsigned char)));
}

/**************************************************************************************************
 * 析构
 *************************************************************************************************/
ECG12LeadManager::~ECG12LeadManager()
{
    if (NULL != _data)
    {
        delete _data;
        _data = NULL;
    }

    if (_exportUsbWidget)
    {
        _exportUsbWidget->deleteLater();
        _exportUsbWidget = NULL;
    }

    if (_exportWifiWidget)
    {
        _exportWifiWidget->deleteLater();
        _exportWifiWidget = NULL;
    }
}

/**************************************************************************************************
 * 开始捕获
 *************************************************************************************************/
void ECG12LeadManager::startCapture()
{
    if (ecg12LDataPrint.isPrint() || ecg12LeadManager.isTransfer())
    {
        return;
    }

    bool isacquireVisible = ecg12LDataAcquire.isVisible();
    int barValue = ecg12LDataAcquire.getBarValue();

    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if(isacquireVisible)
    {
        if (barValue < 100)
        {
            ecg12LDataMessNote.showMessage(DATA_ACQUIRE_CANCEL);
        }
    }
    else
    {
        ecg12LDataAcquire.dataInit();
        ecg12LDataAcquire.autoShow();
    }

    return;
}

/**************************************************************************************************
 * 传输快照。
 *************************************************************************************************/
void ECG12LeadManager::acquireDataTransfer(int type)
{
    if(ECG12Lead_TYPE_WIFI == (TransferTypeECG12Lead)type)
    {
        ecg12LExportDataWidget.setCurType(EXPORT_ECG12LEAD_DATA_BY_WIFI);

        setTransferType(EMAIL_SELECT);
        setIsTriggerTransfer(true);
        showEmailList();
    }
    else if(ECG12Lead_TYPE_USB == (TransferTypeECG12Lead)type)
    {
        if(!UDiskInspector::checkUsbConnectStatus())
        {
            //警告没插U盘
            IMessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
            messageBox.exec();
            return ;
        }

        setTransferFlag(true);
        _exportUsbWidget->init();
        _exportUsbWidget->setVisible(true);
        while(!isExportProcessFinish())
        {
            if (_exportUsbWidget->isTransferCancel())
            {
                return;
            }

            Util::waitInEventLoop(200);
        }

        int fileFormat = ECG_TRANSFER_FILE_FORMAT_PDF;
        superConfig.getNumValue("ECG12L|TransmissionFormat", fileFormat);
        ECG12LeadFileBuilderBase *builder = NULL;
        if(fileFormat == ECG_TRANSFER_FILE_FORMAT_PDF)
        {
            builder = new ECG12LeadPdfBuilder(_data);
        }

        connect(&usbManager, SIGNAL(usbExportFileFinish(int)), this, SLOT(usbExportFileFinishPrecess(int)), Qt::UniqueConnection);

        //the ownership of the builder will pass to the usbmanager,
        //usb manager will deleet it when export finish
        if(usbManager.export12LeadData(builder))
        {
            setTransferType(USB_SELECT);
        }

    }

    return;
}

/**************************************************************************************************
 * 打印快照。
 *************************************************************************************************/
void ECG12LeadManager::acquireDataPrint()
{
    ecg12LDataPrint.setPrintRealTime(true);
    ecg12LDataPrint.PrintECG12LData(_data);

    return ;
}

/**************************************************************************************************
 * 12导快照回顾
 *************************************************************************************************/
void ECG12LeadManager::startReview()
{
    if (ecg12LDataAcquire.isAcquiring() || ecg12LDataPrint.isPrint() || ecg12LeadManager.isTransfer())
    {
        return;
    }

    bool isReviewVisible = ecg12LSnapShotReview.isVisible();

    while (NULL != QApplication::activeModalWidget())
    {
        QApplication::activeModalWidget()->hide();
    }

    if (!isReviewVisible)
    {
        ecg12LSnapShotReview.dataInit();

        QRect r = windowManager.getMenuArea();
        int x = r.x() + (r.width() - menuManager.width()) / 2;
        int y = r.y() + (r.height() - menuManager.height());
        ecg12LSnapShotReview.autoShow(x, y);
    }

    return;
}

/**************************************************************************************************
 * 获取选择的营救事件路径
 *************************************************************************************************/
void ECG12LeadManager::getPrintSnapShotPath(QStringList &list)
{
    list.clear();

    ECG12LeadDataPrintType type = ecg12LSnapShotReview.getPrintType();

    switch (type)
    {
        case PRINT_12L_SELECT_RESCUE:
        case PRINT_12L_SELECT:
        {
            int index = ecg12LSnapShotReview.getIncidentIndex();
#ifdef CONFIG_UNIT_TEST
            index = 0;
#endif
            list << dataStorageDirManager.getRescueDataDir(index);

            break;
        }

        default:
            break;
    }

    return ;
}

/**************************************************************************************************
 * 传送快照
 *************************************************************************************************/
void ECG12LeadManager::snapShotTransfer(int type)
{
    QVector<bool> flag;
    flag = ecg12LSnapShotReview.getSnapShotCheckFlag();
    if((0 >= flag.count()) && (EMAIL_SELECT >= type))
    {
        //no snapshots
        QString title = trs("SelectSnapshotsToTransfer");
        QString msg = trs("NoSnapshotInCurrentIncident");
        IMessageBox messageBox(title, msg, QStringList(trs("EnglishYESChineseSURE")));
        messageBox.autoShow(180,200);
        messageBox.exec();
        return;
    }

    if(type == USB_SELECT || type == USB_RESCUE)
    {
        if(!usbManager.isUSBExist())
        {
            //警告没插U盘
            IMessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
            messageBox.exec();
            return;
        }

        QVector<int> snapshotIndexList;
        if(type == USB_SELECT)
        {
            if(!ecg12LSnapShotReview._isSelectSnapShot())
            {
                //no select snapshots
                QString title = trs("SelectSnapshotsToTransfer");
                QString msg = trs("PleaseseletSnapShot");
                IMessageBox messageBox(title, msg, QStringList(trs("EnglishYESChineseSURE")));
                messageBox.autoShow(180,200);
                messageBox.exec();
                return;
            }

            //find the select snapshot index
            for(int i = 0; i < flag.count(); i++)
            {
                if(flag[i])
                {
                    snapshotIndexList.append(i);
                }
            }
        }

        setTransferFlag(true);
        _exportUsbWidget->init();
        _exportUsbWidget->setVisible(true);
        while(!isExportProcessFinish())
        {
            if (_exportUsbWidget->isTransferCancel())
            {
                return;
            }

            Util::waitInEventLoop(200);
        }

        int index = ecg12LSnapShotReview.getIncidentIndex();
#ifdef CONFIG_UNIT_TEST
        index = 0;
#endif
        QString incidentDir = dataStorageDirManager.getRescueDataDir(index);
        int fileFormat = ECG_TRANSFER_FILE_FORMAT_PDF;
        superConfig.getNumValue("ECG12L|TransmissionFormat", fileFormat);

        ECG12LeadFileBuilderBase *builder = NULL;
        if(fileFormat == ECG_TRANSFER_FILE_FORMAT_PDF)
        {
            builder = new ECG12LeadPdfBuilder(incidentDir, snapshotIndexList);
        }

        connect(&usbManager, SIGNAL(usbExportFileFinish(int)), this, SLOT(usbExportFileFinishPrecess(int)), Qt::UniqueConnection);

        //the ownership of the builder will pass to the usbmanager,
        //usb manager will deleet it when export finish
        if(usbManager.export12LeadData(builder))
        {
            setTransferType(USB_SELECT);
        }
    }
    else if(EMAIL_SELECT == type)
    {
        setTransferType(EMAIL_SELECT);
        setIsTriggerTransfer(false);
        showEmailList();
    }

    return;
}

/**************************************************************************************************
 * 打印
 *************************************************************************************************/
void ECG12LeadManager::snapShotprint(int type)
{
    int i = 0;

    ecg12LDataPrint.setPrintRealTime(false);
    switch ((RescueDataPrintType) type)
    {
        case PRINT_12L_SELECT:
        {
            QVector<bool> flag;
            flag = ecg12LSnapShotReview.getSnapShotCheckFlag();
            if(0 >= flag.count())
            {
                //no snapshots
                QString title = trs("SelectSnapshotsToPrint");
                QString msg = trs("NoSnapshotInCurrentIncident");
                IMessageBox messageBox(title, msg, QStringList(trs("EnglishYESChineseSURE")));
                messageBox.autoShow(180,200);
                messageBox.exec();
                return;
            }

            for(i = 0; i < flag.count();i++)
            {
                if(flag[i])
                {
                    break;
                }
            }

            if(i == flag.count())
            {
                QString title = trs("SelectSnapshotsToPrint");
                QString msg = trs("PleaseseletSnapShot");
                IMessageBox messageBox(title, msg, QStringList(trs("EnglishYESChineseSURE")));
                messageBox.autoShow(180,200);
                messageBox.exec();
                break;
            }
        }

        case PRINT_12L_SELECT_RESCUE:
        {
            QStringList list;
            ecg12LeadManager.getPrintSnapShotPath(list);
            if (0 == list.count())
            {
                break;
            }

            QString fileStr = DATA_STORE_PATH + list.at(0) + ECG12L_FILE_NAME;
            QScopedPointer<IStorageBackend> file(StorageManager::open(fileStr, QIODevice::ReadOnly));
            if (NULL == file)
            {
                break;
            }

            if (file->getBlockNR() <= 0)
            {
                QString title = trs("PrintCurrentIncidentSnapshots");
                QString msg = trs("NoSnapshotInCurrentIncident");
                IMessageBox messageBox(title, msg, QStringList(trs("EnglishYESChineseSURE")));
                messageBox.autoShow(180,200);
                messageBox.exec();
            }
            else
            {
                printManager.requestPrint(PRINT_TYPE_REVIEW, PRINT_LAYOUY_ID_12L_ECG);
            }

            break;
        }

        default:
            break;
    }

    return;
}

/**************************************************************************************************
 * 12导快照数据E-mail传输
 *************************************************************************************************/
void ECG12LeadManager::showEmailList()
{
    QRect r = windowManager.getMenuArea();
    int x = r.x() + (r.width() - menuManager.width()) ;
    int y = r.y() + (r.height() - menuManager.height());
    mailAddressWidget.autoShow(x+70, y+35);

    return;
}

/**************************************************************************************************
 * 12导快照数据传输是否完成
 *************************************************************************************************/
bool ECG12LeadManager::isExportProcessFinish()
{
    if (usbManager.isUSBExportFinish() && mailManager.isMailSendFinish())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**************************************************************************************************
 * 12导快照传输完成状态设置
 *************************************************************************************************/
void ECG12LeadManager::setTransferFlag(bool transfer)
{
    _isTransfer = transfer;
}

/**************************************************************************************************
 * 12导快照传输完成状态获取
 *************************************************************************************************/
bool ECG12LeadManager::isTransfer()
{
    return _isTransfer;
}

/**************************************************************************************************
 * 12导快照数据处理
 *************************************************************************************************/
void ECG12LeadManager::acquire12LDataProcess()
{
    if (NULL == _data)
    {
        _data = new ECG12LDataStorage::ECG12LeadData();
        if (NULL == _data)
        {
            return ;
        }
    }
    memset(_data, 0, sizeof(ECG12LDataStorage::ECG12LeadData));

    //获取12L数据
    ecg12LDataStorage.getCommonInfo(_data->commonInfo);

    for (int i = 0; i <= ECG_LEAD_V6; ++i)
    {
        waveformCache.readStorageChannel((WaveformID)i,_data->wave[i], ECG12L_WAVE_TIME);
    }
    _data->checkSum = _data->calSum();

    //保存数据
    if (!dataStorageDirManager.isCurRescueFolderFull())
    {
        ecg12LDataStorage.add12LData(_data);
    }

    //读取配置文件，是否自动打印exportDataWidget
    bool tempValue = FALSE;
    superConfig.getNumValue("ECG12L|AutoPrinting12LReport", tempValue);
    if(tempValue)
    {
        ecg12LDataPrint.setPrintRealTime(true);
        ecg12LDataPrint.PrintECG12LData(_data);
    }

    if (systemManager.isSupport(CONFIG_WIFI))
    {
        //读取配置文件，是否自动传送
        superConfig.getNumValue("ECG12L|AutoTransmission", tempValue);
        if(tempValue)
        {
            setIsTriggerTransfer(true);
            ecg12LExportDataWidget.setCurType(EXPORT_ECG12LEAD_DATA_BY_WIFI);
            setTransferType(EMAIL_SELECT);

            showEmailList();
        }
    }
}

/**************************************************************************************************
 * 准备开始发送E-mail
 *************************************************************************************************/
void ECG12LeadManager::startToSendEmail()
{
    if((!networkManager.isWifiWorking()) || (!networkManager.isWiFiConnected()))
    {
        //警告wifi没链接
        IMessageBox messageBox(trs("Warn"), trs("NetworkDisconnected"), QStringList(trs("Yes")));
        QRect r = windowManager.getMenuArea();
        int x = (r.right() - r.left() - messageBox.width())/2 + r.left();
        messageBox.autoShow(x, 250);
        messageBox.exec();
        return;
    }

    if(0 >= mailAddressWidget.getCheckFlagCount())
    {
        IMessageBox messageBox(trs("Warn"), trs("EmailAddressListEmpty"), QStringList(trs("Yes")));
        QRect r = windowManager.getMenuArea();
        int x = (r.right() - r.left() - messageBox.width())/2 + r.left();
        messageBox.autoShow(x, 250);
        messageBox.exec();
        return;
    }

    int fileFormat = ECG_TRANSFER_FILE_FORMAT_PDF;
    superConfig.getNumValue("ECG12L|TransmissionFormat", fileFormat);

    ECG12LeadFileBuilderBase *builder = NULL;

    if (getIsTriggerTransfer())
    {
        setTransferFlag(true);
        _exportWifiWidget->init();
        _exportWifiWidget->setVisible(true);
        while(!isExportProcessFinish())
        {
            if (_exportWifiWidget->isTransferCancel())
            {
                return;
            }

            Util::waitInEventLoop(200);
        }

        if(fileFormat == ECG_TRANSFER_FILE_FORMAT_PDF)
        {
            builder = new ECG12LeadPdfBuilder(_data);
        }
    }
    else
    {
        QVector<int> snapshotIndexList;
        QVector<bool> flag;
        flag = ecg12LSnapShotReview.getSnapShotCheckFlag();
        for(int i = 0; i < flag.count(); i++)
        {
            if(flag[i])
            {
                snapshotIndexList.append(i);
            }
        }

        if(0 >= snapshotIndexList.count())
        {
            IMessageBox messageBox(trs("SelectSnapshotsToTransfer"), trs("PleaseseletSnapShot"), QStringList(trs("EnglishYESChineseSURE")));
            messageBox.autoShow(180,220);
            messageBox.exec();
            return ;
        }
        else if(WIFI_TRANSFER_SBAPSHOTS_MAX_NUM < snapshotIndexList.count())
        {
            IMessageBox messageBox(trs("SelectSnapshotsToTransfer"), trs("PleaseseletSnapShotNoMoreThan8"), QStringList(trs("EnglishYESChineseSURE")));
            messageBox.autoShow(180,220);
            messageBox.exec();
            return ;
        }

        setTransferFlag(true);
        _exportWifiWidget->init();
        _exportWifiWidget->setVisible(true);
        while(!isExportProcessFinish())
        {
            if (_exportWifiWidget->isTransferCancel())
            {
                return;
            }

            Util::waitInEventLoop(200);
        }

        int index = ecg12LSnapShotReview.getIncidentIndex();
        QString incidentDir = dataStorageDirManager.getRescueDataDir(index);

        if(fileFormat == ECG_TRANSFER_FILE_FORMAT_PDF)
        {
            builder = new ECG12LeadPdfBuilder(incidentDir, snapshotIndexList);
        }
    }

    connect(&mailManager, SIGNAL(wifiExportFileFinish(int)), this, SLOT(wifiExportFileFinishPrecess(int)), Qt::UniqueConnection);

    //the ownership of the builder will pass to the usbmanager,
    //usb manager will deleet it when export finish
    mailManager.export12LeadData(builder);

    return;
}

void ECG12LeadManager::usbExportFileFinishPrecess(int status)
{
    QString msg = "";
    DataExporterBase::ExportStatus exportStatus = (DataExporterBase::ExportStatus)status;

    disconnect(&usbManager, SIGNAL(usbExportFileFinish(int)), this, SLOT(usbExportFileFinishPrecess(int)));

    if ((DataExporterBase::Cancel == exportStatus) || (!isTransfer()))
    {
        //如果取消则在前面已经设置过标记位，这里不再设置
        return ;
    }

    if (_exportUsbWidget->isVisible())
    {
        if (DataExporterBase::Disconnect == exportStatus)
        {
            msg = trs("TransferDisconnect");
        }
        else if (DataExporterBase::NoSpace == exportStatus)
        {
            msg = trs("WarnLessUSBFreeSpace");
        }
        else if(DataExporterBase::Failed == exportStatus)
        {
            msg = trs("TransferFailed");
        }
        else
        {
            //Other
        }

        if (!msg.isEmpty())
        {
            _exportUsbWidget->setVisible(false);

            IMessageBox messageBox(trs("Warn"), msg, QStringList(trs("EnglishYESChineseSURE")));
            QRect r = windowManager.getMenuArea();
            int x = (r.right() - r.left() - messageBox.width())/2 + r.left();
            messageBox.autoShow(x, 220);
            messageBox.exec();
        }
    }
    else
    {
        if(DataExporterBase::Success == exportStatus)
        {
            ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_TRANSFER_SUCCESS, true);
            QTimer::singleShot(5000, this, SLOT(closeTransferPrompt()));
        }
        else
        {
            ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_TRANSFER_FAILED, true);
            QTimer::singleShot(5000, this, SLOT(closeTransferPrompt()));
        }
    }

    setTransferFlag(false);

    return;
}

void ECG12LeadManager::wifiExportFileFinishPrecess(int status)
{
    QString msg = "";
    MailManager::Status exportStatus = (MailManager::Status)status;

    disconnect(&mailManager, SIGNAL(wifiExportFileFinish(int)), this, SLOT(wifiExportFileFinishPrecess(int)));

    if ((MailManager::Cancel == exportStatus) || (!isTransfer()))
    {
        //如果取消则在前面已经设置过标记位，这里不再设置
        return ;
    }

    if (_exportWifiWidget->isVisible())
    {
        if (MailManager::ConnectFail == exportStatus)
        {
            msg = trs("MailConnectFail");
        }
        else if (MailManager::AuthFail == exportStatus)
        {
            msg = trs("MailLogFail");
        }
        else if(MailManager::SendFail == exportStatus)
        {
            msg = trs("MailSendFail");
        }
        else
        {
            //other
        }

        if (!msg.isEmpty())
        {
            _exportWifiWidget->setVisible(false);

            IMessageBox messageBox(trs("Warn"), msg, QStringList(trs("EnglishYESChineseSURE")));
            QRect r = windowManager.getMenuArea();
            int x = (r.right() - r.left() - messageBox.width())/2 + r.left();
            messageBox.autoShow(x, 220);
            messageBox.exec();
        }
    }
    else
    {
        if(MailManager::Success == exportStatus)
        {
            ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_TRANSFER_SUCCESS, true);
            QTimer::singleShot(5000, this, SLOT(closeTransferPrompt()));
        }
        else
        {
            ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_TRANSFER_FAILED, true);
            QTimer::singleShot(5000, this, SLOT(closeTransferPrompt()));
        }
    }

    setTransferFlag(false);

    return;
}

void ECG12LeadManager::transfercancel()
{
    setTransferFlag(false);
}

void ECG12LeadManager::closeTransferPrompt()
{
    ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_TRANSFER_SUCCESS, false);
    ecgOneShotAlarm.setOneShotAlarm(ECG_ONESHOT_ALARM_TRANSFER_FAILED, false);
}
