/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/

#include "NIBPZeroPointContent.h"
#include <QHBoxLayout>
#include "NIBPParam.h"
#include "NIBPRepairMenuWindow.h"
#include <QLabel>
#include "Button.h"
#include "NIBPEventDefine.h"
#include "NIBPServiceStateDefine.h"
#include <QShowEvent>

class NIBPZeroPointContentPrivate
{
public:
    NIBPZeroPointContentPrivate();
    QLabel *value;
    Button *startSwitchBtn;        // 关闭气阀、校零按钮

    bool startSwitch;                 // 关闭气阀、校零按钮切换标志
    bool zeroPointFlag;               // 进入模式标记
};




NIBPZeroPointContentPrivate::NIBPZeroPointContentPrivate()
    : value(NULL),
      startSwitchBtn(NULL),
      startSwitch(false),
      zeroPointFlag(false)
{
}

NIBPZeroPointContent *NIBPZeroPointContent::getInstance()
{
    static NIBPZeroPointContent *instance = NULL;
    if (!instance)
    {
        instance = new NIBPZeroPointContent();
    }
    return instance;
}


//校零模式
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPZeroPointContent::NIBPZeroPointContent()
    : MenuContent(trs("ServiceCalibrateZero"),
                  trs("NIBPCalibrateZero")),
      d_ptr(new NIBPZeroPointContentPrivate)
{
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPZeroPointContent::layoutExec()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    QLabel *l = new QLabel(trs("ServicePressure"));
    layout->addWidget(l);

    d_ptr->value = new QLabel();
    d_ptr->value->setText(InvStr());
    layout->addWidget(d_ptr->value);

    l = new QLabel();
    l->setText(Unit::getSymbol(nibpParam.getUnit()));
    layout->addWidget(l);

    d_ptr->startSwitchBtn = new Button();
    d_ptr->startSwitchBtn->setText(trs("ServiceStart"));
    d_ptr->startSwitchBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->startSwitch = true;
    connect(d_ptr->startSwitchBtn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    layout->addWidget(d_ptr->startSwitchBtn, 0, Qt::AlignRight);
}

void NIBPZeroPointContent::showEvent(QShowEvent *ev)
{
    d_ptr->startSwitchBtn->setFixedWidth(d_ptr->value->width());
    MenuContent::showEvent(ev);
}

/**************************************************************************************************
 * 初始化。
 *************************************************************************************************/
void NIBPZeroPointContent::init(void)
{
    d_ptr->value->setText(InvStr());
    startSwitch(true);
    d_ptr->zeroPointFlag = false;
}

/**************************************************************************************************
 * 进入校零模式指令。
 *************************************************************************************************/
bool NIBPZeroPointContent::focusNextPrevChild(bool next)
{
    init();

    nibpParam.switchState(NIBP_SERVICE_CALIBRATE_ZERO_STATE);

    MenuContent::focusNextPrevChild(next);

    return next;
}

/**************************************************************************************************
 * 关闭气阀指令。
 *************************************************************************************************/
void NIBPZeroPointContent::onStartReleased()
{
    nibpParam.provider().serviceValve(true);
}

/**************************************************************************************************
 * 校零指令。
 *************************************************************************************************/
void NIBPZeroPointContent::onZeroReleased()
{
    nibpParam.provider().servicePressureZero();
}

/**************************************************************************************************
 * 关闭气阀、校零按钮信号槽。
 *************************************************************************************************/
void NIBPZeroPointContent::onBtnReleased()
{
    if (d_ptr->zeroPointFlag)
    {
        if (d_ptr->startSwitch)
        {
            onStartReleased();
        }
        else
        {
            onZeroReleased();
        }
    }
    else
    {
        NIBPRepairMenuWindow::getInstance()->messageBox();
    }
}

/**************************************************************************************************
 * 压力值。
 *************************************************************************************************/
void NIBPZeroPointContent::setCuffPressure(int pressure)
{
    if (pressure == -1)
    {
        d_ptr->value->setText(InvStr());
    }
    else
    {
        UnitType unit = nibpParam.getUnit();
        if (unit == UNIT_MMHG)
        {
            d_ptr->value->setNum(pressure);
            return;
        }

        QString str = Unit::convert(unit, UNIT_MMHG, pressure);
        d_ptr->value->setText(str);
    }
}

/**************************************************************************************************
 * 进入模式应答。
 *************************************************************************************************/
void NIBPZeroPointContent::unPacket(bool flag)
{
    d_ptr->zeroPointFlag = flag;
    if (flag)
    {
        d_ptr->value->setText(InvStr());
        d_ptr->startSwitchBtn->setEnabled(true);
    }
    else
    {
        d_ptr->value->setText("999");
        d_ptr->startSwitchBtn->setEnabled(false);
    }
}

/**************************************************************************************************
 * 关闭气阀、校零按钮切换。
 *************************************************************************************************/
void NIBPZeroPointContent::startSwitch(bool flag)
{
    if (flag)
    {
        d_ptr->startSwitchBtn->setText(trs("ServiceStart"));
        d_ptr->startSwitch = true;
    }
    else
    {
        d_ptr->startSwitchBtn->setText(trs("ServiceSetZero"));
        d_ptr->startSwitch = false;
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPZeroPointContent::~NIBPZeroPointContent()
{
    delete d_ptr;
}
