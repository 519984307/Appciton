
/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.13  11:30
 **/
#include "SelectDefaultConfigMenuContent.h"
#include "ConfigManager.h"
#include "IConfig.h"
#include "ComboBox.h"
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include "LanguageManager.h"
#include <QList>
#include "Debug.h"

class SelectDefaultConfigMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ADULT_DEFCONFIG = 0,
        ITEM_CBO_PED_DEFCONFIG,
        ITEM_CBO_NEO_DEFCONFIG,
        ITEM_CBO_MENU_MAX,
    };
    SelectDefaultConfigMenuContentPrivate();
    ~SelectDefaultConfigMenuContentPrivate() {}
    /**
     * @brief updateUserConfig
     */
    void updateUserConfig();

    QMap <MenuItem, ComboBox *> combos;
};

SelectDefaultConfigMenuContentPrivate::SelectDefaultConfigMenuContentPrivate()
{
    combos.clear();
}
SelectDefaultConfigMenuContent::SelectDefaultConfigMenuContent():
    MenuContent(trs("DefaultConfig"),
                trs("DefaultConfigDesc")),
    d_ptr(new SelectDefaultConfigMenuContentPrivate)
{
    connect(&configManager, SIGNAL(userDefineConfigChanged()), this, SLOT(onUserDefinedConfigChanged()));
}

SelectDefaultConfigMenuContent::~SelectDefaultConfigMenuContent()
{
    delete d_ptr;
}

void SelectDefaultConfigMenuContentPrivate::updateUserConfig()
{
    QList<ConfigManager::UserDefineConfigInfo> infos =
        configManager.getUserDefineConfigInfos();
    QString type;
    QString prefix = "ConfigManager|Default|";

    for (int i = ITEM_CBO_ADULT_DEFCONFIG; i < ITEM_CBO_MENU_MAX; i++)
    {
        MenuItem menuItem = static_cast<MenuItem>(i);
        combos[menuItem]->blockSignals(true);
        //  remove the previous user define configs
        while (combos[menuItem]->count() > 2)
        {
            combos[menuItem]->removeItem(combos[menuItem]->count() - 1);
        }

        //  load new user define configs
        QString patientType = PatientSymbol::convert(static_cast<PatientType>(i));
        for (int j = 0; j < infos.size(); j++)
        {
            // 剔除病人类型不匹配的item
            if (infos.at(j).patType != patientType)
            {
                continue;
            }
            combos[menuItem]->addItem(infos.at(j).name);
        }

        QString nodeName = prefix + combos[menuItem]->property("nodeName").toString();
        switch (menuItem)
        {
        case ITEM_CBO_ADULT_DEFCONFIG:
            nodeName += PatientSymbol::convert(PATIENT_TYPE_ADULT);
            break;
        case ITEM_CBO_PED_DEFCONFIG:
            nodeName += PatientSymbol::convert(PATIENT_TYPE_PED);;
            break;
        case ITEM_CBO_NEO_DEFCONFIG:
            nodeName += PatientSymbol::convert(PATIENT_TYPE_NEO);;
            break;
        default:
            break;
        }

        systemConfig.getStrAttr(nodeName, "type", type);
        if (type.toUpper() == "FACTORY")
        {
            combos[menuItem]->setCurrentIndex(0);
        }
        else if (type.toUpper() == "CURRENT")
        {
            combos[menuItem]->setCurrentIndex(1);
        }
        else
        {
            QString name;
            QString filename;
            systemConfig.getStrAttr(nodeName, "name", name);
            systemConfig.getStrValue(nodeName, filename);
            int j = 0;
            for (; j < infos.size(); j++)
            {
                if (name == infos.at(j).name && filename == infos.at(j).fileName)
                {
                    break;
                }
            }

            if (j >= infos.size())
            {
                //  not found in user ocnfig, set to default
                combos[menuItem]->setCurrentIndex(0);
            }
            else
            {
                combos[menuItem]->setCurrentIndex(j + 2);
            }
        }
        combos[menuItem]->blockSignals(false);
    }
}
void SelectDefaultConfigMenuContent::readyShow()
{
    //  reload the user configs
    d_ptr->updateUserConfig();
}
void SelectDefaultConfigMenuContent::onCurrentIndexChanged(int index)
{
    ComboBox *combo = qobject_cast <ComboBox *> (sender());

    if (combo)
    {
        QString nodeIndex = "ConfigManager|Default|";
        QList<ConfigManager::UserDefineConfigInfo> infos = configManager.getUserDefineConfigInfos();
        ConfigManager::UserDefineConfigInfo info;
        int itemId = combo->property("Item").toInt();
        switch (itemId)
        {
        case 0:
            nodeIndex = nodeIndex + PatientSymbol::convert(PATIENT_TYPE_ADULT);
            break;
        case 1:
            nodeIndex = nodeIndex + PatientSymbol::convert(PATIENT_TYPE_PED);
            break;
        case 2:
            nodeIndex = nodeIndex + PatientSymbol::convert(PATIENT_TYPE_NEO);
            break;
        default:
            nodeIndex = nodeIndex + "";
            break;
        }
        switch (index)
        {
        case 0:
            //  factory
            systemConfig.setStrAttr(nodeIndex, "type", "Factory");
            systemConfig.setStrAttr(nodeIndex, "name", "");
            systemConfig.setStrValue(nodeIndex, combo->property("factoryConfigFile").toString());
            break;
        case 1:
            //  current
            systemConfig.setStrAttr(nodeIndex, "type", "Current");
            systemConfig.setStrAttr(nodeIndex, "name", "");
            systemConfig.setStrValue(nodeIndex, combo->property("currentConfigFile").toString());
            break;
        default:
            //  user define
            if (index >= infos.size() + 2 && index < 0)
            {
                qdebug("index out of range.");
                break;
            }
            info = infos.at(index - 2);
            systemConfig.setStrAttr(nodeIndex, "type", "User");
            systemConfig.setStrAttr(nodeIndex, "name", info.name);
            systemConfig.setStrValue(nodeIndex, info.fileName);
            break;
        }

        systemConfig.requestSave();
    }
}
void SelectDefaultConfigMenuContent::onUserDefinedConfigChanged()
{
    //  reload the user configs
    d_ptr->updateUserConfig();
}
void SelectDefaultConfigMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *combox;
    int itemID;

    //  adult default config
    label = new QLabel(trs("AdultDefaultConfig"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combox = new ComboBox;
    combox->addItems(QStringList()
                     << trs("FactoryConfig")
                     << trs("CurrentConfig"));
    layout->addWidget(combox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SelectDefaultConfigMenuContentPrivate
                         ::ITEM_CBO_ADULT_DEFCONFIG, combox);
    connect(combox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    itemID = SelectDefaultConfigMenuContentPrivate::ITEM_CBO_ADULT_DEFCONFIG;
    combox->setProperty("Item", qVariantFromValue(itemID));
    combox->setProperty("factoryConfigFile",
                        qVariantFromValue(configManager.factoryConfigFilename(PATIENT_TYPE_ADULT)));
    combox->setProperty("currentConfigFile",
                        qVariantFromValue(configManager.runningConfigFilename(PATIENT_TYPE_ADULT)));

    //  ped default config
    label = new QLabel(trs("PedDefaultConfig"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combox = new ComboBox;
    combox->addItems(QStringList()
                     << trs("FactoryConfig")
                     << trs("CurrentConfig"));
    layout->addWidget(combox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SelectDefaultConfigMenuContentPrivate
                         ::ITEM_CBO_PED_DEFCONFIG, combox);
    connect(combox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    itemID = SelectDefaultConfigMenuContentPrivate::ITEM_CBO_PED_DEFCONFIG;
    combox->setProperty("Item", qVariantFromValue(itemID));
    combox->setProperty("factoryConfigFile", qVariantFromValue(configManager.factoryConfigFilename(PATIENT_TYPE_PED)));
    combox->setProperty("currentConfigFile", qVariantFromValue(configManager.runningConfigFilename(PATIENT_TYPE_PED)));

    //  neo default config
    label = new QLabel(trs("NeoDefaultConfig"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combox = new ComboBox;
    combox->addItems(QStringList()
                     << trs("FactoryConfig")
                     << trs("CurrentConfig"));
    layout->addWidget(combox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SelectDefaultConfigMenuContentPrivate
                         ::ITEM_CBO_NEO_DEFCONFIG, combox);
    connect(combox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    itemID = SelectDefaultConfigMenuContentPrivate::ITEM_CBO_NEO_DEFCONFIG;
    combox->setProperty("Item", qVariantFromValue(itemID));
    combox->setProperty("factoryConfigFile", qVariantFromValue(configManager.factoryConfigFilename(PATIENT_TYPE_NEO)));
    combox->setProperty("currentConfigFile", qVariantFromValue(configManager.runningConfigFilename(PATIENT_TYPE_NEO)));

    //  add a stretch
    layout->setRowStretch(d_ptr->combos.count(), 1);

    // disable these options boxes for fixing to the current configuration
    d_ptr->combos[SelectDefaultConfigMenuContentPrivate::ITEM_CBO_ADULT_DEFCONFIG]->setEnabled(false);
    d_ptr->combos[SelectDefaultConfigMenuContentPrivate::ITEM_CBO_PED_DEFCONFIG]->setEnabled(false);
    d_ptr->combos[SelectDefaultConfigMenuContentPrivate::ITEM_CBO_NEO_DEFCONFIG]->setEnabled(false);
}
