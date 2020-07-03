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
#include <QLabel>
#include "Framework/UI/Button.h"
#include "Framework/UI/SpinBox.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QGridLayout>
#include "IBPSymbol.h"
#include "IConfig.h"
#include "IBPParam.h"
#include "MessageBox.h"
#include "KeyInputPanel.h"
#include "MainMenuWindow.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimerEvent>
#include "AlarmLimitWindow.h"
#include "WindowManager.h"
#include "IBPZeroWindow.h"
#include <QDebug>

#define AUTO_SCALE_UPDATE_TIME          (2 * 1000)
#define ZERO_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER             (5000 / ZERO_INTERVAL_TIME)
#define PRINT_WAVE_NUM                  (3)
#define RULER_STEP                      (5)
#define IBP_RULER_MIN_VALUE             (-50)
#define IBP_RULER_MAX_VALUE             (350)

struct ibpGroupData
{
    IBPLabel entitle;            // ibp entitle
    IBPRulerLimit rulerLimit;    // ibp ruler limit

    SpinBox *upScaleSbo;         // up scale spb
    SpinBox *downScaleSbo;       // down scale spb

    QStringList upScaleStrs;     // up scale string lists
    QStringList downScaleStrs;   // down scale string lists
};

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
    };

    explicit IBPMenuContentPrivate(IBPMenuContent *const q_ptr) :
        q_ptr(q_ptr),
        curIBPUnit(UNIT_NONE),
        oneGBox(NULL), twoGBox(NULL), zeroBtn(NULL),
        autoTimerId(-1)
    {
        for (int i = 0; i < IBP_CHN_NR; ++i)
        {
            groupData[i].entitle = IBP_LABEL_NR;
            groupData[i].rulerLimit = IBP_RULER_LIMIT_AUTO;
            groupData[i].upScaleSbo = NULL;
            groupData[i].downScaleSbo = NULL;
        }
    }

    // load settings
    void loadOptions();

    /**
     * @brief updateRulerScaleInfo  update ibp1 ibp2 ruler scale info
     */
    void updateRulerScaleInfo();

    /**
     * @brief updatePrintWaveIds
     */
    void updatePrintWaveIds();

    /**
     * @brief handleRulerCboChange  handle ruler combo box index change
     * @param chn   IBP Channel id
     */
    void handleRulerCboChange(IBPChannel chn, int cboIndex);

    IBPMenuContent *const q_ptr;
    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, SpinBox *> spinBoxs;
    UnitType curIBPUnit;
    QGroupBox *oneGBox;
    QGroupBox *twoGBox;
    Button *zeroBtn;
    int autoTimerId;
    ibpGroupData groupData[IBP_CHN_NR];
    QList<int> waveIdList;
};

void IBPMenuContentPrivate::loadOptions()
{
    IBPLabel ibp1 = ibpParam.getEntitle(IBP_CHN_1);
    IBPLabel ibp2 = ibpParam.getEntitle(IBP_CHN_2);
    groupData[IBP_CHN_1].entitle = ibp1;
    groupData[IBP_CHN_2].entitle = ibp2;

    // 获取当前ibp的波形id
    waveIdList.clear();
    waveIdList.append(ibpParam.getWaveformID(ibp1));
    waveIdList.append(ibpParam.getWaveformID(ibp2));

    combos[ITEM_CBO_ENTITLE_1]->setCurrentIndex(ibp1);
    combos[ITEM_CBO_ENTITLE_2]->setCurrentIndex(ibp2);
    groupData[IBP_CHN_1].rulerLimit = ibpParam.getRulerLimit(IBP_CHN_1);
    groupData[IBP_CHN_2].rulerLimit = ibpParam.getRulerLimit(IBP_CHN_2);
    if (groupData[IBP_CHN_1].rulerLimit == IBP_RULER_LIMIT_MANUAL)
    {
        spinBoxs[ITEM_SBO_UP_SCALE_1]->setEnabled(true);
        spinBoxs[ITEM_SBO_DOWN_SCALE_1]->setEnabled(true);
    }
    else
    {
        spinBoxs[ITEM_SBO_UP_SCALE_1]->setEnabled(false);
        spinBoxs[ITEM_SBO_DOWN_SCALE_1]->setEnabled(false);
    }

    if (groupData[IBP_CHN_2].rulerLimit == IBP_RULER_LIMIT_MANUAL)
    {
        spinBoxs[ITEM_SBO_UP_SCALE_2]->setEnabled(true);
        spinBoxs[ITEM_SBO_DOWN_SCALE_2]->setEnabled(true);
    }
    else
    {
        spinBoxs[ITEM_SBO_UP_SCALE_2]->setEnabled(false);
        spinBoxs[ITEM_SBO_DOWN_SCALE_2]->setEnabled(false);
    }

    if (groupData[IBP_CHN_1].rulerLimit == IBP_RULER_LIMIT_AUTO ||
            groupData[IBP_CHN_2].rulerLimit == IBP_RULER_LIMIT_AUTO)
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

    // update ibp1 ibp2 ruler scale info
    updateRulerScaleInfo();

    combos[ITEM_CBO_RULER_1]->setCurrentIndex(groupData[IBP_CHN_1].rulerLimit);
    combos[ITEM_CBO_RULER_2]->setCurrentIndex(groupData[IBP_CHN_2].rulerLimit);
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(ibpParam.getSweepSpeed());
    if (ibpParam.getMoudleType() == IBP_MODULE_WITLEAF)
    {
        combos[ITEM_CBO_FILTER_MODE]->setCurrentIndex(ibpParam.getFilter());
        combos[ITEM_CBO_SENSITIVITY]->setCurrentIndex(ibpParam.getSensitivity());
    }

    if (zeroBtn)
    {
        zeroBtn->setEnabled(ibpParam.isConnected());
    }
}

void IBPMenuContentPrivate::updateRulerScaleInfo()
{
    if (curIBPUnit == ibpParam.getUnit())
    {
        return;
    }
    curIBPUnit = ibpParam.getUnit();

    combos[ITEM_CBO_RULER_1]->blockSignals(true);
    combos[ITEM_CBO_RULER_2]->blockSignals(true);

    // clear ibp ruler info
    combos[ITEM_CBO_RULER_1]->clear();
    combos[ITEM_CBO_RULER_2]->clear();

    for (int i = 0; i < ibpParam.ibpScaleList.count(); i++)
    {
        if (i == IBP_RULER_LIMIT_AUTO)
        {
            combos[ITEM_CBO_RULER_1]->addItem(trs("Auto"));
            combos[ITEM_CBO_RULER_2]->addItem(trs("Auto"));
        }
        else if (i == IBP_RULER_LIMIT_MANUAL)
        {
            combos[ITEM_CBO_RULER_1]->addItem(trs("Manual"));
            combos[ITEM_CBO_RULER_2]->addItem(trs("Manual"));
        }
        else
        {
            // get ruler info
            QString lowRulerText = QString::number(ibpParam.ibpScaleList.at(i).low);
            QString highRulerText = QString::number(ibpParam.ibpScaleList.at(i).high);
            if (curIBPUnit != UNIT_MMHG)
            {
                lowRulerText = Unit::convert(curIBPUnit, UNIT_MMHG, ibpParam.ibpScaleList.at(i).low);
                highRulerText = Unit::convert(curIBPUnit, UNIT_MMHG, ibpParam.ibpScaleList.at(i).high);
            }
            combos[ITEM_CBO_RULER_1]->addItem(lowRulerText + "~" + highRulerText);
            combos[ITEM_CBO_RULER_2]->addItem(lowRulerText + "~" + highRulerText);
        }
    }

    combos[ITEM_CBO_RULER_1]->blockSignals(false);
    combos[ITEM_CBO_RULER_2]->blockSignals(false);
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
            IBPLabel ibpTitle = ibpParam.getEntitle(IBP_CHN_1);
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
            IBPLabel ibpTitle = ibpParam.getEntitle(IBP_CHN_2);
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

void IBPMenuContentPrivate::handleRulerCboChange(IBPChannel chn, int cboIndex)
{
    if (chn >= IBP_CHN_NR)
    {
        qWarning() << Q_FUNC_INFO << "IBP channel Id is error!";
        return;
    }

    SpinBox *upScale = groupData[chn].upScaleSbo;     // up scale
    SpinBox *downScale = groupData[chn].downScaleSbo;   // down scale
    if (upScale == NULL || downScale == NULL)
    {
        return;
    }

    groupData[chn].rulerLimit = static_cast<IBPRulerLimit> (cboIndex);
    ibpParam.setRulerLimit(groupData[chn].rulerLimit, chn);
    IBPScaleInfo scale = ibpParam.getScaleInfo(chn);
    upScale->setEnabled(false);
    downScale->setEnabled(false);
    // setect manual ruler limit, get the scale info corresponding to IBP label
    if (groupData[chn].rulerLimit == IBP_RULER_LIMIT_MANUAL)
    {
        scale = ibpParam.getIBPScale(groupData[chn].entitle);
        upScale->setEnabled(true);
        downScale->setEnabled(true);
    }

    // set upper sacle info
    upScale->blockSignals(true);
    UnitType defUnit = paramInfo->getUnitOfSubParam(SUB_PARAM_ART_SYS);
    int start = scale.low + RULER_STEP;
    int end = IBP_RULER_MAX_VALUE;
    groupData[chn].upScaleStrs.clear();
    int curValue = 0;
    for (int i = start; i <= end; i += RULER_STEP)
    {
        if (scale.high == i)
        {
            curValue = (i - start) / RULER_STEP;
        }
        if (curIBPUnit == defUnit)
        {
            // default unit mmhg
            groupData[chn].upScaleStrs.append(QString::number(i));
        }
        else
        {
            groupData[chn].upScaleStrs.append(Unit::convert(curIBPUnit, defUnit, i));
        }
    }
    upScale->setStringList(groupData[chn].upScaleStrs);
    upScale->setValue(curValue);
    upScale->blockSignals(false);

    // set down sacle info
    downScale->blockSignals(true);
    start = IBP_RULER_MIN_VALUE;
    end = scale.high - RULER_STEP;
    groupData[chn].downScaleStrs.clear();
    curValue = 0;
    for (int i = start; i <= end; i += RULER_STEP)
    {
        if (scale.low == i)
        {
            curValue = (i - start) / RULER_STEP;
        }
        if (curIBPUnit == defUnit)
        {
            // default unit mmhg
            groupData[chn].downScaleStrs.append(QString::number(i));
        }
        else
        {
            groupData[chn].downScaleStrs.append(Unit::convert(curIBPUnit, defUnit, i));
        }
    }
    downScale->setStringList(groupData[chn].downScaleStrs);
    downScale->setValue(curValue);
    downScale->blockSignals(false);

    // start auto scale time
    bool startAutoTime = false;
    for (int i = 0; i < IBP_CHN_NR; ++i)
    {
        if (groupData[i].rulerLimit == IBP_RULER_LIMIT_AUTO)
        {
            startAutoTime = true;
            break;
        }
    }
    if (startAutoTime)
    {
        autoTimerId = q_ptr->startTimer(AUTO_SCALE_UPDATE_TIME);
    }
    else if (autoTimerId != -1)
    {
        q_ptr->killTimer(autoTimerId);
        autoTimerId = -1;
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
                       << IBPSymbol::convert(IBP_LABEL_ART)
                       << IBPSymbol::convert(IBP_LABEL_PA)
                       << IBPSymbol::convert(IBP_LABEL_CVP)
                       << IBPSymbol::convert(IBP_LABEL_LAP)
                       << IBPSymbol::convert(IBP_LABEL_RAP)
                       << IBPSymbol::convert(IBP_LABEL_ICP)
                       << IBPSymbol::convert(IBP_LABEL_AUXP1)
                       << IBPSymbol::convert(IBP_LABEL_AUXP2));
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
    spinBox->setArrow(false);
    itemID = IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1;
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    gLayout->addWidget(spinBox, 2, 1);
    d_ptr->spinBoxs.insert(IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1, spinBox);
    d_ptr->groupData[IBP_CHN_1].upScaleSbo = spinBox;

    // 通道1下标尺
    label = new QLabel(trs("LowerScale"));
    gLayout->addWidget(label, 3, 0);
    spinBox = new SpinBox();
    spinBox->setArrow(false);
    itemID = IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1;
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    gLayout->addWidget(spinBox, 3, 1);
    d_ptr->spinBoxs.insert(IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1, spinBox);
    d_ptr->groupData[IBP_CHN_1].downScaleSbo = spinBox;

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
                       << IBPSymbol::convert(IBP_LABEL_ART)
                       << IBPSymbol::convert(IBP_LABEL_PA)
                       << IBPSymbol::convert(IBP_LABEL_CVP)
                       << IBPSymbol::convert(IBP_LABEL_LAP)
                       << IBPSymbol::convert(IBP_LABEL_RAP)
                       << IBPSymbol::convert(IBP_LABEL_ICP)
                       << IBPSymbol::convert(IBP_LABEL_AUXP1)
                       << IBPSymbol::convert(IBP_LABEL_AUXP2));
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
    spinBox->setArrow(false);
    itemID = IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2;
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    gLayout->addWidget(spinBox, 2, 1);
    d_ptr->spinBoxs.insert(IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2, spinBox);
    d_ptr->groupData[IBP_CHN_2].upScaleSbo = spinBox;

    // 通道2下标尺
    label = new QLabel(trs("LowerScale"));
    gLayout->addWidget(label, 3, 0);
    spinBox = new SpinBox();
    spinBox->setArrow(false);
    itemID = IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2;
    spinBox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    gLayout->addWidget(spinBox, 3, 1);
    d_ptr->spinBoxs.insert(IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2, spinBox);
    d_ptr->groupData[IBP_CHN_2].downScaleSbo = spinBox;

    // 波形速度
    int row = 0;
    gLayout = new QGridLayout();
    label = new QLabel(trs("IBPSweepSpeed"));
    gLayout->addWidget(label, row, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_62_5)
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_125)
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_250));
//                       << IBPSymbol::convert(IBP_SWEEP_SPEED_500)
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, row, 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED, comboBox);

    if (ibpParam.getMoudleType() == IBP_MODULE_WITLEAF)
    {
        // 滤波模式
        row++;
        label = new QLabel(trs("FilterMode"));
        gLayout->addWidget(label, row, 0);
        comboBox = new ComboBox();
        comboBox->addItems(QStringList()
                           << IBPSymbol::convert(IBP_FILTER_MODE_0)
                           << IBPSymbol::convert(IBP_FILTER_MODE_1));
        itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_FILTER_MODE);
        comboBox->setProperty("Item",
                              qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        gLayout->addWidget(comboBox, row, 1);
        d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_FILTER_MODE, comboBox);

        // 灵敏度
        row++;
        label = new QLabel(trs("Sensitivity"));
        gLayout->addWidget(label, row, 0);
        comboBox = new ComboBox();
        comboBox->addItems(QStringList()
                           << trs(IBPSymbol::convert(IBP_SENSITIVITY_HIGH))
                           << trs(IBPSymbol::convert(IBP_SENSITIVITY_MID))
                           << trs(IBPSymbol::convert(IBP_SENSITIVITY_LOW)));
        itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_SENSITIVITY);
        comboBox->setProperty("Item",
                              qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        gLayout->addWidget(comboBox, row, 1);
        d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_SENSITIVITY, comboBox);
    }

    row++;
    // zero
    Button *btn = new Button(trs("Zero"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->zeroBtn = btn;
    gLayout->addWidget(btn, row, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onZeroButtonRelease()));

    // 添加报警设置链接
    row++;
    btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    gLayout->addWidget(btn, row, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    gLayout->setRowStretch(d_ptr->combos.count() + 1, 1);

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
            if (d_ptr->groupData[IBP_CHN_1].rulerLimit == IBP_RULER_LIMIT_AUTO)
            {
                info = ibpParam.getScaleInfo(IBP_CHN_1);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1]->setValue(info.low);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1]->setValue(info.high);
            }

            if (d_ptr->groupData[IBP_CHN_2].rulerLimit  == IBP_RULER_LIMIT_AUTO)
            {
                info = ibpParam.getScaleInfo(IBP_CHN_2);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2]->setValue(info.low);
                d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2]->setValue(info.high);
            }
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
            ibpParam.setRulerLimit(low, value * scale, IBP_CHN_1);

            // change down scale value range
            d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1]->setRange(IBP_RULER_MIN_VALUE,
                                                                                    value * scale - RULER_STEP);
            break;
        }
        case IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1:
        {
            int high = d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1]->getValue();
            ibpParam.setRulerLimit(value * scale, high, IBP_CHN_1);

            // change up scale value range
            d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1]->setRange(value * scale + RULER_STEP,
                                                                                  IBP_RULER_MAX_VALUE);
            break;
        }
        case IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2:
        {
            int low = d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2]->getValue();
            ibpParam.setRulerLimit(low, value * scale, IBP_CHN_2);

            // change down scale value range
            d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2]->setRange(IBP_RULER_MIN_VALUE,
                                                                                    value * scale - RULER_STEP);
            break;
        }
        case IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2:
        {
            int high = d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2]->getValue();
            ibpParam.setRulerLimit(value * scale, high, IBP_CHN_2);

            // change up scale value range
            d_ptr->spinBoxs[IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2]->setRange(value * scale + RULER_STEP,
                                                                                  IBP_RULER_MAX_VALUE);
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
            if (index == d_ptr->groupData[IBP_CHN_2].entitle)
            {
                box = d_ptr->combos.value(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2);
                d_ptr->groupData[IBP_CHN_2].entitle = d_ptr->groupData[IBP_CHN_1].entitle;
                box->blockSignals(true);
                box->setCurrentIndex(d_ptr->groupData[IBP_CHN_2].entitle);
                ibpParam.setEntitle(d_ptr->groupData[IBP_CHN_2].entitle, IBP_CHN_2);
                box->blockSignals(false);
            }
            d_ptr->groupData[IBP_CHN_1].entitle = static_cast<IBPLabel>(index);
            ibpParam.setEntitle(d_ptr->groupData[IBP_CHN_1].entitle, IBP_CHN_1);
            ComboBox *cbo = d_ptr->combos[IBPMenuContentPrivate::ITEM_CBO_RULER_1];
            cbo->setCurrentIndex(ibpParam.getRulerLimit(d_ptr->groupData[IBP_CHN_1].entitle));
            break;
        }
        case IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2:
        {
            if (index == d_ptr->groupData[IBP_CHN_1].entitle)
            {
                box = d_ptr->combos.value(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_1);
                d_ptr->groupData[IBP_CHN_1].entitle = d_ptr->groupData[IBP_CHN_2].entitle;
                box->blockSignals(true);
                box->setCurrentIndex(d_ptr->groupData[IBP_CHN_1].entitle);
                ibpParam.setEntitle(d_ptr->groupData[IBP_CHN_1].entitle, IBP_CHN_1);
                box->blockSignals(false);
            }
            d_ptr->groupData[IBP_CHN_2].entitle = static_cast<IBPLabel>(index);
            ibpParam.setEntitle(d_ptr->groupData[IBP_CHN_2].entitle, IBP_CHN_2);
            ComboBox *cbo = d_ptr->combos[IBPMenuContentPrivate::ITEM_CBO_RULER_2];
            cbo->setCurrentIndex(ibpParam.getRulerLimit(d_ptr->groupData[IBP_CHN_2].entitle));
            break;
        }
        case IBPMenuContentPrivate::ITEM_CBO_RULER_1:
        {
            d_ptr->handleRulerCboChange(IBP_CHN_1, index);
            break;
        }
        case IBPMenuContentPrivate::ITEM_CBO_RULER_2:
        {
            d_ptr->handleRulerCboChange(IBP_CHN_2, index);
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

void IBPMenuContent::onZeroButtonRelease()
{
    IBPZeroWindow w;
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal|WindowManager::ShowBehaviorCloseOthers);
}

void IBPMenuContent::onAlarmBtnReleased()
{
    SubParamID paramId = ibpParam.getSubParamID(d_ptr->groupData[IBP_CHN_1].entitle);
    QString subParamName = paramInfo.getSubParamName(paramId, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}
