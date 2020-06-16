/**
 ** This file is part of the Project project. ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/10
 **/

#include "COMeasureWindow.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/UI/Button.h"
#include "COMeasureResultWidget.h"
#include "Framework/Utility/Unit.h"
#include "COMeasureWidget.h"
#include "ColorManager.h"
#include <QGridLayout>
#include <QLabel>
#include <QBoxLayout>
#include "COParam.h"
#include "FontManager.h"
#include "SystemManager.h"
#include <QTimerEvent>
#include <qmath.h>
#include "PatientManager.h"
#include "FloatHandle.h"
#include "MeasureSettingWindow.h"

/* support 6 measure reuslt at most */
#define MAX_MEASURE_RESULT_NUM   6

#define DEMO_TIMER_PERIOD   40  /* update the wave data at 25 HZ */

#define WAVE_DURATION_AFTER_MEASURE (10 * 1000)

#define CHECK_INJECT_PERIOD (100)

#define COMPLETE_MESSAGE_SHOW_DURATION (1000)

#define MEASUREMENT_TIMEOUT_DURATION    (65000)  /* 65 seconds */

#define STOP_MESSAGE_DISPLAY_DURATTION (1500)

static short tbDemoWave[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x08, 0x08, 0x09, 0x0a, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0d, 0x0e, 0x0f, 0x10, 0x10, 0x11, 0x12, 0x12, 0x13, 0x14, 0x16, 0x17, 0x18, 0x19,
    0x1a, 0x1b, 0x1c, 0x1e, 0x1f, 0x20, 0x22, 0x23, 0x24, 0x26, 0x27, 0x28, 0x2a, 0x2b, 0x2c, 0x2e,
    0x2f, 0x31, 0x32, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41,
    0x42, 0x43, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52,
    0x52, 0x53, 0x54, 0x54, 0x55, 0x55, 0x56, 0x56, 0x56, 0x56, 0x57, 0x57, 0x57, 0x57, 0x58, 0x58,
    0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x57, 0x57, 0x57,
    0x57, 0x57, 0x57, 0x57, 0x56, 0x56, 0x56, 0x56, 0x55, 0x55, 0x55, 0x55, 0x55, 0x54, 0x54, 0x54,
    0x54, 0x53, 0x53, 0x52, 0x52, 0x51, 0x51, 0x50, 0x50, 0x4f, 0x4f, 0x4e, 0x4e, 0x4d, 0x4d, 0x4c,
    0x4b, 0x4b, 0x4a, 0x4a, 0x49, 0x49, 0x48, 0x48, 0x47, 0x46, 0x46, 0x45, 0x45, 0x44, 0x44, 0x43,
    0x43, 0x42, 0x42, 0x42, 0x41, 0x41, 0x40, 0x40, 0x3f, 0x3f, 0x3f, 0x3e, 0x3e, 0x3d, 0x3d, 0x3d,
    0x3c, 0x3c, 0x3b, 0x3b, 0x3a, 0x3a, 0x39, 0x39, 0x39, 0x38, 0x37, 0x37, 0x37, 0x36, 0x36, 0x36,
    0x35, 0x35, 0x34, 0x34, 0x34, 0x33, 0x33, 0x33, 0x32, 0x31, 0x31, 0x30, 0x30, 0x30, 0x2f, 0x2f,
    0x2f, 0x2e, 0x2d, 0x2d, 0x2d, 0x2c, 0x2c, 0x2b, 0x2b, 0x2b, 0x2a, 0x2a, 0x2a, 0x29, 0x29, 0x29,
    0x28, 0x28, 0x27, 0x27, 0x27, 0x26, 0x26, 0x26, 0x25, 0x25, 0x25, 0x25, 0x24, 0x24, 0x24, 0x23,
    0x23, 0x23, 0x22, 0x22, 0x22, 0x21, 0x21, 0x20, 0x20, 0x20, 0x1f, 0x1f, 0x1f, 0x1e, 0x1e, 0x1e,
    0x1d, 0x1d, 0x1c, 0x1c, 0x1c, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1a, 0x1a, 0x19, 0x19, 0x19, 0x19,
    0x19, 0x18, 0x18, 0x18, 0x17, 0x17, 0x17, 0x17, 0x16, 0x16, 0x16, 0x15, 0x15, 0x15, 0x15, 0x15,
    0x14, 0x14, 0x13, 0x13, 0x13, 0x13, 0x13, 0x12, 0x12, 0x12, 0x12, 0x12, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0e,
    0x0e, 0x0e, 0x0e, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0c, 0x0c, 0x0c,
    0x0c, 0x0c, 0x0c, 0x0c, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0a, 0x0a,
    0x0a, 0x0a, 0x0a, 0x0a, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

class COMeasureWindowPrivate
{
public:
    enum MeasureStopReason
    {
        STOP_REASON_COMPLETED,
        STOP_REASON_CANCEL,
        STOP_REASON_SENSOR_OFF,
        STOP_REASON_TIMEOUT,
        STOP_REASON_FAIL,
    };

    explicit COMeasureWindowPrivate(COMeasureWindow * const q_ptr)
        : q_ptr(q_ptr), ctrlBtn(NULL), settingBtn(NULL), saveBtn(NULL), printBtn(NULL),
          calcBtn(NULL), measureWidget(NULL), coAvgLabel(NULL), coAvgVal(NULL), ciAvgLabel(NULL),
          ciAvgVal(NULL), bsaLabel(NULL), bsaVal(NULL), demoTimerID(-1), measureTimeoutTimerID(-1),
          waitStateTimerID(-1), checkInjectTimerID(-1), noInjectCount(0), stopMessageTimerID(-1),
          demoDataReadIndex(0), bsa(0.0f), isMeasuring(false), sensorOff(false)
    {
        for (int i = 0; i < MAX_MEASURE_RESULT_NUM; i++)
        {
            resultWidget[i] = NULL;
        }
    }

    /**
     * @brief stopTimer helper function to stop timer
     * @param timerID pointer of the timer id
     */
    void stopTimer(int *timerID)
    {
        if (*timerID != -1)
        {
            q_ptr->killTimer(*timerID);
            *timerID = -1;
        }
    }

    /**
     * @brief handleMeasureResult handle the measure result when measure complete
     */
    void handleMeasureResult();

    /**
     * @brief startMeasure start the measuring
     */
    void startMeasure();

    /**
     * @brief stopMeasure stop the measuring
     */
    void stopMeasure(MeasureStopReason reason);

    /**
     * @brief getAverageCo get the average co value from the checked result
     * @return the average co
     */
    short getAverageCo() const;

    /**
     * @brief getAverageCi get the average ci value
     * @return the average ci or InvData()
     */
    short getAverageCi() const;

    /**
     * @brief coValToStringHelper help function to convert the co value to string
     * @param value the co related value
     * @return  the string
     */
    QString coValToStringHelper(short value)
    {
        if (value == InvData())
        {
            return InvStr();
        }

        return QString::number(value * 1.0 / 10, 'f', 1);
    }

    /**
     * @brief calcBsa calculate the BSA
     * @param height height in cm
     * @param weight weight in kg
     * @return the bsa in m^2
     */
    qreal calcBsa(qreal height, qreal weight)
    {
        /* use DuBois formula */
        return 0.20247 * qPow(height / 100, 0.725) * qPow(weight, 0.425);
    }

    COMeasureWindow * const q_ptr;
    Button *ctrlBtn;
    Button *settingBtn;
    Button *saveBtn;
    Button *printBtn;
    Button *calcBtn;
    COMeasureWidget *measureWidget;
    COMeasureResultWidget *resultWidget[MAX_MEASURE_RESULT_NUM];
    QLabel *coAvgLabel;
    QLabel *coAvgVal;
    QLabel *ciAvgLabel;
    QLabel *ciAvgVal;
    QLabel *bsaLabel;
    QLabel *bsaVal;

    int demoTimerID;            /* demo wave generate timer */
    /*
     * add a timer to check whether timeout. Usually, timeout message should from module,
     * but we need a timer to handle the case when no timeout message from module,
     * the timer is longer than the module timeout duration
     */
    int measureTimeoutTimerID;
    int waitStateTimerID;       /* timer to control the wait state */
    int checkInjectTimerID;     /* timer to check wether has injected  */
    int noInjectCount;
    int stopMessageTimerID;     /* timer to control the display of stop message */

    size_t demoDataReadIndex;   /* index of reading demo data */

    float bsa;                  /* body surface area */

    bool isMeasuring;           /* record whether we are in measuring state */
    bool sensorOff;             /* check whether in sensor state */
};

COMeasureWindow::COMeasureWindow()
    : Dialog(), pimpl(new COMeasureWindowPrivate(this))
{
    setFixedSize(themeManager.defaultWindowSize());
    setWindowTitle(trs("COMeasure"));

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(4, 2, 4, 2);
    setWindowLayout(layout);

    QColor color = colorManager.getColor(paramInfo.getParamName(PARAM_CO));
    pimpl->measureWidget = new COMeasureWidget();
    QPalette pal = pimpl->measureWidget->palette();
    pal.setColor(QPalette::WindowText, color);
    pimpl->measureWidget->setPalette(pal);
    layout->addWidget(pimpl->measureWidget, 0, 0, 2, 3);

    pimpl->ctrlBtn = new Button(trs("Start"));
    pimpl->ctrlBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(pimpl->ctrlBtn, 0, 3, 1, 1, Qt::AlignVCenter);
    connect(pimpl->ctrlBtn, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));

    pimpl->settingBtn = new Button(trs("Setting"));
    pimpl->settingBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(pimpl->settingBtn, 1, 3, 1, 1, Qt::AlignVCenter);
    connect(pimpl->settingBtn, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));

    int resultOnEachRow  = MAX_MEASURE_RESULT_NUM / 2;
    for (int i = 0; i < MAX_MEASURE_RESULT_NUM; i++)
    {
        COMeasureResultWidget *rw = new COMeasureResultWidget();
        connect(rw, SIGNAL(chechedStateChanged(bool)), this, SLOT(onResultChecked()));
        pal = rw->palette();
        pal.setColor(QPalette::WindowText, color);
        rw->setPalette(pal);
        rw->setEnabled(false);
        layout->addWidget(rw, 2 + i / resultOnEachRow, i % resultOnEachRow, 1, 1);
        pimpl->resultWidget[i] = rw;
    }

    QGridLayout *valueLabelLayout = new QGridLayout;
    pimpl->coAvgLabel = new QLabel(QString("C.O. Avg.\n(%1)").arg(Unit::getSymbol(UNIT_LMin)));
    pimpl->coAvgLabel->setFont(fontManager.textFont(fontManager.getFontSize(2)));
    valueLabelLayout->addWidget(pimpl->coAvgLabel, 0, 0);
    pimpl->coAvgVal = new QLabel(InvStr());
    pimpl->coAvgVal->setAlignment(Qt::AlignCenter);
    pimpl->coAvgVal->setFont(fontManager.textFont(fontManager.getFontSize(5)));
    valueLabelLayout->addWidget(pimpl->coAvgVal, 0, 1);

    pimpl->ciAvgLabel = new QLabel(QString::fromUtf8("C.I. Avg.\n(L/min/m²)"));
    pimpl->ciAvgLabel->setFont(fontManager.textFont(fontManager.getFontSize(2)));
    valueLabelLayout->addWidget(pimpl->ciAvgLabel, 1, 0);
    pimpl->ciAvgVal = new QLabel(InvStr());
    pimpl->ciAvgVal->setAlignment(Qt::AlignCenter);
    pimpl->ciAvgVal->setFont(fontManager.textFont(fontManager.getFontSize(5)));
    valueLabelLayout->addWidget(pimpl->ciAvgVal, 1, 1);

    pimpl->bsaLabel = new QLabel(QString::fromUtf8("BSA (m²)"));
    pimpl->bsaLabel->setFont(fontManager.textFont(fontManager.getFontSize(2)));
    valueLabelLayout->addWidget(pimpl->bsaLabel, 2, 0);
    pimpl->bsaVal = new QLabel(InvStr());
    pimpl->bsaVal->setAlignment(Qt::AlignCenter);
    pimpl->bsaVal->setFont(fontManager.textFont(fontManager.getFontSize(5)));
    valueLabelLayout->addWidget(pimpl->bsaVal, 2, 1);

    layout->addLayout(valueLabelLayout, 2, 3, 2, 1);

    QBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch(1);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    pimpl->saveBtn = new Button(trs("Save"));
    pimpl->saveBtn->setButtonStyle(Button::ButtonTextOnly);
    btnLayout->addWidget(pimpl->saveBtn, 2);
    connect(pimpl->saveBtn, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    pimpl->printBtn = new Button(trs("Print"));
    pimpl->printBtn->setButtonStyle(Button::ButtonTextOnly);
    btnLayout->addWidget(pimpl->printBtn, 2);
    connect(pimpl->printBtn, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    pimpl->calcBtn = new Button(trs("Calculate"));
    pimpl->calcBtn->setButtonStyle(Button::ButtonTextOnly);
    btnLayout->addWidget(pimpl->calcBtn, 2);
    connect(pimpl->calcBtn, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
    btnLayout->addStretch(1);

    layout->addLayout(btnLayout, 4, 0, 1, 4);

    pimpl->saveBtn->setEnabled(false);
    pimpl->calcBtn->setEnabled(false);
    pimpl->printBtn->setEnabled(false);
}

COMeasureWindow::~COMeasureWindow()
{
}

void COMeasureWindow::setMeasureResult(short co, short ci)
{
    if (!pimpl->isMeasuring)
    {
        return;
    }

    /* got result */
    pimpl->measureWidget->stopMeasure();
    pimpl->measureWidget->setCo(co);
    if (co != InvData() && ci == InvData() && !isEqual(pimpl->bsa, 0.0f))
    {
        /* calculate ci base on the co */
        ci = co / pimpl->bsa;
    }
    pimpl->measureWidget->setCi(ci);
    pimpl->handleMeasureResult();
}

void COMeasureWindow::setTb(short tb)
{
    /* only update tb value when in measuring state */
    if (pimpl->isMeasuring)
    {
        pimpl->measureWidget->setTb(tb);
    }
}

void COMeasureWindow::addMeasureWaveData(short wave)
{
    pimpl->measureWidget->addMeasureWave(wave);
    if (wave > 2 && pimpl->isMeasuring && pimpl->checkInjectTimerID != -1)
    {
        /* consider injection has started when the wave value > 2, stop injection check */
        pimpl->stopTimer(&pimpl->checkInjectTimerID);
        pimpl->noInjectCount = 0;
        pimpl->measureWidget->setMessage(trs("MeasurementInProgress"));
    }
}

void COMeasureWindow::setSensorOff(bool off)
{
    if (pimpl->sensorOff != off)
    {
        pimpl->sensorOff = off;
        pimpl->stopMeasure(COMeasureWindowPrivate::STOP_REASON_SENSOR_OFF);
    }
}

void COMeasureWindow::start()
{
    if (!pimpl->isMeasuring)
    {
        /* set the UI in measure state */
        pimpl->startMeasure();
    }
}

void COMeasureWindow::cancel()
{
    if (pimpl->isMeasuring)
    {
        pimpl->stopMeasure(COMeasureWindowPrivate::STOP_REASON_CANCEL);
    }
}

void COMeasureWindow::timeout()
{
    if (pimpl->isMeasuring)
    {
        pimpl->stopMeasure(COMeasureWindowPrivate::STOP_REASON_TIMEOUT);
    }
}

void COMeasureWindow::fail()
{
    if (pimpl->isMeasuring)
    {
        pimpl->stopMeasure(COMeasureWindowPrivate::STOP_REASON_FAIL);
    }
}

void COMeasureWindow::showEvent(QShowEvent *ev)
{
    Dialog::showEvent(ev);

    if (!pimpl->measureWidget->getMeasureData().isValid())
    {
        /* load tb and ti if the measure data in the measure widget is invalid */
        pimpl->measureWidget->setTb(coParam.getTb());
        pimpl->measureWidget->setTi(coParam.getTi());
    }

    if (systemManager.getCurWorkMode() != WORK_MODE_DEMO)
    {
        if (!coParam.isConnected())
        {
            /* module is not connected yet and not in demo mode */
            pimpl->ctrlBtn->setEnabled(false);
            pimpl->measureWidget->setMessage(trs("ModuleNotConnected"));
        }
        else if (pimpl->sensorOff)
        {
            pimpl->ctrlBtn->setEnabled(false);
            pimpl->measureWidget->setMessage(trs("NoInjectateProbe"));
        }
        else
        {
            pimpl->ctrlBtn->setEnabled(true);
            if (!pimpl->isMeasuring && pimpl->waitStateTimerID == -1)
            {
                /* ready for new measurement */
                pimpl->measureWidget->setMessage(trs("ReadyForNewMeasurement"));
            }
        }
    }
    else
    {
        pimpl->ctrlBtn->setEnabled(true);
        if (!pimpl->isMeasuring && pimpl->waitStateTimerID == -1)
        {
            /* ready for new measurement */
            pimpl->measureWidget->setMessage(trs("ReadyForNewMeasurement"));
        }
    }

    /* calculate the bsa */
    if (isEqual(patientManager.getHeight(), 0.0f) || isEqual(patientManager.getWeight(), 0.0f))
    {
        pimpl->bsa = 0.0f;
        pimpl->bsaVal->setText(InvStr());
    }
    else
    {
        pimpl->bsa = pimpl->calcBsa(patientManager.getHeight(), patientManager.getWeight());
        pimpl->bsaVal->setText(QString::number(pimpl->bsa, 'f', 3));
    }
}

void COMeasureWindow::timerEvent(QTimerEvent *ev)
{
    if (pimpl->demoTimerID == ev->timerId())
    {
        if (pimpl->demoDataReadIndex >= sizeof(tbDemoWave) / sizeof(tbDemoWave[0]))
        {
            pimpl->demoDataReadIndex = 0;
            pimpl->stopTimer(&pimpl->demoTimerID);
            setMeasureResult(38, 15);
        }

        setTb(coParam.getTb() - tbDemoWave[pimpl->demoDataReadIndex] / 10);
        addMeasureWaveData(tbDemoWave[pimpl->demoDataReadIndex++]);
    }
    else if (pimpl->checkInjectTimerID == ev->timerId())
    {
        pimpl->noInjectCount += 1;
        if (pimpl->noInjectCount == 5)
        {
            /* show message to notify injection */
            pimpl->measureWidget->setMessage(trs("InjectNow"));
        }
    }
    else if (pimpl->measureTimeoutTimerID == ev->timerId())
    {
        pimpl->stopMeasure(COMeasureWindowPrivate::STOP_REASON_TIMEOUT);
    }
    else if (pimpl->stopMessageTimerID == ev->timerId())
    {
        pimpl->stopTimer(&pimpl->stopMessageTimerID);
        /* show wait message after stop message */
        pimpl->waitStateTimerID = startTimer(WAVE_DURATION_AFTER_MEASURE);
        pimpl->measureWidget->setMessage(trs("PleaseWait"));
    }
    else if (pimpl->waitStateTimerID == ev->timerId())
    {
        pimpl->stopTimer(&pimpl->waitStateTimerID);
        /* we can start another round of measuring */
        pimpl->ctrlBtn->setEnabled(true);
        pimpl->measureWidget->setMessage(trs("ReadyForNewMeasurement"));
    }
}

void COMeasureWindow::btnClicked()
{
    Button *btn = static_cast<Button *>(sender());
    if (btn == pimpl->ctrlBtn)
    {
        if (!pimpl->isMeasuring)
        {
            /* set the UI in measure state */
            pimpl->startMeasure();
        }
        else
        {
            cancel();
        }
    }
    else if (btn == pimpl->settingBtn)
    {
        MeasureSettingWindow *p = MeasureSettingWindow::getInstance();
        p->popup(trs("COMenu"), QVariant(), WindowManagerInterface::ShowBehaviorHideOthers);
    }
    else if (btn == pimpl->saveBtn)
    {
        coParam.setAverageResult(pimpl->getAverageCo(), pimpl->getAverageCi());
    }
    else if (btn == pimpl->printBtn)
    {
    }
    else if (btn == pimpl->calcBtn)
    {
    }
}

void COMeasureWindow::onResultChecked()
{
    short avgCo = pimpl->getAverageCo();
    pimpl->coAvgVal->setText(pimpl->coValToStringHelper(avgCo));

    short avgCi = pimpl->getAverageCi();
    pimpl->ciAvgVal->setText(pimpl->coValToStringHelper(avgCi));

    if (avgCo != InvData())
    {
        pimpl->saveBtn->setEnabled(true);
    }
}

void COMeasureWindow::onWorkModeChanged()
{
    /* when the work mode change, clear all the reuslt and info */
    pimpl->measureWidget->setTb(InvData());
    pimpl->measureWidget->setTi(InvData());
    pimpl->measureWidget->setCo(InvData());
    pimpl->measureWidget->setCi(InvData());
    for (int i = 0; i < MAX_MEASURE_RESULT_NUM; i++)
    {
        pimpl->resultWidget[i]->setMeasureData(COMeasureData());
        pimpl->resultWidget[i]->setChecked(false);
        pimpl->resultWidget[i]->setEnabled(false);
    }
    /* update average co and ci */
    onResultChecked();
}

void COMeasureWindowPrivate::handleMeasureResult()
{
    stopMeasure(STOP_REASON_COMPLETED);
    COMeasureData data = measureWidget->getMeasureData();
    if (data.isValid())
    {
        /* load the measure data to measure result widget */
        /* first move the result */
        for (int i = MAX_MEASURE_RESULT_NUM - 1; i > 0; --i)
        {
            bool isChecked = resultWidget[i-1]->isChecked();
            bool isEnabled = resultWidget[i-1]->isEnabled();

            resultWidget[i]->setMeasureData(resultWidget[i-1]->getMeasureData());
            resultWidget[i]->blockSignals(true);
            resultWidget[i]->setChecked(isChecked);
            resultWidget[i]->blockSignals(false);
            resultWidget[i]->setEnabled(isEnabled);
        }

        /* load current result */
        resultWidget[0]->setMeasureData(data);
        /* default enable */
        resultWidget[0]->setEnabled(true);
        /* default check */
        resultWidget[0]->blockSignals(true);
        resultWidget[0]->setChecked(true);
        resultWidget[0]->blockSignals(false);
        /* force calculate the average value */
        q_ptr->onResultChecked();
    }
}

void COMeasureWindowPrivate::startMeasure()
{
    ctrlBtn->setText(trs("Cancel"));
    isMeasuring = true;
    measureWidget->setWaveDataRate(coParam.getMeasureWaveRate());
    /* load tb and ti */
    measureWidget->setTb(coParam.getTb());
    measureWidget->setTi(coParam.getTi());
    measureWidget->setCo(InvData());
    measureWidget->setCi(InvData());
    measureWidget->startMeasure();
    checkInjectTimerID = q_ptr->startTimer(CHECK_INJECT_PERIOD);

    /* control the module to enter measure state */
    if (systemManager.getCurWorkMode() != WORK_MODE_DEMO)
    {
        coParam.startMeasure();
    }
    else
    {
        demoTimerID = q_ptr->startTimer(DEMO_TIMER_PERIOD);
    }
}

void COMeasureWindowPrivate::stopMeasure(MeasureStopReason reason)
{
    isMeasuring = false;
    ctrlBtn->setText(trs("Start"));
    ctrlBtn->setEnabled(false);
    measureWidget->stopMeasure();
    if (checkInjectTimerID != -1)
    {
        stopTimer(&checkInjectTimerID);
        noInjectCount = 0;
    }

    if (measureTimeoutTimerID != -1)
    {
        stopTimer(&measureTimeoutTimerID);
    }

    if (reason == STOP_REASON_COMPLETED)
    {
        stopMessageTimerID = q_ptr->startTimer(STOP_MESSAGE_DISPLAY_DURATTION);
        measureWidget->setMessage(trs("MeasureCompleted"));
    }
    else if (reason == STOP_REASON_CANCEL)
    {
        /* user cancel, just show the wait message */
        stopMessageTimerID = q_ptr->startTimer(STOP_MESSAGE_DISPLAY_DURATTION);
        measureWidget->setMessage(trs("MeasurementCancel"));
    }
    else if (reason == STOP_REASON_SENSOR_OFF)
    {
        /* stop because of senfor off */
        measureWidget->setMessage(trs("NoInjectateProbe"));
    }
    else if (reason == STOP_REASON_TIMEOUT)
    {
        stopMessageTimerID = q_ptr->startTimer(STOP_MESSAGE_DISPLAY_DURATTION);
        measureWidget->setMessage(trs("MeasurementTimeout"));
    }
    else if (reason == STOP_REASON_FAIL)
    {
        /* show the fail message for a while */
        stopMessageTimerID = q_ptr->startTimer(STOP_MESSAGE_DISPLAY_DURATTION);
        measureWidget->setMessage(trs("MeasurementFail"));
    }

    /* control the module to exit measure state */
    if (systemManager.getCurWorkMode() != WORK_MODE_DEMO)
    {
        coParam.stopMeasure();
    }
    else
    {
        stopTimer(&demoTimerID);
    }
}

short COMeasureWindowPrivate::getAverageCo() const
{
    int count = 0;
    int coSum = 0;
    for (int i = 0; i < MAX_MEASURE_RESULT_NUM; i++)
    {
        if (resultWidget[i]->isChecked() && resultWidget[i]->getMeasureData().isValid())
        {
            coSum += resultWidget[i]->getMeasureData().co;
            count += 1;
        }
    }

    if (count == 0)
    {
        return InvData();
    }

    return coSum / count;
}

short COMeasureWindowPrivate::getAverageCi() const
{
    int count = 0;
    int ciSum = 0;
    /* try to caculate the ci from the module */
    for (int i = 0; i < MAX_MEASURE_RESULT_NUM; i++)
    {
        if (resultWidget[i]->isChecked() && resultWidget[i]->getMeasureData().ci != InvData())
        {
            ciSum += resultWidget[i]->getMeasureData().ci;
            count += 1;
        }
    }

    if (count > 0)
    {
        return ciSum / count;
    }

    return InvData();
}
