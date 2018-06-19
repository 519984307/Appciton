#include "ConfigEditAlarmLimitMenu.h"
#include "ConfigEditMenuGrp.h"
#include "AlarmLimitMenu.h"
#include "IConfig.h"
#include "PatientDefine.h"
#include "LoadConfigMenu.h"
//创建配置告警参数的私有类
//方便管理，层次分明
class ConfigEditAlarmLimitMenuPrivate
{
public:
    ConfigEditAlarmLimitMenuPrivate()
    {
    }

    void loadOptions();

    QList<SetItem *> itemList;
};
//加载当前的报警参数限制位信息接口
void ConfigEditAlarmLimitMenuPrivate::loadOptions()
{
    int enable = 0;
    int low = 0;
    int high = 0;
    int step = 0;
    int tmp = 0;
    int scale = 0;

    //获得当前的编辑配置
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    if(!config)
    {
        return;
    }

    QString patStr;
    int type = 0;
    //从指定路径读取type的取值
    config->getNumValue("General|DefaultPatientType", type);
    //将type取值转换成对应的字符串patStr
    patStr = PatientSymbol::convert((PatientType)type);

    //获取开始读取到itemList的数量
    int count = itemList.count();
    SetItem *item = NULL;
    //轮询读取每个item的属性（从XML文件）
    for (int i = 0; i < count; i++)
    {
        //从第一个item开始
        item = itemList.at(i);
        if (NULL == item)
        {
            continue;
        }
        //读取此时item所处的子id号
        SubParamID subID = item->sid;
        //获得指定路径   例如"AlarmSource|Adult|"
        QString prefix = "AlarmSource|" + patStr + "|";
        //获得更具体的指定路径 例如"AlarmSource|Adult|HR_PR"
        prefix += paramInfo.getSubParamName(subID, true);
        //获得子id对应的参数类型
        int type = (int)paramInfo.getUnitOfSubParam(subID);
        //根据子id读取xml文件中type类型
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
        //获取指定名字 例如"HR_PR(bpm)"
        QString name = trs(paramInfo.getSubParamName(subID)) + " (";
        name += Unit::localeSymbol((UnitType)type) + ")";
        //设置item标签的名字
        item->combo->label->setText(name);
        //获取指定路径上属性的值 例如"AlarmSource|Adult|HR_PR"的“Enable”属性值
        config->getNumAttr(prefix, "Enable", enable);
        //获取指定路径上属性的值 例如"AlarmSource|Adult|HR_PR"的“Prio”属性值
        int index=0;
        config->getNumAttr(prefix, "Prio", index);
        //获得更具体的路径，例如"AlarmSource|Adult|HR_PR|bpm|Low"
        prefix += "|";
        prefix += Unit::getSymbol((UnitType)type);
        //获取指定路径的值
        config->getNumValue(prefix + "|Low", low);
        config->getNumValue(prefix + "|High", high);
        config->getNumValue(prefix + "|Step", step);
        config->getNumValue(prefix + "|Scale", scale);

        //设置当前item的属性值
        item->combo->setCurrentIndex(enable);
        item->priority->combolist->setCurrentIndex(index);
        index = 0;
        //初始化模式为浮点型
        ISpinMode mode = ISPIN_MODE_FLOAT;
        int lowMinValue = 0, lowMaxValue = 0, highMinValue = 0, highMaxValue = 0, stepValue = 0;
        double fLowMinValue = 0, fLowMaxValue = 0, fHighMinValue = 0, fHighMaxValue = 0, fStepValue = 0;
        if(scale == 1)
        {
            mode = ISPIN_MODE_INT;
        }
        //设置item的上下限的模式
        item->lower->setMode(mode);
        item->upper->setMode(mode);
        //整形模式下的处理
        if (ISPIN_MODE_INT == mode)
        {

            stepValue = step;
            //读取指定路径中的属性值 例如"AlarmSource|Adult|HR_PR|bpm|Low"中的"Min"
            config->getNumAttr(prefix + "|Low", "Min", lowMinValue);
            lowMaxValue = high - stepValue;
            highMinValue = low + stepValue;
            config->getNumAttr(prefix + "|High", "Max", highMaxValue);
            //根据读取到值设置相应参数值
            item->lower->setRange(lowMinValue, lowMaxValue);
            item->lower->setValue(low);
            item->lower->setStep(stepValue);
            item->upper->setRange(highMinValue, highMaxValue);
            item->upper->setValue(high);
            item->upper->setStep(stepValue);
        }
        else //浮点型模式的处理
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
//配置告警参数的构造函数接口
//初始化子菜单名称、构造配置告警参数私有对象
//增加报警参数条目的SetItem
//布局显示
ConfigEditAlarmLimitMenu::ConfigEditAlarmLimitMenu()
    : SubMenu(trs("AlarmLimitMenu")), d_ptr(new ConfigEditAlarmLimitMenuPrivate())
{
    setDesc(trs("AlarmLimitMenuDesc"));
    //增加报警参数条目的SetItem
    for(int i = 0; i < SUB_PARAM_NR; ++i)
    {
        int isEnable = 0;
        //获取id号
        ParamID id = paramInfo.getParamID((SubParamID) i);
        //根据id号获取参数名字 例如"ECG"
        QString paramName = paramInfo.getParamName(id);
        switch(id)
        {
        case PARAM_SPO2:
        case PARAM_NIBP:
        case PARAM_CO2:
        case PARAM_TEMP:
            //获取machineconfig.xml文件中指定路径的值  例如"ECG"
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
        //如果使能，就增加链表节点
        if (1 == isEnabled())
        {
            SetItem *item = new SetItem(id, (SubParamID)i);
            //item链表节点增加
            d_ptr->itemList.append(item);
        }
    }
    //启动配置编辑报警限制页面显示
    startLayout();

    connect(&loadConfigMenu, SIGNAL(disableWidgets()), this, SLOT(disableWidgets()));
}
void ConfigEditAlarmLimitMenu::disableWidgets()
{
    for(int i=0; i<d_ptr->itemList.count(); i++)
    {
        d_ptr->itemList.at(i)->setEnabled(false);
    }
}
//析构函数接口
ConfigEditAlarmLimitMenu::~ConfigEditAlarmLimitMenu()
{

}
//布局执行接口
//添加标签、条目等信息，将其加入子菜单布局中
void ConfigEditAlarmLimitMenu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW - SCROLL_BAR_WIDTH, submenuH);
    setTitleEnable(true);
    int fontSize = fontManager.getFontSize(1);
    int itemW = (submenuW - 6 - SCROLL_BAR_WIDTH) / 5;
    int itemW0 = submenuW - 6 - SCROLL_BAR_WIDTH - 4 * itemW;

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

    l = new QLabel(trs("AlarmPriority"));
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
        //建立编辑改变原值后的处理接口
        connect(item->lower, SIGNAL(valueChange(QString,int)),
                this, SLOT(_limitChanged(QString,int)));

        item->upper->setFixedSize(itemW, ITEM_H);
        item->upper->setFont(fontManager.textFont(fontSize));
        item->upper->enableCycle(false);
        item->upper->setID(i * 2 + 1);
        connect(item->upper, SIGNAL(valueChange(QString,int)),
                this, SLOT(_limitChanged(QString, int)));

        item->priority->label->setFixedSize(0, 0);
        item->priority->label->setAlignment(Qt::AlignCenter);
        item->priority->combolist->setFixedSize(itemW, ITEM_H);
        item->priority->addItem(trs("high"));
        item->priority->addItem(trs("normal"));
        item->priority->addItem(trs("low"));
        item->priority->SetID(i);
        item->priority->setFont(defaultFont());
        connect(item->priority, SIGNAL(currentIndexChanged(int, int)),
                this, SLOT(_comboListIndexChanged(int, int)));

        mainLayout->addWidget(item);
    }
}
//报警参数显示
void ConfigEditAlarmLimitMenu::readyShow()
{
    d_ptr->loadOptions();
}

//用于更改报警参数限制值的接口
void ConfigEditAlarmLimitMenu::_limitChanged(QString valueStr, int id)
{
    //获得当前的配置文件指针接口
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int curID = id / 2;

    if (curID >= d_ptr->itemList.count() || !config)
    {
        return;
    }

    // 获得此时参数改变的item
    SetItem *item = d_ptr->itemList.at(curID);
    if (NULL == item)
    {
        return;
    }

    QString patStr;
    int type = 0;
    //获得XML中指定节点的数值
    config->getNumValue("General|DefaultPatientType", type);
    //将身份类型编号转换为字符串形式
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
        if (scale != 1)//保存报警参数的下限 浮点型
        {
            double value =  valueStr.toDouble();
            item->upper->getRange(dmin, dmax);
            dmin = value + (double)step/scale;
            item->upper->setRange(dmin, dmax);

            config->setNumValue(prefix + "|Low", (int) (value * scale));
        }
        else//保存报警参数的下限 整型
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
        if (scale != 1)//保存报警参数的上限 浮点型
        {
            double value = valueStr.toDouble();
            item->lower->getRange(dmin, dmax);
            dmax = value - (double) step / scale;
            item->lower->setRange(dmin, dmax);
            config->setNumValue(prefix + "|High", (int) (value * scale));
        }
        else//保存报警参数的上限 整型
        {
            int value = valueStr.toInt();
            item->lower->getRange(min, max);
            max = value - step;
            item->lower->setRange(min, max);
            config->setNumValue(prefix + "|High", value);
        }
    }

}
//关闭或者打开报警参数限制
void ConfigEditAlarmLimitMenu::_comboListIndexChanged(int id, int index)
{
    IComboList *combo = qobject_cast<IComboList*>(sender());
    if(!combo)
    {
        qdebug("Invalid Signal Sender");
        return;
    }
    // 获得当前的编辑配置
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    //判断传参的合法性
    if (id >= d_ptr->itemList.count() || !config)
    {
        return;
    }


    QString patStr;
    int type = 0;
    //获取指定路径的值
    config->getNumValue("General|DefaultPatientType", type);
    patStr = PatientSymbol::convert((PatientType)type);

    SetItem *item = d_ptr->itemList.at(id);
    SubParamID subID = item->sid;
    QString prefix = "AlarmSource|" + patStr + "|";
    prefix += paramInfo.getSubParamName(subID, true);

    if(combo->combolist == item->combo->combolist)
    {
        //改变指定路径上的属性值
        config->setNumAttr(prefix, "Enable", index);
    }
    else if(combo->combolist == item->priority->combolist)
    {
        config->setNumAttr(prefix, "Prio", index);
    }

}

