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

#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)
#define INFLATE_WAIT_NUMBER                    (100000 / CALIBRATION_INTERVAL_TIME)

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

    Button *modeBtn;                // 进入/退出模式
    bool isPressureControlMode;     // 是否处于压力操控模式
    int inModeTimerID;              // 进入压力操控模式定时器ID
    int timeoutNum;
    int isInflate;                  // 是否充气;
    int inflateTimerID;             // 充气和放气回复定时器ID
    int pressureTimerID;            // 获取压力定时器ID
    int pressure;
    int inflateTimeoutNum;          // 充气超时

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
      modeBtn(NULL), isPressureControlMode(false), inModeTimerID(-1),
      timeoutNum(0), isInflate(true), inflateTimerID(-1), pressureTimerID(-1),
      pressure(InvData()), inflateTimeoutNum(0)
{
    machineConfig.getStrValue("NIBP", moduleStr);
}

// 压力控制模式
/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPPressureControlContent::layoutExec()
{
    if (d_ptr->moduleStr != "SUNTECH_NIBP")
    {
        n5LayoutExec();
    }
    else
    {
        suntechLayoutExec();
    }
}

void NIBPPressureControlContent::n5LayoutExec()
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

void NIBPPressureControlContent::suntechLayoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    ComboBox *combo;
    QLabel *label;
    int itemID;

    // Pump
    label = new QLabel(trs("Pump"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On")
                    );
    itemID = NIBPPressureControlContentPrivate::ITEM_CBO_PUMP;
    combo->setProperty("Item",
                       qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NIBPPressureControlContentPrivate::ITEM_CBO_PUMP, combo);

    // control valve
    label = new QLabel(trs("ControlValve"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("On")
                    << trs("Off")
                    );
    itemID = NIBPPressureControlContentPrivate::ITEM_CBO_CONTROL_VALVE;
    combo->setProperty("Item",
                       qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NIBPPressureControlContentPrivate::ITEM_CBO_CONTROL_VALVE, combo);

    // dump valve
    label = new QLabel(trs("DumpValve"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("On")
                    << trs("Off")
                    );
    itemID = NIBPPressureControlContentPrivate::ITEM_CBO_DUMP_VALVE;
    combo->setProperty("Item",
                       qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NIBPPressureControlContentPrivate::ITEM_CBO_DUMP_VALVE, combo);

    // 压力值
    label = new QLabel(trs("ServicePressure"));
    layout->addWidget(label, d_ptr->combos.count(), 0);

    QHBoxLayout *hLayout = new QHBoxLayout();
    label = new QLabel(InvStr());
    hLayout->addWidget(label);
    d_ptr->value = label;
    label = new QLabel();
    label->setText(Unit::getSymbol(nibpParam.getUnit()));
    hLayout->addWidget(label);
    layout->addLayout(hLayout, d_ptr->combos.count(), 1);

    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
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
                messbox.exec();
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
            d_ptr->value->setNum(nibpParam.getManometerPressure());
        }
    }
}

void NIBPPressureControlContent::showEvent(QShowEvent *e)
{
    Q_UNUSED(e)
    // 默认设置为过压保护开启
    if (d_ptr->moduleStr != "SUNTECH_NIBP")
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
    if (d_ptr->moduleStr == "SUNTECH_NIBP")
    {
        killTimer(d_ptr->pressureTimerID);
        d_ptr->pressureTimerID = -1;
    }
}

/**************************************************************************************************
 * 充气、放气控制按钮。
 *************************************************************************************************/
void NIBPPressureControlContent::inflateBtnReleased()
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

void NIBPPressureControlContent::enterPressureContrlReleased()
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

void NIBPPressureControlContent::onComboBoxIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }

    unsigned char pump = d_ptr->combos[NIBPPressureControlContentPrivate::ITEM_CBO_PUMP]->currentIndex();
    unsigned char controlValve = d_ptr->combos[NIBPPressureControlContentPrivate::ITEM_CBO_CONTROL_VALVE]->currentIndex();
    unsigned char dumpValve = d_ptr->combos[NIBPPressureControlContentPrivate::ITEM_CBO_DUMP_VALVE]->currentIndex();
    nibpParam.provider().controlPneumatics(pump, controlValve, dumpValve);
}

void NIBPPressureControlContent::onOverpressureReleased(int index)
{
    nibpParam.provider().servicePressureProtect(!index);
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
    if (d_ptr->moduleStr != "SUNTECH_NIBP")
    {
        d_ptr->isPressureControlMode = false;
        d_ptr->modeBtn->setEnabled(true);
        d_ptr->modeBtn->setText(trs("EnterPressureContrlMode"));
        d_ptr->overpressureCbo->setEnabled(false);
        d_ptr->inflateBtn->setEnabled(false);
    }
}

