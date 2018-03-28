#include "CPRMenu.h"
#include "CPRParam.h"
#include "CPRSymbol.h"
#include "IComboList.h"
#include "LabelButton.h"
#include "LanguageManager.h"
#include "SetWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>


CPRMenu *CPRMenu::_selfObj = NULL;

/**************************************************************************************************
 * 使能CPR。
 *************************************************************************************************/
void CPRMenu::_enableSlot(int index)
{
    cprParam.setEnable((CPREnable)index);
    _activeControl();
}

/**************************************************************************************************
 * 语音提示。
 *************************************************************************************************/
void CPRMenu::_voicePromptSlot(int index)
{
    cprParam.setVoicePrompt((CPRVoicePrompt)index);
}

/**************************************************************************************************
 * 节拍器控制。
 *************************************************************************************************/
void CPRMenu::_metronomeSlot(int index)
{
    cprParam.setMetronome((CPRMetronome)index);
}

/**************************************************************************************************
 * 节拍器节律改变。
 *************************************************************************************************/
void CPRMenu::_metronomeRateSlot(void)
{
    SetWidget set;
    set.setUnit(UNIT_PPM);
    set.setRange(50, 150);
    set.setMaxInputLength(3);
    set.setCurrentValue(cprParam.getMetronomeRate());
    if (set.exec() == 1)
    {
        int v;
        set.getIntValue(v);
        cprParam.setMetronomeRate(v);
        _metronomeRate->setValue(v);
    }
}

/**************************************************************************************************
 * 最低可接受按压节律下限改变。
 *************************************************************************************************/
void CPRMenu::_lowRateLimitSlot(void)
{
    int low = cprParam.getLowRateLimit();
    int high = cprParam.getHighRateLimit();

    SetWidget set;
    set.setUnit(UNIT_CPM);
    set.setRange(50, high - 1);
    set.setMaxInputLength(3);
    set.setCurrentValue(low);
    if (set.exec() == 1)
    {
        int v;
        set.getIntValue(v);
        cprParam.setLowRateLimit(v);
        _lowRateLimit->setValue(v);
    }
}

/**************************************************************************************************
 * 最低可接受按压节律上限改变。
 *************************************************************************************************/
void CPRMenu::_highRateLimitSlot(void)
{
    int low = cprParam.getLowRateLimit();
    int high = cprParam.getHighRateLimit();

    SetWidget set;
    set.setUnit(UNIT_CPM);
    set.setRange(low + 1, 150);
    set.setMaxInputLength(3);
    set.setCurrentValue(high);
    if (set.exec() == 1)
    {
        int v;
        set.getIntValue(v);
        cprParam.setHighRateLimit(v);
        _highRateLimit->setValue(v);
    }
}

/**************************************************************************************************
 * 最低可接受按压深度改变。
 *************************************************************************************************/
void CPRMenu::_minimumDepthSlot(void)
{
    int depth = cprParam.getMiminumDepth();

    SetWidget set;
    set.setUnit(UNIT_INCH, UNIT_CM);
    set.setRange(0.75, 4.0);
    set.setFloatBits(2);
    set.setMaxInputLength(4);
    set.setCurrentValue((double)(depth / 1000.0));
    if (set.exec() == 1)
    {
        float v;
        set.getFloatValue(v);
        cprParam.setMinimumDepth((int)(v * 1000));
        _minimumDepth->setValue(v, 2);
    }
}

/**************************************************************************************************
 * 单位更改。
 *************************************************************************************************/
void CPRMenu::_unitSlot(int index)
{
    cprParam.setUnit(index ? UNIT_CM : UNIT_INCH);
    _loadMinimumDepth();
}

/**************************************************************************************************
 * 根据CPR的使能状态控制设置选项是否可用。
 *************************************************************************************************/
void CPRMenu::_activeControl(void)
{
    // 在CPR使能状态下不能设置。
    bool enable = !cprParam.getEnable();
    _metronome->setEnabled(enable);
    _metronomeRate->setEnabled(enable);
    _highRateLimit->setEnabled(enable);
    _lowRateLimit->setEnabled(enable);
    _minimumDepth->setEnabled(enable);
}

/**************************************************************************************************
 * 载入最小按压深度。
 *************************************************************************************************/
void CPRMenu::_loadMinimumDepth(void)
{
    double depth = cprParam.getMiminumDepth() / 1000.0;
    if (cprParam.getUnit() == UNIT_CM)
    {
        depth = Unit::convert(UNIT_CM, depth, UNIT_INCH, depth);
    }

    _minimumDepth->setValue(depth, 2);
}

/**************************************************************************************************
 * 载入当前配置。
 *************************************************************************************************/
void CPRMenu::_loadOptions(void)
{
    _enable->setCurrentIndex(cprParam.getEnable());
    _voicePrompt->setCurrentIndex(cprParam.getVoicePrompt());
    _metronome->setCurrentIndex(cprParam.getMetronome());
    _unit->setCurrentIndex((cprParam.getUnit() == UNIT_INCH) ? 0 : 1);

    _metronomeRate->setValue(cprParam.getMetronomeRate());
    _highRateLimit->setValue(cprParam.getHighRateLimit());
    _lowRateLimit->setValue(cprParam.getLowRateLimit());

    _loadMinimumDepth();
    _activeControl();
}

/**************************************************************************************************
 * 显示前确定配置。
 *************************************************************************************************/
void CPRMenu::readyShow(void)
{
    _loadOptions();
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void CPRMenu::layoutExec(void)
{
    setMenuSize(SUB_MENU_WIDTH, SUB_MENU_HEIGHT);

    int fontSize = 18;
    int btnWidth = SUB_MENU_ITEM_WIDTH / 3;
    int labelWidth = btnWidth * 2;

    _enable = new IComboList(trs("CPREnable"));
    _enable->setFont(fontManager.textFont(fontSize));
    _enable->addItem(trs(CPRSymbol::convert(CPR_ENABLE_OFF)));
    _enable->addItem(trs(CPRSymbol::convert(CPR_ENABLE_ON)));
    connect(_enable, SIGNAL(currentIndexChanged(int)), this, SLOT(_enableSlot(int)));
    _enable->label->setFixedSize(labelWidth, ITEM_H);
    _enable->combolist->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_enable);

    _voicePrompt = new IComboList(trs("CPRVoicePrompt"));
    _voicePrompt->setFont(fontManager.textFont(fontSize));
    _voicePrompt->addItem(trs(CPRSymbol::convert(CPR_VOCIE_PROMPT_OFF)));
    _voicePrompt->addItem(trs(CPRSymbol::convert(CPR_VOCIE_PROMPT_ON)));
    connect(_voicePrompt, SIGNAL(currentIndexChanged(int)), this, SLOT(_voicePromptSlot(int)));
    _voicePrompt->label->setFixedSize(labelWidth, ITEM_H);
    _voicePrompt->combolist->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_voicePrompt);

    _metronome = new IComboList(trs("CPRMetronome"));
    _metronome->setFont(fontManager.textFont(fontSize));
    _metronome->addItem(trs(CPRSymbol::convert(CPR_METRONOME_OFF)));
    _metronome->addItem(trs(CPRSymbol::convert(CPR_METRONOME_ON)));
    connect(_metronome, SIGNAL(currentIndexChanged(int)), this, SLOT(_metronomeSlot(int)));
    _metronome->label->setFixedSize(labelWidth, ITEM_H);
    _metronome->combolist->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_metronome);

    _metronomeRate = new LabelButton(trs("CPRMetronomeRate"));
    _metronomeRate->setFont(fontManager.textFont(fontSize));
    connect(_metronomeRate->button, SIGNAL(released()), this, SLOT(_metronomeRateSlot()));
    _metronomeRate->label->setFixedSize(labelWidth, ITEM_H);
    _metronomeRate->button->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_metronomeRate);

    _lowRateLimit = new LabelButton(trs("CPRLowRateLimit"));
    _lowRateLimit->setFont(fontManager.textFont(fontSize));
    connect(_lowRateLimit->button, SIGNAL(released()), this, SLOT(_lowRateLimitSlot()));
    _lowRateLimit->label->setFixedSize(labelWidth, ITEM_H);
    _lowRateLimit->button->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_lowRateLimit);

    _highRateLimit = new LabelButton(trs("CPRHighRateLimit"));
    _highRateLimit->setFont(fontManager.textFont(fontSize));
    connect(_highRateLimit->button, SIGNAL(released()), this, SLOT(_highRateLimitSlot()));
    _highRateLimit->label->setFixedSize(labelWidth, ITEM_H);
    _highRateLimit->button->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_highRateLimit);

    _minimumDepth = new LabelButton(trs("CPRMinimumDepth"));
    _minimumDepth->setFont(fontManager.textFont(fontSize));
    connect(_minimumDepth->button, SIGNAL(released(int)), this, SLOT(_minimumDepthSlot()));
    _minimumDepth->label->setFixedSize(labelWidth, ITEM_H);
    _minimumDepth->button->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_minimumDepth);

    _unit = new IComboList(trs("CPRUnit"));
    _unit->setFont(fontManager.textFont(fontSize));
    _unit->addItem(Unit::getSymbol(UNIT_INCH));
    _unit->addItem(Unit::getSymbol(UNIT_CM));
    connect(_unit, SIGNAL(currentIndexChanged(int)), this, SLOT(_unitSlot(int)));
    _unit->label->setFixedSize(labelWidth, ITEM_H);
    _unit->combolist->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_unit);

    _returnBtn = new LButton();
    _returnBtn->setText(trs("SupervisorReturn"));
    _returnBtn->setFont(fontManager.textFont(fontSize));
    _returnBtn->setFixedSize(btnWidth, ITEM_H);
    addSetMenuWin(_returnBtn, true);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CPRMenu::CPRMenu() : SubMenu(trs("CPRMenu"))
{
    setDesc(trs("CPRMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CPRMenu::~CPRMenu()
{

}
