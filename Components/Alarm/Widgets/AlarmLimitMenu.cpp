#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "IComboList.h"
#include "LabelButton.h"
#include "LanguageManager.h"
#include "ParamInfo.h"
#include "IConfig.h"
#include "NIBPParam.h"
#include "CO2Param.h"
#include "TEMPParam.h"
#include "MenuManager.h"
#include "PatientManager.h"
#include "ParamManager.h"
#include "AlarmLimitMenu.h"
#include "SystemAlarm.h"
#include "RESPParam.h"
#include "AlarmSymbol.h"
#include "AlarmConfig.h"
#include "LoadConfigMenu.h"

AlarmLimitMenu *AlarmLimitMenu::_selfObj = NULL;

/**************************************************************************************************
 * 菜单中某个按钮被按下。
 *************************************************************************************************/
void AlarmLimitMenu::_limitChange(QString valueStr, int id)
{
    int curID = id / 2;
    if (curID >= _itemList.count())
    {
        return;
    }

    // 单位。
    SetItem *item = _itemList.at(curID);
    if (NULL == item)
    {
        return;
    }

    ParamID paramID = item->pid;
    SubParamID subID = item->sid;
    UnitType type = paramManager.getSubParamUnit(paramID, subID);

    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subID, type);
    if(0 == id % 2)
    {
        //low limit changed
        if(config.scale == 1)
        {
            int min, max;
            int value = valueStr.toInt();
            item->upper->getRange(min, max);
            min = value + config.step;
            item->upper->setRange(min, max);
            config.lowLimit = value;
        }
        else
        {
            double dmin, dmax;
            double value = valueStr.toDouble();
            item->upper->getRange(dmin, dmax);
            dmin = value + (double) config.step / config.scale;
            item->upper->setRange(dmin, dmax);
            config.lowLimit = value * config.scale;
        }
    }
    else
    {
        //high limit changed
        if(config.scale == 1)
        {
            int min, max;
            int value = valueStr.toInt();
            item->lower->getRange(min, max);
            max = value - config.step;
            item->lower->setRange(min, max);
            config.highLimit = value;
        }
        else
        {
            double dmin, dmax;
            double value = valueStr.toDouble();
            item->lower->getRange(dmin, dmax);
            dmax = value - (double)config.step / config.scale;
            item->lower->setRange(dmin, dmax);
            config.highLimit = value * config.scale;
        }
    }
    alarmConfig.setLimitAlarmConfig(subID, type, config);
}

/**************************************************************************************************
 * 报警开关选项改变。
 *************************************************************************************************/
void AlarmLimitMenu::_comboListIndexChanged(int id, int index)
{
    IComboList *combo = qobject_cast<IComboList*>(sender());
    if(!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }

    // 参数的字符名。
    if (id >= _itemList.count())
    {
        return;
    }

    SetItem *item = _itemList.at(id);
    if (0 == id)
    {
        systemConfig.setNumValue("PrimaryCfg|Alarm|ApneaTime", index);
        respParam.setApneaTime((ApneaAlarmTime)index);
        co2Param.setApneaTime((ApneaAlarmTime)index);
        return;
    }

    SubParamID subID = item->sid;
    if(combo->combolist == item->combo->combolist)
    {
        alarmConfig.setLimitAlarmEnable(subID, index);

        if (0 == index)
        {
            systemAlarm.setOneShotAlarm(SOME_LIMIT_ALARM_DISABLED, true);
        }
        else
        {
            checkAlarmEnableStatus();
        }
    }
    else if(combo->combolist == item->priority->combolist)
    {
        currentConfig.setNumAttr(QString("AlarmSource|%1|%2").arg(patientManager.getTypeStr()).arg(paramInfo.getSubParamName(subID, true)),
                                  "Prio",
                                  index);
    }

}

/**************************************************************************************************
 * 检查参数超限报警状态。
 *************************************************************************************************/
void AlarmLimitMenu::checkAlarmEnableStatus()
{
    int count = _itemList.count();
    SetItem *item = NULL;

    for (int i = 1; i < count; i++)
    {
        item = _itemList.at(i);
        if (NULL == item)
        {
            continue;
        }

        SubParamID subID = item->sid;
        if (!alarmConfig.isLimitAlarmEnable(subID))
        {
            systemAlarm.setOneShotAlarm(SOME_LIMIT_ALARM_DISABLED, true);
            return;
        }
    }

    systemAlarm.setOneShotAlarm(SOME_LIMIT_ALARM_DISABLED, false);
}

/**************************************************************************************************
 * IBP报警项设置。
 *************************************************************************************************/
void AlarmLimitMenu::setIBPAlarmItem(IBPPressureName ibp1, IBPPressureName ibp2)
{
    SetItem *item;
    QList<SubParamID> id;
    int count = _itemList.count();
    switch (ibp1)
    {
    case IBP_PRESSURE_ART:
        id.append(SUB_PARAM_ART_SYS);
        id.append(SUB_PARAM_ART_DIA);
        id.append(SUB_PARAM_ART_MAP);
        id.append(SUB_PARAM_ART_PR);
        break;
    case IBP_PRESSURE_PA:
        id.append(SUB_PARAM_PA_SYS);
        id.append(SUB_PARAM_PA_DIA);
        id.append(SUB_PARAM_PA_MAP);
        id.append(SUB_PARAM_PA_PR);
        break;
    case IBP_PRESSURE_CVP:
        id.append(SUB_PARAM_CVP_MAP);
        id.append(SUB_PARAM_CVP_PR);
        break;
    case IBP_PRESSURE_LAP:
        id.append(SUB_PARAM_LAP_MAP);
        id.append(SUB_PARAM_LAP_PR);
        break;
    case IBP_PRESSURE_RAP:
        id.append(SUB_PARAM_RAP_MAP);
        id.append(SUB_PARAM_RAP_PR);
        break;
    case IBP_PRESSURE_ICP:
        id.append(SUB_PARAM_ICP_MAP);
        id.append(SUB_PARAM_ICP_PR);
        break;
    case IBP_PRESSURE_AUXP1:
        id.append(SUB_PARAM_AUXP1_SYS);
        id.append(SUB_PARAM_AUXP1_DIA);
        id.append(SUB_PARAM_AUXP1_MAP);
        id.append(SUB_PARAM_AUXP1_PR);
        break;
    case IBP_PRESSURE_AUXP2:
        id.append(SUB_PARAM_AUXP2_SYS);
        id.append(SUB_PARAM_AUXP2_DIA);
        id.append(SUB_PARAM_AUXP2_MAP);
        id.append(SUB_PARAM_AUXP2_PR);
        break;
    default:
        break;
    }

    switch (ibp2)
    {
    case IBP_PRESSURE_ART:
        id.append(SUB_PARAM_ART_SYS);
        id.append(SUB_PARAM_ART_DIA);
        id.append(SUB_PARAM_ART_MAP);
        id.append(SUB_PARAM_ART_PR);
        break;
    case IBP_PRESSURE_PA:
        id.append(SUB_PARAM_PA_SYS);
        id.append(SUB_PARAM_PA_DIA);
        id.append(SUB_PARAM_PA_MAP);
        id.append(SUB_PARAM_PA_PR);
        break;
    case IBP_PRESSURE_CVP:
        id.append(SUB_PARAM_CVP_MAP);
        id.append(SUB_PARAM_CVP_PR);
        break;
    case IBP_PRESSURE_LAP:
        id.append(SUB_PARAM_LAP_MAP);
        id.append(SUB_PARAM_LAP_PR);
        break;
    case IBP_PRESSURE_RAP:
        id.append(SUB_PARAM_RAP_MAP);
        id.append(SUB_PARAM_RAP_PR);
        break;
    case IBP_PRESSURE_ICP:
        id.append(SUB_PARAM_ICP_MAP);
        id.append(SUB_PARAM_ICP_PR);
        break;
    case IBP_PRESSURE_AUXP1:
        id.append(SUB_PARAM_AUXP1_SYS);
        id.append(SUB_PARAM_AUXP1_DIA);
        id.append(SUB_PARAM_AUXP1_MAP);
        id.append(SUB_PARAM_AUXP1_PR);
        break;
    case IBP_PRESSURE_AUXP2:
        id.append(SUB_PARAM_AUXP2_SYS);
        id.append(SUB_PARAM_AUXP2_DIA);
        id.append(SUB_PARAM_AUXP2_MAP);
        id.append(SUB_PARAM_AUXP2_PR);
        break;
    default:
        break;
    }

    for (int i = 1; i < count; i ++)
    {
        item = _itemList.at(i);
        if (item->pid == PARAM_IBP)
        {
            item->setVisible(false);
        }
        for (int i = 0; i < id.count(); i ++)
        {
            if (item->sid == id.at(i))
            {
                item->setVisible(true);
                break;
            }
        }
    }
}

void AlarmLimitMenu::_onConfigUpdated()
{
    alarmConfig.alarmConfigClear();
    readyShow();
}
/**************************************************************************************************
 * 载入初始配置。
 *************************************************************************************************/
void AlarmLimitMenu::_loadOptions(void)
{
    int enable = 0;
    QString low;
    QString high;
    QString step;
    QString tmpStr;


    SetItem *item = _itemList.at(0);
    if (NULL != item)
    {
        systemConfig.getNumValue("PrimaryCfg|Alarm|ApneaTime", enable);
        item->combo->setCurrentIndex(enable);
    }

    int count = _itemList.count();
    for (int i = 1; i < count; i++)
    {
        item = _itemList.at(i);
        if (NULL == item)
        {
            continue;
        }

        SubParamID subID = item->sid;
        ParamID id = item->pid;
        UnitType type = paramManager.getSubParamUnit(id, subID);
        LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subID, type);

        item->combo->setCurrentIndex(alarmConfig.isLimitAlarmEnable(subID));

        if(config.scale == 1)
        {
            item->lower->setMode(ISPIN_MODE_INT);
            item->lower->setRange(config.minLowLimit, config.highLimit - config.step);
            item->lower->setValue(config.lowLimit);
            item->lower->setStep(config.step);

            item->upper->setMode(ISPIN_MODE_INT);
            item->upper->setRange(config.lowLimit + config.step, config.maxHighLimit);
            item->upper->setValue(config.highLimit);
            item->upper->setStep(config.step);

        }
        else
        {
            double fStepValue, fLowMinValue, fLowMaxValue, fHighMinValue, fHighMaxValue;
            fStepValue = (double)config.step / config.scale;
            fLowMinValue = (double)config.minLowLimit / config.scale;
            fLowMaxValue = (double)(config.highLimit - config.step) / config.scale;
            fHighMinValue = (double)(config.lowLimit + config.step) / config.scale;
            fHighMaxValue = (double)config.maxHighLimit / config.scale;

            item->lower->setMode(ISPIN_MODE_FLOAT);
            item->lower->setRange(fLowMinValue, fLowMaxValue);
            item->lower->setValue((double)config.lowLimit / config.scale);
            item->lower->setStep(fStepValue);

            item->upper->setMode(ISPIN_MODE_FLOAT);
            item->upper->setRange(fHighMinValue, fHighMaxValue);
            item->upper->setValue((double)config.highLimit / config.scale);
            item->upper->setStep(fStepValue);
        }

        int index=0;
        currentConfig.getNumAttr(QString("AlarmSource|%1|%2").arg(patientManager.getTypeStr()).arg(paramInfo.getSubParamName(subID, true)),
                                  "Prio",
                                  index);
        item->priority->combolist->setCurrentIndex(index);
    }
}

/**************************************************************************************************
 * 显示前确定配置。
 *************************************************************************************************/
void AlarmLimitMenu::readyShow(void)
{
    SubMenu::readyShow();
    _loadOptions();
}

/**************************************************************************************************
 * 隐藏前清理。
 *************************************************************************************************/
void AlarmLimitMenu::readyhide(void)
{
    SubMenu::readyhide();
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void AlarmLimitMenu::layoutExec(void)
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW - SCROLL_BAR_WIDTH, submenuH);
    setTitleEnable(true);
    int totalW = submenuW - 6 - SCROLL_BAR_WIDTH;
    int itemW = (totalW - 12) / 5;
    int itemW0 = totalW - itemW * 4;

    // 第一行，标签。
    QHBoxLayout *labelLayout = new QHBoxLayout();
    QLabel *l = new QLabel(trs("Parameter"));
    l->setFixedSize(itemW0, ITEM_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(defaultFont());
    labelLayout->addWidget(l);

    l = new QLabel(trs("AlarmStatus"));
    l->setFixedSize(itemW, ITEM_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(defaultFont());
    labelLayout->addWidget(l);

    l = new QLabel(trs("LowerLimit"));
    l->setFixedSize(itemW, ITEM_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(defaultFont());
    labelLayout->addWidget(l);

    l = new QLabel(trs("UpperLimit"));
    l->setFixedSize(itemW, ITEM_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(defaultFont());
    labelLayout->addWidget(l);

    l = new QLabel(trs("AlarmPriority"));
    l->setFixedSize(itemW, ITEM_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(defaultFont());
    labelLayout->addWidget(l);

    mainLayout->addLayout(labelLayout);

    QString name;
    SetItem *item = _itemList.at(0);
    name = trs("RESPApneaTime");

    item->combo->label->setText(name);
    item->combo->label->setFixedSize(itemW0, ITEM_H);
    item->combo->label->setAlignment(Qt::AlignCenter);
    item->combo->combolist->setFixedSize(itemW, ITEM_H);
    for (int i = 0; i < APNEA_ALARM_TIME_NR; ++i)
    {
        item->combo->combolist->addItem(trs(AlarmSymbol::convert((ApneaAlarmTime)i)));
    }
    item->combo->SetID(0);
    item->combo->setFont(defaultFont());
    item->combo->label->setFont(defaultFont());
    connect(item->combo, SIGNAL(currentIndexChanged(int, int)),
            this, SLOT(_comboListIndexChanged(int, int)));
    item->lower->setVisible(false);
    item->upper->setVisible(false);
    item->priority->setVisible(false);
    mainLayout->addWidget(item, 0, Qt::AlignLeft);

    if (!systemManager.isSupport(CONFIG_CO2) && !systemManager.isSupport(CONFIG_RESP))
    {
        item->setVisible(false);
    }

    int count = _itemList.count();
    for (int i = 1; i < count; i++)
    {
        item = _itemList.at(i);
        name = trs(paramInfo.getSubParamName(item->sid)) + " (";
        name += Unit::localeSymbol(paramManager.getSubParamUnit(item->pid, item->sid)) + ")";

        item->combo->label->setText(name);
        item->combo->label->setFixedSize(itemW0, ITEM_H);
        item->combo->label->setAlignment(Qt::AlignCenter);
        item->combo->combolist->setFixedSize(itemW, ITEM_H);
        item->combo->addItem(trs("Off"));
        item->combo->addItem(trs("On"));
        item->combo->SetID(i);
        item->combo->setFont(defaultFont());
        item->combo->label->setFont(fontManager.textFont(fontManager.getFontSize(1)));
        connect(item->combo, SIGNAL(currentIndexChanged(int, int)),
                this, SLOT(_comboListIndexChanged(int, int)));

        item->lower->setFixedSize(itemW, ITEM_H);
        item->lower->setFont(defaultFont());
        item->lower->enableCycle(false);
        item->lower->setID(i * 2);
        connect(item->lower, SIGNAL(valueChange(QString,int)),
                this, SLOT(_limitChange(QString,int)));

        item->upper->setFixedSize(itemW, ITEM_H);
        item->upper->setFont(defaultFont());
        item->upper->enableCycle(false);
        item->upper->setID(i * 2 + 1);
        connect(item->upper, SIGNAL(valueChange(QString,int)),
                this, SLOT(_limitChange(QString, int)));

        item->priority->label->setFixedSize(0,0);
        item->priority->label->setVisible(false);
        item->priority->combolist->setFixedSize(itemW, ITEM_H);
        item->priority->combolist->addItem(trs("high"));
        item->priority->combolist->addItem(trs("normal"));
        item->priority->combolist->addItem(trs("low"));
        item->priority->SetID(i);
        item->priority->setFont(fontManager.textFont(fontManager.getFontSize(1)));
        connect(item->priority, SIGNAL(currentIndexChanged(int, int)),
                this, SLOT(_comboListIndexChanged(int, int)));

        mainLayout->addWidget(item);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmLimitMenu::AlarmLimitMenu() : SubMenu(trs("AlarmLimitMenu"))
{
    _itemList.clear();
    setDesc(trs("AlarmLimitMenuDesc"));

    //LAT alarm item
    SetItem *item = new SetItem(PARAM_NONE, SUB_PARAM_NONE);
    _itemList.append(item);

    QList<ParamID> pids;
    paramManager.getParams(pids);
    for (int i = 0; i< SUB_PARAM_NR; ++i)
    {
        ParamID id = paramInfo.getParamID((SubParamID)i);
        if (-1 != pids.indexOf(id))
        {
            item = new SetItem(id, (SubParamID)i);
            _itemList.append(item);
        }
    }

    startLayout();

    checkAlarmEnableStatus();

    connect(&configManager, SIGNAL(configUpdated()), this, SLOT(_onConfigUpdated()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmLimitMenu::~AlarmLimitMenu()
{
    _itemList.clear();
}
