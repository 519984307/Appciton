#include "PACEMenu.h"
#include "PACEParam.h"
#include "PACESymbol.h"
#include "IComboList.h"
#include "LabelButton.h"
#include "LanguageManager.h"
#include "SetWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>


PACEMenu *PACEMenu::_selfObj = NULL;

/**************************************************************************************************
 * PACE模式。
 *************************************************************************************************/
void PACEMenu::_modeSlot(int index)
{
    paceParam.setMode((PACEMode)index);
}

/**************************************************************************************************
 * 输出节律改变。
 *************************************************************************************************/
void PACEMenu::_rateSlot(int index)
{
    paceParam.setRate(index * 2 + 30);
}

/**************************************************************************************************
 * 输出电流改变。
 *************************************************************************************************/
void PACEMenu::_currentSlot(int index)
{
    if (index == 0)
    {
        paceParam.setCurrent(index);
        return;
    }

    paceParam.setCurrent((index - 1) * 2 + 8);
}

/**************************************************************************************************
 * 载入当前配置。
 *************************************************************************************************/
void PACEMenu::_loadOptions(void)
{
    _mode->setCurrentIndex(paceParam.getMode());
    _rate->setCurrentIndex((paceParam.getRate() - 30) / 2);

    int current = paceParam.getCurrent();
    if (current == 0)
    {
        _current->setCurrentIndex(current);
    }
    else
    {
        current = (current - 8) / 2 + 1;
        _current->setCurrentIndex(current);
    }
}

/**************************************************************************************************
 * 显示前确定配置。
 *************************************************************************************************/
void PACEMenu::readyShow(void)
{
    _loadOptions();
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void PACEMenu::layoutExec(void)
{
    setMenuSize(SUB_MENU_WIDTH, SUB_MENU_HEIGHT);

    int fontSize = 18;
    int btnWidth = SUB_MENU_ITEM_WIDTH / 3;
    int labelWidth = btnWidth * 2;

    _rate = new IComboList(trs("PACERate"));
    _rate->setFont(fontManager.textFont(fontSize));
    for (int i = 30; i <= 180; i++, i++)
    {
        _rate->addItem(QString::number(i));
    }
    connect(_rate, SIGNAL(currentIndexChanged(int)), this, SLOT(_rateSlot(int)));
    _rate->label->setFixedSize(labelWidth, ITEM_H);
    _rate->combolist->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_rate);

    _current = new IComboList(trs("PACECurrent"));
    _current->setFont(fontManager.textFont(fontSize));
    _current->addItem("0");
    for (int i = 8; i <= 140; i++, i++)
    {
        _current->addItem(QString::number(i));
    }
    connect(_current, SIGNAL(currentIndexChanged(int)), this, SLOT(_currentSlot(int)));
    _current->label->setFixedSize(labelWidth, ITEM_H);
    _current->combolist->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_current);

    _mode = new IComboList(trs("PACEMode"));
    _mode->setFont(fontManager.textFont(fontSize));
    _mode->addItem(trs(PACESymbol::convert(PACE_MODE_DEMAND)));
    _mode->addItem(trs(PACESymbol::convert(PACE_MODE_FIXED_RATE)));
    connect(_mode, SIGNAL(currentIndexChanged(int)), this, SLOT(_modeSlot(int)));
    _mode->label->setFixedSize(labelWidth, ITEM_H);
    _mode->combolist->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_mode);

    _returnBtn = new LButton();
    _returnBtn->setText(trs("SupervisorReturn"));
    _returnBtn->setFont(fontManager.textFont(fontSize));
    _returnBtn->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_returnBtn, true);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PACEMenu::PACEMenu() : SubMenu(trs("PACEMenu"))
{
    setDesc(trs("PACEMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PACEMenu::~PACEMenu()
{

}
