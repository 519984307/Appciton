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
#include "ComboBox.h"
#include "LanguageManager.h"
#include "IConfig.h"

class MachineConfigModuleContentPrivte
{
public:
    enum MenuItem
    {
        ITEM_CBO_IBP_CO = 0,
        ITEM_CBO_CO2,
        ITEM_CBO_ECG12,
        ITEM_CBO_PRINTER,
        ITEM_CBO_HDMI,
        ITEM_CBO_WIFI,
        ITEM_CBO_SCREEN,
        ITEM_CBO_NUSERCALL,
        ITEM_CBO_ANANLOG,
    };

    MachineConfigModuleContentPrivte();
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
};

MachineConfigModuleContentPrivte::MachineConfigModuleContentPrivte()
{
    combos.clear();
}

void MachineConfigModuleContentPrivte::loadOptions()
{
    int index;

    index = 0;
    systemConfig.getNumValue("MachineConfigModule|IBPModule", index);
    combos[ITEM_CBO_IBP_CO]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("MachineConfigModule|CO2Module", index);
    combos[ITEM_CBO_CO2]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("MachineConfigModule|ECG12LeadsModule", index);
    combos[ITEM_CBO_ECG12]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("MachineConfigModule|PrinterModule", index);
    combos[ITEM_CBO_PRINTER]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("MachineConfigModule|HDMIModule", index);
    combos[ITEM_CBO_HDMI]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("MachineConfigModule|WifiModule", index);
    combos[ITEM_CBO_WIFI]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("MachineConfigModule|ScreenModule", index);
    combos[ITEM_CBO_SCREEN]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("MachineConfigModule|NurseCallingModule", index);
    combos[ITEM_CBO_NUSERCALL]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("MachineConfigModule|AnalogOutputModule", index);
    combos[ITEM_CBO_ANANLOG]->setCurrentIndex(index);
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

    label = new QLabel(trs("IBPCOModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("M601")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_IBP_CO, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_IBP_CO;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("CO2Module"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("PHASEIN")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_CO2, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_CO2;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("ECG12LeadsModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("Enable")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_ECG12, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_ECG12;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("PrinterModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("Enable")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_PRINTER, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_PRINTER;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("HDMIModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("Enable")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_HDMI, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_HDMI;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("WifiModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("Enable")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_WIFI, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_WIFI;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("ScreenModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("Enable")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_SCREEN, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_SCREEN;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("NurseCallingModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("Enable")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_NUSERCALL, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_NUSERCALL;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("AnalogOutputModule"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("Enable")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(MachineConfigModuleContentPrivte
                         ::ITEM_CBO_ANANLOG, combo);
    itemId = MachineConfigModuleContentPrivte::ITEM_CBO_ANANLOG;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    layout->setRowStretch(d_ptr->combos.count(), 1);
}

void MachineConfigModuleContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    int indexType = combo->property("Item").toInt();
    QString str;
    switch (indexType)
    {
    case MachineConfigModuleContentPrivte::ITEM_CBO_IBP_CO:     // IBP和CO为模块一同设置
        systemConfig.setNumValue(QString("MachineConfigModule|IBPModule"), index);
        systemConfig.setNumValue(QString("MachineConfigModule|COModule"), index);
        return;
    case MachineConfigModuleContentPrivte::ITEM_CBO_CO2:
        str = "CO2Module";
        break;
    case MachineConfigModuleContentPrivte::ITEM_CBO_ECG12:
        str = "ECG12LeadsModule";
        break;
    case MachineConfigModuleContentPrivte::ITEM_CBO_PRINTER:
        str = "PrinterModule";
        break;
    case MachineConfigModuleContentPrivte::ITEM_CBO_HDMI:
        str = "HDMIModule";
        break;
    case MachineConfigModuleContentPrivte::ITEM_CBO_WIFI:
        str = "WifiModule";
        break;
    case MachineConfigModuleContentPrivte::ITEM_CBO_SCREEN:
        str = "ScreenModule";
        break;
    case MachineConfigModuleContentPrivte::ITEM_CBO_NUSERCALL:
        str = "NurseCallingModule";
        break;
    case MachineConfigModuleContentPrivte::ITEM_CBO_ANANLOG:
        str = "AnalogOutputModule";
        d_ptr->combos[MachineConfigModuleContentPrivte::ITEM_CBO_ANANLOG]->setEnabled(!index);
        break;
    default:
        return;
    }
    systemConfig.setNumValue(QString("MachineConfigModule|%1").arg(str), index);
}



























