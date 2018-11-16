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
#include "MessageBox.h"

#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)

enum NIBPCalibratePoint
{
    NIBP_CALIBRATE_ZERO,
    NIBP_CALIBRATE_HIGH_VALUE,
};
class NIBPCalibrateContentPrivate
{
public:
    NIBPCalibrateContentPrivate()
        : label(NULL), point2Spb(NULL),
          pressurevalue(0), calibrateFlag(false),
          point(NIBP_CALIBRATE_ZERO), calibrateTimerID(-1),
          timeoutNum(0), inModeTimerID(-1)
    {
    }
    QList<Button *> btnList;
//    CalibrateSetItem *item;
    QLabel *label;
    SpinBox *point2Spb;

    int  pressurevalue;                     // 校准点的值
    bool calibrateFlag;                     // 进入模式标志

    NIBPCalibratePoint point;
    int calibrateTimerID;
    int timeoutNum;

    int inModeTimerID;                      // 进入校准模式定时器ID
};

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
    layout->setAlignment(Qt::AlignTop);

    QLabel *label;
    Button *button;
    SpinBox *spinBox;

    button = new Button(trs("EnterCalibrateMode"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 0, 2);
    connect(button, SIGNAL(released()), this, SLOT(inCalibrateMode()));

    label = new QLabel(trs("CalibratePoint1"));
    layout->addWidget(label, 1, 0);

    spinBox = new SpinBox();
    spinBox->setValue(0);
    spinBox->setEnabled(false);
    layout->addWidget(spinBox, 1, 1);

    button = new Button(trs("ServiceCalibrate"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    connect(button, SIGNAL(released()), this, SLOT(onBtn1Calibrated()));
    layout->addWidget(button, 1, 2);
    d_ptr->btnList.append(button);

    label = new QLabel(trs("CalibratePoint2"));
    layout->addWidget(label, 2, 0);

    spinBox = new SpinBox();
    spinBox->setValue(250);
    spinBox->setRange(0, 250);
    layout->addWidget(spinBox, 2, 1);
    d_ptr->point2Spb = spinBox;

    button = new Button(trs("ServiceCalibrate"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    connect(button, SIGNAL(released()), this, SLOT(onBtn2Calibrated()));
    layout->addWidget(button, 2, 2);
    d_ptr->btnList.append(button);

    layout->setRowStretch(3, 1);
}

/**************************************************************************************************
 * 初始化。
 *************************************************************************************************/
void NIBPCalibrateContent::init()
{
//    d_ptr->label->setText("");

//    d_ptr->btnFlag1 = false;
//    d_ptr->btnFlag2 = false;
//    d_ptr->item = d_ptr->itemList.at(0);
//    d_ptr->item->btn->setText(trs("ServiceCalibrate"));
//    d_ptr->item->btn->setEnabled(true);

//    d_ptr->item = d_ptr->itemList.at(1);
//    d_ptr->item->range->setValue(250);
////    d_ptr->item->range->disable(false);
//    d_ptr->item->range->setEnabled(true);
//    d_ptr->item->btn->setText(trs("ServiceCalibrate"));
//    d_ptr->item->btn->setEnabled(true);

//    d_ptr->calibrateFlag = true;
}

bool NIBPCalibrateContent::focusNextPrevChild(bool next)
{
    init();

//    nibpParam.switchState(NIBP_SERVICE_CALIBRATE_STATE);

    MenuContent::focusNextPrevChild(next);

    return next;
}

void NIBPCalibrateContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->calibrateTimerID == ev->timerId())
    {
        bool reply = nibpParam.getCalibrationReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            Button *btn = d_ptr->btnList.at(d_ptr->point);
            if (reply && nibpParam.getCalibrationResult())
            {
                btn->setText(trs("CalibrationSuccess"));
            }
            else
            {
                btn->setText(trs("CalibrationFail"));
            }
            killTimer(d_ptr->calibrateTimerID);
            d_ptr->calibrateTimerID = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
    else if (d_ptr->inModeTimerID == ev->timerId())
    {
        bool reply = nibpParam.getCalibrationReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            if (reply && nibpParam.getCalibrationResult())
            {
                Button *btn = d_ptr->btnList.at(0);
                btn->setEnabled(true);
                btn = d_ptr->btnList.at(1);
                btn->setEnabled(true);
                d_ptr->calibrateFlag = true;
            }
            else
            {
                MessageBox messbox(trs("Warn"), trs("NIBPModuleEnterFail"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
                messbox.exec();
            }
            killTimer(d_ptr->inModeTimerID);
            d_ptr->inModeTimerID = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
}

/**************************************************************************************************
 * 校准按钮1信号槽
 *************************************************************************************************/
void NIBPCalibrateContent::onBtn1Calibrated()
{
//    d_ptr->label->setText("");
    if (d_ptr->calibrateFlag)
    {
        unsigned char cmd[2];
        cmd[0] = 0x00;
        cmd[1] = 0x00;

        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT, cmd, 2);

        Button *btn = d_ptr->btnList.at(0);
        btn->setText(trs("ServiceCalibrating"));

        d_ptr->point = NIBP_CALIBRATE_ZERO;
        d_ptr->calibrateTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
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
    if (d_ptr->calibrateFlag)
    {
        Button *btn = d_ptr->btnList.at(1);
        int value = d_ptr->point2Spb->getValue();
        d_ptr->pressurevalue = value;

        unsigned char cmd[2];
        cmd[0] = d_ptr->pressurevalue & 0xFF;
        cmd[1] = (d_ptr->pressurevalue & 0xFF00) >> 8;
        nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT, cmd, 2);

        btn->setText(trs("ServiceCalibrating"));

        d_ptr->point = NIBP_CALIBRATE_HIGH_VALUE;
        d_ptr->calibrateTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    }
    else
    {
        NIBPRepairMenuWindow::getInstance()->messageBox();
    }
}

void NIBPCalibrateContent::inCalibrateMode()
{
    d_ptr->inModeTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    nibpParam.switchState(NIBP_SERVICE_CALIBRATE_STATE);
}

///**************************************************************************************************
// * 进入模式的应答。
// *************************************************************************************************/
// void NIBPCalibrateContent::unPacket(bool flag)
//{
//    d_ptr->calibrateFlag = flag;
//    if (flag)
//    {
//        d_ptr->item = d_ptr->itemList.at(0);
//        d_ptr->item->btn->setEnabled(true);

//        d_ptr->item = d_ptr->itemList.at(1);
//        // d_ptr->item->range->disable(true);
//        d_ptr->item->range->setEnabled(false);
//        d_ptr->item->btn->setEnabled(false);
//    }
//    else
//    {
//        d_ptr->item = d_ptr->itemList.at(0);
//        d_ptr->item->btn->setEnabled(false);

//        d_ptr->item = d_ptr->itemList.at(1);
//        // d_ptr->item->range->disable(true);
//        d_ptr->item->range->setEnabled(false);
//        d_ptr->item->btn->setEnabled(false);
//    }
//}

void NIBPCalibrateContent::setText(QString str)
{
    d_ptr->label->setText(str);
}

/**************************************************************************************************
 * 校准点应答。
 *************************************************************************************************/
// void NIBPCalibrateContent::unPacketPressure(bool flag)
//{
//    if (d_ptr->btnFlag1)
//    {
//        d_ptr->btnFlag1 = false;
//        if (flag)
//        {
//            d_ptr->item = d_ptr->itemList.at(0);
//            d_ptr->item->btn->setText(trs("ServiceSuccess"));

//            d_ptr->item = d_ptr->itemList.at(1);
//            d_ptr->item->range->setValue(250);
////            d_ptr->item->range->disable(false);
//            d_ptr->item->range->setEnabled(true);
//            d_ptr->item->btn->setEnabled(true);
//            d_ptr->label->setText(trs("ServiceCalibrateNextOne"));
//        }
//        else
//        {
//            d_ptr->item = d_ptr->itemList.at(0);
//            d_ptr->item->btn->setText(trs("ServiceAgain"));

//            d_ptr->item = d_ptr->itemList.at(1);
////            d_ptr->item->range->disable(true);
//            d_ptr->item->range->setEnabled(false);
//            d_ptr->item->btn->setEnabled(false);
//            d_ptr->label->setText(trs("CalibrateFail") + "," + trs("ServiceCalibrateAgain"));
//        }
//    }
//    if (d_ptr->btnFlag2)
//    {
//        d_ptr->btnFlag2 = false;
//        if (flag)
//        {
//            d_ptr->item = d_ptr->itemList.at(0);
//            d_ptr->item->btn->setText(trs("ServiceCalibrate"));
//            d_ptr->item->btn->setEnabled(true);

//            d_ptr->item = d_ptr->itemList.at(1);
////            d_ptr->item->range->disable(true);
//            d_ptr->item->range->setEnabled(false);
//            d_ptr->item->btn->setText(trs("ServiceCalibrate"));
//            d_ptr->item->btn->setEnabled(false);
//            d_ptr->label->setText(trs("CalibrateSuccess"));
//        }
//        else
//        {
//            d_ptr->item = d_ptr->itemList.at(0);
//            d_ptr->item->btn->setText(trs("ServiceCalibrate"));
//            d_ptr->item->btn->setEnabled(true);

//            d_ptr->item = d_ptr->itemList.at(1);
//            d_ptr->item->range->setValue(250);
////            d_ptr->item->range->disable(true);
//            d_ptr->item->range->setEnabled(false);
//            d_ptr->item->btn->setText(trs("ServiceCalibrate"));
//            d_ptr->item->btn->setEnabled(false);
//            d_ptr->label->setText(trs("CalibrateFail") + "," + trs("ServiceCalibrateAgain"));
//        }
//    }
//    d_ptr->btnFlag1 = false;
//    d_ptr->btnFlag2 = false;
//}

NIBPCalibrateContent::~NIBPCalibrateContent()
{
    delete d_ptr;
}

