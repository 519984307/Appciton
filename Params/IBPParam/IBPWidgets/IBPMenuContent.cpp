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

struct IBPGroupData
{
    IBPLabel entitle;            // ibp entitle
    IBPRulerLimit rulerLimit;    // ibp ruler limit

    ComboBox *entitleCbo;        // ibp entitle combo box
    ComboBox *rulerCbo;          // ruler limit combo box

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
            groupData[i].entitleCbo = NULL;
            groupData[i].rulerCbo = NULL;
            groupData[i].upScaleSbo = NULL;
            groupData[i].downScaleSbo = NULL;
        }
    }

    // load settings
    void loadOptions();

    /**
     * @brief handleEntitleCboChange  handle entitle combo box change
     * @param chn  IBP Channel
     * @param ibpEntitle  ibp entitle
     */
    void handleEntitleCboChange(IBPChannel chn, IBPLabel ibpEntitle);

    /**
     * @brief updateRulerCboScaleInfo  update ibp1 ibp2 ruler comboBox scale info
     */
    void updateRulerCboScaleInfo();

    /**
     * @brief updatePrintWaveIds
     */
    void updatePrintWaveIds();

    /**
     * @brief updateRulerSboScaleInfo  update ruler spin box scale info
     * @param chn   IBP Channel id
     * @param rulerLimit  ruler limit
     */
    void updateRulerSboScaleInfo(IBPChannel chn, IBPRulerLimit rulerLimit);

    /**
     * @brief handleScaleSboChange  handle scale spin box change
     * @param chn     IBP Channel id
     * @param sboIndex  spin box index
     * @param upScaleSbo true: up scale spin box change; false down scale spin box change
     */
    void handleScaleSboChange(IBPChannel chn, int value, bool upScaleSbo);

    /**
     * @brief handleAutoRuler  handle auto ruler
     */
    void handleAutoRuler();

    IBPMenuContent *const q_ptr;
    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, SpinBox *> spinBoxs;
    UnitType curIBPUnit;
    QGroupBox *oneGBox;
    QGroupBox *twoGBox;
    Button *zeroBtn;
    int autoTimerId;
    IBPGroupData groupData[IBP_CHN_NR];
    QList<int> waveIdList;
};

void IBPMenuContentPrivate::loadOptions()
{
    waveIdList.clear();
    for (int i = IBP_CHN_1; i < IBP_CHN_NR; ++i)
    {
        IBPChannel chn = static_cast<IBPChannel> (i);
        IBPLabel entitle = ibpParam.getEntitle(chn);
        groupData[chn].entitle = entitle;
        waveIdList.append(ibpParam.getWaveformID(entitle));
        if (groupData[chn].entitleCbo)
        {
            groupData[chn].entitleCbo->blockSignals(true);
            groupData[chn].entitleCbo->setCurrentIndex(entitle);
            groupData[chn].entitleCbo->blockSignals(false);
        }
        groupData[chn].rulerLimit = ibpParam.getRulerLimit(chn);
    }


    // update ibp1 ibp2 ruler comboBox scale info
    updateRulerCboScaleInfo();

    // update ibp ruler spin box scale info
    updateRulerSboScaleInfo(IBP_CHN_1, groupData[IBP_CHN_1].rulerLimit);
    updateRulerSboScaleInfo(IBP_CHN_2, groupData[IBP_CHN_2].rulerLimit);

    combos[ITEM_CBO_SWEEP_SPEED]->blockSignals(true);
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(ibpParam.getSweepSpeed());
    combos[ITEM_CBO_SWEEP_SPEED]->blockSignals(false);
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

void IBPMenuContentPrivate::handleEntitleCboChange(IBPChannel chn, IBPLabel ibpEntitle)
{
    if (chn >= IBP_CHN_NR)
    {
        qWarning() << Q_FUNC_INFO << "IBP channel Id is error!";
        return;
    }

    IBPLabel oldIBPEntitle = groupData[chn].entitle;
    /*
     * Changed pressure entitle is consistent with other channel,
     * then update the pressure entitle and ruler of other channel.
     */
    for (int i = 0; i < IBP_CHN_NR; ++i)
    {
        if (ibpEntitle == groupData[i].entitle && chn != i)
        {
            if (groupData[i].entitleCbo)
            {
                groupData[i].entitle = oldIBPEntitle;
                groupData[i].entitleCbo->blockSignals(true);
                groupData[i].entitleCbo->setCurrentIndex(oldIBPEntitle);
                groupData[i].entitleCbo->blockSignals(false);
            }
            if (groupData[i].rulerCbo)
            {
                groupData[i].rulerLimit = ibpParam.getRulerLimit(oldIBPEntitle);
                groupData[i].rulerCbo->setCurrentIndex(groupData[i].rulerLimit);
            }
            ibpParam.setEntitle(oldIBPEntitle, static_cast<IBPChannel>(i));
            break;
        }
    }

    // update select pressure entitle and ruler of current channel
    groupData[chn].entitle = ibpEntitle;
    ibpParam.setEntitle(groupData[chn].entitle, chn);
    if (groupData[chn].rulerCbo)
    {
        groupData[chn].rulerLimit = ibpParam.getRulerLimit(groupData[chn].entitle);
        groupData[chn].rulerCbo->setCurrentIndex(groupData[chn].rulerLimit);
    }
}

void IBPMenuContentPrivate::updateRulerCboScaleInfo()
{
    if (curIBPUnit == ibpParam.getUnit())
    {
        return;
    }
    curIBPUnit = ibpParam.getUnit();

    for (int chn = IBP_CHN_1; chn < IBP_CHN_NR; ++chn)
    {
        ComboBox *comBox = groupData[chn].rulerCbo;
        if (comBox == NULL)
        {
            continue;
        }

        comBox->blockSignals(true);

        // clear ibp ruler info
        comBox->clear();

        for (int i = 0; i < ibpParam.ibpScaleList.count(); i++)
        {
            if (i == IBP_RULER_LIMIT_AUTO)
            {
                comBox->addItem(trs("Auto"));
            }
            else if (i == IBP_RULER_LIMIT_MANUAL)
            {
                comBox->addItem(trs("Manual"));
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
                comBox->addItem(lowRulerText + "~" + highRulerText);
            }
        }
        comBox->setCurrentIndex(groupData[chn].rulerLimit);
        comBox->blockSignals(false);
    }
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

void IBPMenuContentPrivate::updateRulerSboScaleInfo(IBPChannel chn, IBPRulerLimit rulerLimit)
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

    groupData[chn].rulerLimit = rulerLimit;
    ibpParam.setRulerLimit(groupData[chn].rulerLimit, chn);
    IBPScaleInfo scale = ibpParam.getScaleInfo(chn);
    upScale->setEnabled(false);
    downScale->setEnabled(false);
    // setect manual ruler limit, get the scale info corresponding to IBP label
    if (groupData[chn].rulerLimit == IBP_RULER_LIMIT_MANUAL)
    {
        upScale->setEnabled(true);
        downScale->setEnabled(true);
    }

    // set upper sacle info
    UnitType defUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_ART_SYS);
    int start = scale.low + RULER_STEP;
    int end = IBP_RULER_MAX_VALUE;
    groupData[chn].upScaleStrs.clear();
    QStringList stringLists;
    int curValue = 0;
    for (int i = start; i <= end; i += RULER_STEP)
    {
        if (scale.high == i)
        {
            curValue = (i - start) / RULER_STEP;
        }
        // default unit mmhg
        groupData[chn].upScaleStrs.append(QString::number(i));
        if (curIBPUnit != defUnit)
        {
            stringLists.append(Unit::convert(curIBPUnit, defUnit, i));
        }
    }
    upScale->blockSignals(true);
    if (curIBPUnit != defUnit)
    {
        upScale->setStringList(stringLists);
    }
    else
    {
        upScale->setStringList(groupData[chn].upScaleStrs);
    }
    upScale->setValue(curValue);
    upScale->blockSignals(false);

    // set down sacle info
    start = IBP_RULER_MIN_VALUE;
    end = scale.high - RULER_STEP;
    groupData[chn].downScaleStrs.clear();
    curValue = 0;
    stringLists.clear();
    for (int i = start; i <= end; i += RULER_STEP)
    {
        if (scale.low == i)
        {
            curValue = (i - start) / RULER_STEP;
        }
        // default unit mmhg
        groupData[chn].downScaleStrs.append(QString::number(i));
        if (curIBPUnit != defUnit)
        {
            stringLists.append(Unit::convert(curIBPUnit, defUnit, i));
        }
    }
    downScale->blockSignals(true);
    if (curIBPUnit != defUnit)
    {
        downScale->setStringList(stringLists);
    }
    else
    {
        downScale->setStringList(groupData[chn].downScaleStrs);
    }
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

void IBPMenuContentPrivate::handleScaleSboChange(IBPChannel chn, int value, bool upScaleSbo)
{
    if (chn >= IBP_CHN_NR)
    {
        qWarning() << Q_FUNC_INFO << "IBP channel Id is error!";
        return;
    }

    int lowScale = 0;
    int highScale = 0;
    int start = 0;
    int end = 0;
    int scaleValue;
    SpinBox *box;
    if (upScaleSbo)    // up scale spin box change, update down scale spin box range
    {
        lowScale = groupData[chn].downScaleStrs.at(groupData[chn].downScaleSbo->getValue()).toInt();
        highScale = groupData[chn].upScaleStrs.at(value).toInt();
        start = IBP_RULER_MIN_VALUE;
        end = highScale - RULER_STEP;
        scaleValue = lowScale;
        box = groupData[chn].downScaleSbo;
    }
    else    // down scale spin box change, update up scale spin box range
    {
        lowScale = groupData[chn].downScaleStrs.at(value).toInt();
        highScale = groupData[chn].upScaleStrs.at(groupData[chn].upScaleSbo->getValue()).toInt();
        start = lowScale + RULER_STEP;
        end = IBP_RULER_MAX_VALUE;
        scaleValue = highScale;
        box = groupData[chn].upScaleSbo;
    }

    UnitType defUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_ART_SYS);
    ibpParam.setRulerLimit(lowScale, highScale, chn);

    // add new scale info
    int curIndex = 0;
    QStringList defUnitScaleInfoLists;
    QStringList displayStrLists;
    for (int i = start; i <= end; i += RULER_STEP)
    {
        if (scaleValue == i)
        {
            curIndex = (i - start) / RULER_STEP;
        }

        defUnitScaleInfoLists.append(QString::number(i));
        if (curIBPUnit != defUnit)
        {
            displayStrLists.append(Unit::convert(curIBPUnit, defUnit, i));
        }
    }
    box->blockSignals(true);
    if (curIBPUnit != defUnit)
    {
        box->setStringList(displayStrLists);
    }
    else
    {
        box->setStringList(defUnitScaleInfoLists);
    }
    box->setValue(curIndex);
    box->blockSignals(false);

    if (upScaleSbo)    // up scale spin box change, update down scale info
    {
       groupData[chn].downScaleStrs = defUnitScaleInfoLists;
    }
    else    // down scale spin box change, update up scale spin info
    {
       groupData[chn].upScaleStrs = defUnitScaleInfoLists;
    }
}

void IBPMenuContentPrivate::handleAutoRuler()
{
    for (int chn = IBP_CHN_1; chn < IBP_CHN_NR; ++chn)
    {
        if (groupData[chn].rulerLimit != IBP_RULER_LIMIT_AUTO)
        {
            continue;
        }

        SpinBox *upScale = groupData[chn].upScaleSbo;     // up scale
        SpinBox *downScale = groupData[chn].downScaleSbo;   // down scale
        if (upScale == NULL || downScale == NULL)
        {
            continue;
        }

        IBPScaleInfo scale = ibpParam.getScaleInfo(static_cast<IBPChannel>(chn));
        // set upper sacle info
        UnitType defUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_ART_SYS);
        int start = scale.low + RULER_STEP;
        int end = IBP_RULER_MAX_VALUE;
        groupData[chn].upScaleStrs.clear();
        QStringList stringLists;
        int curValue = 0;
        for (int i = start; i <= end; i += RULER_STEP)
        {
            if (scale.high == i)
            {
                curValue = (i - start) / RULER_STEP;
            }
            // default unit mmhg
            groupData[chn].upScaleStrs.append(QString::number(i));
            if (curIBPUnit != defUnit)
            {
                stringLists.append(Unit::convert(curIBPUnit, defUnit, i));
            }
        }
        upScale->blockSignals(true);
        if (curIBPUnit != defUnit)
        {
            upScale->setStringList(stringLists);
        }
        else
        {
            upScale->setStringList(groupData[chn].upScaleStrs);
        }
        upScale->setValue(curValue);
        upScale->blockSignals(false);

        // set down sacle info
        start = IBP_RULER_MIN_VALUE;
        end = scale.high - RULER_STEP;
        groupData[chn].downScaleStrs.clear();
        curValue = 0;
        stringLists.clear();
        for (int i = start; i <= end; i += RULER_STEP)
        {
            if (scale.low == i)
            {
                curValue = (i - start) / RULER_STEP;
            }
            // default unit mmhg
            groupData[chn].downScaleStrs.append(QString::number(i));
            if (curIBPUnit != defUnit)
            {
                stringLists.append(Unit::convert(curIBPUnit, defUnit, i));
            }
        }
        downScale->blockSignals(true);
        if (curIBPUnit != defUnit)
        {
            downScale->setStringList(stringLists);
        }
        else
        {
            downScale->setStringList(groupData[chn].downScaleStrs);
        }
        downScale->setValue(curValue);
        downScale->blockSignals(false);
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
    d_ptr->groupData[IBP_CHN_1].entitleCbo = comboBox;

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
    d_ptr->groupData[IBP_CHN_1].rulerCbo = comboBox;

    // 通道1上标尺
    label = new QLabel(trs("UpperScale"));
    gLayout->addWidget(label, 2, 0);
    spinBox = new SpinBox();
    spinBox->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_STRING);
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
    spinBox->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_STRING);
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
    d_ptr->groupData[IBP_CHN_2].entitleCbo = comboBox;

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
    d_ptr->groupData[IBP_CHN_2].rulerCbo = comboBox;

    // 通道2上标尺
    label = new QLabel(trs("UpperScale"));
    gLayout->addWidget(label, 2, 0);
    spinBox = new SpinBox();
    spinBox->setArrow(false);
    spinBox->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_STRING);
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
    spinBox->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_STRING);
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
            d_ptr->handleAutoRuler();
        }
    }
}

void IBPMenuContent::onSpinBoxValueChanged(int value, int)
{
    SpinBox *box = qobject_cast<SpinBox *>(sender());
    if (box)
    {
        IBPMenuContentPrivate::MenuItem item
            = (IBPMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_1:
            d_ptr->handleScaleSboChange(IBP_CHN_1, value, true);
            break;
        case IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_1:
            d_ptr->handleScaleSboChange(IBP_CHN_1, value, false);
            break;
        case IBPMenuContentPrivate::ITEM_SBO_UP_SCALE_2:
            d_ptr->handleScaleSboChange(IBP_CHN_2, value, true);
            break;
        case IBPMenuContentPrivate::ITEM_SBO_DOWN_SCALE_2:
            d_ptr->handleScaleSboChange(IBP_CHN_2, value, false);
            break;
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
            d_ptr->handleEntitleCboChange(IBP_CHN_1, static_cast<IBPLabel>(index));
            break;
        case IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2:
            d_ptr->handleEntitleCboChange(IBP_CHN_2, static_cast<IBPLabel>(index));
            break;
        case IBPMenuContentPrivate::ITEM_CBO_RULER_1:
        {
            d_ptr->updateRulerSboScaleInfo(IBP_CHN_1, static_cast<IBPRulerLimit> (index));
            break;
        }
        case IBPMenuContentPrivate::ITEM_CBO_RULER_2:
        {
            d_ptr->updateRulerSboScaleInfo(IBP_CHN_2, static_cast<IBPRulerLimit> (index));
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
