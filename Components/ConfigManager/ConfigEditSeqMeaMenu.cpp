#include "ConfigEditSeqMeaMenu.h"
#include "ConfigEditMenuGrp.h"
#include "IComboList.h"
#include "../../Params/NIBPParam/NIBPSeqMeaSet/SeqMeaSymbol.h"

class ConfigEditSeqMeaMenuPrivate
{
public:
    enum LabelListId {
        Sequence,
        MeasureTime,
        IntervalTime,
        LabelListMax,
    };

    enum ComboListId {
        Sequence_a,
        Sequence_b,
        Sequence_c,
        Sequence_d,
        Sequence_e,
        ComboListMax,
    };

    ConfigEditSeqMeaMenuPrivate()
    {
        memset(combos, 0, sizeof(combos));
        memset(Labels, 0, sizeof(Labels));
        memset(combosr, 0, sizeof(combosr));

        //常量存放在静态变量区
        Labels[Sequence] = "Sequence";
        Labels[MeasureTime] = "MeasureTime";
        Labels[IntervalTime] = "IntervalTime";
    }

    void loadOptions();

    const char *Labels[LabelListMax];
    IComboList *combos[ComboListMax];
    ComboList  *combosr[ComboListMax];

};

void ConfigEditSeqMeaMenuPrivate::loadOptions()
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int index  = 0;
    int i =0;

    if (combos[Sequence_a]->count() == 0 && combosr[Sequence_a]->count() == 0)
    {
        //添加下拉框条目
        combos[Sequence_a]->label->setText("A");
        //间隔时间下拉框
        for(i=0;i<SEQMEA_NR;i++)
        {
            combos[Sequence_a]->addItem(SeqMeaSymbol::convert((SeqMeaTime)i));
        }
        //测量时间下拉框
        for(i=0;i<SEQMEAINT_NR;i++)
        {
            combosr[Sequence_a]->addItem(SeqMeaSymbol::convert((SeqMeaIntervalTime)i));
        }
    }

    if (combos[Sequence_b]->count() == 0 && combosr[Sequence_b]->count() == 0)
    {
        //添加下拉框条目
        combos[Sequence_b]->label->setText("B");
        //间隔时间下拉框
        for(i=0;i<SEQMEA_NR;i++)
        {
            combos[Sequence_b]->addItem(SeqMeaSymbol::convert((SeqMeaTime)i));
        }
        //测量时间下拉框
        for(i=0;i<SEQMEAINT_NR;i++)
        {
            combosr[Sequence_b]->addItem(SeqMeaSymbol::convert((SeqMeaIntervalTime)i));
        }
    }

    if (combos[Sequence_c]->count() == 0 && combosr[Sequence_c]->count() == 0)
    {
        //添加下拉框条目
        combos[Sequence_c]->label->setText("C");
        //间隔时间下拉框
        for(i=0;i<SEQMEA_NR;i++)
        {
            combos[Sequence_c]->addItem(SeqMeaSymbol::convert((SeqMeaTime)i));
        }
        //测量时间下拉框
        for(i=0;i<SEQMEAINT_NR;i++)
        {
            combosr[Sequence_c]->addItem(SeqMeaSymbol::convert((SeqMeaIntervalTime)i));
        }
    }

    if (combos[Sequence_d]->count() == 0 && combosr[Sequence_d]->count() == 0)
    {
        //添加下拉框条目
        combos[Sequence_d]->label->setText("D");
        //间隔时间下拉框
        for(i=0;i<SEQMEA_NR;i++)
        {
            combos[Sequence_d]->addItem(SeqMeaSymbol::convert((SeqMeaTime)i));
        }
        //测量时间下拉框
        for(i=0;i<SEQMEAINT_NR;i++)
        {
            combosr[Sequence_d]->addItem(SeqMeaSymbol::convert((SeqMeaIntervalTime)i));
        }
    }

    if (combos[Sequence_e]->count() == 0 && combosr[Sequence_e]->count() == 0)
    {
        //添加下拉框条目
        combos[Sequence_e]->label->setText("E");
        //间隔时间下拉框
        for(i=0;i<SEQMEA_NR;i++)
        {
            combos[Sequence_e]->addItem(SeqMeaSymbol::convert((SeqMeaTime)i));
        }
        //测量时间下拉框
        for(i=0;i<SEQMEAINT_NR;i++)
        {
            combosr[Sequence_e]->addItem(SeqMeaSymbol::convert((SeqMeaIntervalTime)i));
        }
    }
}

ConfigEditSeqMeaMenu::ConfigEditSeqMeaMenu()
    :SubMenu(trs("SeqMeaMenu")),
    d_ptr(new ConfigEditSeqMeaMenuPrivate())
{
    setDesc(trs("SeqMeaMenuDesc"));
    startLayout();
}

ConfigEditSeqMeaMenu::~ConfigEditSeqMeaMenu()
{

}


void ConfigEditSeqMeaMenu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    int i;

#if 1
    QHBoxLayout *hhlayout = new QHBoxLayout();
    hhlayout->setContentsMargins(QMargins(0, 0, 0, 0));
    hhlayout->addStretch();

    //设置标签头
    for(i=0;i<ConfigEditSeqMeaMenuPrivate::LabelListMax;i++)
    {
        QLabel *label = new QLabel(trs(d_ptr->Labels[i]));
        label->setFont(defaultFont());
        label->setFixedSize(labelWidth,ITEM_H);
        hhlayout->addWidget(label);
    }
    mainLayout->addLayout(hhlayout);

    //设置序列测量时间下拉框
    for(i=0;i<ConfigEditSeqMeaMenuPrivate::ComboListMax;i++)
    {
        //申请一个标签下拉框，用于填充子菜单布局中
        IComboList *combo = new IComboList(trs(d_ptr->combos[i]->label->text()));
        combo->setFont(defaultFont());
        //设置标签的形状大小
        combo->label->setFixedSize(labelWidth, ITEM_H);
        //设置下拉框的形状大小
        combo->combolist->setFixedSize(btnWidth, ITEM_H);
        //建立槽函数
        connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
        //对成员变量进行设置数值（安全设置） 后续再看。。。
        combo->setProperty("comboId", qVariantFromValue(i));
        //将此布局加入到子菜单中的链表节点上
        mainLayout->addWidget(combo);
        //存放每个下拉框的指针
        d_ptr->combos[i] = combo;

        //设置测量时间间隔下拉框
        //申请一个标签下拉框，用于填充子菜单布局中
        ComboList *combo1 = new ComboList();
        combo1->setFont(defaultFont());
        //设置下拉框的形状大小
        combo1->setFixedSize(btnWidth,ITEM_H);
        //建立槽函数
        connect(combo1, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
        //对成员变量进行设置数值（安全设置） 后续再看。。。
        combo1->setProperty("comboId", qVariantFromValue(i));
        //将此布局加入到子菜单中的链表节点上
        mainLayout->addWidget(combo1);
        //存放每个下拉框的指针
        d_ptr->combosr[i] = combo1;
    }

#endif
}

void ConfigEditSeqMeaMenu::readyShow()
{
    d_ptr->loadOptions();
}

void ConfigEditSeqMeaMenu::onBtnClick()
{

}

void ConfigEditSeqMeaMenu::onComboListConfigChanged(int index)
{
    IComboList *combo = qobject_cast<IComboList *>(sender());
    if(!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }

    ConfigEditSeqMeaMenuPrivate::ComboListId comboId = (ConfigEditSeqMeaMenuPrivate::ComboListId) combo->property("comboId").toInt();
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    switch (comboId)
    {

    default:
        qdebug("Invalid combo id.");
        break;
    }
}
