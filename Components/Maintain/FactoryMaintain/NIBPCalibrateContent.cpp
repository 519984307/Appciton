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
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/SpinBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QGridLayout>
#include "NIBPEventDefine.h"
#include "NIBPServiceStateDefine.h"
#include "MessageBox.h"
#include "IConfig.h"
#include "WindowManager.h"
#include <QTimerEvent>

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
        : label(NULL), pointLabel(NULL),
          pressurevalue(0), isCalibrating(false),
          point(NIBP_CALIBRATE_ZERO), calibrateTimerID(-1),
          timeoutNum(0), inModeTimerID(-1), isCalibrateMode(false),
          modeBtn(NULL), calibrateBtn1(NULL), calibrateBtn2(NULL),
          unitLableOne(NULL), unitLableTwo(NULL)
    {
    }
    void loadOptions(void);
    QList<Button *> btnList;
//    CalibrateSetItem *item;
    QLabel *label;
    QLabel *pointLabel;

    int  pressurevalue;                     // 校准点的值
    bool isCalibrating;                     // 是否处于正在校准

    NIBPCalibratePoint point;
    int calibrateTimerID;
    int timeoutNum;

    int inModeTimerID;                      // 进入校准模式定时器ID
    bool isCalibrateMode;                   // 是否处于校准模式
    Button *modeBtn;                        // 进入/退出模式按钮
    Button *calibrateBtn1;
    Button *calibrateBtn2;
    QLabel *unitLableOne;                   // 单位显示字符串1
    QLabel *unitLableTwo;                   // 单位显示字符串1
    QString moduleStr;                      // 运行模块字符串
};

void NIBPCalibrateContentPrivate::loadOptions(void)
{
    isCalibrateMode = false;
    modeBtn->setEnabled(true);
    modeBtn->setText(trs("EnterCalibrateMode"));
    calibrateBtn1->setText(trs("ServiceCalibrate"));
    calibrateBtn2->setText(trs("ServiceCalibrate"));
    calibrateBtn1->setEnabled(false);
    calibrateBtn2->setEnabled(false);
    UnitType unit = nibpParam.getUnit();
    UnitType defUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
    unitLableOne->setText(Unit::getSymbol(nibpParam.getUnit()));
    unitLableTwo->setText(Unit::getSymbol(nibpParam.getUnit()));
    if (unit != defUnit)
    {
        pointLabel->setText(Unit::convert(unit, defUnit, 250));  // 250mmHg对应的kPa单位换算
    }
    else
    {
        pointLabel->setText("250");
    }
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
    QLabel *pressureLabel;

    button = new Button(trs("EnterCalibrateMode"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 0, 3);
    connect(button, SIGNAL(released()), this, SLOT(inCalibrateMode()));
    d_ptr->modeBtn = button;

    label = new QLabel(trs("CalibratePoint1"));
    layout->addWidget(label, 1, 0);

    pressureLabel = new QLabel();
    pressureLabel->setText("0");
    layout->addWidget(pressureLabel, 1, 1);

    label = new QLabel();
    label->setText("mmHg");
    layout->addWidget(label, 1, 2);
    d_ptr->unitLableOne = label;

    button = new Button(trs("ServiceCalibrate"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    connect(button, SIGNAL(released()), this, SLOT(onBtn1Calibrated()));
    layout->addWidget(button, 1, 3);
    d_ptr->btnList.append(button);
    d_ptr->calibrateBtn1 = button;

    label = new QLabel(trs("CalibratePoint2"));
    layout->addWidget(label, 2, 0);

    pressureLabel = new QLabel();
    layout->addWidget(pressureLabel, 2, 1);
    d_ptr->pointLabel = pressureLabel;

    label = new QLabel();
    label->setText("mmHg");
    layout->addWidget(label, 2, 2);
    d_ptr->unitLableTwo = label;

    button = new Button(trs("ServiceCalibrate"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    connect(button, SIGNAL(released()), this, SLOT(onBtn2Calibrated()));
    layout->addWidget(button, 2, 3);
    d_ptr->calibrateBtn2 = button;
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
                }
                else
                {
                    d_ptr->isCalibrateMode = true;
                    d_ptr->modeBtn->setText(trs("QuitCalibrateMode"));
                    Button *btn = d_ptr->btnList.at(0);
                    btn->setEnabled(true);
                    btn = d_ptr->btnList.at(1);
                    btn->setEnabled(true);
                }
            }
            else
            {
                MessageBox messbox(trs("Warn"), trs("NIBPModuleEnterFail"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
                windowManager.showWindow(&messbox,
                                         WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
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

void NIBPCalibrateContent::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e);
    d_ptr->loadOptions();
    if (d_ptr->moduleStr == "BLM_N5")
    {
        nibpParam.provider().serviceCalibrate(false);
        if (nibpParam.curMachineType() == NIBP_STATE_MACHINE_SERVICE)
        {
            nibpParam.switchState(NIBP_SERVICE_STANDBY_STATE);
        }
    }
}

/**************************************************************************************************
 * 校准按钮1信号槽
 *************************************************************************************************/
void NIBPCalibrateContent::onBtn1Calibrated()
{
//    d_ptr->label->setText("");
    if (d_ptr->isCalibrateMode && !d_ptr->isCalibrating)
    {
        d_ptr->isCalibrating = true;
        unsigned char cmd[2];
        cmd[0] = 0x00;
        cmd[1] = 0x00;
        if (d_ptr->moduleStr == "BLM_N5")
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
    if (d_ptr->isCalibrateMode && !d_ptr->isCalibrating)
    {
        d_ptr->isCalibrating = true;
        Button *btn = d_ptr->btnList.at(1);
        int value = 250;
        d_ptr->pressurevalue = value;

        unsigned char cmd[2];
        cmd[0] = d_ptr->pressurevalue & 0xFF;
        cmd[1] = (d_ptr->pressurevalue & 0xFF00) >> 8;
        if (d_ptr->moduleStr == "BLM_N5")
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
    if (d_ptr->moduleStr == "BLM_N5")
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
        if (!nibpParam.getConnectedState())
        {
            MessageBox messbox(trs("Warn"), trs("NIBPModuleEnterFail"), false);
            messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
            windowManager.showWindow(&messbox,
                                     WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
            return;
        }
        if (d_ptr->isCalibrateMode)
        {
            d_ptr->modeBtn->setText(trs("EnterCalibrateMode"));
            Button *btn = d_ptr->btnList.at(0);
            btn->setEnabled(false);
            btn->setText(trs("ServiceCalibrate"));
            btn = d_ptr->btnList.at(1);
            btn->setEnabled(false);
            btn->setText(trs("ServiceCalibrate"));
            d_ptr->isCalibrateMode = false;
        }
        else
        {
            d_ptr->modeBtn->setText(trs("QuitCalibrateMode"));
            d_ptr->isCalibrateMode = true;
            Button *btn = d_ptr->btnList.at(0);
            btn->setEnabled(true);
            btn = d_ptr->btnList.at(1);
            btn->setEnabled(true);
            nibpParam.provider().controlPneumatics(0, 1, 1);
        }
    }
}

NIBPCalibrateContent::~NIBPCalibrateContent()
{
    delete d_ptr;
}

void NIBPCalibrateContent::init()
{
    d_ptr->loadOptions();
}
