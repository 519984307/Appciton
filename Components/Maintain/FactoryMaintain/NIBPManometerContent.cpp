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
#include "Button.h"
#include "MessageBox.h"
#include "IConfig.h"

class NIBPManometerContentPrivate
{
public:
    NIBPManometerContentPrivate()
        : value(NULL), inModeTimerID(-1),
          timeoutNum(0), pressureTimerID(-1),
          pressure(InvData()), isManometerMode(false),
          modeBtn(NULL)
    {
        machineConfig.getStrValue("NIBP", moduleStr);
    }

    QLabel *value;
    int inModeTimerID;          // 进入压力计模式定时器ID
    int timeoutNum;
    int pressureTimerID;        // 获取压力定时器ID
    int pressure;
    bool isManometerMode;       // 是否处于压力计模式
    Button *modeBtn;            // 进入/退出模式

    QString moduleStr;         // 运行模块字符串
};

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
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    QLabel *label;
    Button *button;

    button = new Button(trs("EnterManometerMode"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 0, 2);
    connect(button, SIGNAL(released()), this, SLOT(enterManometerReleased()));
    d_ptr->modeBtn = button;

    label = new QLabel(trs("ServicePressure"));
    layout->addWidget(label, 1, 0);

    label = new QLabel(InvStr());
    layout->addWidget(label, 1, 1);
    d_ptr->value = label;

    label = new QLabel();
    label->setText(Unit::getSymbol(nibpParam.getUnit()));
    layout->addWidget(label, 1, 2);
}

void NIBPManometerContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->inModeTimerID == ev->timerId())
    {
        bool reply = nibpParam.geReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            d_ptr->modeBtn->setEnabled(true);
            if (reply && nibpParam.getResult())
            {
                if (d_ptr->isManometerMode)
                {
                    d_ptr->isManometerMode = false;
                    d_ptr->modeBtn->setText(trs("EnterManometerMode"));
                    killTimer(d_ptr->pressureTimerID);
                    d_ptr->pressureTimerID = -1;
                }
                else
                {
                    d_ptr->isManometerMode = true;
                    d_ptr->modeBtn->setText(trs("QuitManometerMode"));
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
    else if (d_ptr->pressureTimerID == ev->timerId())
    {
        if (d_ptr->pressure != nibpParam.getManometerPressure())
        {
            d_ptr->pressure = nibpParam.getManometerPressure();
            d_ptr->value->setNum(nibpParam.getManometerPressure());
        }
    }
}

void NIBPManometerContent::enterManometerReleased()
{
    if (d_ptr->moduleStr != "SUNTECH_NIBP")
    {
        d_ptr->inModeTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
        d_ptr->modeBtn->setEnabled(false);
        if (d_ptr->isManometerMode)
        {
            nibpParam.provider().serviceManometer(false);
            nibpParam.switchState(NIBP_SERVICE_STANDBY_STATE);
        }
        else
        {
            nibpParam.switchState(NIBP_SERVICE_MANOMETER_STATE);
        }
    }
    else
    {
        if (d_ptr->isManometerMode)
        {
            d_ptr->modeBtn->setText(trs("EnterManometerMode"));
            d_ptr->isManometerMode = false;
            killTimer(d_ptr->pressureTimerID);
            d_ptr->pressureTimerID = -1;
        }
        else
        {
            // 压力计模式下关闭气泵气阀.
            nibpParam.provider().controlPneumatics(0, 1, 1);
            d_ptr->modeBtn->setText(trs("QuitManometerMode"));
            d_ptr->isManometerMode = true;
            d_ptr->pressureTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
        }
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPManometerContent::~NIBPManometerContent()
{
    delete d_ptr;
}

void NIBPManometerContent::init()
{
    d_ptr->isManometerMode = false;
    d_ptr->modeBtn->setEnabled(true);
    d_ptr->modeBtn->setText(trs("EnterManometerMode"));
}

