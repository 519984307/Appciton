#pragma once
#include "PopupWidget.h"
#include "LabelButton.h"
#include "RescueDataListWidget.h"
#include "ECG12LDataStorage.h"

//12导联营救事件范围选择打印界面
class ECG12LDataPrint : public QObject
{
    Q_OBJECT

public:
    static ECG12LDataPrint &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ECG12LDataPrint();
        }

        return *_selfObj;
    }

    static ECG12LDataPrint *_selfObj;

    ECG12LDataPrint();
    ~ECG12LDataPrint();

public:
    void PrintECG12LData(ECG12LDataStorage::ECG12LeadData *data);

    //绘制打印页
    void drawPrintPage();

    //是否正在打印
    bool isPrint();

    //设置是否为实时打印
    void setPrintRealTime(bool isRealTime);

    //获取是否为实时打印
    bool getPrintRealTime();

private:
    bool _isRealTime;
    RescueDataListWidget *_listWidget;
    LButtonEx *_printBtn;
    LButtonEx *_backBtn;
    IButton *_up;
    IButton *_down;
private:
    bool _isPrint;
};

#define ecg12LDataPrint (ECG12LDataPrint::construction())
#define deleteecg12LDataPrint() (delete ECG12LDataPrint::_selfObj)
