#pragma once

#include "ECG12LExportDataWidget.h"
#include "ECG12LDataStorage.h"
#include "RescueDataDefine.h"

//WIFI传输的最大文件个数
#define WIFI_TRANSFER_SBAPSHOTS_MAX_NUM (8)

class ECG12LeadManager : public QObject
{
    Q_OBJECT

public:
    static ECG12LeadManager &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ECG12LeadManager();
        }
        return *_selfObj;
    }

    static ECG12LeadManager *_selfObj;

    ECG12LeadManager();

    ~ECG12LeadManager();

public:
    //开始捕获
    void startCapture();

    //开始回顾快照
    void startReview();

    //获取选择的营救事件路径
    void getPrintSnapShotPath(QStringList &list);

    //E-mail列表
    void showEmailList();

    void setTransferType(ECG12LEADTransferType type) {_transferType = type;}

    ECG12LEADTransferType getTransferType() {return _transferType;}

    void setIsTriggerTransfer(bool isTrigger) {_isTriggerTransfer = isTrigger;}

    bool getIsTriggerTransfer() { return _isTriggerTransfer;}

    bool isExportProcessFinish();

    void setTransferFlag(bool transfer);

    bool isTransfer();

#ifdef CONFIG_UNIT_TEST
    friend class TestECG12L;
#endif
private slots:
    void acquire12LDataProcess();                   //12导快照数据处理
    void acquireDataTransfer(int type);             //传输快照。
    void acquireDataPrint();                        //打印快照。
    void snapShotTransfer(int type);                //传送快照
    void snapShotprint(int type);                   //快照打印
    void startToSendEmail();                        //开始发送E-mail
    void usbExportFileFinishPrecess(int status);
    void wifiExportFileFinishPrecess(int status);
    void transfercancel();
    void closeTransferPrompt();

public:
    ExportDataWidget* _exportUsbWidget;
    ExportDataWidget* _exportWifiWidget;

private:
    ECG12LEADTransferType _transferType;
    bool _isTriggerTransfer;
    bool _isTransfer;
    ECG12LDataStorage::ECG12LeadData* _data;        //快照数据
};

#define ecg12LeadManager (ECG12LeadManager::construction())
#define deleteecg12LeadManager() (delete ECG12LeadManager::_selfObj)
