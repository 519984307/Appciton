/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/
#include "NIBPCalibrateContent.h"
#include "NIBPSymbol.h"
#include "NIBPParam.h"
#include "NIBPRepairMenuWindow.h"
#include <QLabel>
#include "Button.h"
#include "ComboBox.h"
#include "SpinBox.h"
#include <QGridLayout>
#include "NIBPEventDefine.h"
#include "NIBPServiceStateDefine.h"

class NIBPCalibrateContentPrivate
{
public:
    NIBPCalibrateContentPrivate();
    QList<CalibrateSetItem *> itemList;
    CalibrateSetItem *item;
    QLabel *label;

    bool btnFlag1;                          // 用于判定是第一个校准点的返回值
    bool btnFlag2;                          // 用于判定是第二个校准点的返回值
    int  pressurevalue;                     // 校准点的值
    bool calibrateFlag;                     // 进入模式标志
};


NIBPCalibrateContentPrivate::NIBPCalibrateContentPrivate()
    : item(NULL),
      label(NULL),
      btnFlag1(false),
      btnFlag2(false),
      pressurevalue(0),
      calibrateFlag(false)
{
    itemList.clear();
    itemList.append(new CalibrateSetItem());
    itemList.append(new CalibrateSetItem());
}

NIBPCalibrateContent *NIBPCalibrateContent::getInstance()
{
    static NIBPCalibrateContent *instance = NULL;
    if (!instance)
    {
        instance = new NIBPCalibrateContent;
    }
    return instance;
}

// 校准模式
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPCalibrateContent::NIBPCalibrateContent() :
    MenuContent(trs("ServiceCalibrate"),
                trs("ServiceCalibrateDesc")),
    d_ptr(new NIBPCalibrateContentPrivate)
{
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPCalibrateContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    d_ptr->item = d_ptr->itemList.at(0);
    d_ptr->item->title->setText(trs("CalibratePoint1"));
    d_ptr->item->value->setText(" 0 ");
    d_ptr->item->value->setAlignment(Qt::AlignCenter);
    d_ptr->item->range->setVisible(false);
    d_ptr->item->btn->setText(trs("ServiceCalibrate"));
    d_ptr->item->btn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->item->btn, SIGNAL(released()), this, SLOT(onBtn1Calibrated()));
    layout->addWidget(d_ptr->item, 0, 0);

    d_ptr->item = d_ptr->itemList.at(1);
    d_ptr->item->title->setText(trs("CalibratePoint2"));
    d_ptr->item->value->setVisible(false);
//    d_ptr->item->range->setSuffix(Unit::getSymbol(UNIT_MMHG));
//    d_ptr->item->range->setSuffixSpace(1);
//    d_ptr->item->range->setFont(fontManager.textFont(fontSize));
//    d_ptr->item->range->setMode(ISPIN_MODE_INT);
    d_ptr->item->range->setRange(200, 280);
    d_ptr->item->range->setStep(1);
    d_ptr->item->range->setValue(250);
    d_ptr->item->btn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->item->btn->setText(trs("ServiceCalibrate"));
    connect(d_ptr->item->btn, SIGNAL(released()), this, SLOT(onBtn2Calibrated()));
    layout->addWidget(d_ptr->item, 1, 0);

    d_ptr->label = new QLabel("");
    layout->addWidget(d_ptr->label, 2, 0);

    layout->setRowStretch(3, 1);
}

/**************************************************************************************************
 * 初始化。
 *************************************************************************************************/
void NIBPCalibrateContent::init()
{
    d_ptr->label->setText("");

    d_ptr->btnFlag1 = false;
    d_ptr->btnFlag2 = false;
    d_ptr->item = d_ptr->itemList.at(0);
    d_ptr->item->btn->setText(trs("ServiceCalibrate"));
    d_ptr->item->btn->setEnabled(true);

    d_ptr->item = d_ptr->itemList.at(1);
    d_ptr->item->range->setValue(250);
//    d_ptr->item->range->disable(false);
    d_ptr->item->range->setEnabled(true);
    d_ptr->item->btn->setText(trs("ServiceCalibrate"));
    d_ptr->item->btn->setEnabled(true);

    d_ptr->calibrateFlag = false;
}

bool NIBPCalibrateContent::focusNextPrevChild(bool next)
{
    init();

    nibpParam.switchState(NIBP_SERVICE_CALIBRATE_STATE);

    MenuContent::focusNextPrevChild(next);

    return next;
}

/**************************************************************************************************
 * 校准按钮1信号槽
 *************************************************************************************************/
void NIBPCalibrateContent::onBtn1Calibrated()
{
    d_ptr->label->setText("");
    if (d_ptr->calibrateFlag)
    {
        if (!d_ptr->btnFlag1)
        {
            d_ptr->btnFlag1 = true;
            unsigned char cmd[2];
            cmd[0] = 0x00;
            cmd[1] = 0x00;

            nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT, cmd, 2);

            d_ptr->item = d_ptr->itemList.at(0);
            d_ptr->item->btn->setText(trs("ServiceCalibrating"));

            d_ptr->item = d_ptr->itemList.at(1);
            // d_ptr->item->range->disable(true);
            d_ptr->item->range->setEnabled(false);
            d_ptr->item->btn->setEnabled(false);
        }
    }
    else
    {
        NIBPRepairMenuWindow::getInstance()->messageBox();
    }
}

/**************************************************************************************************
 * 校准按钮2信号槽
 *************************************************************************************************/
void NIBPCalibrateContent::onBtn2Calibrated()
{
    d_ptr->label->setText("");
    if (d_ptr->calibrateFlag)
    {
        if (!d_ptr->btnFlag2)
        {
            d_ptr->btnFlag2 = true;
            d_ptr->item = d_ptr->itemList.at(1);
            int value = d_ptr->item->range->getValue();
            d_ptr->pressurevalue = value;

            unsigned char cmd[2];
            cmd[0] = d_ptr->pressurevalue & 0xFF;
            cmd[1] = (d_ptr->pressurevalue & 0xFF00) >> 8;
            nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT, cmd, 2);

            d_ptr->item = d_ptr->itemList.at(1);
            d_ptr->item->btn->setText(trs("ServiceCalibrating"));
            // d_ptr->item->range->disable(true);
            d_ptr->item->range->setEnabled(false);
            d_ptr->item->btn->setEnabled(false);
        }
    }
    else
    {
        NIBPRepairMenuWindow::getInstance()->messageBox();
    }
}

/**************************************************************************************************
 * 进入模式的应答。
 *************************************************************************************************/
void NIBPCalibrateContent::unPacket(bool flag)
{
    d_ptr->calibrateFlag = flag;
    if (flag)
    {
        d_ptr->item = d_ptr->itemList.at(0);
        d_ptr->item->btn->setEnabled(true);

        d_ptr->item = d_ptr->itemList.at(1);
        // d_ptr->item->range->disable(true);
        d_ptr->item->range->setEnabled(false);
        d_ptr->item->btn->setEnabled(false);
    }
    else
    {
        d_ptr->item = d_ptr->itemList.at(0);
        d_ptr->item->btn->setEnabled(false);

        d_ptr->item = d_ptr->itemList.at(1);
        // d_ptr->item->range->disable(true);
        d_ptr->item->range->setEnabled(false);
        d_ptr->item->btn->setEnabled(false);
    }
}

void NIBPCalibrateContent::setText(QString str)
{
    d_ptr->label->setText(str);
}

/**************************************************************************************************
 * 校准点应答。
 *************************************************************************************************/
void NIBPCalibrateContent::unPacketPressure(bool flag)
{
    if (d_ptr->btnFlag1)
    {
        d_ptr->btnFlag1 = false;
        if (flag)
        {
            d_ptr->item = d_ptr->itemList.at(0);
            d_ptr->item->btn->setText(trs("ServiceSuccess"));

            d_ptr->item = d_ptr->itemList.at(1);
            d_ptr->item->range->setValue(250);
//            d_ptr->item->range->disable(false);
            d_ptr->item->range->setEnabled(true);
            d_ptr->item->btn->setEnabled(true);
            d_ptr->label->setText(trs("ServiceCalibrateNextOne"));
        }
        else
        {
            d_ptr->item = d_ptr->itemList.at(0);
            d_ptr->item->btn->setText(trs("ServiceAgain"));

            d_ptr->item = d_ptr->itemList.at(1);
//            d_ptr->item->range->disable(true);
            d_ptr->item->range->setEnabled(false);
            d_ptr->item->btn->setEnabled(false);
            d_ptr->label->setText(trs("CalibrateFail") + "," + trs("ServiceCalibrateAgain"));
        }
    }
    if (d_ptr->btnFlag2)
    {
        d_ptr->btnFlag2 = false;
        if (flag)
        {
            d_ptr->item = d_ptr->itemList.at(0);
            d_ptr->item->btn->setText(trs("ServiceCalibrate"));
            d_ptr->item->btn->setEnabled(true);

            d_ptr->item = d_ptr->itemList.at(1);
//            d_ptr->item->range->disable(true);
            d_ptr->item->range->setEnabled(false);
            d_ptr->item->btn->setText(trs("ServiceCalibrate"));
            d_ptr->item->btn->setEnabled(false);
            d_ptr->label->setText(trs("CalibrateSuccess"));
        }
        else
        {
            d_ptr->item = d_ptr->itemList.at(0);
            d_ptr->item->btn->setText(trs("ServiceCalibrate"));
            d_ptr->item->btn->setEnabled(true);

            d_ptr->item = d_ptr->itemList.at(1);
            d_ptr->item->range->setValue(250);
//            d_ptr->item->range->disable(true);
            d_ptr->item->range->setEnabled(false);
            d_ptr->item->btn->setText(trs("ServiceCalibrate"));
            d_ptr->item->btn->setEnabled(false);
            d_ptr->label->setText(trs("CalibrateFail") + "," + trs("ServiceCalibrateAgain"));
        }
    }
    d_ptr->btnFlag1 = false;
    d_ptr->btnFlag2 = false;
}

NIBPCalibrateContent::~NIBPCalibrateContent()
{
    delete d_ptr;
}

