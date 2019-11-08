/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/10
 **/

#include "IBPMenuContent.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IBPSymbol.h"
#include "IConfig.h"
#include "IBPParam.h"
#include "MessageBox.h"
#include "Button.h"
#include "SpinBox.h"
#include "KeyInputPanel.h"
#include "MainMenuWindow.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimerEvent>
#include "AlarmLimitWindow.h"

#define AUTO_SCALE_UPDATE_TIME          (2 * 1000)
#define ZERO_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER             (5000 / ZERO_INTERVAL_TIME)
#define PRINT_WAVE_NUM (3)

class IBPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ENTITLE_1 = 1,
        ITEM_CBO_RULER_1,
        ITEM_SBO_UP_SCALE_1,
        ITEM_SBO_DOWN_SCALE_1,
        ITEM_CBO_ENTITLE_2,
        ITEM_CBO_RULER_2,
        ITEM_SBO_UP_SCALE_2,
        ITEM_SBO_DOWN_SCALE_2,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_FILTER_MODE,
        ITEM_CBO_SENSITIVITY,
        ITEM_CBO_CALIB_ZERO
    };

    explicit IBPMenuContentPrivate(IBPMenuContent *const q_ptr) :
        q_ptr(q_ptr),
        oneGBox(NULL), twoGBox(NULL),
        autoTimerId(-1),
        zeroTimerId(-1),
        timeoutNum(0)
    {}

    // load settings
    void loadOptions();

    /**
     * @brief updatePrintWaveIds
     */
    void updatePrintWaveIds();

    IBPMenuContent *const q_ptr;
    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> buttons;
    QMap<MenuItem, SpinBox *> spinBoxs;
    QGroupBox *oneGBox;
    QGroupBox *twoGBox;
    IBPPressureName ibp1;
    IBPPressureName ibp2;
    int autoTimerId;
    IBPRulerLimit rulerLimit1;
    IBPRulerLimit rulerLimit2;
    int zeroTimerId;
    int timeoutNum;                     // 最多超时等待50次
    QList<int> waveIdList;
};

void IBPMenuContentPrivate::loadOptions()
{
    ibp1 = ibpParam.getEntitle(IBP_INPUT_1);
    ibp2 = ibpParam.getEntitle(IBP_INPUT_2);

    // 获取当前ibp的波形id
    waveIdList.clear();
    waveIdList.append(ibpParam.getWaveformID(ibp1));
    waveIdList.append(ibpParam.getWaveformID(ibp2));

    combos[ITEM_CBO_ENTITLE_1]->setCurrentIndex(ibp1);
    combos[ITEM_CBO_ENTITLE_2]->setCurrentIndex(ibp2);
    rulerLimit1 = ibpParam.getRulerLimit(IBP_INPUT_1);
    rulerLimit2 = ibpParam.getRulerLimit(IBP_INPUT_2);
    if (rulerLimit1 == IBP_RULER_LIMIT_MANUAL)
    {
        spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1]->setEnabled(true);
        spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1]->setEnabled(true);
    }
    else
    {
        spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1]->setEnabled(false);
        spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1]->setEnabled(false);
    }

    if (rulerLimit2 == IBP_RULER_LIMIT_MANUAL)
    {
        spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2]->setEnabled(true);
        spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2]->setEnabled(true);
    }
    else
    {
        spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2]->setEnabled(false);
        spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2]->setEnabled(false);
    }

    if (rulerLimit1 == IBP_RULER_LIMIT_AOTU || rulerLimit2 == IBP_RULER_LIMIT_AOTU)
    {
        if (autoTimerId == -1)
        {
            autoTimerId = q_ptr->startTimer(AUTO_SCALE_UPDATE_TIME);  // 2s
        }
    }
    else
    {
        if (autoTimerId != -1)
        {
            q_ptr->killTimer(autoTimerId);
            autoTimerId = -1;
        }
    }

    combos[ITEM_CBO_RULER_1]->setCurrentIndex(rulerLimit1);
    combos[ITEM_CBO_RULER_2]->setCurrentIndex(rulerLimit2);
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(ibpParam.getSweepSpeed());
    combos[ITEM_CBO_FILTER_MODE]->setCurrentIndex(ibpParam.getFilter());
    combos[ITEM_CBO_SENSITIVITY]->setCurrentIndex(ibpParam.getSensitivity());
}

void IBPMenuContentPrivate::updatePrintWaveIds()
{
    // ibp1
    int cboItem = 0;
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        QString path;
        path = QString("Print|SelectWave%1").arg(i + 1);
        int waveId = WAVE_NONE;
        systemConfig.getNumValue(path, waveId);
        // 旧的打印波形与当前保存的波形一致时
        if (waveId == waveIdList.at(0))
        {
            IBPPressureName ibpTitle = ibpParam.getEntitle(IBP_INPUT_1);
            int ibpWaveID = ibpParam.getWaveformID(ibpTitle);
            // 替换波形
            if (ibpWaveID != waveId)
            {
                systemConfig.setNumValue(path, ibpWaveID);
            }
            cboItem = i;
            break;
        }
    }

    // ibp2
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        if (cboItem == i)
        {
            continue;
        }

        QString path;
        path = QString("Print|SelectWave%1").arg(i + 1);
        int waveId = WAVE_NONE;
        systemConfig.getNumValue(path, waveId);
        // 旧的打印波形与当前保存的波形一致时
        if (waveId == waveIdList.at(1))
        {
            IBPPressureName ibpTitle = ibpParam.getEntitle(IBP_INPUT_2);
            int ibpWaveID = ibpParam.getWaveformID(ibpTitle);
            // 替换波形
            if (ibpWaveID != waveId)
            {
                systemConfig.setNumValue(path, ibpWaveID);
            }
            break;
        }
    }
}

IBPMenuContent::IBPMenuContent()
    : MenuContent(trs("IBPMenu"), trs("IBPMenuDesc")),
      d_ptr(new IBPMenuContentPrivate(this))
{
}

IBPMenuContent::~IBPMenuContent()
{
    delete d_ptr;
}

void IBPMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void IBPMenuContent::layoutExec()
{
    QVBoxLayout *layout = new QVBoxLayout(this);


    ComboBox *comboBox;
    QLabel *label;
    Button *button;
    SpinBox *spinBox;
    int itemID;

    // 通道1 QGroupBox
    QGridLayout *gLayout = new QGridLayout();
    d_ptr->oneGBox = new QGroupBox("IBP1");
    d_ptr->oneGBox->setLayout(gLayout);
    layout->addWidget(d_ptr->oneGBox);

    // 标名选择1
    label = new QLabel(trs("PressureEntitle"));
    gLayout->addWidget(label, 0, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << IBPSymbol::convert(IBP_PRESSURE_ART)
                       << IBPSymbol::convert(IBP_PRESSURE_PA)
                       << IBPSymbol::convert(IBP_PRESSURE_CVP)
                       << IBPSymbol::convert(IBP_PRESSURE_LAP)
                       << IBPSymbol::convert(IBP_PRESSURE_RAP)
                       << IBPSymbol::convert(IBP_PRESSURE_ICP)
                       << IBPSymbol::convert(IBP_PRESSURE_AUXP1)
                       << IBPSymbol::convert(IBP_PRESSURE_AUXP2));
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_1);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, 0, 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_1, comboBox);

    // 通道一标尺
    label = new QLabel(trs("Ruler"));
    gLayout->addWidget(label, 1, 0);
    comboBox = new ComboBox();
    for (int i = 0; i < ibpParam.ibpScaleList.count(); i++)
    {
        if (i == 0)
        {
            comboBox->addItem(trs("Auto"));
        }
        else if (i == ibpParam.ibpScaleList.count() - 1)
        {
            comboBox->addItem(trs("Manual"));
        }
        else
        {
            comboBox->addItem(QString::number(ibpParam.ibpScaleList.at(i).low) + "~" +
                              QString::number(ibpParam.ibpScaleList.at(i).high));
        }
    }
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_RULER_1);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, 1, 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_RULER_1, comboBox);

    // 通道1上标尺
    label = new QLabel(trs("UpperScale"));
    gLayout->addWidget(label, 2, 0);
    spinBox = new SpinBox();
    spinBox->setStep(5);
    spinBox->setArrow(false);
    itemID = IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1;
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    gLayout->addWidget(spinBox, 2, 1);
    d_ptr->spinBoxs.insert(IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1, spinBox);

    // 通道1下标尺
    label = new QLabel(trs("LowerScale"));
    gLayout->addWidget(label, 3, 0);
    spinBox = new SpinBox();
    spinBox->setStep(5);
    spinBox->setArrow(false);
    itemID = IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1;
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    gLayout->addWidget(spinBox, 3, 1);
    d_ptr->spinBoxs.insert(IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1, spinBox);

    // 通道2 QGroupBoxd
    gLayout = new QGridLayout();
    d_ptr->twoGBox = new QGroupBox("IBP2");
    d_ptr->twoGBox->setLayout(gLayout);
    layout->addWidget(d_ptr->twoGBox);

    // 标名选择2
    label = new QLabel(trs("PressureEntitle"));
    gLayout->addWidget(label, 0, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << IBPSymbol::convert(IBP_PRESSURE_ART)
                       << IBPSymbol::convert(IBP_PRESSURE_PA)
                       << IBPSymbol::convert(IBP_PRESSURE_CVP)
                       << IBPSymbol::convert(IBP_PRESSURE_LAP)
                       << IBPSymbol::convert(IBP_PRESSURE_RAP)
                       << IBPSymbol::convert(IBP_PRESSURE_ICP)
                       << IBPSymbol::convert(IBP_PRESSURE_AUXP1)
                       << IBPSymbol::convert(IBP_PRESSURE_AUXP2));
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, 0, 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2, comboBox);

    // 通道二标尺
    label = new QLabel(trs("Ruler"));
    gLayout->addWidget(label, 1, 0);
    comboBox = new ComboBox();
    for (int i = 0; i < ibpParam.ibpScaleList.count(); i++)
    {
        if (i == 0)
        {
            comboBox->addItem(trs("Auto"));
        }
        else if (i == ibpParam.ibpScaleList.count() - 1)
        {
            comboBox->addItem(trs("Manual"));
        }
        else
        {
            comboBox->addItem(QString::number(ibpParam.ibpScaleList.at(i).low) + "~" +
                              QString::number(ibpParam.ibpScaleList.at(i).high));
        }
    }
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_RULER_2);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, 1, 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_RULER_2, comboBox);

    // 通道2上标尺
    label = new QLabel(trs("UpperScale"));
    gLayout->addWidget(label, 2, 0);
    spinBox = new SpinBox();
    spinBox->setStep(5);
    spinBox->setArrow(false);
    itemID = IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2;
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    gLayout->addWidget(spinBox, 2, 1);
    d_ptr->spinBoxs.insert(IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2, spinBox);

    // 通道2下标尺
    label = new QLabel(trs("LowerScale"));
    gLayout->addWidget(label, 3, 0);
    spinBox = new SpinBox();
    spinBox->setStep(5);
    spinBox->setArrow(false);
    itemID = IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2;
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    gLayout->addWidget(spinBox, 3, 1);
    d_ptr->spinBoxs.insert(IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2, spinBox);

    // 波形速度
    gLayout = new QGridLayout();
    label = new QLabel(trs("IBPSweepSpeed"));
    gLayout->addWidget(label, 0, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_62_5)
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_125)
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_250)
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_500));
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, 0, 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED, comboBox);

    // 滤波模式
    label = new QLabel(trs("FilterMode"));
    gLayout->addWidget(label, 1, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << IBPSymbol::convert(IBP_FILTER_MODE_0)
                       << IBPSymbol::convert(IBP_FILTER_MODE_1));
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_FILTER_MODE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, 1, 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_FILTER_MODE, comboBox);

    // 灵敏度
    label = new QLabel(trs("Sensitivity"));
    gLayout->addWidget(label, 2, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_HIGH))
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_MID))
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_LOW)));
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_SENSITIVITY);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, 2, 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_SENSITIVITY, comboBox);

    // 校零
    label = new QLabel(trs("IBPZeroCalib"));
    gLayout->addWidget(label, 3, 0);
    button = new Button(trs("IBPZeroStart"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_CALIB_ZERO);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    gLayout->addWidget(button, 3, 1);
    d_ptr->buttons.insert(IBPMenuContentPrivate::ITEM_CBO_CALIB_ZERO, button);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    gLayout->addWidget(btn, 5, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    gLayout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count() + 1, 1);

    layout->addLayout(gLayout);
}

void IBPMenuContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->autoTimerId == ev->timerId())
    {
        if (!this->isVisible())
        {
            killTimer(d_ptr->autoTimerId);
            d_ptr->autoTimerId = -1;
        }
        else
        {
            IBPScaleInfo info;
            if (d_ptr->rulerLimit1 == IBP_RULER_LIMIT_AOTU)
            {
                info = ibpParam.getScaleInfo(IBP_INPUT_1);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1]->setValue(info.low);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1]->setValue(info.high);
            }

            if (d_ptr->rulerLimit1 == IBP_RULER_LIMIT_AOTU)
            {
                info = ibpParam.getScaleInfo(IBP_INPUT_2);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2]->setValue(info.low);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2]->setValue(info.high);
            }
        }
    }
    else if (d_ptr->zeroTimerId == ev->timerId())
    {
        if (ibpParam.getIBPZeroResult() || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            d_ptr->buttons[IBPMenuContentPrivate::ITEM_CBO_CALIB_ZERO]->setEnabled(true);
            d_ptr->buttons[IBPMenuContentPrivate::ITEM_CBO_CALIB_ZERO]->setText(trs("IBPZeroStart"));
            killTimer(d_ptr->zeroTimerId);
            d_ptr->zeroTimerId = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
}

void IBPMenuContent::onSpinBoxValueChanged(int value, int scale)
{
    SpinBox *box = qobject_cast<SpinBox *>(sender());
    if (box)
    {
        IBPMenuContentPrivate::MenuItem item
            = (IBPMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1:
        {
            int low = d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1]->getValue();
            ibpParam.setRulerLimit(low, value * scale, IBP_INPUT_1);
            break;
        }
        case IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1:
        {
            int high = d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1]->getValue();
            ibpParam.setRulerLimit(value * scale, high, IBP_INPUT_1);
            break;
        }
        case IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2:
        {
            int low = d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2]->getValue();
            ibpParam.setRulerLimit(low, value * scale, IBP_INPUT_2);
            break;
        }
        case IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2:
        {
            int high = d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2]->getValue();
            ibpParam.setRulerLimit(value * scale, high, IBP_INPUT_2);
            break;
        }
        default:
            break;
        }
    }
}

void IBPMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        IBPMenuContentPrivate::MenuItem item
            = (IBPMenuContentPrivate::MenuItem) box->property("Item").toInt();
        switch (item)
        {
        case IBPMenuContentPrivate::ITEM_CBO_ENTITLE_1:
        {
            if (index == static_cast<int>(d_ptr->ibp2))
            {
                box = d_ptr->combos.value(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2);
                d_ptr->ibp2 = d_ptr->ibp1;
                box->blockSignals(true);
                box->setCurrentIndex(static_cast<int>(d_ptr->ibp2));
                ibpParam.setEntitle(d_ptr->ibp2, IBP_INPUT_2);
                box->blockSignals(false);
            }
            d_ptr->ibp1 = static_cast<IBPPressureName>(index);
            ibpParam.setEntitle(d_ptr->ibp1, IBP_INPUT_1);
            ComboBox *cbo = d_ptr->combos[IBPMenuContentPrivate::ITEM_CBO_RULER_1];
            cbo->setCurrentIndex(ibpParam.getRulerLimit(d_ptr->ibp1));
            break;
        }
        case IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2:
        {
            if (index == static_cast<int>(d_ptr->ibp1))
            {
                box = d_ptr->combos.value(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_1);
                d_ptr->ibp1 = d_ptr->ibp2;
                box->blockSignals(true);
                box->setCurrentIndex(static_cast<int>(d_ptr->ibp1));
                ibpParam.setEntitle(d_ptr->ibp1, IBP_INPUT_1);
                box->blockSignals(false);
            }
            d_ptr->ibp2 = static_cast<IBPPressureName>(index);
            ibpParam.setEntitle(d_ptr->ibp2, IBP_INPUT_2);
            ComboBox *cbo = d_ptr->combos[IBPMenuContentPrivate::ITEM_CBO_RULER_2];
            cbo->setCurrentIndex(ibpParam.getRulerLimit(d_ptr->ibp2));
            break;
        }
        case IBPMenuContentPrivate::ITEM_CBO_RULER_1:
        {
            d_ptr->rulerLimit1 = static_cast<IBPRulerLimit>(index);
            ibpParam.setRulerLimit(static_cast<IBPRulerLimit>(index), IBP_INPUT_1);
            if (index == IBP_RULER_LIMIT_MANUAL)
            {
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1]->setEnabled(true);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1]->setEnabled(true);
            }
            else
            {
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1]->setEnabled(false);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1]->setEnabled(false);
            }
            if (index == IBP_RULER_LIMIT_AOTU)
            {
                if (d_ptr->autoTimerId == -1)
                {
                    d_ptr->autoTimerId = startTimer(AUTO_SCALE_UPDATE_TIME);
                }
            }
            else
            {
                if (d_ptr->rulerLimit2 != IBP_RULER_LIMIT_AOTU && d_ptr->autoTimerId != -1)
                {
                    killTimer(d_ptr->autoTimerId);
                    d_ptr->autoTimerId = -1;
                }
            }
            IBPScaleInfo scale = ibpParam.getScaleInfo(IBP_INPUT_1);
            d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1]->setValue(scale.high);
            d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1]->setValue(scale.low);
            break;
        }
        case IBPMenuContentPrivate::ITEM_CBO_RULER_2:
        {
            d_ptr->rulerLimit2 = (IBPRulerLimit)index;
            ibpParam.setRulerLimit(static_cast<IBPRulerLimit>(index), IBP_INPUT_2);
            if (index == IBP_RULER_LIMIT_MANUAL)
            {
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2]->setEnabled(true);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2]->setEnabled(true);
            }
            else
            {
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2]->setEnabled(false);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2]->setEnabled(false);
            }
            if (index == IBP_RULER_LIMIT_AOTU)
            {
                if (d_ptr->autoTimerId == -1)
                {
                    d_ptr->autoTimerId = startTimer(AUTO_SCALE_UPDATE_TIME);
                }
            }
            else
            {
                if (d_ptr->rulerLimit1 != IBP_RULER_LIMIT_AOTU && d_ptr->autoTimerId != -1)
                {
                    killTimer(d_ptr->autoTimerId);
                    d_ptr->autoTimerId = -1;
                }
            }
            IBPScaleInfo scale = ibpParam.getScaleInfo(IBP_INPUT_2);
            d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2]->setValue(scale.high);
            d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2]->setValue(scale.low);
            break;
        }
        case IBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
            ibpParam.setSweepSpeed((IBPSweepSpeed)index);
            break;
        case IBPMenuContentPrivate::ITEM_CBO_FILTER_MODE:
            ibpParam.setFilter((IBPFilterMode)index);
            break;
        case IBPMenuContentPrivate::ITEM_CBO_SENSITIVITY:
            ibpParam.setSensitivity((IBPSensitivity)index);
            break;
        default:
            break;
        }

        if (item ==  IBPMenuContentPrivate::ITEM_CBO_ENTITLE_1
                || item ==  IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2)
        {
            d_ptr->updatePrintWaveIds();
        }
    }
}

void IBPMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        IBPMenuContentPrivate::MenuItem item
            = (IBPMenuContentPrivate::MenuItem) button->property("Item").toInt();
        switch (item)
        {
        case IBPMenuContentPrivate::ITEM_CBO_CALIB_ZERO:
            ibpParam.zeroCalibration(IBP_INPUT_1);
            ibpParam.zeroCalibration(IBP_INPUT_2);
            button->setText(trs("ZeroInProgress"));
            button->setEnabled(false);
            d_ptr->zeroTimerId = startTimer(ZERO_INTERVAL_TIME);
            break;
        default:
            break;
        }
    }
}

void IBPMenuContent::onAlarmBtnReleased()
{
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_ART_SYS, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}
