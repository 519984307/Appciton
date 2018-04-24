#include "ConfigEditPAMenu.h"
#include "ConfigEditMenuGrp.h"
#include "IComboList.h"
#include "COSymbol.h"
#include "LabelButton.h"
#include "NumberInput.h"
#include "../PAParam/PASymbol.h"

class ConfigEditPAMenuPrivate
{
public:

    ConfigEditPAMenuPrivate()
    {
        ;
    }

    void loadOptions();

    IComboList  *icombolist_PD;//压力标名
    IComboList  *icombolist_WR;//波形标尺
    LabelButton *labelbutton_WUR;//波形上标尺
    LabelButton *labelbutton_WDR;//波形下标尺
    IComboList  *icombolist_SS;//波形速度
    IComboList  *icombolist_FF;//滤波频率
    IComboList  *icombolist_S;//灵敏度
    IComboList  *icombolist_PPVM;//PPV测量
    IComboList  *icombolist_PPVS;//PPV来源
    IComboList  *icombolist_grid;//网格
    LabelButton *labelbutton_PAZ;//PA校零
    LabelButton *labelbutton_DOS;//标名顺序设置
};

void ConfigEditPAMenuPrivate::loadOptions()
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int index  = 0;
    int i      = 0;

    //装载压力标名数据
    if(icombolist_PD->count()==0)
    {
        for(i=0;i<PD_NR;i++)
        {
            icombolist_PD[0].addItem(PASymbol::convert((PressureDesigner_d)i));
        }
        config->getNumValue("PA|PressureDesigner", index);
        icombolist_PD[0].setCurrentIndex(index);
    }

    index = 0;
    //装载波形标尺数据
    if(icombolist_WR->count()==0)
    {
        for(i=0;i<WAVEFORM_PULER_NR;i++)
        {
            icombolist_WR[0].addItem(PASymbol::convert((WaveformRuler_d)i));
        }
        config->getNumValue("PA|WaveformRuler", index);
        icombolist_WR[0].setCurrentIndex(index);
    }

    //装载波形上标尺数据
    index = 0;
    config->getNumValue("PA|WaveUpRuler", index);

    if(index!=0)
        labelbutton_WUR->button->setText(QString::number(index));
    else
        labelbutton_WUR->button->setText("10");

    //装载波形下标尺数据
    index = 0;
    config->getNumValue("PA|WaveDownRuler", index);

    if(index!=0)
        labelbutton_WDR->button->setText(QString::number(index));
    else
        labelbutton_WDR->button->setText("-10");

    //装载波形速度数据
    index = 0;
    if(icombolist_SS->count()==0)
    {
        for(i=0;i<SWEEP_SPEED_NR;i++)
        {
            icombolist_SS[0].addItem(PASymbol::convert((SweepSpeed_d)i));
        }
        config->getNumValue("PA|SweepSpeed", index);
        icombolist_SS[0].setCurrentIndex(index);
    }

    //装载滤波频率数据
    index = 0;
    if(icombolist_FF->count()==0)
    {
        for(i=0;i<FREQUENCY_FILTERING_NR;i++)
        {
            icombolist_FF[0].addItem(PASymbol::convert((FrequencyFiltering_d)i));
        }
        config->getNumValue("PA|FrequencyFiltering", index);
        icombolist_FF[0].setCurrentIndex(index);
    }

    //装载灵敏度数据
    index = 0;
    if(icombolist_S->count()==0)
    {
        icombolist_S[0].addItem(trs("HIGH"));
        icombolist_S[0].addItem(trs("NORMAL"));
        icombolist_S[0].addItem(trs("LOW"));
        config->getNumValue("PA|Sensitivity", index);
        icombolist_S[0].setCurrentIndex(index);
    }

    //装载PPV测量数据
    index = 0;
    if(icombolist_PPVM->count()==0)
    {
        icombolist_PPVM[0].addItem(trs("ON"));
        icombolist_PPVM[0].addItem(trs("OFF"));
        config->getNumValue("PA|PPVMeasure", index);
        icombolist_PPVM[0].setCurrentIndex(index);
    }

    //装载PPV来源数据
    index = 0;
    if(icombolist_PPVS->count()==0)
    {
        for(i=0;i<PPV_SOURCE_NR;i++)
        {
            icombolist_PPVS[0].addItem(PASymbol::convert((PPVSource_d)i));
        }
        config->getNumValue("PA|PPVSource", index);
        icombolist_PPVS[0].setCurrentIndex(index);
    }

    //装载网格数据
    index = 0;
    if(icombolist_grid->count()==0)
    {
        icombolist_grid[0].addItem(trs("ON"));
        icombolist_grid[0].addItem(trs("OFF"));
        config->getNumValue("PA|grid", index);
        icombolist_grid[0].setCurrentIndex(index);
    }

    //装载PA校零数据
    index = 0;
    config->getNumValue("PA|PAZero", index);

    if(index!=0)
        labelbutton_PAZ->button->setText(QString::number(index));
    else
        labelbutton_PAZ->button->setText("ON");

    //装载标名顺序设置按钮
    labelbutton_DOS->label->setText("DesignerOrderSetUp");
    labelbutton_DOS->button->setText(">>");
}

ConfigEditPAMenu::ConfigEditPAMenu()
    :SubMenu(trs("PAMenu")),
    d_ptr(new ConfigEditPAMenuPrivate())
{
    setDesc(trs("PAMenuDesc"));
    startLayout();
}

ConfigEditPAMenu::~ConfigEditPAMenu()
{

}

void ConfigEditPAMenu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;


    d_ptr->icombolist_PD = new IComboList(trs("PressureDesigner"));
    d_ptr->icombolist_PD->setFont(defaultFont());
    d_ptr->icombolist_PD->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->icombolist_PD->combolist->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->icombolist_PD, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
    mainLayout->addWidget(d_ptr->icombolist_PD);

    d_ptr->icombolist_WR = new IComboList(trs("WaveformRuler"));
    d_ptr->icombolist_WR->setFont(defaultFont());
    d_ptr->icombolist_WR->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->icombolist_WR->combolist->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->icombolist_WR, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
    mainLayout->addWidget(d_ptr->icombolist_WR);

    d_ptr->labelbutton_WUR = new LabelButton(trs("WaveUpRuler"));
    d_ptr->labelbutton_WUR->setFont(defaultFont());
    d_ptr->labelbutton_WUR->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->labelbutton_WUR->button->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->labelbutton_WUR->button, SIGNAL(realReleased()), this, SLOT(onButtonClicked()));
    mainLayout->addWidget(d_ptr->labelbutton_WUR);

    d_ptr->labelbutton_WDR = new LabelButton(trs("WaveDownRuler"));
    d_ptr->labelbutton_WDR->setFont(defaultFont());
    d_ptr->labelbutton_WDR->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->labelbutton_WDR->button->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->labelbutton_WDR->button, SIGNAL(realReleased()), this, SLOT(onButtonClicked()));
    mainLayout->addWidget(d_ptr->labelbutton_WDR);

    d_ptr->icombolist_SS = new IComboList(trs("SweepSpeed"));
    d_ptr->icombolist_SS->setFont(defaultFont());
    d_ptr->icombolist_SS->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->icombolist_SS->combolist->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->icombolist_SS, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
    mainLayout->addWidget(d_ptr->icombolist_SS);

    d_ptr->icombolist_FF = new IComboList(trs("FrequencyFiltering"));
    d_ptr->icombolist_FF->setFont(defaultFont());
    d_ptr->icombolist_FF->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->icombolist_FF->combolist->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->icombolist_FF, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
    mainLayout->addWidget(d_ptr->icombolist_FF);

    d_ptr->icombolist_S = new IComboList(trs("Sensitivity"));
    d_ptr->icombolist_S->setFont(defaultFont());
    d_ptr->icombolist_S->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->icombolist_S->combolist->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->icombolist_S, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
    mainLayout->addWidget(d_ptr->icombolist_S);

    d_ptr->icombolist_PPVM = new IComboList(trs("PPVMeasure"));
    d_ptr->icombolist_PPVM->setFont(defaultFont());
    d_ptr->icombolist_PPVM->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->icombolist_PPVM->combolist->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->icombolist_PPVM, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
    mainLayout->addWidget(d_ptr->icombolist_PPVM);

    d_ptr->icombolist_PPVS = new IComboList(trs("PPVSource"));
    d_ptr->icombolist_PPVS->setFont(defaultFont());
    d_ptr->icombolist_PPVS->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->icombolist_PPVS->combolist->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->icombolist_PPVS, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
    mainLayout->addWidget(d_ptr->icombolist_PPVS);

    d_ptr->icombolist_grid = new IComboList(trs("Grid"));
    d_ptr->icombolist_grid->setFont(defaultFont());
    d_ptr->icombolist_grid->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->icombolist_grid->combolist->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->icombolist_grid, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
    mainLayout->addWidget(d_ptr->icombolist_grid);

    d_ptr->labelbutton_PAZ = new LabelButton(trs("PAZero"));
    d_ptr->labelbutton_PAZ->setFont(defaultFont());
    d_ptr->labelbutton_PAZ->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->labelbutton_PAZ->button->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->labelbutton_PAZ->button, SIGNAL(realReleased()), this, SLOT(onButtonClicked()));
    mainLayout->addWidget(d_ptr->labelbutton_PAZ);

    d_ptr->labelbutton_DOS = new LabelButton("");
    d_ptr->labelbutton_DOS->setFont(defaultFont());
    d_ptr->labelbutton_DOS->label->setFixedSize(labelWidth,ITEM_H);
    d_ptr->labelbutton_DOS->button->setFixedSize(btnWidth,ITEM_H);
    connect(d_ptr->labelbutton_DOS->button, SIGNAL(realReleased()), this, SLOT(onButtonClicked()));
    mainLayout->addWidget(d_ptr->labelbutton_DOS);

    mainLayout->addStretch(1);

}

void ConfigEditPAMenu::readyShow()
{
    d_ptr->loadOptions();
}


void ConfigEditPAMenu::onComboListConfigChanged(int index)
{
    IComboList *combo = qobject_cast<IComboList *>(sender());
    if(!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }
    //根据索引设置对应列表的参数值
    //ConfigEditCOMenuPrivate::ComboListId comboId = (ConfigEditCOMenuPrivate::ComboListId) combo->property("comboId").toInt();

    Config *config = configEditMenuGrp.getCurrentEditConfig();

}
void ConfigEditPAMenu::onButtonClicked()
{

    LButton *btn = qobject_cast<LButton *>(sender());
    NumberInput numberPad;
#if 1
    if(btn==d_ptr->labelbutton_WUR->button)
    {

        numberPad.setTitleBarText(trs("WaveUpRuler"));
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
    else if(btn==d_ptr->labelbutton_WDR->button)
    {

        numberPad.setTitleBarText(trs("WaveDownRuler"));
        numberPad.setMaxInputLength(5);
        numberPad.setInitString(btn->text());
        if (numberPad.exec())
        {
            QString text = numberPad.getStrValue();
            bool ok = false;
            float value = text.toFloat(&ok);
            short actualValue = value;
            if (ok)
            {
                if (actualValue <= 0)
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
    else if(btn==d_ptr->labelbutton_PAZ->button)
    {

        numberPad.setTitleBarText(trs("PAZero"));
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
    else if(btn==d_ptr->labelbutton_DOS->button)
    {

        numberPad.setTitleBarText(trs("DesignerOrderSetUp"));
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
#endif
}
