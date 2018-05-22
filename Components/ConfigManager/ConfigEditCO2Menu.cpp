#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ConfigEditCO2Menu.h"
#include "CO2Param.h"
#include "RESPParam.h"
#include "IComboList.h"
#include "LabelButton.h"
#include "ISpinBox.h"
#include "LanguageManager.h"
#include "MenuManager.h"
#include "ConfigEditMenuGrp.h"
#include "Config.h"
ConfigCO2Menu *ConfigCO2Menu::_selfObj = NULL;

/**************************************************************************************************
 * 窒息时间改变。
 *************************************************************************************************/
//void ConfigCO2Menu::_apneaTimeSlot(int index)
//{
//    co2Param.setApneaTime((CO2ApneaTime)index);
//    respParam.setApneaTime((RESPApneaTime)index);
//}

/**************************************************************************************************
 * 速度改变。
 *************************************************************************************************/
void ConfigCO2Menu::_speedSlot(int index)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
   // co2Param.setSweepSpeed((CO2SweepSpeed)index);
    config->setNumValue("CO2|SweepSpeed",index);
}

/**************************************************************************************************
 * FICO2显示开关。
 *************************************************************************************************/
void ConfigCO2Menu::_fico2DisplaySlot(int index)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
   // int index  = 0;
   // co2Param.setFiCO2Display((CO2FICO2Display)index);
    config->setNumValue("CO2|FICO2Display",index);
}

/**************************************************************************************************
 * O2补偿气体改变。
 *************************************************************************************************/
void ConfigCO2Menu::_o2CompenReleased(QString strValue)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
   // int index  = 0;
    //co2Param.setCompensation(CO2_COMPEN_O2, strValue.toInt());
    config->setNumValue("CO2|Compensation|O2",strValue.toInt());
}

/**************************************************************************************************
 * N2O补偿气体改变。
 *************************************************************************************************/
void ConfigCO2Menu::_n2oCompenReleased(QString strValue)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    //int index  = 0;
    //co2Param.setCompensation(CO2_COMPEN_N2O, strValue.toInt());
    config->setNumValue("CO2|Compensation|NO2",strValue.toInt());
}

/**************************************************************************************************
 * 执行校零。
 *************************************************************************************************/
void ConfigCO2Menu::_zeroCalibReleased(void)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int index  = 1;
    //co2Param.zeroCalibration();
    config->setNumValue("CO2|CeroCalibration",index);
}

/**************************************************************************************************
 * 载入当前配置。
 *************************************************************************************************/
void ConfigCO2Menu::_loadOptions(void)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int index  = 0;
    // 窒息时间。
//    _apneaTime->setCurrentIndex(co2Param.getApneaTime());
    config->getNumValue("CO2|SweepSpeed",index);
    // 波形速度。CO2SweepSpeed
    _speed->setCurrentIndex(index);

    // 气体补偿。
    config->getNumValue("CO2|Compensation|O2",index);
    _o2Compen->setValue(index);
    config->getNumValue("CO2|Compensation|NO2",index);
    _n2oCompen->setValue(index);

    // 显示控制。
    config->getNumValue("CO2|FICO2Display",index);
    _fico2Display->setCurrentIndex(index);
}

/**************************************************************************************************
 * 显示前确定配置。
 *************************************************************************************************/
void ConfigCO2Menu::readyShow(void)
{
    _loadOptions();
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void ConfigCO2Menu::layoutExec(void)
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;


    _speed = new IComboList(trs("CO2SweepSpeed"));
    _speed->setFont(defaultFont());
    _speed->addItem(CO2Symbol::convert(CO2_SWEEP_SPEED_62_5));
    _speed->addItem(CO2Symbol::convert(CO2_SWEEP_SPEED_125));
    _speed->addItem(CO2Symbol::convert(CO2_SWEEP_SPEED_250));
    connect(_speed, SIGNAL(currentIndexChanged(int)), this, SLOT(_speedSlot(int)));
    _speed->label->setFixedSize(labelWidth, ITEM_H);
    _speed->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_speed);

    _fico2Display = new IComboList(trs("CO2FiCO2Display"));
    _fico2Display->setFont(defaultFont());
    _fico2Display->addItem(CO2Symbol::convert(CO2_FICO2_DISPLAY_OFF));
    _fico2Display->addItem(CO2Symbol::convert(CO2_FICO2_DISPLAY_ON));
    connect(_fico2Display, SIGNAL(currentIndexChanged(int)), this, SLOT(_fico2DisplaySlot(int)));
    _fico2Display->label->setFixedSize(labelWidth, ITEM_H);
    _fico2Display->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_fico2Display);

    _o2Compen = new ISpinBox(trs("O2Compensation"));
    _o2Compen->setRadius(4.0, 4.0);
    _o2Compen->setRange(0, 100);
    _o2Compen->setMode(ISPIN_MODE_INT);
    _o2Compen->setStep(1);
    _o2Compen->setFont(defaultFont());
    _o2Compen->setBorderColor(Qt::black);
    _o2Compen->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _o2Compen->setLayoutSpacing(ICOMBOLIST_SPACE);
    _o2Compen->enableArrow(false);
    connect(_o2Compen, SIGNAL(valueChange(QString,int)),
            this, SLOT(_o2CompenReleased(QString)));
    _o2Compen->setFixedHeight(ITEM_H);
    _o2Compen->setValueWidth(btnWidth);
    mainLayout->addWidget(_o2Compen);

    _n2oCompen = new ISpinBox(trs("N2OCompensation"));
    _n2oCompen->setRadius(4.0, 4.0);
    _n2oCompen->setRange(0, 100);
    _n2oCompen->setMode(ISPIN_MODE_INT);
    _n2oCompen->setStep(1);
    _n2oCompen->setFont(defaultFont());
    _n2oCompen->setBorderColor(Qt::black);
    _n2oCompen->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _n2oCompen->setLayoutSpacing(ICOMBOLIST_SPACE);
    _n2oCompen->enableArrow(false);
    connect(_n2oCompen, SIGNAL(valueChange(QString,int)),
            this, SLOT(_n2oCompenReleased(QString)));
    _n2oCompen->setFixedHeight(ITEM_H);
    _n2oCompen->setValueWidth(btnWidth);
    mainLayout->addWidget(_n2oCompen);

    _zeroCalib = new LabelButton(trs("CO2ZeroCalib"));
    _zeroCalib->setFont(defaultFont());
    _zeroCalib->setValue(trs("CO2ZeroStart"));
    connect(_zeroCalib->button, SIGNAL(realReleased()), this, SLOT(_zeroCalibReleased()));
    _zeroCalib->label->setFixedSize(labelWidth, ITEM_H);
    _zeroCalib->button->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_zeroCalib);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ConfigCO2Menu::ConfigCO2Menu() : SubMenu(trs("CO2")),
                                 _speed(NULL),
                                 _fico2Display(NULL),
                                 _o2Compen(NULL),
                                 _n2oCompen(NULL),
                                 _zeroCalib(NULL)
{
    setDesc(trs("CO2Desc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ConfigCO2Menu::~ConfigCO2Menu()
{

}
