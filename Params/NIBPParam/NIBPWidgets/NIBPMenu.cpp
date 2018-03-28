#include "NIBPMenu.h"
#include "NIBPParam.h"
#include "IComboList.h"
#include "LanguageManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "NIBPParam.h"
#include "MenuManager.h"
#include "IConfig.h"

NIBPMenu *NIBPMenu::_selfObj = NULL;

/**************************************************************************************************
 * 测量模式改变。
 *************************************************************************************************/
void NIBPMenu::_measureModeSlot(int index)
{
    nibpParam.setMeasurMode((NIBPMode)index);
}

/**************************************************************************************************
 * 自动测量的时间间隔改变。
 *************************************************************************************************/
void NIBPMenu::_autoIntervalSlot(int index)
{
    nibpParam.setAutoInterval((NIBPAutoInterval)index);
}

/**************************************************************************************************
 * 预充气值改变。
 *************************************************************************************************/
void NIBPMenu::_initialCuffChangeSlot(int index)
{
    nibpParam.setInitPressure(index);
}

/**************************************************************************************************
 * STAT按钮链接。
 *************************************************************************************************/
void NIBPMenu::_statBtnSlot()
{
    if (nibpParam.currentState() == NIBP_STATE_ERROR)
    {
        return;
    }
    //退出STAT模式
    if (nibpParam.isSTATMeasure())
    {
        _statBtn->setText(trs("STATSTART"));
        nibpCountdownTime.STATMeasureStop();
    }
    //进入STAT模式
    else
    {
        _statBtn->setText(trs("STATSTOP"));
    }

    nibpParam.setMeasurMode(NIBP_MODE_STAT);
}

/**************************************************************************************************
 * 刷新STAT按钮显示。
 *************************************************************************************************/
void NIBPMenu::statBtnShow(void)
{
    if (nibpParam.isConnected())
    {
        _statBtn->setDisabled(false);
    }
    else
    {
        _statBtn->setDisabled(true);
    }

    if (nibpParam.isSTATMeasure())
    {
        _statBtn->setText(trs("STATSTOP"));
    }
    else
    {
        _statBtn->setText(trs("STATSTART"));
    }
}

/**************************************************************************************************
 * 载入当前配置。
 *************************************************************************************************/
void NIBPMenu::_loadOptions(void)
{
    //智能充气
//    _intelligentInflate->setCurrentIndex(nibpParam.getIntelligentInflate());

    // 显示模式。
    int index = nibpParam.getSuperMeasurMode();
    _measureMode->setCurrentIndex(index);
//    if(index == NIBP_MODE_MANUAL)
//    {
//        _autoInterval->setDisabled(true);
//    }
//    else
//    {
//        _autoInterval->setDisabled(false);
//    }

    // 时间
    _autoInterval->setCurrentIndex(nibpParam.getAutoInterval());

    int unit = UNIT_MMHG;
    superConfig.getNumValue("Local|NIBPUnit", unit);
    _initialCuff->clear();
    PatientType type = patientManager.getType();
    if (type == PATIENT_TYPE_ADULT)
    {
        for (unsigned i = 0; i < NIBP_ADULT_INITIAL_CUFF_NR; ++i)
        {
            QString str = NIBPSymbol::convert((NIBPAdultInitialCuff)i);
            if (unit == UNIT_KPA)
            {
                str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
            }
            str = str + " " + Unit::getSymbol((UnitType)unit);
            _initialCuff->addItem(str);
        }
        superRunConfig.getNumValue("NIBP|AdultInitialCuffInflation", index);
        _initialCuff->setCurrentIndex(index);
    }
    else if (type == PATIENT_TYPE_PED)
    {
        for (unsigned i = 0; i < NIBP_PREDIATRIC_INITIAL_CUFF_NR; ++i)
        {
            QString str = NIBPSymbol::convert((NIBPPrediatrictInitialCuff)i);
            if (unit == UNIT_KPA)
            {
                str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
            }
            str = str + " " + Unit::getSymbol((UnitType)unit);
            _initialCuff->addItem(str);
        }
        superRunConfig.getNumValue("NIBP|PedInitialCuffInflation", index);
        _initialCuff->setCurrentIndex(index);
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        for (unsigned i = 0; i < NIBP_NEONATAL_INITIAL_CUFF_NR; ++i)
        {
            QString str = NIBPSymbol::convert((NIBPNeonatalInitialCuff)i);
            if (unit == UNIT_KPA)
            {
                str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
            }
            str = str + " " + Unit::getSymbol((UnitType)unit);
            _initialCuff->addItem(str);
        }
        superRunConfig.getNumValue("NIBP|NeoInitialCuffInflation", index);
        _initialCuff->setCurrentIndex(index);
    }

    statBtnShow();
}

/**************************************************************************************************
 * 显示前确定配置。
 *************************************************************************************************/
void NIBPMenu::readyShow(void)
{
    _loadOptions();
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void NIBPMenu::layoutExec(void)
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

//    _intelligentInflate = new IComboList(trs("IntelligentInflate"));
//    _intelligentInflate->setFont(defaultFont());
//    _intelligentInflate->label->setFixedSize(labelWidth, ITEM_H);
//    _intelligentInflate->combolist->setFixedSize(btnWidth, ITEM_H);
//    mainLayout->addWidget(_intelligentInflate);
//    _intelligentInflate->addItem(trs(NIBPSymbol::convert(NIBP_INTELLIGENT_INFLATE_OFF)));
//    _intelligentInflate->addItem(trs(NIBPSymbol::convert(NIBP_INTELLIGENT_INFLATE_ON)));
//    connect(_intelligentInflate, SIGNAL(currentIndexChanged(int)),
//            this, SLOT(_intelligentInflateSlot(int)));

    _measureMode = new IComboList(trs("NIBPMeasureMode"));
    _measureMode->setFont(defaultFont());
    _measureMode->label->setFixedSize(labelWidth, ITEM_H);
    _measureMode->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_measureMode);
    _measureMode->addItem(trs(NIBPSymbol::convert(NIBP_MODE_MANUAL)));
    _measureMode->addItem(trs(NIBPSymbol::convert(NIBP_MODE_AUTO)));
    connect(_measureMode, SIGNAL(currentIndexChanged(int)), this, SLOT(_measureModeSlot(int)));

    _autoInterval = new IComboList(trs("NIBPAutoInterval"));
    _autoInterval->setFont(defaultFont());
    _autoInterval->label->setFixedSize(labelWidth, ITEM_H);
    _autoInterval->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_autoInterval);

    _initialCuff = new IComboList(trs("NIBPInitialCuff"));
    _initialCuff->setFont(defaultFont());
    _initialCuff->label->setFixedSize(labelWidth, ITEM_H);
    _initialCuff->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_initialCuff, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_initialCuffChangeSlot(int)));
    mainLayout->addWidget(_initialCuff);

    for (int i = 0; i < NIBP_AUTO_INTERVAL_NR; i++)
    {
        _autoInterval->addItem(trs(NIBPSymbol::convert((NIBPAutoInterval)i)));
    }
    connect(_autoInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(_autoIntervalSlot(int)));

    _statBtn = new LButtonEx();
    _statBtn->setText(trs("STATSTART"));
    _statBtn->setFont(defaultFont());
    _statBtn->setFixedSize(btnWidth, ITEM_H);
    connect(_statBtn, SIGNAL(realReleased()), this, SLOT(_statBtnSlot()));
    mainLayout->addWidget(_statBtn, 0, Qt::AlignRight);
    int index = NIBP_PR_DISPLAY_NR;
    superConfig.getNumValue("NIBP|StatFunction", index);
    if (index == NIBP_PR_DISPLAY_OFF)
    {
        _statBtn->hide();
    }
    else
    {
        _statBtn->show();
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPMenu::NIBPMenu() : SubMenu(trs("NIBPMenu"))
{
    setDesc(trs("NIBPMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPMenu::~NIBPMenu()
{

}
