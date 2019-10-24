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
#include "SpinBox.h"
#include "MessageBox.h"
#include "LanguageManager.h"

#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)

class NIBPZeroPointContentPrivate
{
public:
    NIBPZeroPointContentPrivate()
        : inModeTimerID(-1), pumpTimerID(-1), valveTimerID(-1),
          zeroTimerID(-1), pressureTimerID(-1), pressure(InvData()),
          modeBtn(NULL), isZeroMode(false),
          isOnPump(false), isOnValve(false), pumpSpx(NULL),
          pumpBtn(NULL), valveBtn(NULL), zeroBtn(NULL),
          value(NULL), timeoutNum(0)
    {}
    void loadOptions(void);
    int inModeTimerID;          // 进入校零模式定时器ID
    int pumpTimerID;            // 气泵控制定时器ID
    int valveTimerID;           // 气阀控制定时器ID
    int zeroTimerID;            // 校零定时器ID
    int pressureTimerID;        // 获取压力定时器ID
    int pressure;               // 实时压力
    Button *modeBtn;            // 进入/退出模式
    bool isZeroMode;            // 是否处于校零模式
    bool isOnPump;              // 是否处于打开气泵
    bool isOnValve;             // 是否处于打开气阀
    SpinBox *pumpSpx;           // 占空比
    Button *pumpBtn;            // 气泵开关
    Button *valveBtn;           // 气阀开关
    Button *zeroBtn;            // 校零
    QLabel *value;              // 当前压力值
    int timeoutNum;            // 回复超时
};
void NIBPZeroPointContentPrivate::loadOptions(void)
{
    isZeroMode = false;
    modeBtn->setEnabled(true);
    modeBtn->setText(trs("EnterZeroMode"));
    pumpBtn->setEnabled(false);
    valveBtn->setEnabled(false);
    zeroBtn->setEnabled(false);
    zeroBtn->setText(trs("ServiceCalibrateZero"));
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
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    Button *button;
    QLabel *label;

    button = new Button(trs("EnterZeroMode"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 0, 2);
    connect(button, SIGNAL(released()), this, SLOT(enterZeroReleased()));
    d_ptr->modeBtn = button;

    label = new QLabel(trs("PumpSpeed"));
    layout->addWidget(label, 1, 0, Qt::AlignCenter);

    d_ptr->pumpSpx = new SpinBox();
    d_ptr->pumpSpx->setRange(0, 100);
    d_ptr->pumpSpx->setValue(0);
    d_ptr->pumpSpx->setStep(1);
    layout->addWidget(d_ptr->pumpSpx, 1, 1);

    button = new Button(trs("Off"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 1, 2);
    connect(button, SIGNAL(released()), this, SLOT(pumpControlReleased()));
    d_ptr->pumpBtn = button;

    label = new QLabel(trs("ValveControl"));
    layout->addWidget(label, 2, 0, Qt::AlignCenter);

    button = new Button(trs("Off"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 2, 2);
    connect(button, SIGNAL(released()), this, SLOT(valveControlReleased()));
    d_ptr->valveBtn = button;

    label = new QLabel(trs("ServicePressure"));
    layout->addWidget(label, 3, 0, Qt::AlignCenter);

    label = new QLabel(InvStr());
    layout->addWidget(label, 3, 1, Qt::AlignCenter);
    d_ptr->value = label;

    button = new Button(trs("ServiceCalibrateZero"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    layout->addWidget(button, 3, 2);
    connect(button, SIGNAL(released()), this, SLOT(calibrateZeroReleased()));
    d_ptr->zeroBtn = button;

    layout->setRowStretch(4, 1);
}

void NIBPZeroPointContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->inModeTimerID == ev->timerId())
    {
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            d_ptr->modeBtn->setEnabled(true);
            if (reply && nibpParam.getResult())
            {
                if (d_ptr->isZeroMode)
                {
                    d_ptr->isZeroMode = false;
                    d_ptr->modeBtn->setText(trs("EnterZeroMode"));
                    d_ptr->zeroBtn->setEnabled(false);
                    d_ptr->pumpBtn->setEnabled(false);
                    d_ptr->valveBtn->setEnabled(false);
                    killTimer(d_ptr->pressureTimerID);
                    d_ptr->pressureTimerID = -1;
                }
                else
                {
                    d_ptr->isZeroMode = true;
                    d_ptr->modeBtn->setText(trs("QuitZeroMode"));
                    d_ptr->zeroBtn->setEnabled(true);
                    d_ptr->pumpBtn->setEnabled(true);
                    d_ptr->valveBtn->setEnabled(true);
                    d_ptr->pressureTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
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
    }
    else if (d_ptr->pumpTimerID == ev->timerId())
    {
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            if (reply && nibpParam.getResult())
            {
                if (d_ptr->isOnPump)
                {
                    d_ptr->isOnPump = false;
                    d_ptr->pumpBtn->setText(trs("Off"));
                }
                else
                {
                    d_ptr->isOnPump = true;
                    d_ptr->pumpBtn->setText(trs("On"));
                }
            }
            else
            {
                MessageBox messbox(trs("Warn"), trs("OperationFailedPleaseAgain"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
                windowManager.showWindow(&messbox,
                                         WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
            }
            killTimer(d_ptr->pumpTimerID);
            d_ptr->pumpTimerID = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
    else if (d_ptr->valveTimerID == ev->timerId())
    {
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            if (reply && nibpParam.getResult())
            {
                if (d_ptr->isOnValve)
                {
                    d_ptr->isOnValve = false;
                    d_ptr->valveBtn->setText(trs("Off"));
                }
                else
                {
                    d_ptr->isOnValve = true;
                    d_ptr->valveBtn->setText(trs("On"));
                }
            }
            else
            {
                MessageBox messbox(trs("Warn"), trs("OperationFailedPleaseAgain"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
                windowManager.showWindow(&messbox,
                                         WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
            }
            killTimer(d_ptr->valveTimerID);
            d_ptr->valveTimerID = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
    else if (d_ptr->zeroTimerID == ev->timerId())
    {
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            if (reply && nibpParam.getResult())
            {
                d_ptr->zeroBtn->setText(trs("CalibrationSuccess"));
            }
            else
            {
                d_ptr->zeroBtn->setText(trs("CalibrationFail"));
            }
            killTimer(d_ptr->zeroTimerID);
            d_ptr->zeroTimerID = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
    else if (d_ptr->pressureTimerID == ev->timerId())
    {
        if (d_ptr->pressure != nibpParam.getManometerPressure())
        {
            d_ptr->pressure = nibpParam.getManometerPressure();
            d_ptr->value->setNum(nibpParam.getManometerPressure());
        }
    }
}

bool NIBPZeroPointContent::focusNextPrevChild(bool next)
{
    d_ptr->zeroBtn->setText(trs("ServiceCalibrateZero"));
    return MenuContent::focusNextPrevChild(next);
}

void NIBPZeroPointContent::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e);
    d_ptr->loadOptions();
    nibpParam.provider().serviceCalibrateZero(false);
    if (nibpParam.curMachineType() == NIBP_STATE_MACHINE_SERVICE)
    {
        nibpParam.switchState(NIBP_SERVICE_STANDBY_STATE);
    }
}

void NIBPZeroPointContent::enterZeroReleased()
{
    d_ptr->inModeTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    d_ptr->modeBtn->setEnabled(false);
    if (d_ptr->isZeroMode)
    {
        nibpParam.provider().serviceCalibrateZero(false);
        nibpParam.switchState(NIBP_SERVICE_STANDBY_STATE);
    }
    else
    {
        nibpParam.switchState(NIBP_SERVICE_CALIBRATE_ZERO_STATE);
    }
}

void NIBPZeroPointContent::pumpControlReleased()
{
    d_ptr->pumpTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    if (d_ptr->isOnPump)
    {
        nibpParam.provider().servicePump(false, 0);
    }
    else
    {
        unsigned char value = d_ptr->pumpSpx->getValue();
        nibpParam.provider().servicePump(true, value);
    }
}

void NIBPZeroPointContent::valveControlReleased()
{
    d_ptr->valveTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    if (d_ptr->isOnValve)
    {
        nibpParam.provider().serviceValve(true);
    }
    else
    {
        nibpParam.provider().serviceValve(false);
    }
}

void NIBPZeroPointContent::calibrateZeroReleased()
{
    d_ptr->zeroTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    nibpParam.provider().servicePressureZero();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPZeroPointContent::~NIBPZeroPointContent()
{
    delete d_ptr;
}

void NIBPZeroPointContent::init()
{
    d_ptr->loadOptions();
}
