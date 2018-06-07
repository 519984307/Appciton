#pragma once
#include "SubMenu.h"

class IComboList;

//Print配置
class Supervisor12LMenu : public SubMenu
{
    Q_OBJECT

public:
    static Supervisor12LMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new Supervisor12LMenu();
        }

        return *_selfObj;
    }
    static Supervisor12LMenu *_selfObj;

    ~Supervisor12LMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _notchFilterChangeSlot(int index);
    void _12lFrequenceChangeSlot(int index);
    void _printFormatSlot(int index);
    void _pdfReportSlot(int index);
//    void _displayFormatSlot(int index);
    void _timeWindowFor2x6PDFSlot(int index);
    void _autoPrint12LeadSlot(int index);
//    void _autoTransforViaWifiSlot(int index);
//    void _transforFileFormatSlot(int index);
    void _realTimePrintTopLeadSlot(int index);
    void _realTimePrintMidLeadSlot(int index);
    void _realTimePrintBotLeadSlot(int index);

private:
    Supervisor12LMenu();

    IComboList *_notchFilter;             // 频率
    IComboList *_12LFrequency;            // 12导带宽
    IComboList *_printSnapshotFormat;     // 快照打印格式
    //IComboList *_pdfReportFormat;         // PDF报告格式
    //IComboList *_displayFormat;           // 显示格式
    IComboList *_timeWindowFor2x6PDF;     // 时间间隔
    IComboList *_autoPrint12Lead;         // 自动打印12导
//    IComboList *_autoTransforViaWifi;     // 自动传输
//    IComboList *_transforFileFormat;      // 传输文件格式
    IComboList *_realTimePrintTopLead;    // 实时打印顶部导联
    IComboList *_realTimePrintMidLead;    // 实时打印中部导联
    IComboList *_realTimePrintBotLead;    // 实时打印底部导联
};
#define supervisor12LMenu (Supervisor12LMenu::construction())

