#include <QVBoxLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "SupervisorPrintMenu.h"
#include "SupervisorConfigManager.h"
#include "SupervisorMenuManager.h"

SupervisorPrintMenu *SupervisorPrintMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorPrintMenu::SupervisorPrintMenu() : SubMenu(trs("SupervisorPrintMenu"))
{
    setDesc(trs("SupervisorPrintMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void SupervisorPrintMenu::readyShow()
{
    int index = 0;
    currentConfig.getNumValue("Print|PresentingECG", index);
    _ECGRhythmAutoPrint->setCurrentIndex(index);

    currentConfig.getNumValue("Print|CheckPatient", index);
    _checkPatientAlarmAutoPrint->setCurrentIndex(index);

    currentConfig.getNumValue("Print|PhysiologicalAlarm", index);
    _phyAlarmAutoPrint->setCurrentIndex(index);

    currentConfig.getNumValue("Print|CoderMarker", index);
    _codemarkerAutoPrint->setCurrentIndex(index);

    currentConfig.getNumValue("Print|NIBPReading", index);
    _NIBPAutoPrint->setCurrentIndex(index);

//    currentConfig.getNumValue("Print|DiagnosticECG", index);
//    _diagECGAutoPrint->setCurrentIndex(index);

//    currentConfig.getNumValue("Print|SummaryReportSnapshotPrintingInAED", index);
//    _autoPrintInAED->setCurrentIndex(index);

//    currentConfig.getNumValue("Print|Print30JSelfTestReport", index);
//    _30jSelftest->setCurrentIndex(index);
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SupervisorPrintMenu::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW- ICOMBOLIST_SPACE;
    int fontsize = 15;
    int btnWidth = itemW / 3;
    int labelWidth = itemW - btnWidth;

    //ecg rhythm
    _ECGRhythmAutoPrint = new IComboList(trs("ECGRhythm"));
    _ECGRhythmAutoPrint->setFont(fontManager.textFont(fontsize));
    _ECGRhythmAutoPrint->label->setFixedSize(labelWidth, ITEM_H);
    _ECGRhythmAutoPrint->combolist->setFixedSize(btnWidth, ITEM_H);
    _ECGRhythmAutoPrint->addItem(trs("Disable"));
    _ECGRhythmAutoPrint->addItem(trs("Enable"));
    connect(_ECGRhythmAutoPrint, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_ECGRhythmAutoPrintSlot(int)));
    mainLayout->addWidget(_ECGRhythmAutoPrint);

    //check patient
    _checkPatientAlarmAutoPrint = new IComboList(trs("CheckPatient"));
    _checkPatientAlarmAutoPrint->setFont(fontManager.textFont(fontsize));
    _checkPatientAlarmAutoPrint->label->setFixedSize(labelWidth, ITEM_H);
    _checkPatientAlarmAutoPrint->combolist->setFixedSize(btnWidth, ITEM_H);
    _checkPatientAlarmAutoPrint->addItem(trs("Disable"));
    _checkPatientAlarmAutoPrint->addItem(trs("Enable"));
    connect(_checkPatientAlarmAutoPrint, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_checkPatientAlarmAutoPrintSlot(int)));
    mainLayout->addWidget(_checkPatientAlarmAutoPrint);


    //phy alarm
    _phyAlarmAutoPrint = new IComboList(trs("PhyAlarm"));
    _phyAlarmAutoPrint->setFont(fontManager.textFont(fontsize));
    _phyAlarmAutoPrint->label->setFixedSize(labelWidth, ITEM_H);
    _phyAlarmAutoPrint->combolist->setFixedSize(btnWidth, ITEM_H);
    _phyAlarmAutoPrint->addItem(trs("Disable"));
    _phyAlarmAutoPrint->addItem(trs("Enable"));
    connect(_phyAlarmAutoPrint, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_phyAlarmAutoPrintSlot(int)));
    mainLayout->addWidget(_phyAlarmAutoPrint);

    //code marker
    _codemarkerAutoPrint = new IComboList(trs("CodeMarker"));
    _codemarkerAutoPrint->setFont(fontManager.textFont(fontsize));
    _codemarkerAutoPrint->label->setFixedSize(labelWidth, ITEM_H);
    _codemarkerAutoPrint->combolist->setFixedSize(btnWidth, ITEM_H);
    _codemarkerAutoPrint->addItem(trs("Disable"));
    _codemarkerAutoPrint->addItem(trs("Enable"));
    connect(_codemarkerAutoPrint, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_codemarkerAutoPrintSlot(int)));
    mainLayout->addWidget(_codemarkerAutoPrint);

    //NIBP Reading
    _NIBPAutoPrint = new IComboList(trs("NIBPReading"));
    _NIBPAutoPrint->setFont(fontManager.textFont(fontsize));
    _NIBPAutoPrint->label->setFixedSize(labelWidth, ITEM_H);
    _NIBPAutoPrint->combolist->setFixedSize(btnWidth, ITEM_H);
    _NIBPAutoPrint->addItem(trs("Disable"));
    _NIBPAutoPrint->addItem(trs("Enable"));
    connect(_NIBPAutoPrint, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_NIBPAutoPrintSlot(int)));
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        mainLayout->addWidget(_NIBPAutoPrint);
    }

    //Diagnostic ECG
//    _diagECGAutoPrint = new IComboList(trs("DiagnosticECG"));
//    _diagECGAutoPrint->setFont(fontManager.textFont(fontsize));
//    _diagECGAutoPrint->label->setFixedSize(labelWidth, ITEM_H);
//    _diagECGAutoPrint->combolist->setFixedSize(btnWidth, ITEM_H);
//    _diagECGAutoPrint->addItem(trs("Disable"));
//    _diagECGAutoPrint->addItem(trs("Enable"));
//    connect(_diagECGAutoPrint, SIGNAL(currentIndexChanged(int)), this,
//            SLOT(_diagECGAutoPrintSlot(int)));
//    mainLayout->addWidget(_diagECGAutoPrint);

    //auto print in AED
//    _autoPrintInAED = new IComboList(trs("SupervisorAutoPrintInAED"));
//    _autoPrintInAED->setFont(fontManager.textFont(fontsize));
//    _autoPrintInAED->label->setFixedSize(labelWidth, ITEM_H);
//    _autoPrintInAED->combolist->setFixedSize(btnWidth, ITEM_H);
//    _autoPrintInAED->addItem(trs("Disable"));
//    _autoPrintInAED->addItem(trs("Enable"));
//    connect(_autoPrintInAED, SIGNAL(currentIndexChanged(int)), this,
//            SLOT(_autoPrintInAEDSlot(int)));
//    mainLayout->addWidget(_autoPrintInAED);

    //auto print 30J self-test
//    _30jSelftest = new IComboList(trs("SupervisorAutoPrint30Jtest"));
//    _30jSelftest->setFont(fontManager.textFont(fontsize));
//    _30jSelftest->label->setFixedSize(labelWidth, ITEM_H);
//    _30jSelftest->combolist->setFixedSize(btnWidth, ITEM_H);
//    _30jSelftest->addItem(trs("Disable"));
//    _30jSelftest->addItem(trs("Enable"));
//    connect(_30jSelftest, SIGNAL(currentIndexChanged(int)), this,
//            SLOT(_autoPrint30JtestResultSlot(int)));
//    mainLayout->addWidget(_30jSelftest);
}

/**************************************************************************************************
 * ecg rhythm change。
 *************************************************************************************************/
void SupervisorPrintMenu::_ECGRhythmAutoPrintSlot(int index)
{
    currentConfig.setNumValue("Print|PresentingECG", index);
}

/**************************************************************************************************
 * ecg foranalysis change。
 *************************************************************************************************/
void SupervisorPrintMenu::_ECGForeAnalysisAutoPrintSlot(int index)
{
    currentConfig.setNumValue("Print|ECGAnalysis", index);
}

/**************************************************************************************************
 * shock delivery。
 *************************************************************************************************/
void SupervisorPrintMenu::_shockDeliveryAutoPrintSlot(int index)
{
    currentConfig.setNumValue("Print|ShockDelivery", index);
}

/**************************************************************************************************
 * check alarm。
 *************************************************************************************************/
void SupervisorPrintMenu::_checkPatientAlarmAutoPrintSlot(int index)
{
    currentConfig.setNumValue("Print|CheckPatient", index);
}

/**************************************************************************************************
 * pacer start up。
 *************************************************************************************************/
void SupervisorPrintMenu::_pacerStartAutoPrintSlot(int index)
{
    currentConfig.setNumValue("Print|PacerStartUp", index);
}

/**************************************************************************************************
 * phy alarm。
 *************************************************************************************************/
void SupervisorPrintMenu::_phyAlarmAutoPrintSlot(int index)
{
    currentConfig.setNumValue("Print|PhysiologicalAlarm", index);
}

/**************************************************************************************************
 * coder marker。
 *************************************************************************************************/
void SupervisorPrintMenu::_codemarkerAutoPrintSlot(int index)
{
    currentConfig.setNumValue("Print|CoderMarker", index);
}

/***************************************************************************************************
 * nibp
 **************************************************************************************************/
void SupervisorPrintMenu::_NIBPAutoPrintSlot(int index)
{
    currentConfig.setNumValue("Print|NIBPReading", index);
}

/***************************************************************************************************
 * diag ecg
 **************************************************************************************************/
//void SupervisorPrintMenu::_diagECGAutoPrintSlot(int index)
//{
//    currentConfig.setNumValue("Print|DiagnosticECG", index);
//}

/**************************************************************************************************
 * auto print in AED。
 *************************************************************************************************/
//void SupervisorPrintMenu::_autoPrintInAEDSlot(int index)
//{
//    currentConfig.setNumValue("Print|SummaryReportSnapshotPrintingInAED", index);
//}

/**************************************************************************************************
 * auto print 30J 自测结果。
 *************************************************************************************************/
//void SupervisorPrintMenu::_autoPrint30JtestResultSlot(int index)
//{
//    currentConfig.setNumValue("Print|Print30JSelfTestReport", index);
//}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorPrintMenu::~SupervisorPrintMenu()
{

}


