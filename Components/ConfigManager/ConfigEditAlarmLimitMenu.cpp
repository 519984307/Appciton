#include "ConfigEditAlarmLimitMenu.h"
#include "ConfigEditMenuGrp.h"
#include "AlarmLimitMenu.h"
#include "IConfig.h"
#include "PatientDefine.h"

class ConfigEditAlarmLimitMenuPrivate
{
public:
    ConfigEditAlarmLimitMenuPrivate()
    {
    }

    void loadOptions();

    QList<SetItem *> itemList;
};

void ConfigEditAlarmLimitMenuPrivate::loadOptions()
{
    int enable = 0;
    int low = 0;
    int high = 0;
    int step = 0;
    int tmp = 0;
    int scale = 0;

    Config *config = configEditMenuGrp.getCurrentEditConfig();
    if(!config)
    {
        return;
    }

    QString patStr;
    int type = 0;
    config->getNumValue("General|DefaultPatientType", type);
    patStr = PatientSymbol::convert((PatientType)type);

    int count = itemList.count();
    SetItem *item = NULL;
    for (int i = 0; i < count; i++)
    {
        item = itemList.at(i);
        if (NULL == item)
        {
            continue;
        }

        SubParamID subID = item->sid;
        QString prefix = "AlarmSource|" + patStr + "|";
        prefix += paramInfo.getSubParamName(subID, true);

        int type = (int)paramInfo.getUnitOfSubParam(subID);
        switch (subID)
        {
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_FICO2:
                config->getNumValue("Local|CO2Unit", type);
                break;
            case SUB_PARAM_NIBP_DIA:
            case SUB_PARAM_NIBP_MAP:
            case SUB_PARAM_NIBP_SYS:
                config->getNumValue("Local|NIBPUnit", type);
                break;
            case SUB_PARAM_T1:
            case SUB_PARAM_T2:
            case SUB_PARAM_TD:
                config->getNumValue("Local|TEMPUnit", type);
                break;
            default:
                break;
        }

        QString name = trs(paramInfo.getSubParamName(subID)) + " (";
        name += Unit::localeSymbol((UnitType)type) + ")";

        item->combo->label->setText(name);

        config->getNumAttr(prefix, "Enable", enable);

        prefix += "|";
        prefix += Unit::getSymbol((UnitType)type);
        config->getNumValue(prefix + "|Low", low);
        config->getNumValue(prefix + "|High", high);
        config->getNumValue(prefix + "|Step", step);
        config->getNumValue(prefix + "|Scale", scale);

        item->combo->setCurrentIndex(enable);

        ISpinMode mode = ISPIN_MODE_FLOAT;
        int lowMinValue = 0, lowMaxValue = 0, highMinValue = 0, highMaxValue = 0, stepValue = 0;
        double fLowMinValue = 0, fLowMaxValue = 0, fHighMinValue = 0, fHighMaxValue = 0, fStepValue = 0;
        if(scale == 1)
        {
            mode = ISPIN_MODE_INT;
        }

        item->lower->setMode(mode);
        item->upper->setMode(mode);
        if (ISPIN_MODE_INT == mode)
        {
            stepValue = step;
            config->getNumAttr(prefix + "|Low", "Min", lowMinValue);
            lowMaxValue = high - stepValue;
            highMinValue = low + stepValue;
            config->getNumAttr(prefix + "|High", "Max", highMaxValue);
            item->lower->setRange(lowMinValue, lowMaxValue);
            item->lower->setValue(low);
            item->lower->setStep(stepValue);
            item->upper->setRange(highMinValue, highMaxValue);
            item->upper->setValue(high);
            item->upper->setStep(stepValue);
        }
        else
        {
            fStepValue = (double)step / scale;
            config->getNumAttr(prefix + "|Low", "Min", tmp);
            fLowMinValue = (double)tmp / scale;
            fLowMaxValue = (double)high / scale - fStepValue;
            fHighMinValue = (double)low / scale + fStepValue;
            config->getNumAttr(prefix + "|High", "Max", tmp);
            fHighMaxValue = (double)tmp / scale;
            item->lower->setRange(fLowMinValue, fLowMaxValue);
            item->lower->setValue((double)low / scale);
            item->lower->setStep(fStepValue);
            item->upper->setRange(fHighMinValue, fHighMaxValue);
            item->upper->setValue((double)high / scale);
            item->upper->setStep(fStepValue);
        }
    }
}

ConfigEditAlarmLimitMenu::ConfigEditAlarmLimitMenu()
    : SubMenu(trs("AlarmLimitMenu")), d_ptr(new ConfigEditAlarmLimitMenuPrivate())
{
    setDesc(trs("AlarmLimitMenuDesc"));

    for(int i = 0; i < SUB_PARAM_NR; ++i)
    {
        int isEnable = 0;
        ParamID id = paramInfo.getParamID((SubParamID) i);
        QString paramName = paramInfo.getParamName(id);
        switch(id)
        {
        case PARAM_SPO2:
        case PARAM_NIBP:
        case PARAM_CO2:
        case PARAM_TEMP:
            machineConfig.getNumValue(paramName + "Enable", isEnable);
            break;
        case PARAM_DUP_RESP:
            if (systemManager.isSupport(CONFIG_CO2) || systemManager.isSupport(CONFIG_RESP))
            {
                isEnable = 1;
            }
            else
            {
                isEnable = 0;
            }
            break;
        default:
            isEnable = 1;
        }

        if (1 == isEnabled())
        {
            SetItem *item = new SetItem(id, (SubParamID)i);
            d_ptr->itemList.append(item);
        }
    }

    startLayout();
}

ConfigEditAlarmLimitMenu::~ConfigEditAlarmLimitMenu()
{

}

void ConfigEditAlarmLimitMenu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW - SCROLL_BAR_WIDTH, submenuH);
    setTitleEnable(true);
    int fontSize = fontManager.getFontSize(1);
    int itemW = (submenuW - 6 - SCROLL_BAR_WIDTH) / 4;
    int itemW0 = submenuW - 6 - SCROLL_BAR_WIDTH - 3 * itemW;

    // 第一行，标签。
    QHBoxLayout *labelLayout = new QHBoxLayout();
    QLabel *l = new QLabel(trs("Parameter"));
    l->setFixedSize(itemW0, ITEM_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    labelLayout->addWidget(l);

    l = new QLabel(trs("AlarmStatus"));
    l->setFixedSize(itemW, ITEM_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    labelLayout->addWidget(l);

    l = new QLabel(trs("LowerLimit"));
    l->setFixedSize(itemW, ITEM_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    labelLayout->addWidget(l);

    l = new QLabel(trs("UpperLimit"));
    l->setFixedSize(itemW, ITEM_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    labelLayout->addWidget(l);

    mainLayout->addLayout(labelLayout);

    SetItem *item;
    int count = d_ptr->itemList.count();
    for (int i = 0; i < count; i++)
    {
        item = d_ptr->itemList.at(i);

        item->combo->label->setFixedSize(itemW0, ITEM_H);
        item->combo->label->setAlignment(Qt::AlignCenter);
        item->combo->combolist->setFixedSize(itemW, ITEM_H);
        item->combo->addItem(trs("Off"));
        item->combo->addItem(trs("On"));
        item->combo->SetID(i);
        item->combo->setFont(fontManager.textFont(fontSize));
        item->combo->label->setFont(fontManager.textFont(fontSize));
        connect(item->combo, SIGNAL(currentIndexChanged(int, int)),
                this, SLOT(_comboListIndexChanged(int, int)));

        item->lower->setFixedSize(itemW, ITEM_H);
        item->lower->setFont(fontManager.textFont(fontSize));
        item->lower->enableCycle(false);
        item->lower->setID(i * 2);
        connect(item->lower, SIGNAL(valueChange(QString,int)),
                this, SLOT(_limitChanged(QString,int)));

        item->upper->setFixedSize(itemW, ITEM_H);
        item->upper->setFont(fontManager.textFont(fontSize));
        item->upper->enableCycle(false);
        item->upper->setID(i * 2 + 1);
        connect(item->upper, SIGNAL(valueChange(QString,int)),
                this, SLOT(_limitChanged(QString, int)));

        mainLayout->addWidget(item);
    }
}

void ConfigEditAlarmLimitMenu::readyShow()
{
    d_ptr->loadOptions();
}

void ConfigEditAlarmLimitMenu::_limitChanged(QString valueStr, int id)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int curID = id / 2;
    if (curID >= d_ptr->itemList.count() || !config)
    {
        return;
    }

    // 单位。
    SetItem *item = d_ptr->itemList.at(curID);
    if (NULL == item)
    {
        return;
    }

    QString patStr;
    int type = 0;
    config->getNumValue("General|DefaultPatientType", type);
    patStr = PatientSymbol::convert((PatientType)type);


    SubParamID subID = item->sid;
    type = (int)paramInfo.getUnitOfSubParam(subID);
    switch (subID)
    {
        case SUB_PARAM_ETCO2:
        case SUB_PARAM_FICO2:
            config->getNumValue("Local|CO2Unit", type);
            break;
        case SUB_PARAM_NIBP_DIA:
        case SUB_PARAM_NIBP_MAP:
        case SUB_PARAM_NIBP_SYS:
            config->getNumValue("Local|NIBPUnit", type);
            break;
        case SUB_PARAM_T1:
        case SUB_PARAM_T2:
        case SUB_PARAM_TD:
            config->getNumValue("Local|TEMPUnit", type);
            break;
        default:
            break;
    }

    QString prefix = "AlarmSource|" + patStr + "|";
    prefix += paramInfo.getSubParamName(subID, true);
    prefix += "|";
    prefix += Unit::getSymbol((UnitType)type);

    int scale = 1;
    config->getNumValue(prefix + "|Scale", scale);

    int step = 1;
    config->getNumValue(prefix + "|Step", step);
    int min, max;
    double dmin, dmax;
    if (0 == id % 2)
    {
        if (scale != 1)
        {
            double value =  valueStr.toDouble();
            item->upper->getRange(dmin, dmax);
            dmin = value + (double)step/scale;
            item->upper->setRange(dmin, dmax);
            config->setNumValue(prefix + "|Low", (int) (value * scale));
        }
        else
        {
            int value = valueStr.toInt();
            item->upper->getRange(min, max);
            min = value + step;
            item->upper->setRange(min, max);
            config->setNumValue(prefix + "|Low", value);
        }
    }
    else
    {
        if (scale != 1)
        {
            double value = valueStr.toDouble();
            item->lower->getRange(dmin, dmax);
            dmax = value - (double) step / scale;
            item->lower->setRange(dmin, dmax);
            config->setNumValue(prefix + "|High", (int) (value * scale));
        }
        else
        {
            int value = valueStr.toInt();
            item->lower->getRange(min, max);
            max = value - step;
            item->lower->setRange(min, max);
            config->setNumValue(prefix + "|High", value);
        }
    }

}

void ConfigEditAlarmLimitMenu::_comboListIndexChanged(int id, int index)
{
    // 参数的字符名。
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    if (id >= d_ptr->itemList.count() || !config)
    {
        return;
    }


    QString patStr;
    int type = 0;
    config->getNumValue("General|DefaultPatientType", type);
    patStr = PatientSymbol::convert((PatientType)type);

    SetItem *item = d_ptr->itemList.at(id);
    SubParamID subID = item->sid;
    QString prefix = "AlarmSource|" + patStr + "|";
    prefix += paramInfo.getSubParamName(subID, true);
    config->setNumAttr(prefix, "Enable", index);
}

