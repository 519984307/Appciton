#include <QVBoxLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "Supervisor12LMenu.h"
#include "SupervisorConfigManager.h"
#include "SupervisorMenuManager.h"
#include "PrintDefine.h"
#include "ECGDefine.h"
#include "ECGSymbol.h"

Supervisor12LMenu *Supervisor12LMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
Supervisor12LMenu::Supervisor12LMenu() : SubMenu(trs("Supervisor12LMenu"))
{
    setDesc(trs("Supervisor12LMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void Supervisor12LMenu::readyShow()
{
    int index = 0;

    superConfig.getNumValue("ECG12L|NotchFilter", index);
    _notchFilter->setCurrentIndex(index);

    superConfig.getNumValue("ECG12L|ECG12LeadBandwidth", index);
    _12LFrequency->setCurrentIndex(index);

    superConfig.getNumValue("ECG12L|PrintSnapshotFormat", index);
    _printSnapshotFormat->setCurrentIndex(index);

    superConfig.getNumValue("ECG12L|PDFReportFormat", index);
    _pdfReportFormat->setCurrentIndex(index);

    superConfig.getNumValue("ECG12L|DisplayFormat", index);
    _displayFormat->setCurrentIndex(index);

    superConfig.getNumValue("ECG12L|TimeIntervalFor2x6Report", index);
    _timeWindowFor2x6PDF->setCurrentIndex(index);

    superConfig.getNumValue("ECG12L|AutoPrinting12LReport", index);
    _autoPrint12Lead->setCurrentIndex(index);

    superConfig.getNumValue("ECG12L|AutoTransmission", index);
    _autoTransforViaWifi->setCurrentIndex(index);

    superConfig.getNumValue("ECG12L|TransmissionFormat", index);
    _transforFileFormat->setCurrentIndex(index);

    int leadNameConvention = 0, displayMode = 0;
    superConfig.getNumValue("ECG|ECGLeadConvention", leadNameConvention);
    superConfig.getNumValue("ECG12L|DisplayFormat", displayMode);
    _realTimePrintTopLead->clear();
    _realTimePrintMidLead->clear();
    _realTimePrintBotLead->clear();
    for (int i = ECG_LEAD_I; i <= ECG_LEAD_V6; ++i)
    {
        _realTimePrintTopLead->addItem(ECGSymbol::convert((ECGLead)i,
            (ECGLeadNameConvention)leadNameConvention, true, displayMode));

        _realTimePrintMidLead->addItem(ECGSymbol::convert((ECGLead)i,
            (ECGLeadNameConvention)leadNameConvention, true, displayMode));

        _realTimePrintBotLead->addItem(ECGSymbol::convert((ECGLead)i,
            (ECGLeadNameConvention)leadNameConvention, true, displayMode));
    }

    superConfig.getNumValue("ECG12L|RealtimePrintTopLead", index);
    _realTimePrintTopLead->setCurrentIndex(index);

    superConfig.getNumValue("ECG12L|RealtimePrintMiddleLead", index);
    _realTimePrintMidLead->setCurrentIndex(index);

    superConfig.getNumValue("ECG12L|RealtimePrintBottomLead", index);
    _realTimePrintBotLead->setCurrentIndex(index);
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void Supervisor12LMenu::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE - SCROLL_BAR_WIDTH;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 3;
    int labelWidth = itemW - btnWidth;

    //工频
    _notchFilter = new IComboList(trs("SupervisorNotchFilter"));
    _notchFilter->setFont(fontManager.textFont(fontSize));
    _notchFilter->label->setFixedSize(labelWidth, ITEM_H);
    _notchFilter->combolist->setFixedSize(btnWidth, ITEM_H);
    _notchFilter->addItem(ECGSymbol::convert(ECG_NOTCH_OFF1));
    _notchFilter->addItem(ECGSymbol::convert(ECG_NOTCH_50HZ));
    _notchFilter->addItem(ECGSymbol::convert(ECG_NOTCH_60HZ));
    connect(_notchFilter, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_notchFilterChangeSlot(int)));
    mainLayout->addWidget(_notchFilter);

    //12l带宽
    _12LFrequency = new IComboList(trs("Supervisor12lFrequency"));
    _12LFrequency->setFont(fontManager.textFont(fontSize));
    _12LFrequency->label->setFixedSize(labelWidth, ITEM_H);
    _12LFrequency->combolist->setFixedSize(btnWidth, ITEM_H);
    _12LFrequency->addItem(ECGSymbol::convert(ECG_BANDWIDTH_0525_40HZ));
    _12LFrequency->addItem(ECGSymbol::convert(ECG_BANDWIDTH_0525_150HZ));
    connect(_12LFrequency, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_12lFrequenceChangeSlot(int)));
    mainLayout->addWidget(_12LFrequency);

    //print snapshot format
    _printSnapshotFormat = new IComboList(trs("Print12LSnapshotFormat"));
    _printSnapshotFormat->setFont(fontManager.textFont(fontSize));
    _printSnapshotFormat->label->setFixedSize(labelWidth, ITEM_H);
    _printSnapshotFormat->combolist->setFixedSize(btnWidth, ITEM_H);
    for (int i = 0; i < PRINT_12LEAD_SNAPSHOT_NR; ++i)
    {
        _printSnapshotFormat->addItem(PrintSymbol::convert((Print12LeadSnapshotFormat)i));
    }

    connect(_printSnapshotFormat, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_printFormatSlot(int)));
    mainLayout->addWidget(_printSnapshotFormat);

    //PDF Report format
    _pdfReportFormat = new IComboList(trs("PDFReportFormat"));
    _pdfReportFormat->setFont(fontManager.textFont(fontSize));
    _pdfReportFormat->label->setFixedSize(labelWidth, ITEM_H);
    _pdfReportFormat->combolist->setFixedSize(btnWidth, ITEM_H);
    for (int i = 0; i < PRINT_12LEAD_PDF_NR; ++i)
    {
        _pdfReportFormat->addItem(PrintSymbol::convert((Print12LeadPDFFormat)i));
    }

    connect(_pdfReportFormat, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_pdfReportSlot(int)));
    mainLayout->addWidget(_pdfReportFormat);

    //display format
    _displayFormat = new IComboList(trs("ECG12LDisplayFormat"));
    _displayFormat->setFont(fontManager.textFont(fontSize));
    _displayFormat->label->setFixedSize(labelWidth, ITEM_H);
    _displayFormat->combolist->setFixedSize(btnWidth, ITEM_H);
    for (int i = 0; i < DISPLAY_12LEAD_NR; ++i)
    {
        _displayFormat->addItem(trs(ECGSymbol::convert((Display12LeadFormat)i)));
    }

    connect(_displayFormat, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_displayFormatSlot(int)));
    mainLayout->addWidget(_displayFormat);

    //时间间隔
    _timeWindowFor2x6PDF = new IComboList(trs("SupervisorTimeWindow2x6PDF"));
    _timeWindowFor2x6PDF->setFont(fontManager.textFont(fontSize));
    _timeWindowFor2x6PDF->label->setFixedSize(labelWidth, ITEM_H);
    _timeWindowFor2x6PDF->combolist->setFixedSize(btnWidth, ITEM_H);
    _timeWindowFor2x6PDF->addItem("0-5s");
    _timeWindowFor2x6PDF->addItem("2.5-7.5s");
    _timeWindowFor2x6PDF->addItem("5-10s");
    connect(_timeWindowFor2x6PDF, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_timeWindowFor2x6PDFSlot(int)));
    mainLayout->addWidget(_timeWindowFor2x6PDF);

    //auto print
    _autoPrint12Lead = new IComboList(trs("SupervisorAutoPrint12L"));
    _autoPrint12Lead->setFont(fontManager.textFont(fontSize));
    _autoPrint12Lead->label->setFixedSize(labelWidth, ITEM_H);
    _autoPrint12Lead->combolist->setFixedSize(btnWidth, ITEM_H);
    _autoPrint12Lead->addItem(trs("Disable"));
    _autoPrint12Lead->addItem(trs("Enable"));
    connect(_autoPrint12Lead, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_autoPrint12LeadSlot(int)));
    mainLayout->addWidget(_autoPrint12Lead);

    //auto transfor
    _autoTransforViaWifi = new IComboList(trs("SupervisorAutoTransforByWifi"));
    _autoTransforViaWifi->setFont(fontManager.textFont(fontSize));
    _autoTransforViaWifi->label->setFixedSize(labelWidth, ITEM_H);
    _autoTransforViaWifi->combolist->setFixedSize(btnWidth, ITEM_H);
    _autoTransforViaWifi->addItem(trs("Disable"));
    _autoTransforViaWifi->addItem(trs("Enable"));
    connect(_autoTransforViaWifi, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_autoTransforViaWifiSlot(int)));
    if (systemManager.isSupport(CONFIG_WIFI))
    {
        mainLayout->addWidget(_autoTransforViaWifi);
    }

    //file format
    _transforFileFormat = new IComboList(trs("SupervisorTransforFileFormat"));
    _transforFileFormat->setFont(fontManager.textFont(fontSize));
    _transforFileFormat->label->setFixedSize(labelWidth, ITEM_H);
    _transforFileFormat->combolist->setFixedSize(btnWidth, ITEM_H);
    _transforFileFormat->addItem("PDF");
    _transforFileFormat->addItem("JSON");
    connect(_transforFileFormat, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_transforFileFormatSlot(int)));
    mainLayout->addWidget(_transforFileFormat);

    //real-time top lead
    _realTimePrintTopLead = new IComboList(trs("RealTimePrintTopLead"));
    _realTimePrintTopLead->setFont(fontManager.textFont(fontSize));
    _realTimePrintTopLead->label->setFixedSize(labelWidth, ITEM_H);
    _realTimePrintTopLead->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_realTimePrintTopLead, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_realTimePrintTopLeadSlot(int)));
    mainLayout->addWidget(_realTimePrintTopLead);

    //real-time middle lead
    _realTimePrintMidLead = new IComboList(trs("RealTimePrintMidLead"));
    _realTimePrintMidLead->setFont(fontManager.textFont(fontSize));
    _realTimePrintMidLead->label->setFixedSize(labelWidth, ITEM_H);
    _realTimePrintMidLead->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_realTimePrintMidLead, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_realTimePrintMidLeadSlot(int)));
    mainLayout->addWidget(_realTimePrintMidLead);

    //real-time bottom lead
    _realTimePrintBotLead = new IComboList(trs("RealTimePrintBottomLead"));
    _realTimePrintBotLead->setFont(fontManager.textFont(fontSize));
    _realTimePrintBotLead->label->setFixedSize(labelWidth, ITEM_H);
    _realTimePrintBotLead->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_realTimePrintBotLead, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_realTimePrintBotLeadSlot(int)));
    mainLayout->addWidget(_realTimePrintBotLead);
}

/**************************************************************************************************
 * 工频改变。
 *************************************************************************************************/
void Supervisor12LMenu::_notchFilterChangeSlot(int index)
{
    superConfig.setNumValue("ECG12L|NotchFilter", index);
}

/**************************************************************************************************
 * 12l带宽改变。
 *************************************************************************************************/
void Supervisor12LMenu::_12lFrequenceChangeSlot(int index)
{
    superConfig.setNumValue("ECG12L|ECG12LeadBandwidth", index);
}

/**************************************************************************************************
 * print format。
 *************************************************************************************************/
void Supervisor12LMenu::_printFormatSlot(int index)
{
    superConfig.setNumValue("ECG12L|PrintSnapshotFormat", index);
}

/**************************************************************************************************
 * PDF Report format。
 *************************************************************************************************/
void Supervisor12LMenu::_pdfReportSlot(int index)
{
    superConfig.setNumValue("ECG12L|PDFReportFormat", index);
}

/**************************************************************************************************
 * display format。
 *************************************************************************************************/
void Supervisor12LMenu::_displayFormatSlot(int index)
{
    superConfig.setNumValue("ECG12L|DisplayFormat", index);

    int leadNameConvention = 0;
    superConfig.getNumValue("ECG|ECGLeadConvention", leadNameConvention);
    int topLead = _realTimePrintTopLead->currentIndex();
    int midLead = _realTimePrintMidLead->currentIndex();
    int botLead = _realTimePrintBotLead->currentIndex();
    _realTimePrintTopLead->clear();
    _realTimePrintMidLead->clear();
    _realTimePrintBotLead->clear();
    for (int i = ECG_LEAD_I; i <= ECG_LEAD_V6; ++i)
    {
        _realTimePrintTopLead->addItem(ECGSymbol::convert((ECGLead)i,
            (ECGLeadNameConvention)leadNameConvention, true, index));

        _realTimePrintMidLead->addItem(ECGSymbol::convert((ECGLead)i,
            (ECGLeadNameConvention)leadNameConvention, true, index));

        _realTimePrintBotLead->addItem(ECGSymbol::convert((ECGLead)i,
            (ECGLeadNameConvention)leadNameConvention, true, index));
    }

    _realTimePrintTopLead->setCurrentIndex(topLead);
    _realTimePrintMidLead->setCurrentIndex(midLead);
    _realTimePrintBotLead->setCurrentIndex(botLead);
}

/**************************************************************************************************
 * Time Window For 2x6 PDF format
 *************************************************************************************************/
void Supervisor12LMenu::_timeWindowFor2x6PDFSlot(int index)
{
    superConfig.setNumValue("ECG12L|TimeIntervalFor2x6Report", index);
}

/**************************************************************************************************
 * auto print 12L。
 *************************************************************************************************/
void Supervisor12LMenu::_autoPrint12LeadSlot(int index)
{
    superConfig.setNumValue("ECG12L|AutoPrinting12LReport", index);
}

/**************************************************************************************************
 * 自动传输。
 *************************************************************************************************/
void Supervisor12LMenu::_autoTransforViaWifiSlot(int index)
{
    superConfig.setNumValue("ECG12L|AutoTransmission", index);
}

/**************************************************************************************************
 * 传输文件格式。
 *************************************************************************************************/
void Supervisor12LMenu::_transforFileFormatSlot(int index)
{
    superConfig.setNumValue("ECG12L|TransmissionFormat", index);
}

/**************************************************************************************************
 * real-time print top lead。
 *************************************************************************************************/
void Supervisor12LMenu::_realTimePrintTopLeadSlot(int index)
{
    superConfig.setNumValue("ECG12L|RealtimePrintTopLead", index);
}

/**************************************************************************************************
 * real-time print middle lead。
 *************************************************************************************************/
void Supervisor12LMenu::_realTimePrintMidLeadSlot(int index)
{
    superConfig.setNumValue("ECG12L|RealtimePrintMiddleLead", index);
}

/**************************************************************************************************
 * real-time print bottom lead。
 *************************************************************************************************/
void Supervisor12LMenu::_realTimePrintBotLeadSlot(int index)
{
    superConfig.setNumValue("ECG12L|RealtimePrintBottomLead", index);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
Supervisor12LMenu::~Supervisor12LMenu()
{

}

