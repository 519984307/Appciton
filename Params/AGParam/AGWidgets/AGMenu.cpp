#include "AGMenu.h"
#include "LanguageManager.h"
#include "MenuManager.h"
#include "IComboList.h"
#include "AGSymbol.h"
#include "AGParam.h"

AGMenu *AGMenu::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AGMenu::~AGMenu()
{

}

void AGMenu::readyShow(){}

void AGMenu::readyhide(){}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void AGMenu::layoutExec()
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    _speed = new IComboList(trs("AGSweepSpeed"));
    _speed->setFont(defaultFont());
    for (int i = 0; i < AG_SWEEP_SPEED_NR; i ++)
    {
        _speed->addItem(AGSymbol::convert((AGSweepSpeed)i));
    }
    connect(_speed, SIGNAL(currentIndexChanged(int)), this, SLOT(_speedSlot(int)));
    _speed->label->setFixedSize(labelWidth, ITEM_H);
    _speed->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_speed);

}

/**************************************************************************************************
 * 波形速度槽函数。
 *************************************************************************************************/
void AGMenu::_speedSlot(int index)
{
    agParam.setSweepSpeed((AGSweepSpeed)index);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AGMenu::AGMenu() : SubMenu(trs("AGMenu"))
{
    setDesc(trs("AGMenuDesc"));
    startLayout();
}
