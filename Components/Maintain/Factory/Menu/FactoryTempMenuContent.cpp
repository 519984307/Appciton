/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/25
 **/
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QProcess>
#include "LanguageManager.h"
#include "FactoryTempMenuContent.h"
#include "Button.h"
#include "ComboBox.h"
#include "TEMPParam.h"
#include <QTimer>
#include "FactoryMaintainManager.h"
#include <QStackedWidget>
#include <QGroupBox>
#include "FontManager.h"

#define TEMP_VALUE_UPDATE_TIME                 (100)
#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)

enum TempCalibrateValue
{
    TEMP_CALIBRATE_0,
    TEMP_CALIBRATE_5,
    TEMP_CALIBRATE_10,
    TEMP_CALIBRATE_15,
    TEMP_CALIBRATE_20,
    TEMP_CALIBRATE_25,
    TEMP_CALIBRATE_30,
    TEMP_CALIBRATE_35,
    TEMP_CALIBRATE_40,
    TEMP_CALIBRATE_45,
    TEMP_CALIBRATE_50,
    TEMP_CALIBRATE_NR,
};

enum TempCalibrateChannel
{
    TEMP_CALIBRATE_CHANNEL_1,
    TEMP_CALIBRATE_CHANNEL_2,
};

class FactoryTempMenuContentPrivate
{
public:
    static QString labelStr[TEMP_CALIBRATE_NR];

    FactoryTempMenuContentPrivate();

    QLabel *tempTitle;
    ComboBox *channel;

    Button *lbtn[TEMP_CALIBRATE_NR];
    QLabel *label[TEMP_CALIBRATE_NR];
    QLabel *calibrateResult[TEMP_CALIBRATE_NR];

    int calibrationTimerId;
    int timeoutNum;

    TempCalibrateChannel calibrateChannel;
    int calibrateValue;

    QTimer *tempValueTimer;
    UnitType curUnitType;
};

FactoryTempMenuContentPrivate::FactoryTempMenuContentPrivate()
    : tempTitle(NULL),
      channel(NULL),
      calibrationTimerId(-1),
      timeoutNum(0),
      calibrateChannel(TEMP_CALIBRATE_CHANNEL_1),
      calibrateValue(0),
      tempValueTimer(NULL),
      curUnitType(tempParam.getUnit())
{
    for (int i = 0; i < TEMP_CALIBRATE_NR; i++)
    {
        label[i] = NULL;
        lbtn[i] = NULL;
        calibrateResult[i] = NULL;
    }
}

QString FactoryTempMenuContentPrivate::labelStr[11] =
{
    "7409.3",
    "5742.9",
    "4491.1",
    "3541.3",
    "2813.9",
    "2252",
    "1814.5",
    "1471.1",
    "1199.8",
    "984",
    "811.3"
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactoryTempMenuContent::FactoryTempMenuContent()
    : MenuContent(trs("TEMPCalibrateMenu"),
                  trs("TEMPCalibrateMenuDesc")),
      d_ptr(new FactoryTempMenuContentPrivate)
{
    // 增加温度值更新
    d_ptr->tempValueTimer = new QTimer(this);
    connect(d_ptr->tempValueTimer, SIGNAL(timeout()), this, SLOT(timeOut()));
}

void FactoryTempMenuContent::layoutExec()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QGridLayout *layout = new QGridLayout();
    layout->setMargin(2);
    layout->setSpacing(10);
    this->setFocusPolicy(Qt::NoFocus);
    QHBoxLayout *hl;
    QLabel *label;
    Button *button;
    ComboBox *combo;

    hl = new QHBoxLayout;
    label = new QLabel;
    label->setFont(fontManager.textFont(20));
    hl->addWidget(label);
    d_ptr->tempTitle = label;

    vLayout->addWidget(label, 0, Qt::AlignCenter);

    label = new QLabel(trs("TEMPChannel"));
    layout->addWidget(label, 1, 0);

    combo = new ComboBox;
    combo->addItem(trs("Temp1"));
    combo->addItem(trs("Temp2"));
    combo->setFixedWidth(150);
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onChannelReleased(int)));
    d_ptr->channel = combo;
    layout->addWidget(combo, 1, 1);

    UnitType type = tempParam.getUnit();
    for (int i = 0; i < TEMP_CALIBRATE_NR; i++)
    {
        QString btnStr;
        if (type == UNIT_TC)
        {
            btnStr = QString::number(i * 5);
        }
        else
        {
            btnStr = QString::number(i * 9 + 32);
        }
        label = new QLabel(QString("%1: %2%3")
                           .arg(trs("TEMPCalibrateOhm"))
                           .arg(d_ptr->labelStr[i])
                           .arg(trs("Ohm")));
        layout->addWidget(label, 2 + i, 0);
        d_ptr->label[i] = label;
        button = new Button(QString("%1%2%3")
                                .arg(trs("TEMPCalibrate"))
                                .arg(btnStr)
                                .arg(trs(Unit::getSymbol(type))));
        button->setButtonStyle(Button::ButtonTextOnly);
        button->setFixedWidth(150);
        button->setProperty("Item", qVariantFromValue(i));
        connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
        layout->addWidget(button, 2 + i, 1);
        d_ptr->lbtn[i] = button;

        label = new QLabel;
        d_ptr->calibrateResult[i] = label;
        layout->addWidget(label, 2 + i, 2, Qt::AlignCenter);
    }

    layout->setRowStretch(12, 1);
    vLayout->addLayout(layout);
}

void FactoryTempMenuContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->calibrationTimerId == ev->timerId())
    {
        bool reply = tempParam.getCalibrationReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            if (reply && tempParam.getCalibrationResult())
            {
                d_ptr->calibrateResult[d_ptr->calibrateValue]->setText(trs("CalibrationSuccess"));
            }
            else
            {
                d_ptr->calibrateResult[d_ptr->calibrateValue]->setText(trs("CalibrationFail"));
            }
            killTimer(d_ptr->calibrationTimerId);
            d_ptr->calibrationTimerId = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
}

void FactoryTempMenuContent::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);

    // 停止温度更新定时器
    d_ptr->tempValueTimer->stop();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void FactoryTempMenuContent::readyShow()
{
    // 更新显示信息
    UnitType type = tempParam.getUnit();
    if ( type != d_ptr->curUnitType)
    {
        d_ptr->curUnitType = type;
        for (int i = 0; i < TEMP_CALIBRATE_NR; i++)
        {
            QString btnStr;
            if (type == UNIT_TC)
            {
                btnStr = QString::number(i * 5);
            }
            else
            {
                btnStr = QString::number(i * 9 + 32);
            }
            d_ptr->label[i]->setText(QString("%1: %2%3")
                               .arg(trs("TEMPCalibrateOhm"))
                               .arg(d_ptr->labelStr[i])
                               .arg(trs("Ohm")));
            d_ptr->lbtn[i]->setText(QString("%1%2%3")
                                    .arg(trs("TEMPCalibrate"))
                                    .arg(btnStr)
                                    .arg(trs(Unit::getSymbol(type))));
        }
    }

    d_ptr->channel->setCurrentIndex(0);
    d_ptr->calibrateChannel = TEMP_CALIBRATE_CHANNEL_1;

    for (int i = 0; i < TEMP_CALIBRATE_NR; ++i)
    {
        d_ptr->calibrateResult[i]->setText(trs("WaitingCalibration"));
    }

    if (tempParam.getErrorDisable())
    {
        showError(trs("TEMPModuleDisable"));
        return;
    }

    if (!tempParam.isServiceProviderOk())
    {
        showError(trs("TEMPModuleDisConnect"));
        return;
    }

    if (tempParam.getDisconnected())
    {
        showError(trs("TEMPCommunicationStop"));
        return;
    }

    // 开启温度更新定时器
    d_ptr->tempValueTimer->start(TEMP_VALUE_UPDATE_TIME);
}

/**************************************************************************************************
 * 功能:错误信息显示
 *************************************************************************************************/
void FactoryTempMenuContent::showError(QString str)
{
    d_ptr->tempTitle->setText(str);
}

/**************************************************************************************************
 * 功能:更改体温通道
 *************************************************************************************************/
void FactoryTempMenuContent::onChannelReleased(int channel)
{
    d_ptr->calibrateChannel = static_cast<TempCalibrateChannel>(channel);

    for (int i = 0; i < TEMP_CALIBRATE_NR; ++i)
    {
        d_ptr->calibrateResult[i]->setPixmap(QPixmap::fromImage(QImage()));
    }
}

/**************************************************************************************************
 * 功能:按钮释放
 *************************************************************************************************/
void FactoryTempMenuContent::onBtnReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    int value = button->property("Item").toInt();
    d_ptr->calibrateValue = value;
    d_ptr->calibrateResult[d_ptr->calibrateValue]->setText(trs("Calibrating"));
    if (!tempParam.isServiceProviderOk())
    {
        return;
    }
    tempParam.sendCalibrateData(d_ptr->calibrateChannel, d_ptr->calibrateValue);
    d_ptr->calibrationTimerId = startTimer(CALIBRATION_INTERVAL_TIME);
}


void FactoryTempMenuContent::timeOut()
{
    int16_t t1;
    int16_t t2;
    int16_t td;
    tempParam.getTEMP(t1, t2, td);
    Q_UNUSED(td)
    UnitType type = tempParam.getUnit();
    // 体温通道1
    if (d_ptr->calibrateChannel == TEMP_CALIBRATE_CHANNEL_1)
    {
        QString tStr;
        if (t1 == InvData() || t1 > 500 || t1 < 0)
        {
            tStr = InvStr();
        }
        else
        {
            // 校准时可以显示实时温度
            tStr = Unit::convert(type, UNIT_TC, t1 / 10.0);
        }
        d_ptr->tempTitle->setText(QString("%1: %2").arg(trs("TEMP1")).arg(tStr));
        return;
    }
    // 体温通道2
    if (d_ptr->calibrateChannel == TEMP_CALIBRATE_CHANNEL_2)
    {
        QString tStr;
        if (t2 == InvData() || t2 > 500 || t2 < 0)
        {
            tStr = InvStr();
        }
        else
        {
            // 校准时可以显示实时温度
            tStr = Unit::convert(type, UNIT_TC, t2 / 10.0);
        }
        d_ptr->tempTitle->setText(QString("%1: %2").arg(trs("TEMP2")).arg(tStr));
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryTempMenuContent::~FactoryTempMenuContent()
{
    delete d_ptr;
}

