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
#include "FactoryWindowManager.h"
#include "TEMPParam.h"
#include <QTimer>
#include "FactoryMaintainManager.h"
#include <QStackedWidget>
#include <QGroupBox>

class FactoryTempMenuContentPrivate
{
public:
    static QString btnStr[10];
    static QString labelStr[10];

    FactoryTempMenuContentPrivate();

    QLabel *tempChannel;
    QLabel *tempValue;

    QLabel *tempError;
    QStackedWidget *stackedwidget;
    ComboBox *channel;

    Button *lbtn[10];
    QLabel *calibrateResult[10];

    QImage success;
    QImage fault;

    QTimer *timer;

    int calibrateChannel;
    int calibrateValue;
};

FactoryTempMenuContentPrivate::FactoryTempMenuContentPrivate()
    : tempChannel(NULL),
      tempValue(NULL),
      tempError(NULL),
      stackedwidget(NULL),
      channel(NULL),
      success(""),
      fault(""),
      timer(NULL),
      calibrateChannel(0),
      calibrateValue(0)
{
    for (int i = 0; i < 10; i++)
    {
        lbtn[i] = NULL;
        calibrateResult[i] = NULL;
    }
}

QString FactoryTempMenuContentPrivate::btnStr[10] =
{
    "TEMPCalibrate0",
    "TEMPCalibrate10",
    "TEMPCalibrate15",
    "TEMPCalibrate20",
    "TEMPCalibrate25",
    "TEMPCalibrate30",
    "TEMPCalibrate35",
    "TEMPCalibrate40",
    "TEMPCalibrate45",
    "TEMPCalibrate50"
};

QString FactoryTempMenuContentPrivate::labelStr[10] =
{
    "TEMPCalibrate0is7409.3",
    "TEMPCalibrate10is4491.1",
    "TEMPCalibrate15is3541.3",
    "TEMPCalibrate20is2813.9",
    "TEMPCalibrate25is2252",
    "TEMPCalibrate30is1814.5",
    "TEMPCalibrate35is1471.1",
    "TEMPCalibrate40is1199.8",
    "TEMPCalibrate45is984",
    "TEMPCalibrate50is811.3"
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactoryTempMenuContent::FactoryTempMenuContent()
    : MenuContent(trs("TEMPCalibrate"),
                  trs("TEMPCalibrateDesc")),
      d_ptr(new FactoryTempMenuContentPrivate)
{
}

void FactoryTempMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);
    QHBoxLayout *hl;
    QLabel *label;
    Button *button;
    ComboBox *combo;

    hl = new QHBoxLayout;
    label = new QLabel;
    hl->addWidget(label);
    d_ptr->tempChannel = label;

    label = new QLabel(trs("---"));
    hl->addWidget(label);
    d_ptr->tempValue = label;

    QStackedWidget *stackWidget = new QStackedWidget;
    QGroupBox *groupBox = new QGroupBox;
    groupBox->setStyleSheet("border:none");
    groupBox->setLayout(hl);
    stackWidget->addWidget(groupBox);

    label = new QLabel();
    stackWidget->addWidget(label);
    d_ptr->stackedwidget = stackWidget;
    d_ptr->tempError = label;

    layout->addWidget(stackWidget, 0, 1, Qt::AlignRight);

    label = new QLabel(trs("TEMPChannel"));
    layout->addWidget(label, 1, 0);

    combo = new ComboBox;
    combo->addItem("Temp1");
    combo->addItem(trs("Temp2"));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onChannelReleased(int)));
    d_ptr->channel = combo;
    layout->addWidget(combo, 1, 1);

    for (int i = 0; i < 10; i++)
    {
        label = new QLabel(trs(d_ptr->labelStr[i]));
        layout->addWidget(label, 2 + i, 0);
        button = new Button(trs(d_ptr->btnStr[i]));
        button->setButtonStyle(Button::ButtonTextOnly);
        button->setProperty("Item", qVariantFromValue(i));
        connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
        layout->addWidget(button, 2 + i, 1);
        d_ptr->lbtn[i] = button;

        label = new QLabel;
        d_ptr->calibrateResult[i] = label;
        layout->addWidget(label, 2 + i, 2);
    }

    layout->setRowStretch(12, 1);

    d_ptr->success = QImage("/usr/local/iDM/icons/select.png");
    d_ptr->success = d_ptr->success.scaled(20, 20);
    d_ptr->fault = QImage("/usr/local/iDM/icons/cancel.png");
    d_ptr->fault = d_ptr->fault.scaled(20, 20);


    d_ptr->timer = new QTimer();
    d_ptr->timer->setInterval(2000);
    connect(d_ptr->timer, SIGNAL(timeout()), this, SLOT(timeOut()));
}

void FactoryTempMenuContent::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);

    if (NULL != d_ptr->timer)
    {
        d_ptr->timer->stop();
    }
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void FactoryTempMenuContent::readyShow()
{
    d_ptr->channel->setFocus();
    d_ptr->channel->setCurrentIndex(0);
    d_ptr->calibrateChannel = 0;
    d_ptr->tempChannel->setText(trs("TEMP1"));

    for (int i = 0; i < 10; ++i)
    {
        d_ptr->calibrateResult[i]->setPixmap(QPixmap::fromImage(QImage()));
    }

    if (tempParam.getErrorDisable())
    {
        showError(trs("TEMPModuleDisable"));
    }

    if (!tempParam.isServiceProviderOk())
    {
        showError(trs("TEMPModuleDisConnect"));
        return;
    }

    if (tempParam.getDisconnected())
    {
        showError(trs("TEMPCommunicationStop"));
    }
}

/**************************************************************************************************
 * 功能:获取校准结果
 *************************************************************************************************/
void FactoryTempMenuContent::getResult(int channel, int value, bool flag)
{
    d_ptr->timer->stop();
    int index = d_ptr->calibrateValue;
    if (d_ptr->calibrateChannel == channel && d_ptr->calibrateValue == value)
    {
        if (flag)
        {
            d_ptr->calibrateResult[index]->setPixmap(QPixmap::fromImage(d_ptr->success));
        }
        else
        {
            d_ptr->calibrateResult[index]->setPixmap(QPixmap::fromImage(d_ptr->fault));
        }
    }
    else
    {
        d_ptr->calibrateResult[index]->setPixmap(QPixmap::fromImage(d_ptr->fault));
    }
}

/**************************************************************************************************
 * 功能:显示体温值
 *************************************************************************************************/
void FactoryTempMenuContent::setTEMPValue(int16_t t1, int16_t t2)
{
    QString str;
    if (d_ptr->channel->currentIndex() == 0)
    {
        str = QString::number(t1 / 10.0, 'g', 1);
    }
    else
    {
        str = QString::number(t2 / 10.0, 'g', 1);
    }
    d_ptr->tempValue->setText(str);
    d_ptr->stackedwidget->setCurrentIndex(0);
}

/**************************************************************************************************
 * 功能:错误信息显示
 *************************************************************************************************/
void FactoryTempMenuContent::showError(QString str)
{
    d_ptr->tempError->setText(str);
    d_ptr->stackedwidget->setCurrentIndex(1);
}

/**************************************************************************************************
 * 功能:更改体温通道
 *************************************************************************************************/
void FactoryTempMenuContent::onChannelReleased(int channel)
{
    d_ptr->calibrateChannel = channel;
    if (channel == 0)
    {
        d_ptr->tempChannel->setText(trs("TEMP1"));
    }
    else
    {
        d_ptr->tempChannel->setText(trs("TEMP2"));
    }
    for (int i = 0; i < 10; ++i)
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

    if (!tempParam.isServiceProviderOk())
    {
        return;
    }
    tempParam.sendCalibrateData(d_ptr->calibrateChannel, d_ptr->calibrateValue);
    d_ptr->timer->start();
}

/**************************************************************************************************
 * 功能:超时
 *************************************************************************************************/
void FactoryTempMenuContent::timeOut()
{
    d_ptr->calibrateResult[d_ptr->calibrateValue]->setPixmap(QPixmap::fromImage(d_ptr->fault));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryTempMenuContent::~FactoryTempMenuContent()
{
    delete d_ptr;
}

