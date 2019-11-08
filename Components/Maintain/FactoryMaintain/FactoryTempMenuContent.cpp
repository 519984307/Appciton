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
#include "Framework/Language/LanguageManager.h"
#include "FactoryTempMenuContent.h"
#include "Button.h"
#include "ComboBox.h"
#include "TEMPParam.h"
#include <QTimer>
#include <QStackedWidget>
#include <QGroupBox>
#include "FontManager.h"
#include <QTimerEvent>
#include "MenuWindow.h"
#include "ScrollArea.h"
#include "ThemeManager.h"
#include <QScrollBar>
#include <QStackedWidget>

#define TEMP_VALUE_UPDATE_TIME                 (100)
#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)
#define CALIBRATE_ON_TIME_NUMBER               (5000)

Q_DECLARE_METATYPE(QList<int>)

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
    TEMP_CALIBRATE_CHANNEL_NR
};

enum TempCalibrateState
{
    TEMP_CALIBRATE_STATE_WAITING,
    TEMP_CALIBRATE_STATE_CALIBRATING,
    TEMP_CALIBRATE_STATE_SUCCESS,
    TEMP_CALIBRATE_STATE_FAIL
};

class FactoryTempMenuContentPrivate
{
public:
    static QString labelStr[TEMP_CALIBRATE_NR];

    FactoryTempMenuContentPrivate();

    QLabel *tempTitle;
    QLabel *calibrate1Ohm;
    QLabel *calibrate2Ohm;
    Button *lbtn[TEMP_CALIBRATE_CHANNEL_NR][TEMP_CALIBRATE_NR];
    QLabel *ohmLabel[TEMP_CALIBRATE_NR];
    QLabel *calibrateResultLbl[TEMP_CALIBRATE_CHANNEL_NR][TEMP_CALIBRATE_NR];

    int calibrationTimerId;  // 校准时钟
    int onTimeId;  // 定时发送时钟
    int timeoutNum;

    TempCalibrateChannel calibrateChannel;
    TempCalibrateValue calibrateValue;

    QTimer *tempValueTimer;
    UnitType curUnitType;

    TempCalibrateState calibrateResult[TEMP_CALIBRATE_CHANNEL_NR][TEMP_CALIBRATE_NR];

    ScrollArea *scrollArea;
    QStackedWidget *stackedWidget;

    void updateResultLabel(int column, int row);

    void updateWaitingLabel();
};

FactoryTempMenuContentPrivate::FactoryTempMenuContentPrivate()
    : tempTitle(NULL),
      calibrate1Ohm(NULL),
      calibrate2Ohm(NULL),
      calibrationTimerId(-1),
      onTimeId(-1),
      timeoutNum(0),
      calibrateChannel(TEMP_CALIBRATE_CHANNEL_1),
      calibrateValue(TEMP_CALIBRATE_0),
      tempValueTimer(NULL),
      curUnitType(tempParam.getUnit()),
      scrollArea(NULL),
      stackedWidget(NULL)
{
    for (int column = 0; column < TEMP_CALIBRATE_CHANNEL_NR; column++)
    {
        for (int row = 0; row < TEMP_CALIBRATE_NR; row++)
        {
            calibrateResult[column][row] = TEMP_CALIBRATE_STATE_WAITING;
            lbtn[column][row] = NULL;
            calibrateResultLbl[column][row] = NULL;
        }
    }
    for (int i = 0; i < TEMP_CALIBRATE_NR; i++)
    {
        ohmLabel[i] = NULL;
    }
}

void FactoryTempMenuContentPrivate::updateResultLabel(int column, int row)
{
    TempCalibrateState state = calibrateResult[column][row];
    QString text;
    switch (state)
    {
    case TEMP_CALIBRATE_STATE_WAITING:
        text = trs("WaitingCalibration");
        break;
    case TEMP_CALIBRATE_STATE_CALIBRATING:
        text = trs("Calibrating");
        break;
    case TEMP_CALIBRATE_STATE_SUCCESS:
        text = trs("CalibrationSuccess");
        break;
    case TEMP_CALIBRATE_STATE_FAIL:
        text = trs("CalibrationFail");
        break;
    default:
        break;
    }
    calibrateResultLbl[column][row]->setText(text);
}

void FactoryTempMenuContentPrivate::updateWaitingLabel()
{
    for (int j = 0; j < TEMP_CALIBRATE_CHANNEL_NR; j++)
    {
        for (int i = 0; i < TEMP_CALIBRATE_NR; i++)
        {
            if (calibrateResult[j][i] == TEMP_CALIBRATE_STATE_CALIBRATING)
            {
                calibrateResult[j][i] = TEMP_CALIBRATE_STATE_WAITING;
                updateResultLabel(j, i);
            }
        }
    }
}

QString FactoryTempMenuContentPrivate::labelStr[11] =
{
    "7356",
    "5720",
    "4483",
    "3539",
    "2814",
    "2252",
    "1815",
    "1471",
    "1200",
    "984.2",
    "811.8"
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

    QLabel *label;
    Button *button;

    // 体温信息
    QHBoxLayout *tempInfoLayout = new QHBoxLayout;

    label = new QLabel(trs("TEMPCalibrateOhm"));
    tempInfoLayout->addWidget(label);

    label = new QLabel(trs("Temp1"));
    label->setAlignment(Qt::AlignHCenter);
    tempInfoLayout->addWidget(label);

    label = new QLabel;
    label->setFont(fontManager.textFont(20));
    tempInfoLayout->addWidget(label);
    d_ptr->calibrate1Ohm = label;

    label = new QLabel(trs("Temp2"));
    label->setAlignment(Qt::AlignHCenter);
    tempInfoLayout->addWidget(label);

    label = new QLabel;
    label->setFont(fontManager.textFont(20));
    tempInfoLayout->addWidget(label);
    d_ptr->calibrate2Ohm = label;

    QWidget *container = new QWidget();
    container->setLayout(tempInfoLayout);
    QStackedWidget *stackWidget = new QStackedWidget();
    d_ptr->stackedWidget = stackWidget;
    stackWidget->addWidget(container);

    // 模块错误信息
    label = new QLabel;
    label->setFont(fontManager.textFont(20));
    label->setAlignment(Qt::AlignCenter);
    d_ptr->tempTitle = label;
    stackWidget->addWidget(label);
    vLayout->addWidget(stackWidget);

    // 校准按钮
    UnitType type = tempParam.getUnit();

    for (int i = 0; i < TEMP_CALIBRATE_NR; i++)
    {
        label = new QLabel(QString("%1%2")
                           .arg(d_ptr->labelStr[i])
                           .arg(trs("Ohm")));
        label->setAlignment(Qt::AlignHCenter);
        layout->addWidget(label, 1 + i, 0);
        d_ptr->ohmLabel[i] = label;

        for (int j = 0; j < TEMP_CALIBRATE_CHANNEL_NR; j++)
        {
            QString btnStr;
            QList<int> columnRowNum;
            if (type == UNIT_TC)
            {
                btnStr = QString::number(i * 5);
            }
            else
            {
                btnStr = QString::number(i * 9 + 32);
            }

            button = new Button(QString("%1%2")
                                .arg(btnStr)
                                .arg(trs(Unit::getSymbol(type))));
            button->setButtonStyle(Button::ButtonTextOnly);
            button->setFixedHeight(themeManger.getAcceptableControlHeight());
            columnRowNum.append(j);
            columnRowNum.append(i);
            button->setProperty("Item", qVariantFromValue(columnRowNum));
            connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
            layout->addWidget(button, 1 + i, 2 * j + 1);
            d_ptr->lbtn[j][i] = button;
            d_ptr->lbtn[j][i]->installEventFilter(this);

            label = new QLabel;
            d_ptr->calibrateResultLbl[j][i] = label;
            layout->addWidget(label, 1 + i, 2 * j + 2, Qt::AlignCenter);
        }
    }

    ScrollArea *area = new ScrollArea();
    container = new QWidget();
    container->setLayout(layout);
    area->setWidget(container);
    area->setAcceptDrops(true);
    area->setContentsMargins(0, 0, 0, 0);
    area->setFloatbarPolicy(ScrollArea::FloatBarShowForeverWhenNeeded);
    area->setScrollDirection(ScrollArea::ScrollVertical);
    area->installEventFilter(this);
    area->verticalScrollBar()->setMaximum(container->height());
    d_ptr->scrollArea = area;

    vLayout->addWidget(area);
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
                d_ptr->calibrateResult[d_ptr->calibrateChannel][d_ptr->calibrateValue] = TEMP_CALIBRATE_STATE_SUCCESS;
            }
            else
            {
                d_ptr->calibrateResult[d_ptr->calibrateChannel][d_ptr->calibrateValue] = TEMP_CALIBRATE_STATE_FAIL;
            }
            killTimer(d_ptr->calibrationTimerId);
            d_ptr->calibrationTimerId = -1;
            d_ptr->timeoutNum = 0;
            d_ptr->updateResultLabel(d_ptr->calibrateChannel, d_ptr->calibrateValue);
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
    else if (d_ptr->onTimeId == ev->timerId())
    {
        tempParam.sendCalibrateData(static_cast<int>(d_ptr->calibrateChannel),
                                        static_cast<int>(d_ptr->calibrateValue));
        d_ptr->calibrationTimerId = startTimer(CALIBRATION_INTERVAL_TIME);
        killTimer(d_ptr->onTimeId);
        d_ptr->onTimeId = -1;
    }
}

bool FactoryTempMenuContent::eventFilter(QObject *obj, QEvent *ev)
{
    Button *btn = qobject_cast<Button*>(obj);
    if (btn == d_ptr->lbtn[0][0] || btn == d_ptr->lbtn[TEMP_CALIBRATE_CHANNEL_2][TEMP_CALIBRATE_50])
    {
        if (ev->type() == QEvent::FocusIn)
        {
            d_ptr->scrollArea->ensureWidgetVisible(btn);
        }
    }
    return MenuContent::eventFilter(obj, ev);
}

void FactoryTempMenuContent::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);

    // 停止温度更新定时器
    d_ptr->tempValueTimer->stop();
    tempParam.exitCalibrateState();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void FactoryTempMenuContent::readyShow()
{
    for (int i = 0; i < TEMP_CALIBRATE_NR; i++)
    {
        d_ptr->ohmLabel[i]->setText(QString("%1%2")
                           .arg(d_ptr->labelStr[i])
                           .arg(trs("Ohm")));
    }
    // 更新显示信息
    UnitType type = tempParam.getUnit();
    if ( type != d_ptr->curUnitType)
    {
        d_ptr->curUnitType = type;
        for (int j = 0; j < TEMP_CALIBRATE_CHANNEL_NR; j++)
        {
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

                d_ptr->lbtn[j][i]->setText(QString("%1%2")
                                        .arg(btnStr)
                                        .arg(trs(Unit::getSymbol(type))));
            }
        }
    }
    for (int j = 0; j < TEMP_CALIBRATE_CHANNEL_NR; j++)
    {
        for (int i = 0; i < TEMP_CALIBRATE_NR; ++i)
        {
            d_ptr->calibrateResult[j][i] = TEMP_CALIBRATE_STATE_WAITING;
            d_ptr->updateResultLabel(j, i);
        }
    }
    if (tempParam.getErrorDisable())
    {
        showError(trs("TEMPModuleDisable"));
        return;
    }

    if (!tempParam.isServiceProviderOk())
    {
        showError(trs("TEMPModuleDisconnected"));
        return;
    }

    if (tempParam.getDisconnected())
    {
        showError(trs("TEMPCommunicationStop"));
        return;
    }

    showError(QString());

    // 开启温度更新定时器
    d_ptr->tempValueTimer->start(TEMP_VALUE_UPDATE_TIME);
    // 进入体温校准状态
    tempParam.enterCalibrateState();
}

/**************************************************************************************************
 * 功能:错误信息显示
 *************************************************************************************************/
void FactoryTempMenuContent::showError(QString str)
{
    d_ptr->tempTitle->setText(str);
    if (str.isEmpty())
    {
        d_ptr->stackedWidget->setCurrentIndex(0);
    }
    else
    {
        d_ptr->stackedWidget->setCurrentIndex(1);
    }
}

void FactoryTempMenuContent::calibrateValueOnTime(int time)
{
    d_ptr->onTimeId = startTimer(time);
}
/**************************************************************************************************
 * 功能:按钮释放
 *************************************************************************************************/
void FactoryTempMenuContent::onBtnReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    d_ptr->updateWaitingLabel();
    QVariant variant = button->property("Item");
    if (!variant.canConvert<QList<int> >())
    {
        return;
    }
    QList<int> rowColumn = qvariant_cast<QList<int> >(variant);
    d_ptr->calibrateChannel = static_cast<TempCalibrateChannel>(rowColumn.at(0));
    d_ptr->calibrateValue = static_cast<TempCalibrateValue>(rowColumn.at(1));
    if (d_ptr->calibrateChannel  < TEMP_CALIBRATE_CHANNEL_1 || d_ptr->calibrateChannel  > TEMP_CALIBRATE_CHANNEL_2
            || d_ptr->calibrateValue < TEMP_CALIBRATE_0 || d_ptr->calibrateValue > TEMP_CALIBRATE_50)
    {
        return;
    }
    d_ptr->calibrateResult[d_ptr->calibrateChannel][d_ptr->calibrateValue] = TEMP_CALIBRATE_STATE_CALIBRATING;
    d_ptr->updateResultLabel(static_cast<int>(d_ptr->calibrateChannel),
                                static_cast<int>(d_ptr->calibrateValue));
    if (!tempParam.isServiceProviderOk())
    {
        return;
    }

    calibrateValueOnTime(CALIBRATE_ON_TIME_NUMBER);
}

void FactoryTempMenuContent::timeOut()
{
    QString tStr;
    int curOhm1 = tempParam.getOhm(SUB_PARAM_T1);
    int curOhm2 = tempParam.getOhm(SUB_PARAM_T2);

    if (curOhm1 == InvData() || curOhm1 > 9000 || curOhm1 < 0)
    {
        tStr = InvStr();
    }
    else
    {
        tStr = QString::number(curOhm1);
    }
    d_ptr->calibrate1Ohm->setText(QString("%1%2").arg(tStr).arg(trs("Ohm")));

    if (curOhm2 == InvData() || curOhm2 > 9000 || curOhm2 < 0)
    {
        tStr = InvStr();
    }
    else
    {
        tStr = QString::number(curOhm2);
    }
    d_ptr->calibrate2Ohm->setText(QString("%1%2").arg(tStr).arg(trs("Ohm")));

    return;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryTempMenuContent::~FactoryTempMenuContent()
{
    delete d_ptr;
}

