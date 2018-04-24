#include "ConfigEditCOMenu.h"
#include "ConfigEditMenuGrp.h"
#include "IComboList.h"
#include "COSymbol.h"
#include "LabelButton.h"
#include "NumberInput.h"

class ConfigEditCOMenuPrivate
{
public:

    ConfigEditCOMenuPrivate()
    {
        ;
    }

    void loadOptions();

    LabelButton *labelbutton_height;//身高
    LabelButton *labelbutton_weight;//体重
    LabelButton *labelbutton_ratdio;//co系数
    IComboList  *icombolist_ITS;//注射液温度来源
    LabelButton *labelbutton_IT;//注射液温度
    IComboList  *icombolist_CO_SM;//开始co测量
};

void ConfigEditCOMenuPrivate::loadOptions()
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    float index  = 0;

    //装载身高数据
    config->getNumValue("CO|height", index);

    if(index!=0)
        labelbutton_height->button->setText(QString::number(index));
    else
        labelbutton_height->button->setText("170.0");

    //装载体重数据
    config->getNumValue("CO|weight", index);

    if(index!=0)
        labelbutton_weight->button->setText(QString::number(index));
    else
        labelbutton_weight->button->setText("64.0");

    //装载CO系数
    config->getNumValue("CO|ratdio", index);

    if(index!=0)
        labelbutton_ratdio->button->setText(QString::number(index));
    else
        labelbutton_ratdio->button->setText("0.542");

    //装载注射液温度来源参数
    config->getNumValue("CO|InjectionTempSource", index);
    if(icombolist_ITS->count()==0)
    {
        icombolist_ITS[0].addItem("AUTO");
        icombolist_ITS[0].addItem("MANUAL");
    }

    if(index!=0)
        icombolist_ITS[0].setCurrentItem("MANUAL");
    else
        icombolist_ITS[0].setCurrentItem("AUTO");

    //装载注射液温度
    config->getNumValue("CO|InjectionTemp", index);

    if(index!=0)
        labelbutton_IT->button->setText(QString::number(index));
    else
        labelbutton_IT->button->setText("20.0");

    //装载co是否开始测量参数
    config->getNumValue("CO|StartMeasure", index);
    if(icombolist_CO_SM->count()==0)
    {
        icombolist_CO_SM[0].addItem("OFF");
        icombolist_CO_SM[0].addItem("ON");
    }

    if(index!=0)
        icombolist_CO_SM->setCurrentItem("ON");
    else
        icombolist_CO_SM->setCurrentItem("OFF");
}

ConfigEditCOMenu::ConfigEditCOMenu()
    :SubMenu(trs("COMenu")),
    d_ptr(new ConfigEditCOMenuPrivate())
{
    setDesc(trs("COMenuDesc"));
    startLayout();
}

ConfigEditCOMenu::~ConfigEditCOMenu()
{

}

void ConfigEditCOMenu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    d_ptr->labelbutton_height = new LabelButton(trs("height"));
    d_ptr->labelbutton_height->setFont(defaultFont());
    d_ptr->labelbutton_height->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->labelbutton_height->button->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->labelbutton_height->button, SIGNAL(realReleased()), this, SLOT(onButtonClicked()));
    mainLayout->addWidget(d_ptr->labelbutton_height);

    d_ptr->labelbutton_weight = new LabelButton(trs("weight"));
    d_ptr->labelbutton_weight->setFont(defaultFont());
    d_ptr->labelbutton_weight->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->labelbutton_weight->button->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->labelbutton_weight->button, SIGNAL(realReleased()), this, SLOT(onButtonClicked()));
    mainLayout->addWidget(d_ptr->labelbutton_weight);

    d_ptr->labelbutton_ratdio = new LabelButton(trs("COratdio"));
    d_ptr->labelbutton_ratdio->setFont(defaultFont());
    d_ptr->labelbutton_ratdio->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->labelbutton_ratdio->button->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->labelbutton_ratdio->button, SIGNAL(realReleased()), this, SLOT(onButtonClicked()));
    mainLayout->addWidget(d_ptr->labelbutton_ratdio);

    d_ptr->icombolist_ITS = new IComboList(trs("InjectionTempSource"));
    d_ptr->icombolist_ITS->setFont(defaultFont());
    d_ptr->icombolist_ITS->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->icombolist_ITS->combolist->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->icombolist_ITS, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
    mainLayout->addWidget(d_ptr->icombolist_ITS);

    d_ptr->labelbutton_IT = new LabelButton(trs("InjectionTemp"));
    d_ptr->labelbutton_IT->setFont(defaultFont());
    d_ptr->labelbutton_IT->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->labelbutton_IT->button->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->labelbutton_IT->button, SIGNAL(realReleased()), this, SLOT(onButtonClicked()));
    mainLayout->addWidget(d_ptr->labelbutton_IT);

    d_ptr->icombolist_CO_SM = new IComboList(trs("StartMeasure"));
    d_ptr->icombolist_CO_SM->setFont(defaultFont());
    d_ptr->icombolist_CO_SM->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->icombolist_CO_SM->combolist->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->icombolist_CO_SM, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
    mainLayout->addWidget(d_ptr->icombolist_CO_SM);

    mainLayout->addStretch(1);

}

void ConfigEditCOMenu::readyShow()
{
    d_ptr->loadOptions();
}


void ConfigEditCOMenu::onComboListConfigChanged(int /*index*/)
{
    IComboList *combo = qobject_cast<IComboList *>(sender());
    if(!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }
    //根据索引设置对应列表的参数值
    //ConfigEditCOMenuPrivate::ComboListId comboId = (ConfigEditCOMenuPrivate::ComboListId) combo->property("comboId").toInt();

//    Config *config = configEditMenuGrp.getCurrentEditConfig();

}
void ConfigEditCOMenu::onButtonClicked()
{

    LButton *btn = qobject_cast<LButton *>(sender());
    NumberInput numberPad;

    if(btn==d_ptr->labelbutton_height->button)
    {

        numberPad.setTitleBarText(trs("height"));
        numberPad.setMaxInputLength(5);
        numberPad.setInitString(btn->text());
        if (numberPad.exec())
        {
            QString text = numberPad.getStrValue();
            bool ok = false;
            float value = text.toFloat(&ok);
            unsigned short actualValue = value;
            if (ok)
            {
                if (actualValue >= 1 && actualValue <= 999)
                {
                   btn->setText(text);
                    //coParam.setCORatio(actualValue);
                   //添加xml文件写入功能
                }
                else
                {
                    //IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.001-0.999", QStringList(trs("EnglishYESChineseSURE")));
                    //messageBox.exec();
                }

            }
        }
    }
    else if(btn==d_ptr->labelbutton_weight->button)
    {

        numberPad.setTitleBarText(trs("weight"));
        numberPad.setMaxInputLength(5);
        numberPad.setInitString(btn->text());
        if (numberPad.exec())
        {
            QString text = numberPad.getStrValue();
            bool ok = false;
            float value = text.toFloat(&ok);
            unsigned short actualValue = value;
            if (ok)
            {
                if (actualValue >= 1 && actualValue <= 999)
                {
                   btn->setText(text);
                    //coParam.setCORatio(actualValue);
                }
                else
                {
                    //IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.001-0.999", QStringList(trs("EnglishYESChineseSURE")));
                    //messageBox.exec();
                }

            }
        }
    }
    else if(btn==d_ptr->labelbutton_ratdio->button)
    {

        numberPad.setTitleBarText(trs("CORatio"));
        numberPad.setMaxInputLength(5);
        numberPad.setInitString(btn->text());
        if (numberPad.exec())
        {
            QString text = numberPad.getStrValue();
            bool ok = false;
            float value = text.toFloat(&ok);
            unsigned short actualValue = value * 1000;
            if (ok)
            {
                if (actualValue >= 1 && actualValue <= 999)
                {
                   btn->setText(text);
                    //coParam.setCORatio(actualValue);
                }
                else
                {
                    //IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.001-0.999", QStringList(trs("EnglishYESChineseSURE")));
                    //messageBox.exec();
                }

            }
        }
    }
    else if(btn==d_ptr->labelbutton_IT->button)
    {

        numberPad.setTitleBarText(trs("InjectionTemp"));
        numberPad.setMaxInputLength(5);
        numberPad.setInitString(btn->text());
        if (numberPad.exec())
        {
            QString text = numberPad.getStrValue();
            bool ok = false;
            float value = text.toFloat(&ok);
            unsigned short actualValue = value;
            if (ok)
            {
                if (actualValue >= 1 && actualValue <= 999)
                {
                   btn->setText(text);
                    //coParam.setCORatio(actualValue);
                }
                else
                {
                    //IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.001-0.999", QStringList(trs("EnglishYESChineseSURE")));
                    //messageBox.exec();
                }

            }
        }
    }
}
