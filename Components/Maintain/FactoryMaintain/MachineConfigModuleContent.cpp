/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/
#include "MachineConfigModuleContent.h"
#include <QGridLayout>
#include <QMap>
#include <QLabel>
#include <QFile>
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include "IConfig.h"
#include "RESPSymbol.h"
#include "NIBPSymbol.h"
#include "TEMPSymbol.h"
#include "SPO2Symbol.h"
#include "CO2Symbol.h"
#include "IBPSymbol.h"
#ifdef Q_WS_QWS
#include <QWSServer>
#include "SystemManager.h"
#endif
#include "SoftKeyManager.h"
#include "MessageBox.h"
#include <QProcess>
#include "SystemBoardProvider.h"

class MachineConfigModuleContentPrivte
{
public:
    enum MenuItem
    {
        ITEM_CBO_ECG12,
        ITEM_CBO_SPO2,
        ITEM_CBO_NIBP,
        ITEM_CBO_RESP,
        ITEM_CBO_CO2,
        ITEM_CBO_AG,
        ITEM_CBO_CO,
        ITEM_CBO_IBP,
#ifdef ENABLE_O2_APNEASTIMULATION
        ITEM_CBO_O2,
#endif
        ITEM_CBO_PRINTER,
        ITEM_CBO_TEMP,
        ITEM_CBO_WIFI,
#ifdef Q_WS_QWS
        ITEM_CBO_TSCREEN,
#endif
        ITEM_CBO_BACKLIGHT,
        ITEM_CBO_NIBP_NEO_MEASURE,
        ITEM_CBO_SCREEN_INFO,
        ITEM_CBO_NEO_MACHINE,
        ITEM_CBO_HDMI,
        ITEM_CBO_NURSE_CALL,
        ITEM_CBO_ANALOG_OUTPUT,
        ITEM_CBO_SYNC_DEFIBRILLATION,
        ITEM_CBO_SPO2_CONFIGURE,
        ITEM_CBO_NETWORK_DEBUG,
        ITEM_CBO_MAX
    };

    MachineConfigModuleContentPrivte()
                    : isSyncIBPCO(false)
    {
    }
    /**
     * @brief loadOptions
     */
    void loadOptions();

    /**
     * @brief configUpdateHint 配置更新提示
     */
    void configUpdateHint(void);

    /**
     * @brief setCombosBlockSignalStatus 设置cbo锁住信号状态
     * @param isBlockSignals
     */
    void setCombosBlockSignalStatus(bool isBlockSignals);

    QMap <MenuItem, ComboBox *> combos;

    QMap <MenuItem, int> itemChangedMap;

    QMap <MenuItem, int> itemInitMap;

    bool isSyncIBPCO;
};

void MachineConfigModuleContentPrivte::loadOptions()
{
    int index;
    QString moduleName;

    setCombosBlockSignalStatus(true);

    index = 0;
    machineConfig.getNumValue("ECG12LEADEnable", index);
    combos[ITEM_CBO_ECG12]->setCurrentIndex(index);
    itemChangedMap[ITEM_CBO_ECG12] = index;

    index = 0;
    moduleName.clear();
    machineConfig.getNumValue("SPO2Enable", index);
    if (index == 0)
    {
        combos[ITEM_CBO_SPO2]->setCurrentIndex(0);
    }
    else
    {
        machineConfig.getStrValue("SPO2", moduleName);
        index = combos[ITEM_CBO_SPO2]->findText(trs(moduleName));
        if (index)
        {
            combos[ITEM_CBO_SPO2]->setCurrentIndex(index);
        }
    }
    itemChangedMap[ITEM_CBO_SPO2] = index;

    index = 0;
    moduleName.clear();
    machineConfig.getNumValue("NIBPEnable", index);
    if (index == 0)
    {
        combos[ITEM_CBO_NIBP]->setCurrentIndex(0);
    }
    else
    {
        machineConfig.getStrValue("NIBP", moduleName);
        index = combos[ITEM_CBO_NIBP]->findText(trs(moduleName));
        if (index)
        {
            combos[ITEM_CBO_NIBP]->setCurrentIndex(index);
        }
    }
    itemChangedMap[ITEM_CBO_NIBP] = index;

    index = 0;
    machineConfig.getNumValue("RESPEnable", index);
    combos[ITEM_CBO_RESP]->setCurrentIndex(index);
    itemChangedMap[ITEM_CBO_RESP] = index;

    index = 0;
    machineConfig.getNumValue("CO2Enable", index);
    if (index == 0)
    {
        combos[ITEM_CBO_CO2]->setCurrentIndex(0);
    }
    else
    {
        machineConfig.getStrValue("CO2", moduleName);
        index = combos[ITEM_CBO_CO2]->findText(trs(moduleName));
        if (index)
        {
            combos[ITEM_CBO_CO2]->setCurrentIndex(index);
        }
    }
    itemChangedMap[ITEM_CBO_CO2] = index;

    index = 0;
    machineConfig.getNumValue("AGEnable", index);
    combos[ITEM_CBO_AG]->setCurrentIndex(index);
    itemChangedMap[ITEM_CBO_AG] = index;

    index = 0;
    machineConfig.getNumValue("COEnable", index);
    combos[ITEM_CBO_CO]->setCurrentIndex(index);
    itemChangedMap[ITEM_CBO_CO] = index;

    index = 0;
    machineConfig.getNumValue("IBPEnable", index);
    if (index == 0)
    {
        combos[ITEM_CBO_IBP]->setCurrentIndex(0);
    }
    else
    {
        machineConfig.getStrValue("IBP", moduleName);
        index = combos[ITEM_CBO_IBP]->findText(trs(moduleName));
        if (index > 0)
        {
            combos[ITEM_CBO_IBP]->setCurrentIndex(index);
        }
    }
    itemChangedMap[ITEM_CBO_IBP] = index;

#ifdef ENABLE_O2_APNEASTIMULATION
    index = 0;
    machineConfig.getNumValue("O2Enable", index);
    combos[ITEM_CBO_O2]->setCurrentIndex(index);
    itemChangedMap[ITEM_CBO_O2] = index;
#endif

#ifdef Q_WS_QWS
    index = 0;
    machineConfig.getNumValue("TouchEnable", index);
    combos[ITEM_CBO_TSCREEN]->setCurrentIndex(index);
    itemChangedMap[ITEM_CBO_TSCREEN] = index;
    if (systemManager.isOnlySupportTouchScreen())
    {
        combos[ITEM_CBO_TSCREEN]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_TSCREEN]->setEnabled(true);
    }

#endif

    index = 0;
    machineConfig.getNumValue("TEMPEnable", index);
    combos[ITEM_CBO_TEMP]->setCurrentIndex(index);
    itemChangedMap[ITEM_CBO_TEMP] = index;

    index = 0;
    machineConfig.getNumValue("PrinterEnable", index);
    combos[ITEM_CBO_PRINTER]->setCurrentIndex(index);
    itemChangedMap[ITEM_CBO_PRINTER] = index;

    index = 0;
    machineConfig.getNumValue("WIFIEnable", index);
    combos[ITEM_CBO_WIFI]->setCurrentIndex(index);
    itemChangedMap[ITEM_CBO_WIFI] = index;

    // load screen type
    index = 0;
    machineConfig.getNumValue("BacklightAdjustment", index);
    combos[ITEM_CBO_BACKLIGHT]->setCurrentIndex(index);

    index = 0;
    machineConfig.getNumValue("NIBPNEOMeasureEnable", index);
    combos[ITEM_CBO_NIBP_NEO_MEASURE]->setCurrentIndex(index);

    index = 0;
    machineConfig.getNumValue("HDMIEnable", index);
    combos[ITEM_CBO_HDMI]->setCurrentIndex(index);

    index = 0;
    machineConfig.getNumValue("NurseCallEnable", index);
    combos[ITEM_CBO_NURSE_CALL]->setCurrentIndex(index);

    index = 0;
    machineConfig.getNumValue("AnalogOutputEnable", index);
    combos[ITEM_CBO_ANALOG_OUTPUT]->setCurrentIndex(index);

    index = 0;
    machineConfig.getNumValue("SyncDefibrillationEnable", index);
    combos[ITEM_CBO_SYNC_DEFIBRILLATION]->setCurrentIndex(index);

    index = 0;
    if (QFile::exists("/etc/init.d/S50telnet") && QFile::exists("/etc/init.d/S50dropbear"))
    {
        index = 1;
    }
    else
    {
        index = 0;
    }
    combos[ITEM_CBO_NETWORK_DEBUG]->setCurrentIndex(index);
    itemChangedMap[ITEM_CBO_NETWORK_DEBUG] = index;



#ifdef HIDE_MACHINE_CONFIG_ITEMS
    combos[ITEM_CBO_ECG12]->setCurrentIndex(0);
    combos[ITEM_CBO_ECG12]->setEnabled(false);
    combos[ITEM_CBO_AG]->setCurrentIndex(0);
    combos[ITEM_CBO_AG]->setEnabled(false);
    combos[ITEM_CBO_WIFI]->setCurrentIndex(0);
    combos[ITEM_CBO_WIFI]->setEnabled(false);
    combos[ITEM_CBO_ANALOG_OUTPUT]->setCurrentIndex(0);
    combos[ITEM_CBO_ANALOG_OUTPUT]->setEnabled(false);
    combos[ITEM_CBO_SYNC_DEFIBRILLATION]->setCurrentIndex(0);
    combos[ITEM_CBO_SYNC_DEFIBRILLATION]->setEnabled(false);
#endif

    QStringList screenInfoList = machineConfig.getChildNodeNameList("ScreenInfo");
    combos[ITEM_CBO_SCREEN_INFO]->clear();
    foreach(QString screenInfo, screenInfoList)
    {
        combos[ITEM_CBO_SCREEN_INFO]->addItem(trs(screenInfo));
    }
    index = 0;
    machineConfig.getNumValue("ScreenInfoEnable", index);
    combos[ITEM_CBO_SCREEN_INFO]->setCurrentIndex(index);

    // 新生儿专用机器
    combos[ITEM_CBO_NEO_MACHINE]->clear();
    combos[ITEM_CBO_NEO_MACHINE]->addItems(QStringList()
                                           << trs("No")
                                           << trs("Yes"));
    index = 0;
    machineConfig.getNumValue("NeonateMachine", index);
    combos[ITEM_CBO_NEO_MACHINE]->setCurrentIndex(index);

    index = 0;
    machineConfig.getNumValue("SpO2ConfigureEnable", index);
    combos[ITEM_CBO_SPO2_CONFIGURE]->setCurrentIndex(index);

    itemInitMap = itemChangedMap;

    setCombosBlockSignalStatus(false);
}

/**
 * @brief setNetworkLogin set can login network or not
 * @brief status allow login or not allowed
 */

static void setNetworkLogin(bool status)
{
    if (status)
    {
        QFile::rename("/etc/init.d/_S50dropbear", "/etc/init.d/S50dropbear");
        QFile::rename("/etc/init.d/_S50telnet", "/etc/init.d/S50telnet");
    }
    else
    {
        QFile::rename("/etc/init.d/S50dropbear", "/etc/init.d/_S50dropbear");
        QFile::rename("/etc/init.d/S50telnet", "/etc/init.d/_S50telnet");
    }
}

void MachineConfigModuleContentPrivte::configUpdateHint()
{
    QString hints = trs("MachineConfigIsUpdatedNow");
    hints += "\n";
    hints += trs("IsRebootMachine");
    MessageBox box(trs("UpdateTips"), hints);
    QDialog::DialogCode statue = static_cast<QDialog::DialogCode>(box.exec());
    if (statue == QDialog::Accepted)
    {
        systemBoardProvider.requestReset();
    }
}

void MachineConfigModuleContentPrivte::setCombosBlockSignalStatus(bool isBlockSignals)
{
    for (int i = ITEM_CBO_ECG12; i < ITEM_CBO_MAX; i++)
    {
       MenuItem item = static_cast<MenuItem>(i);
       combos[item]->blockSignals(isBlockSignals);
    }
}

MachineConfigModuleContent::MachineConfigModuleContent()
    : MenuContent(trs("MachineConfigModuleMenu"),
                  trs("MachineConfigModuleMenuDesc")),
      d_ptr(new MachineConfigModuleContentPrivte)
{
}

MachineConfigModuleContent::~MachineConfigModuleContent()
{
    delete d_ptr;
}

void MachineConfigModuleContent::readyShow()
{
    d_ptr->loadOptions();
}

void MachineConfigModuleContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *combo;
    int itemId;

    // ecg12lead
    label = new QLabel(trs("ECG12LeadsModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_ECG12, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_ECG12;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // spo2 module
    label = new QLabel(trs("SPO2Module"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs(SPO2Symbol::convert(MODULE_BLM_S5))
                    << trs(SPO2Symbol::convert(MODULE_MASIMO_SPO2))
                    << trs(SPO2Symbol::convert(MODULE_RAINBOW_SPO2)));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_SPO2, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_SPO2;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // nibp module
    label = new QLabel(trs("NIBPModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs(NIBPSymbol::convert(MODULE_N5))
                    << trs(NIBPSymbol::convert(MODULE_SUNTECH_NIBP)));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_NIBP, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_NIBP;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // resp module
    label = new QLabel(trs("RESPModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_RESP, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_RESP;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // co2 module
    label = new QLabel(trs("CO2Module"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs(CO2Symbol::convert(MODULE_BLM_CO2))
                    << trs(CO2Symbol::convert(MODULE_MASIMO_CO2)));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_CO2, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_CO2;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // ag module
    label = new QLabel(trs("AGModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_AG, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_AG;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // co module
    label = new QLabel(trs("COModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_CO, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_CO;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // ibp module
    label = new QLabel(trs("IBPModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs(IBPSymbol::convert(IBP_MODULE_SMART_IBP)));
//                    << trs(IBPSymbol::convert(IBP_MODULE_WITLEAF))
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_IBP, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_IBP;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

#ifdef ENABLE_O2_APNEASTIMULATION
    // O2 module
    label = new QLabel(trs("O2Module"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_O2, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_O2;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
#endif

    // temp module
    label = new QLabel(trs("TEMPModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_TEMP, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_TEMP;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // printer module
    label = new QLabel(trs("PrinterModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_PRINTER, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_PRINTER;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // wifi module
    label = new QLabel(trs("WIFIModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_WIFI, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_WIFI;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

#ifdef Q_WS_QWS
    // touch screen module
    label = new QLabel(trs("TouchScreenModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->blockSignals(true);
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("ResistiveScreen")
                    << trs("CapacitiveScreen"));
    combo->blockSignals(false);
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_TSCREEN, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_TSCREEN;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
#endif

    // Backlight Regulation
    label = new QLabel(trs("BacklightAdjustment"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->blockSignals(true);
    combo->addItems(QStringList()
                    << trs(SystemSymbol::convert(BACKLIGHT_MODE_1))
                    << trs(SystemSymbol::convert(BACKLIGHT_MODE_2)));
    combo->blockSignals(false);
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_BACKLIGHT, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_BACKLIGHT;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    layout->setRowStretch(d_ptr->combos.count(), 1);

    label = new QLabel(trs("HDMI"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->blockSignals(true);
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    combo->blockSignals(false);
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_HDMI, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_HDMI;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("NurseCall"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->blockSignals(true);
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    combo->blockSignals(false);
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_NURSE_CALL, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_NURSE_CALL;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("AnalogOutput"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->blockSignals(true);
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    combo->blockSignals(false);
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_ANALOG_OUTPUT, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_ANALOG_OUTPUT;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("SyncDefibrillation"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->blockSignals(true);
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    combo->blockSignals(false);
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_SYNC_DEFIBRILLATION, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_SYNC_DEFIBRILLATION;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("NeoNIBPMeasure"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->blockSignals(true);
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    combo->blockSignals(false);
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_NIBP_NEO_MEASURE, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_NIBP_NEO_MEASURE;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // machine model
    label = new QLabel(trs("ScreenSize"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte::ITEM_CBO_SCREEN_INFO, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_SCREEN_INFO;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // special machine for neonate
    label = new QLabel(trs("NeoMachine"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte::ITEM_CBO_NEO_MACHINE, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_NEO_MACHINE;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // spo2 configure
    label = new QLabel(trs("SpO2Configure"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("LowConfigure")
                    << trs("HighConfigure"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte::ITEM_CBO_SPO2_CONFIGURE, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_SPO2_CONFIGURE;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("NetworkDebug"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->blockSignals(true);
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    combo->blockSignals(false);
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte::ITEM_CBO_NETWORK_DEBUG, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_NETWORK_DEBUG;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));


    layout->setRowStretch(d_ptr->combos.count(), 1);
}

void MachineConfigModuleContent::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);

    if (d_ptr->itemChangedMap != d_ptr->itemInitMap)
    {
        // 添加机器配置更新提示
        d_ptr->configUpdateHint();
    }
}

void MachineConfigModuleContent::onComboBoxIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    int indexType = combo->property("Item").toInt();
    QString enablePath("");
    QString modulePath("");
    QString moduleName("");
    switch (indexType)
    {
        case MachineConfigModuleContentPrivte::ITEM_CBO_ECG12:
        {
            enablePath = "ECG12LEADEnable";
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_SPO2:
        {
            enablePath = "SPO2Enable";
            modulePath = "SPO2";
            if (index > 0)
            {
                moduleName = SPO2Symbol::convert(static_cast<SPO2ModuleType>(index - 1));
            }
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_NIBP:
        {
            enablePath = "NIBPEnable";
            modulePath = "NIBP";
            if (index > 0)
            {
                moduleName = NIBPSymbol::convert(static_cast<NIBPModuleType>(index - 1));
            }
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_RESP:
        {
            enablePath = "RESPEnable";
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_CO2:
        {
            enablePath = "CO2Enable";
            modulePath = "CO2";
            if (index > 0)
            {
                moduleName = CO2Symbol::convert(static_cast<CO2ModuleType>(index - 1));
            }
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_AG:
        {
            enablePath = "AGEnable";
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_CO:
        {
            enablePath = "COEnable";
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_IBP:
        {
            enablePath = "IBPEnable";
            modulePath = "IBP";
            if (index > 0)
            {
                moduleName = IBPSymbol::convert(static_cast<IBPModuleType>(index - 1));
            }
            break;
        }
#ifdef ENABLE_O2_APNEASTIMULATION
        case MachineConfigModuleContentPrivte::ITEM_CBO_O2:
        {
            enablePath = "O2Enable";
            break;
        }
#endif
        case MachineConfigModuleContentPrivte::ITEM_CBO_TEMP:
        {
            enablePath = "TEMPEnable";
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_PRINTER:
        {
            enablePath = "PrinterEnable";
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_WIFI:
        {
            enablePath = "WIFIEnable";
            break;
        }
#ifdef Q_WS_QWS
        case MachineConfigModuleContentPrivte::ITEM_CBO_TSCREEN:
            enablePath = "TouchEnable";
            machineConfig.saveToDisk();
            systemManager.setTouchScreenOnOff(index);
            systemManager.configTouchScreen(index);
            break;
#endif
        case MachineConfigModuleContentPrivte::ITEM_CBO_BACKLIGHT:
        {
            machineConfig.setNumValue("BacklightAdjustment", index);
            machineConfig.saveToDisk();
#ifdef Q_WS_QWS
            BrightnessLevel br = systemManager.getBrightness();
            systemManager.setBrightness(br);
#endif
            return;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_NIBP_NEO_MEASURE:
        {
             enablePath = "NIBPNEOMeasureEnable";
             break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_SCREEN_INFO:
        {
            enablePath = "ScreenInfoEnable";
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_NEO_MACHINE:
        {
            enablePath = "NeonateMachine";
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_HDMI:
        {
             enablePath = "HDMIEnable";
             break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_NURSE_CALL:
        {
             enablePath = "NurseCallEnable";
             break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_ANALOG_OUTPUT:
        {
             enablePath = "AnalogOutputEnable";
             break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_SYNC_DEFIBRILLATION:
        {
             enablePath = "SyncDefibrillationEnable";
             break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_SPO2_CONFIGURE:
        {
            enablePath = "SpO2ConfigureEnable";
            break;
        }
        case MachineConfigModuleContentPrivte::ITEM_CBO_NETWORK_DEBUG:
        {
            d_ptr->itemChangedMap[MachineConfigModuleContentPrivte::ITEM_CBO_NETWORK_DEBUG] = index;
            setNetworkLogin(index);
            break;
        }

        default:
            return;
    }

    if (enablePath.isEmpty())
    {
        return;
    }

    int enableIndex = index;
    if (index == 0)
    {
        enableIndex = 0;
    }
    else if (index > 0 &&
             enablePath != "TouchEnable" &&
             enablePath != "ScreenInfoEnable")
    {
        enableIndex = 1;
    }
    machineConfig.setNumValue(enablePath, enableIndex);

    // ibp模块和co模块使能位形成联动
    if (d_ptr->isSyncIBPCO)
    {
        if (enablePath == "COEnable")
        {
            machineConfig.setNumValue("IBPEnable", index);
            MachineConfigModuleContentPrivte::MenuItem item;
            item = MachineConfigModuleContentPrivte::ITEM_CBO_IBP;
            d_ptr->combos[item]->blockSignals(true);
            d_ptr->combos[MachineConfigModuleContentPrivte::ITEM_CBO_IBP]
                    ->setCurrentIndex(index);
            d_ptr->combos[MachineConfigModuleContentPrivte::ITEM_CBO_IBP]
                    ->blockSignals(false);
        }
        else if (enablePath == "IBPEnable")
        {
            machineConfig.setNumValue("COEnable", index);
            d_ptr->combos[MachineConfigModuleContentPrivte::ITEM_CBO_CO]
                    ->blockSignals(true);
            d_ptr->combos[MachineConfigModuleContentPrivte::ITEM_CBO_CO]
                    ->setCurrentIndex(index);
            d_ptr->combos[MachineConfigModuleContentPrivte::ITEM_CBO_CO]
                    ->blockSignals(false);
        }
    }

    if (index && !modulePath.isEmpty() && !moduleName.isEmpty())
    {
        machineConfig.setStrValue(modulePath, moduleName);
    }
    machineConfig.saveToDisk();

    MachineConfigModuleContentPrivte::MenuItem item =
            static_cast<MachineConfigModuleContentPrivte::MenuItem>(indexType);
    d_ptr->itemChangedMap[item] = index;
}
