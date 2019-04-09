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
#include "IConfig.h"
#include "LanguageManager.h"

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
          pressurevalue(0), calibrateFlag(false), isCalibrating(false),
          point(NIBP_CALIBRATE_ZERO), calibrateTimerID(-1),
          timeoutNum(0), inModeTimerID(-1), isCalibrateMode(false),
          modeBtn(NULL)
    {
    }
    QList<Button *> btnList;
//    CalibrateSetItem *item;
    QLabel *label;
    SpinBox *point2Spb;

    int  pressurevalue;                     // 校准点的值
    bool calibrateFlag;                     // 进入模式标志
    bool isCalibrating;                     // 是否处于正在校准

    NIBPCalibratePoint point;
    int calibrateTimerID;
    int timeoutNum;

    int inModeTimerID;                      // 进入校准模式定时器ID
    bool isCalibrateMode;                   // 是否处于校准模式
    Button *modeBtn;                        // 进入/退出模式按钮

    QString moduleStr;                      // 运行模块字符串
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
    machineConfig.getStrValue("NIBP", d_ptr->moduleStr);
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
    d_ptr->modeBtn = button;

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
    spinBox->setRange(0, 250);
    spinBox->setValue(250);
    spinBox->setEnabled(false);
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

void NIBPCalibrateContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->calibrateTimerID == ev->timerId())
    {
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            d_ptr->isCalibrating = false;
            Button *btn = d_ptr->btnList.at(d_ptr->point);
            if (reply && nibpParam.getResult())
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
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            d_ptr->modeBtn->setEnabled(true);
            if (reply && nibpParam.getResult())
            {
                if (d_ptr->isCalibrateMode)
                {
                    d_ptr->isCalibrateMode = false;
                    d_ptr->modeBtn->setText(trs("EnterCalibrateMode"));
                    Button *btn = d_ptr->btnList.at(0);
                    btn->setEnabled(false);
                    btn->setText(trs("ServiceCalibrate"));
                    btn = d_ptr->btnList.at(1);
                    btn->setEnabled(false);
                    btn->setText(trs("ServiceCalibrate"));
                    d_ptr->point2Spb->setEnabled(false);
                    d_ptr->calibrateFlag = false;
                }
                else
                {
                    d_ptr->isCalibrateMode = true;
                    d_ptr->modeBtn->setText(trs("QuitCalibrateMode"));
                    Button *btn = d_ptr->btnList.at(0);
                    btn->setEnabled(true);
                    btn = d_ptr->btnList.at(1);
                    btn->setEnabled(true);
                    d_ptr->point2Spb->setEnabled(true);
                    d_ptr->calibrateFlag = true;
                }
            }
            else
            {
                MessageBox messbox(trs("Warn"), trs("NIBPModuleEnterFail"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
                windowManager.showWindow(&messbox, WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
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
    if (d_ptr->calibrateFlag && !d_ptr->isCalibrating)
    {
        d_ptr->isCalibrating = true;
        unsigned char cmd[2];
        cmd[0] = 0x00;
        cmd[1] = 0x00;
        if (d_ptr->moduleStr != "SUNTECH_NIBP")
        {
            nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT, cmd, 2);
        }
        else
        {
            nibpParam.provider().servicePressurepoint(cmd, 2);
        }

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
    if (d_ptr->calibrateFlag && !d_ptr->isCalibrating)
    {
        d_ptr->isCalibrating = true;
        Button *btn = d_ptr->btnList.at(1);
        int value = d_ptr->point2Spb->getValue();
        d_ptr->pressurevalue = value;

        unsigned char cmd[2];
        cmd[0] = d_ptr->pressurevalue & 0xFF;
        cmd[1] = (d_ptr->pressurevalue & 0xFF00) >> 8;
        if (d_ptr->moduleStr != "SUNTECH_NIBP")
        {
            nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT, cmd, 2);
        }
        else
        {
            nibpParam.provider().servicePressurepoint(cmd, 2);
        }

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
    if (d_ptr->moduleStr != "SUNTECH_NIBP")
    {
        d_ptr->inModeTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
        d_ptr->modeBtn->setEnabled(false);
        if (d_ptr->isCalibrateMode)
        {
            nibpParam.provider().serviceCalibrate(false);
            nibpParam.switchState(NIBP_SERVICE_STANDBY_STATE);
        }
        else
        {
            nibpParam.switchState(NIBP_SERVICE_CALIBRATE_STATE);
        }
    }
    else
    {
        if (d_ptr->isCalibrateMode)
        {
            d_ptr->modeBtn->setText(trs("EnterCalibrateMode"));
            d_ptr->isCalibrateMode = false;
            Button *btn = d_ptr->btnList.at(0);
            btn->setEnabled(false);
            btn->setText(trs("ServiceCalibrate"));
            btn = d_ptr->btnList.at(1);
            btn->setEnabled(false);
            btn->setText(trs("ServiceCalibrate"));
            d_ptr->calibrateFlag = false;
        }
        else
        {
            d_ptr->modeBtn->setText(trs("QuitCalibrateMode"));
            d_ptr->isCalibrateMode = true;
            Button *btn = d_ptr->btnList.at(0);
            btn->setEnabled(true);
            btn = d_ptr->btnList.at(1);
            btn->setEnabled(true);
            d_ptr->calibrateFlag = true;
        }
    }
}

NIBPCalibrateContent::~NIBPCalibrateContent()
{
    delete d_ptr;
}

void NIBPCalibrateContent::init()
{
    d_ptr->isCalibrateMode = false;
    d_ptr->modeBtn->setEnabled(true);
    d_ptr->modeBtn->setText(trs("EnterCalibrateMode"));
    d_ptr->point2Spb->setEnabled(false);
    d_ptr->btnList.at(0)->setEnabled(false);
    d_ptr->btnList.at(1)->setEnabled(false);
}

