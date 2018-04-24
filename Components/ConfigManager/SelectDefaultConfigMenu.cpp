#include "SelectDefaultConfigMenu.h"
#include "IComboList.h"
#include "ConfigManager.h"
#include "ConfigMaintainMenuGrp.h"
#include "IConfig.h"

class SelectDefaultConfigMenuPrivate
{
public:
    SelectDefaultConfigMenuPrivate()
        :adultDefConfg(NULL),
          pedDefConfig(NULL),
          neoDefCongig(NULL)
    {}

    void updateUserConfig();

    IComboList *adultDefConfg;
    IComboList *pedDefConfig;
    IComboList *neoDefCongig;
};

SelectDefaultConfigMenu::SelectDefaultConfigMenu()
    :SubMenu(trs("DefaultConfig1")),
      d_ptr(new SelectDefaultConfigMenuPrivate())
{
    setDesc(trs("SelectDefaultConfig"));
    startLayout();
}

SelectDefaultConfigMenu::~SelectDefaultConfigMenu()
{

}

void SelectDefaultConfigMenuPrivate::updateUserConfig()
{
    QList<ConfigManager::UserDefineConfigInfo> infos = configManager.getUserDefineConfigInfos();
    IComboList *combos[] = {
        adultDefConfg,
        pedDefConfig,
        neoDefCongig
    };
    QString type;
    QString prefix = "ConfigManager|Default|";

    for(unsigned int i = 0; i < sizeof(combos) / sizeof(combos[0]); i++)
    {
        combos[i]->blockSignals(true);
        //remove the previous user define configs
        while(combos[i]->count() > 2)
        {
            combos[i]->removeItem(combos[i]->count() - 1);
        }

        //load new user define configs
        for(int j = 0; j < infos.size(); j++)
        {
            combos[i]->addItem(infos.at(j).name);
        }

        QString index = prefix + combos[i]->property("nodeName").toString();
        systemConfig.getStrAttr(index, "type", type);
        if(type.toUpper() == "FACTORY")
        {
            combos[i]->setCurrentIndex(0);
        }
        else if(type.toUpper() == "CURRENT")
        {
            combos[i]->setCurrentIndex(1);
        }
        else
        {
            QString name;
            QString filename;
            systemConfig.getStrAttr(index, "name", name);
            systemConfig.getStrValue(index, filename);
            int j = 0;
            for(; j < infos.size(); j++)
            {
                if(name == infos.at(j).name && filename == infos.at(j).fileName)
                {
                    break;
                }
            }

            if(j >= infos.size())
            {
                //not found in user ocnfig, set to default
                combos[i]->setCurrentIndex(0);
            }
            else
            {
                combos[i]->setCurrentIndex(j + 2);
            }
        }
        combos[i]->blockSignals(false);
    }
}

void SelectDefaultConfigMenu::layoutExec()
{
    int submenuW = configMaintainMenuGrp.getSubmenuWidth();
    int submenuH = configMaintainMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW -  ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    IComboList *combo;

    combo = new IComboList(trs("AdultDefaultConfig"));
    combo->label->setFixedSize(labelWidth, ITEM_H);
    combo->combolist->setFixedSize(btnWidth, ITEM_H);
    combo->setFont(fontManager.textFont(fontSize));
    combo->addItem(trs("FactoryConfig"));
    combo->addItem(trs("CurrentConfig"));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    combo->setProperty("nodeName", qVariantFromValue(QString(PatientSymbol::convert(PATIENT_TYPE_ADULT))));
    combo->setProperty("factoryConfigFile", qVariantFromValue(configManager.factoryConfigFilename(PATIENT_TYPE_ADULT)));
    combo->setProperty("currentConfigFile", qVariantFromValue(configManager.runningConfigFilename(PATIENT_TYPE_ADULT)));
    mainLayout->addWidget(combo);
    d_ptr->adultDefConfg = combo;

    combo = new IComboList(trs("PedDefaultConfig"));
    combo->label->setFixedSize(labelWidth, ITEM_H);
    combo->combolist->setFixedSize(btnWidth, ITEM_H);
    combo->setFont(fontManager.textFont(fontSize));
    combo->addItem(trs("FactoryConfig"));
    combo->addItem(trs("CurrentConfig"));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    combo->setProperty("nodeName", qVariantFromValue(QString(PatientSymbol::convert(PATIENT_TYPE_PED))));
    combo->setProperty("factoryConfigFile", qVariantFromValue(configManager.factoryConfigFilename(PATIENT_TYPE_PED)));
    combo->setProperty("currentConfigFile", qVariantFromValue(configManager.runningConfigFilename(PATIENT_TYPE_PED)));
    mainLayout->addWidget(combo);
    d_ptr->pedDefConfig = combo;

    combo = new IComboList(trs("NeoDefaultConfig"));
    combo->label->setFixedSize(labelWidth, ITEM_H);
    combo->combolist->setFixedSize(btnWidth, ITEM_H);
    combo->setFont(fontManager.textFont(fontSize));
    combo->addItem(trs("FactoryConfig"));
    combo->addItem(trs("CurrentConfig"));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    combo->setProperty("nodeName", qVariantFromValue(QString(PatientSymbol::convert(PATIENT_TYPE_NEO))));
    combo->setProperty("factoryConfigFile", qVariantFromValue(configManager.factoryConfigFilename(PATIENT_TYPE_NEO)));
    combo->setProperty("currentConfigFile", qVariantFromValue(configManager.runningConfigFilename(PATIENT_TYPE_NEO)));
    mainLayout->addWidget(combo);
    d_ptr->neoDefCongig = combo;

    d_ptr->updateUserConfig();
    connect(&configManager, SIGNAL(userDefineConfigChanged()), this, SLOT(onUserDefinedConfigChanged()));
}

void SelectDefaultConfigMenu::onCurrentIndexChanged(int index)
{
   IComboList *combo = qobject_cast<IComboList *> (sender());
   if(combo)
   {
       QString nodeIndex = "ConfigManager|Default|"  + combo->property("nodeName").toString();
       QList<ConfigManager::UserDefineConfigInfo> infos = configManager.getUserDefineConfigInfos();
       ConfigManager::UserDefineConfigInfo info;
       switch(index)
       {
       case 0:
           //factory
           systemConfig.setStrAttr(nodeIndex, "type", "Factory");
           systemConfig.setStrAttr(nodeIndex, "name", "");
           systemConfig.setStrValue(nodeIndex, combo->property("factoryConfigFile").toString());
           break;
       case 1:
           //current
           systemConfig.setStrAttr(nodeIndex, "type", "Current");
           systemConfig.setStrAttr(nodeIndex, "name", "");
           systemConfig.setStrValue(nodeIndex, combo->property("currentConfigFile").toString());
           break;
       default:
           //user define
           if(index >= infos.size() + 2 && index < 0) {
               qdebug("index out of range.");
               break;
           }
           info = infos.at(index - 2);
           systemConfig.setStrAttr(nodeIndex, "type", "User");
           systemConfig.setStrAttr(nodeIndex, "name", info.name);
           systemConfig.setStrValue(nodeIndex, info.fileName);
           break;
       }

       systemConfig.save();
   }
}

void SelectDefaultConfigMenu::onUserDefinedConfigChanged()
{
    //reload the user configs
    d_ptr->updateUserConfig();
}

