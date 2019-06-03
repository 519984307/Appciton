/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "ComboBox.h"
#include "NIBPPressureControlContent.h"
#include "NIBPParam.h"
#include "NIBPSymbol.h"
#include "PatientManager.h"
#include "SpinBox.h"
#include "NIBPRepairMenuWindow.h"
#include "Button.h"
#include "MenuWindow.h"
#include <QLayout>
#include "NIBPProviderIFace.h"
#include "NIBPServiceStateDefine.h"
#include "MessageBox.h"
#include "IConfig.h"
#include "LanguageManager.h"

#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)
#define INFLATE_WAIT_NUMBER                    (100000 / CALIBRATION_INTERVAL_TIME)
#define INFLATE_SWITCH_SIGN                    (5)

class NIBPPressureControlContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_PUMP = 1,
        ITEM_CBO_CONTROL_VALVE,
        ITEM_CBO_DUMP_VALVE,
    };

    NIBPPressureControlContentPrivate();
    SpinBox *chargePressure;          // 设定充气压力值
    ComboBox *overpressureCbo;          // 过压保护开关
    Button *inflateBtn;             // 充气、放气控制按钮
    QLabel *value;
    int inflatePressure;               //  充气压力值
    int patientVaulue;                  //  病人类型
    bool inflateSwitch;                 //  充气、放气标志
    bool pressureControlFlag;          //  进入模式标志
    bool overPressureProtect;          // 过压保护标志
    bool holdPressureFlag;             // 控制压力标志

    Button *modeBtn;                // 进入/退出模式
    bool isPressureControlMode;     // 是否处于压力操控模式
    int inModeTimerID;              // 进入压力操控模式定时器ID
    int timeoutNum;
    int isInflate;                  // 是否充气;
    int inflateTimerID;             // 充气和放气回复定时器ID
    int pressureTimerID;            // 获取压力定时器ID
    int pressure;
    int inflateTimeoutNum;          // 充气超时
    int safePressure;
    QString moduleStr;              // 运行模块字符串
    QMap<MenuItem, ComboBox *> combos;
};

NIBPPressureControlContentPrivate::NIBPPressureControlContentPrivate()
    : chargePressure(NULL),
      overpressureCbo(NULL),
      inflateBtn(NULL),
      value(NULL),
      inflatePressure(0),
      patientVaulue(0),
      inflateSwitch(0),
      pressureControlFlag(false),
      overPressureProtect(true),
      holdPressureFlag(false),
      modeBtn(NULL), isPressureControlMode(false), inModeTimerID(-1),
      timeoutNum(0), isInflate(true), inflateTimerID(-1), pressureTimerID(-1),
      pressure(InvData()), inflateTimeoutNum(0), safePressure(0)
{
    machineConfig.getStrValue("NIBP", moduleStr);
}

// 压力控制模式
/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPPressureControlContent::layoutExec()
{
    LayoutExec();
}

void NIBPPressureControlContent::LayoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    Button *button;
    QLabel *label;

    button = new Button(trs("EnterPressureContrlMode"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 0, 2);
    connect(button, SIGNAL(released()), this, SLOT(enterPressureContrlReleased()));
    d_ptr->modeBtn = button;

    label = new QLabel(trs("OverpressureProtect"));
    layout->addWidget(label, 1, 0, Qt::AlignCenter);
    d_ptr->overpressureCbo = new ComboBox();
    d_ptr->overpressureCbo->setEnabled(false);
    d_ptr->overpressureCbo->addItem(trs("ON"));
    d_ptr->overpressureCbo->addItem(trs("Off"));
    d_ptr->overpressureCbo->setCurrentIndex(1);
    layout->addWidget(d_ptr->overpressureCbo, 1, 2);
    connect(d_ptr->overpressureCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(onOverpressureReleased(int)));

    label = new QLabel(trs("InflationPressure"));
    layout->addWidget(label, 2, 0, Qt::AlignCenter);

    d_ptr->chargePressure = new SpinBox();
    d_ptr->chargePressure->setRange(50, 300);
    d_ptr->chargePressure->setValue(250);
    d_ptr->chargePressure->setStep(5);
    layout->addWidget(d_ptr->chargePressure, 2, 1);

    button  = new Button(trs("ServiceInflate"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    layout->addWidget(button, 2, 2);
    connect(button, SIGNAL(released()), this, SLOT(inflateBtnReleased()));
    d_ptr->inflateBtn = button;

    label = new QLabel(trs("ServicePressure"));
    layout->addWidget(label, 3, 0, Qt::AlignCenter);

    label = new QLabel(InvStr());
    layout->addWidget(label, 3, 1, Qt::AlignCenter);
    d_ptr->value = label;

    label = new QLabel();
    label->setText(Unit::getSymbol(nibpParam.getUnit()));
    layout->addWidget(label, 3, 2, Qt::AlignCenter);

    layout->setRowStretch(4, 1);
}


void NIBPPressureControlContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->inModeTimerID == ev->timerId())
    {
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->timeoutNum == INFLATE_WAIT_NUMBER)
        {
            d_ptr->modeBtn->setEnabled(true);
            if (reply && nibpParam.getResult())
            {
                if (d_ptr->isPressureControlMode)
                {
                    d_ptr->isPressureControlMode = false;
                    d_ptr->modeBtn->setText(trs("EnterPressureContrlMode"));
                    d_ptr->inflateBtn->setEnabled(false);
                    d_ptr->overpressureCbo->setEnabled(false);
                    killTimer(d_ptr->pressureTimerID);
                    d_ptr->pressureTimerID = -1;
                }
                else
                {
                    d_ptr->isPressureControlMode = true;
                    d_ptr->modeBtn->setText(trs("QuitPressureContrlMode"));
                    d_ptr->inflateBtn->setEnabled(true);
                    d_ptr->overpressureCbo->setEnabled(true);
                    d_ptr->pressureTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
                }
            }
            else
            {
                MessageBox messbox(trs("Warn"), trs("OperationFailedPleaseAgain"), false);
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
    else if (d_ptr->inflateTimerID == ev->timerId())
    {
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->inflateTimeoutNum == TIMEOUT_WAIT_NUMBER * 10)
        {
            if (reply && nibpParam.getResult())
            {
                if (d_ptr->isInflate)
                {
                    d_ptr->isInflate = false;
                    d_ptr->inflateBtn->setText(trs("ServiceDeflate"));
                }
                else
                {
                    d_ptr->isInflate = true;
                    d_ptr->inflateBtn->setText(trs("ServiceInflate"));
                }
            }
            else
            {
                MessageBox messbox(trs("Warn"), trs("OperationFailedPleaseAgain"), false);
                messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
                windowManager.showWindow(&messbox,
                                         WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
            }
            killTimer(d_ptr->inflateTimerID);
            d_ptr->inflateTimerID = -1;
            d_ptr->inflateTimeoutNum = 0;
            d_ptr->modeBtn->setEnabled(true);
        }
        else
        {
            d_ptr->inflateTimeoutNum++;
        }
    }
    else if (d_ptr->pressureTimerID == ev->timerId())
    {
        if (d_ptr->pressure != nibpParam.getManometerPressure())
        {
            d_ptr->pressure = nibpParam.getManometerPressure();
            d_ptr->value->setNum(d_ptr->pressure);
        }
        if (d_ptr->pressure >= INFLATE_SWITCH_SIGN && d_ptr->isInflate)
        {
            d_ptr->inflateBtn->setText(trs("ServiceDeflate"));
            d_ptr->isInflate = false;
        }
        else if (d_ptr->pressure >= d_ptr->chargePressure->getValue() && d_ptr->holdPressureFlag)
        {
            nibpParam.provider().controlPneumatics(0, 1, 1);  //达到压力值范围时候停止冲压,保持压力
            d_ptr->holdPressureFlag = false;
        }
        else if (d_ptr->pressure < INFLATE_SWITCH_SIGN)
        {
            d_ptr->inflateBtn->setText(trs("ServiceInflate"));
            d_ptr->isInflate = true;
        }
    }
}

void NIBPPressureControlContent::showEvent(QShowEvent *e)
{
    Q_UNUSED(e)
    // 默认设置为过压保护开启
    if (d_ptr->moduleStr == "BLM_N5")
    {
        d_ptr->overpressureCbo->setCurrentIndex(0);
    }
    else
    {
        d_ptr->pressureTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    }
}

void NIBPPressureControlContent::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e)
    if (d_ptr->moduleStr != "BLM_N5")
    {
        killTimer(d_ptr->pressureTimerID);
        d_ptr->pressureTimerID = -1;
        nibpParam.provider().controlPneumatics(0, 0, 0);  //放气
    }
}

/**************************************************************************************************
 * 充气、放气控制按钮。
 *************************************************************************************************/
void NIBPPressureControlContent::inflateBtnReleased()
{
    if (d_ptr->moduleStr == "BLM_N5")
    {
        d_ptr->inflateTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
        if (d_ptr->isInflate)
        {
            int value = d_ptr->chargePressure->getValue();
            nibpParam.provider().servicePressureinflate(value);
            d_ptr->inflateBtn->setText(trs("Inflating"));
            d_ptr->modeBtn->setEnabled(false);
        }
        else
        {
            nibpParam.provider().servicePressuredeflate();
        }
    }
    else
    {
        if (d_ptr->isInflate)
        {
           nibpParam.provider().controlPneumatics(1, 1, 1);  //充气
           d_ptr->holdPressureFlag = true;
        }
        else
        {
            nibpParam.provider().controlPneumatics(0, 0, 0);  //放气
        }
    }
}

void NIBPPressureControlContent::enterPressureContrlReleased()
{
    if (!nibpParam.getConnectedState())
    {
        MessageBox messbox(trs("Warn"), trs("NIBPPressureControlModelEnterFail"), false);
        messbox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        windowManager.showWindow(&messbox, WindowManager::ShowBehaviorNoAutoClose | WindowManager::ShowBehaviorModal);
        return;
    }
    if (d_ptr->moduleStr == "BLM_N5")
    {
        d_ptr->inModeTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
        d_ptr->modeBtn->setEnabled(false);
        if (d_ptr->isPressureControlMode)
        {
            nibpParam.provider().servicePressurecontrol(false);
            nibpParam.switchState(NIBP_SERVICE_STANDBY_STATE);
        }
        else
        {
            nibpParam.switchState(NIBP_SERVICE_PRESSURECONTROL_STATE);
        }
    }
    else
    {
       if (!d_ptr->pressureControlFlag)
       {
            d_ptr->modeBtn->setText(trs("QuitPressureContrlMode"));
            d_ptr->overpressureCbo->setEnabled(true);
            d_ptr->inflateBtn->setEnabled(true);
            d_ptr->pressureControlFlag = true;
       }
       else
       {
           d_ptr->modeBtn->setText((trs("EnterPressureContrlMode")));
           d_ptr->value->setText(InvStr());
           nibpParam.provider().controlPneumatics(0, 0, 0);  //放气
           d_ptr->overpressureCbo->setEnabled(false);
           d_ptr->inflateBtn->setEnabled(false);
           d_ptr->pressureControlFlag = false;
       }
    }
}

void NIBPPressureControlContent::onComboBoxIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }

    unsigned char pump = d_ptr->combos[NIBPPressureControlContentPrivate::ITEM_CBO_PUMP]->currentIndex();
    unsigned char controlValve =
            d_ptr->combos[NIBPPressureControlContentPrivate::ITEM_CBO_CONTROL_VALVE]->currentIndex();
    unsigned char dumpValve = d_ptr->combos[NIBPPressureControlContentPrivate::ITEM_CBO_DUMP_VALVE]->currentIndex();
    nibpParam.provider().controlPneumatics(pump, controlValve, dumpValve);
}

void NIBPPressureControlContent::onOverpressureReleased(int index)
{
    if (d_ptr->moduleStr == "BLM_N5")
    {
        nibpParam.provider().servicePressureProtect(!index);
    }
    else
    {
        if (!index)
        {
            d_ptr->overPressureProtect = true;
            PatientType type = patientManager.getType();
            if (type == PATIENT_TYPE_ADULT)
            {
                d_ptr->safePressure = 290;
            }
            else if (type == PATIENT_TYPE_NEO)
            {
                d_ptr->safePressure = 250;
            }
            else if (type == PATIENT_TYPE_PED)
            {
                d_ptr->safePressure = 150;
            }
            d_ptr->chargePressure->setRange(50, d_ptr->safePressure);
            d_ptr->chargePressure->setValue(100);
        }
        else
        {
            d_ptr->overPressureProtect = false;
            d_ptr->chargePressure->setRange(50, 300);
        }
    }
}

NIBPPressureControlContent *NIBPPressureControlContent::getInstance()
{
    static NIBPPressureControlContent *instance = NULL;
    if (!instance)
    {
        instance = new NIBPPressureControlContent();
    }
    return instance;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPPressureControlContent::NIBPPressureControlContent()
    : MenuContent(trs("ServicePressureControl"),
                  trs("ServicePressureControlDesc")),
      d_ptr(new NIBPPressureControlContentPrivate)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPPressureControlContent::~NIBPPressureControlContent()
{
    delete d_ptr;
}

void NIBPPressureControlContent::init()
{
    if (d_ptr->moduleStr == "BLM_N5")
    {
        d_ptr->isPressureControlMode = false;
        d_ptr->modeBtn->setEnabled(true);
        d_ptr->modeBtn->setText(trs("EnterPressureContrlMode"));
        d_ptr->overpressureCbo->setEnabled(false);
        d_ptr->overpressureCbo->setCurrentIndex(1);
        d_ptr->inflateBtn->setText(trs("ServiceInflate"));
        d_ptr->inflateBtn->setEnabled(false);
    }
}

