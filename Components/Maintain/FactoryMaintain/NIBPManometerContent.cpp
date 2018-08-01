/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2019/7/24
 **/
#include "NIBPManometerContent.h"
#include <QLabel>
#include <QVBoxLayout>
#include "NIBPParam.h"
#include "NIBPRepairMenuWindow.h"
#include "NIBPEventDefine.h"
#include "NIBPServiceStateDefine.h"

class NIBPManometerContentPrivate
{
public:
    NIBPManometerContentPrivate();

    QLabel *value;
};



NIBPManometerContentPrivate::NIBPManometerContentPrivate()
    : value(NULL)
{
}

NIBPManometerContent *NIBPManometerContent::getInstance()
{
    static NIBPManometerContent *instance = NULL;
    if (!instance)
    {
        instance = new NIBPManometerContent();
    }
    return instance;
}

// 压力计模式
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPManometerContent::NIBPManometerContent()
    : MenuContent(trs("ServiceManometer"),
                  trs("NIBPManometerContent")),
      d_ptr(new NIBPManometerContentPrivate)
{
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPManometerContent::layoutExec()
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
}

/**************************************************************************************************
 * 初始化。
 *************************************************************************************************/
void NIBPManometerContent::init(void)
{
    d_ptr->value->setText(InvStr());
}

/**************************************************************************************************
 * 进入压力计模式指令。
 *************************************************************************************************/

bool NIBPManometerContent::focusNextPrevChild(bool next)
{
    init();

    nibpParam.switchState(NIBP_SERVICE_MANOMETER_STATE);

    MenuContent::focusNextPrevChild(next);

    return next;
}

/**************************************************************************************************
 * 压力值。
 *************************************************************************************************/
void NIBPManometerContent::setCuffPressure(int pressure)
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
 * 模式应答。
 *************************************************************************************************/
void NIBPManometerContent::unPacket(bool flag)
{
    if (flag)
    {
        d_ptr->value->setText(InvStr());
    }
    else
    {
        d_ptr->value->setText("999");
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPManometerContent::~NIBPManometerContent()
{
    delete d_ptr;
}

