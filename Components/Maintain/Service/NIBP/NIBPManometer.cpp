#include <QVBoxLayout>
#include <QLabel>
#include "IButton.h"
#include "IComboList.h"
#include "NIBPManometer.h"
#include "Debug.h"
#include "NIBPParamService.h"
#include "NIBPRepair.h"

NIBPManometer *NIBPManometer::_selfObj = NULL;

//压力计模式
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPManometer::NIBPManometer() : SubMenu(trs("ServiceManometer"))
{
    setDesc(trs("NIBPManometer"));

    startLayout();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPManometer::layoutExec()
{
    int submenuW = nibprepair.getSubmenuWidth();
    int submenuH = nibprepair.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int pressuresize = fontManager.getFontSize(5);
    int Service_H = 50;
    int btnWidth = itemW / 2.5;

    QHBoxLayout *labelLayout = new QHBoxLayout();
    QLabel *l = new QLabel(trs("ServicePressure"));
    l->setFixedSize(btnWidth, Service_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(pressuresize));
    labelLayout->addWidget(l);

    _value = new QLabel();
    _value->setFixedSize(btnWidth, Service_H);
    _value->setAlignment(Qt::AlignCenter);
    _value->setFont(fontManager.textFont(pressuresize));
    _value->setText(InvStr());
    labelLayout->addWidget(_value);

    l = new QLabel();
    l->setText(Unit::getSymbol(nibpParamService.getUnit()));
    l->setFixedSize(50, Service_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    labelLayout->addWidget(l);
    mainLayout->addLayout(labelLayout);
}

/**************************************************************************************************
 * 初始化。
 *************************************************************************************************/
void NIBPManometer::init(void)
{
    _value->setText(InvStr());
}

/**************************************************************************************************
 * 进入压力计模式指令。
 *************************************************************************************************/
void NIBPManometer::focusFirstItem()
{
    init();
    // 转换到测量状态。
    nibpParamService.switchState(NIBP_Service_MANOMETER);

    SubMenu::focusFirstItem();
}

/**************************************************************************************************
 * 压力值。
 *************************************************************************************************/
void NIBPManometer::setCuffPressure(int pressure)
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
 * 模式应答。
 *************************************************************************************************/
void NIBPManometer::unPacket(bool flag)
{
    if( flag )
    {
        _value->setText(InvStr());
    }
    else
    {
        _value->setText("999");
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPManometer::~NIBPManometer()
{

}

