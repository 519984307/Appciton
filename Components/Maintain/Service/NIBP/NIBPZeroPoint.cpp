#include "NIBPZeroPoint.h"
#include <QHBoxLayout>
#include "Debug.h"
#include "NIBPParamService.h"
#include "NIBPRepair.h"

NIBPZeroPoint *NIBPZeroPoint::_selfObj = NULL;

//校零模式
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPZeroPoint::NIBPZeroPoint() : SubMenu(trs("ServiceCalibrateZero"))
{
    setDesc(trs("NIBPCalibrateZero"));

    startLayout();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPZeroPoint::layoutExec()
{
    int submenuW = nibprepair.getSubmenuWidth();
    int submenuH = nibprepair.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);
    setTitleEnable(true);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int pressuresize = fontManager.getFontSize(5);
    int Service_H = 50;
    int btnWidth = itemW / 2.5;

    QHBoxLayout *labelLayout_pressure = new QHBoxLayout();
    QLabel *l = new QLabel(trs("ServicePressure"));
    l->setFixedSize(btnWidth, Service_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(pressuresize));
    labelLayout_pressure->addWidget(l);

    _value = new QLabel();
    _value->setFixedSize(btnWidth, Service_H);
    _value->setAlignment(Qt::AlignCenter);
    _value->setFont(fontManager.textFont(pressuresize));
    _value->setText(InvStr());
    labelLayout_pressure->addWidget(_value);

    l = new QLabel();
    l->setText(Unit::getSymbol(nibpParamService.getUnit()));
    l->setFixedSize(50, Service_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    labelLayout_pressure->addWidget(l);
    mainLayout->addLayout(labelLayout_pressure);

    _startSwitchBtn= new LButtonEx();
    _startSwitchBtn->setText(trs("ServiceStart"));
    _startSwitch = true;
    _startSwitchBtn->setFont(fontManager.textFont(fontSize));
    _startSwitchBtn->setFixedSize(btnWidth, ITEM_H);
    connect(_startSwitchBtn, SIGNAL(realReleased()), this, SLOT(_btnReleased()));
    mainLayout->addWidget(_startSwitchBtn, 0, Qt::AlignRight);
}

/**************************************************************************************************
 * 初始化。
 *************************************************************************************************/
void NIBPZeroPoint::init(void)
{
    _value->setText(InvStr());
    startSwitch(true);
    _zeroPointFlag = false;
}

/**************************************************************************************************
 * 进入校零模式指令。
 *************************************************************************************************/
void NIBPZeroPoint::focusFirstItem()
{
    init();
    nibpParamService.switchState(NIBP_Service_CALIBRATE_ZERO);

    SubMenu::focusFirstItem();
}

/**************************************************************************************************
 * 关闭气阀指令。
 *************************************************************************************************/
void NIBPZeroPoint::_startReleased()
{
    nibpParamService.serviceCloseValve(true);
}

/**************************************************************************************************
 * 校零指令。
 *************************************************************************************************/
void NIBPZeroPoint::_zeroReleased()
{
    nibpParamService.servicePressureZero();
}

/**************************************************************************************************
 * 关闭气阀、校零按钮信号槽。
 *************************************************************************************************/
void NIBPZeroPoint::_btnReleased()
{
    if(_zeroPointFlag)
    {
        if(_startSwitch)
        {
            _startReleased();
        }
        else
        {
            _zeroReleased();
        }
    }
    else
    {
        nibprepair.messageBox();
    }
}

/**************************************************************************************************
 * 压力值。
 *************************************************************************************************/
void NIBPZeroPoint::setCuffPressure(int pressure)
{
    if (pressure == -1)
    {
        _value->setText(InvStr());
    }
    else
    {
        UnitType unit = nibpParamService.getUnit();
        if (unit == UNIT_MMHG)
        {
            _value->setNum(pressure);
            return;
        }

        QString str = Unit::convert(unit, UNIT_MMHG, pressure);
        _value->setText(str);
    }
}

/**************************************************************************************************
 * 进入模式应答。
 *************************************************************************************************/
void NIBPZeroPoint::unPacket(bool flag)
{
    _zeroPointFlag = flag;
    if( flag )
    {
        _value->setText(InvStr());
        _startSwitchBtn->setEnabled(true);
    }
    else
    {
        _value->setText("999");
        _startSwitchBtn->setEnabled(false);
    }
}

/**************************************************************************************************
 * 关闭气阀、校零按钮切换。
 *************************************************************************************************/
void NIBPZeroPoint::startSwitch(bool flag)
{
    if(flag)
    {
        _startSwitchBtn->setText(trs("ServiceStart"));
        _startSwitch = true;
    }
    else
    {
        _startSwitchBtn->setText(trs("ServiceSetZero"));
        _startSwitch = false;
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPZeroPoint::~NIBPZeroPoint()
{

}
