#include "RESPMenu.h"
#include "RESPParam.h"
#include "CO2Param.h"
#include "IComboList.h"
#include "LanguageManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "MenuManager.h"
#include "AlarmLimitMenu.h"
#include "PublicMenuManager.h"

RESPMenu *RESPMenu::_selfObj = NULL;

/**************************************************************************************************
 * 波形扫描速度设置。
 *************************************************************************************************/
void RESPMenu::_sweepSpeedSlot(int index)
{
    respParam.setSweepSpeed((RESPSweepSpeed) index);
}

/**************************************************************************************************
 * 窒息时间间隔设置。
 *************************************************************************************************/
//void RESPMenu::_apneaTimeSlot(int /*index*/)
//{
//    respParam.setApneaTime((RESPApneaTime)index);
//    co2Param.setApneaTime((CO2ApneaTime)index);
//}

/**************************************************************************************************
 * 波形放大倍数设置。
 *************************************************************************************************/
void RESPMenu::_zoomSlot(int index)
{
    respParam.setZoom((RESPZoom) index);
}

/**************************************************************************************************
 * 呼吸导联设置。
 *************************************************************************************************/
void RESPMenu::_calcLeadChange(int index)
{
    respParam.setCalcLead((RESPLead)index);
}

/**************************************************************************************************
 * RESP监护功能使能。
 *************************************************************************************************/
void RESPMenu::_monitorSlot(int index)
{
    respParam.setRespMonitoring(index);
}
//报警项设置
void RESPMenu::_alarmLbtnSlot()
{
    alarmLimitMenu.setFocusIndex(SUB_PARAM_NONE+1);
    publicMenuManager.changePage(&alarmLimitMenu, &respMenu);
}
/**************************************************************************************************
 * 载入当前配置。
 *************************************************************************************************/
void RESPMenu::_loadOptions(void)
{
    // 窒息时间。
//    _apneaTime->setCurrentIndex(respParam.getApneaTime());

    // 波速。
    _sweepSpeed->setCurrentIndex(respParam.getSweepSpeed());

    // 缩放倍数。
//    _zoom->setCurrentIndex(respParam.getZoom());

    // 呼吸导联
//    _calcLead->setCurrentIndex(respParam.getCalcLead());

    // 监护使能
    _monitor->setCurrentIndex(respParam.getRespMonitoring());
}

/**************************************************************************************************
 * 显示前确定配置。
 *************************************************************************************************/
void RESPMenu::readyShow(void)
{
    _loadOptions();
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void RESPMenu::layoutExec(void)
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int space = 5;
    int itemW = submenuW - space;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    _sweepSpeed = new IComboList(trs("RESPSweepSpeed"));
    _sweepSpeed->setFont(defaultFont());
    _sweepSpeed->label->setFixedSize(labelWidth, ITEM_H);
    _sweepSpeed->combolist->setFixedSize(btnWidth, ITEM_H);
    _sweepSpeed->setSpacing(space);
    _sweepSpeed->addItem(RESPSymbol::convert(RESP_SWEEP_SPEED_6_25));
    _sweepSpeed->addItem(RESPSymbol::convert(RESP_SWEEP_SPEED_12_5));
    _sweepSpeed->addItem(RESPSymbol::convert(RESP_SWEEP_SPEED_25_0));
    connect(_sweepSpeed, SIGNAL(currentIndexChanged(int)), this, SLOT(_sweepSpeedSlot(int)));
    mainLayout->addWidget(_sweepSpeed);

//    _apneaTime = new IComboList(trs("RESPApneaTime"));
//    _apneaTime->setFont(defaultFont());
//    _apneaTime->label->setFixedSize(labelWidth, ITEM_H);
//    _apneaTime->combolist->setFixedSize(btnWidth, ITEM_H);
//    _apneaTime->setSpacing(space);
//    for (int i = 0; i < RESP_APNEA_TIME_NR; ++i)
//    {
//        _apneaTime->addItem(trs(RESPSymbol::convert((RESPApneaTime)i)));
//    }
//    connect(_apneaTime, SIGNAL(currentIndexChanged(int)), this, SLOT(_apneaTimeSlot(int)));
//    mainLayout->addWidget(_apneaTime);

//    _zoom = new IComboList(trs("RESPZoom"));
//    _zoom->setFont(defaultFont());
//    _zoom->label->setFixedSize(labelWidth, ITEM_H);
//    _zoom->combolist->setFixedSize(btnWidth, ITEM_H);
//    _zoom->setSpacing(space);
//    _zoom->addItem(RESPSymbol::convert(RESP_ZOOM_X025));
//    _zoom->addItem(RESPSymbol::convert(RESP_ZOOM_X050));
//    _zoom->addItem(RESPSymbol::convert(RESP_ZOOM_X100));
//    _zoom->addItem(RESPSymbol::convert(RESP_ZOOM_X200));
//    _zoom->addItem(RESPSymbol::convert(RESP_ZOOM_X300));
//    _zoom->addItem(RESPSymbol::convert(RESP_ZOOM_X400));
//    _zoom->addItem(RESPSymbol::convert(RESP_ZOOM_X500));
//    connect(_zoom, SIGNAL(currentIndexChanged(int)), this, SLOT(_zoomSlot(int)));
//    mainLayout->addWidget(_zoom);

//    _calcLead = new IComboList(trs("RESPLead"));
//    _calcLead->setFont(defaultFont());
//    _calcLead->label->setFixedSize(labelWidth, ITEM_H);
//    _calcLead->combolist->setFixedSize(btnWidth, ITEM_H);
//    _calcLead->setSpacing(space);
//    _calcLead->addItem(RESPSymbol::convert(RESP_LEAD_I));
//    _calcLead->addItem(RESPSymbol::convert(RESP_LEAD_II));
//    connect(_calcLead, SIGNAL(currentIndexChanged(int)), this, SLOT(_calcLeadChange(int)));
//    mainLayout->addWidget(_calcLead);

    _monitor = new IComboList(trs("RESPAutoActivation"));
    _monitor->setFont(defaultFont());
    _monitor->label->setFixedSize(labelWidth, ITEM_H);
    _monitor->combolist->setFixedSize(btnWidth, ITEM_H);
    _monitor->setSpacing(space);
    _monitor->addItem(trs("Disable"));
    _monitor->addItem(trs("Enable"));
    connect(_monitor, SIGNAL(currentIndexChanged(int)), this, SLOT(_monitorSlot(int)));
    mainLayout->addWidget(_monitor);

    _alarmLbtn = new LabelButton("");
    _alarmLbtn->setFont(defaultFont());
    _alarmLbtn->label->setFixedSize(labelWidth, ITEM_H);
    _alarmLbtn->button->setFixedSize(btnWidth, ITEM_H);
    _alarmLbtn->button->setText(trs("AlarmLimitSetUp"));
    connect(_alarmLbtn->button, SIGNAL(realReleased()), this, SLOT(_alarmLbtnSlot()));
    mainLayout->addWidget(_alarmLbtn);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RESPMenu::RESPMenu() : SubMenu(trs("RESPMenu"))
{
    setDesc(trs("RESPMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RESPMenu::~RESPMenu()
{

}
