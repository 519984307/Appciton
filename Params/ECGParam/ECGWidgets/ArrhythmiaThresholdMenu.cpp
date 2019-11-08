/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/16
 **/

#include "ArrhythmiaThresholdMenu.h"
#include "Framework/Language/LanguageManager.h"
#include "ECGAlg2SoftInterface.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVariant>
#include "KeyInputPanel.h"
#include "WindowManager.h"
#include "MessageBox.h"
#include "ECGParam.h"
#include "ComboBox.h"
#include "SpinBox.h"

class ArrhythmiaThresholdMenuPrivate
{
public:
    ArrhythmiaThresholdMenuPrivate()
        : pauseCbo(NULL)
    {}

    // load settings
    void loadOptions();

    QMap<ECGAlg::ARRPara, SpinBox *> spinboxs;
    ComboBox *pauseCbo;
    QMap<ECGAlg::ARRPara, QLabel *> nameLabs;
    QMap<ECGAlg::ARRPara, QLabel *> unitLabs;
};
ArrhythmiaThresholdMenu::ArrhythmiaThresholdMenu()
    : MenuContent(trs("ArrhythmiaThresholdMenu"), trs("ArrhythmiaThresholdMenuDesc")),
      d_ptr(new ArrhythmiaThresholdMenuPrivate)
{
}

ArrhythmiaThresholdMenu::~ArrhythmiaThresholdMenu()
{
}

void ArrhythmiaThresholdMenu::readyShow()
{
}

void ArrhythmiaThresholdMenu::layoutExec()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *label;
    QLabel *unitLab;
    SpinBox *spinbox;
    QHBoxLayout *hLayout;
    int itemID;

    // 停搏报警延迟时间
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("Asystole"));
    d_ptr->nameLabs.insert(ECGAlg::ASYS_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    spinbox = new SpinBox();
    spinbox->setRange(3, 10);
    spinbox->setStep(1);
    itemID = static_cast<int>(ECGAlg::ASYS_THRESHOLD);
    spinbox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinbox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinboxReleased(int, int)));
    hLayout->addWidget(spinbox, 1);
    d_ptr->spinboxs.insert(ECGAlg::ASYS_THRESHOLD, spinbox);
    unitLab = new QLabel("s");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::ASYS_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 心脏暂停时间
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("HeartPause"));
    hLayout->addWidget(label, 2);
    d_ptr->pauseCbo = new ComboBox();
    d_ptr->pauseCbo->addItems(QStringList()
                              << "1.5"
                              << "2.0"
                              << "2.5");
    connect(d_ptr->pauseCbo, SIGNAL(currentIndexChanged(int)), this, SLOT(pauseChangeSlot(int)));
    hLayout->addWidget(d_ptr->pauseCbo, 1);
    unitLab = new QLabel("s");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::PAUSE_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 心动过速阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("Tachycardia"));
    d_ptr->nameLabs.insert(ECGAlg::TACHY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    spinbox = new SpinBox();
    spinbox->setRange(80, 300);
    spinbox->setStep(5);
    itemID = static_cast<int>(ECGAlg::TACHY_THRESHOLD);
    spinbox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinbox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinboxReleased(int, int)));
    hLayout->addWidget(spinbox, 1);
    d_ptr->spinboxs.insert(ECGAlg::TACHY_THRESHOLD, spinbox);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::TACHY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 心动过缓阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("Bradycardia"));
    d_ptr->nameLabs.insert(ECGAlg::BRADY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    spinbox = new SpinBox();
    spinbox->setRange(15, 120);
    spinbox->setStep(5);
    itemID = static_cast<int>(ECGAlg::BRADY_THRESHOLD);
    spinbox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinbox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinboxReleased(int, int)));
    hLayout->addWidget(spinbox, 1);
    d_ptr->spinboxs.insert(ECGAlg::BRADY_THRESHOLD, spinbox);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::BRADY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 极度心动过速阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("Extachycardia"));
    d_ptr->nameLabs.insert(ECGAlg::EXTRETACHY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    spinbox = new SpinBox();
    spinbox->setRange(120, 300);
    spinbox->setStep(5);
    itemID = static_cast<int>(ECGAlg::EXTRETACHY_THRESHOLD);
    spinbox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinbox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinboxReleased(int, int)));
    hLayout->addWidget(spinbox, 1);
    d_ptr->spinboxs.insert(ECGAlg::EXTRETACHY_THRESHOLD, spinbox);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::EXTRETACHY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 极度心动过缓阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("Exbradycardia"));
    d_ptr->nameLabs.insert(ECGAlg::EXTREBRADY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    spinbox = new SpinBox();
    spinbox->setRange(15, 120);
    spinbox->setStep(5);
    itemID = static_cast<int>(ECGAlg::EXTREBRADY_THRESHOLD);
    spinbox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinbox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinboxReleased(int, int)));
    hLayout->addWidget(spinbox, 1);
    d_ptr->spinboxs.insert(ECGAlg::EXTREBRADY_THRESHOLD, spinbox);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::EXTREBRADY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 室速心率阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("VentTachy"));
    d_ptr->nameLabs.insert(ECGAlg::VENT_TACHY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    spinbox = new SpinBox();
    spinbox->setRange(100, 200);
    spinbox->setStep(5);
    itemID = static_cast<int>(ECGAlg::VENT_TACHY_THRESHOLD);
    spinbox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinbox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinboxReleased(int, int)));
    hLayout->addWidget(spinbox, 1);
    d_ptr->spinboxs.insert(ECGAlg::VENT_TACHY_THRESHOLD, spinbox);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::VENT_TACHY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 室缓心率阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("VentBrady"));
    d_ptr->nameLabs.insert(ECGAlg::VENT_BRADY_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    spinbox = new SpinBox();
    spinbox->setRange(15, 60);
    spinbox->setStep(5);
    itemID = static_cast<int>(ECGAlg::VENT_BRADY_THRESHOLD);
    spinbox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinbox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinboxReleased(int, int)));
    hLayout->addWidget(spinbox, 1);
    d_ptr->spinboxs.insert(ECGAlg::VENT_BRADY_THRESHOLD, spinbox);
    unitLab = new QLabel("bpm");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::VENT_BRADY_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 室速PVC阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("TachyPVC"));
    d_ptr->nameLabs.insert(ECGAlg::VENT_TACHY_PVC_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    spinbox = new SpinBox();
    spinbox->setRange(3, 99);
    spinbox->setStep(1);
    itemID = static_cast<int>(ECGAlg::VENT_TACHY_PVC_THRESHOLD);
    spinbox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinbox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinboxReleased(int, int)));
    hLayout->addWidget(spinbox, 1);
    d_ptr->spinboxs.insert(ECGAlg::VENT_TACHY_PVC_THRESHOLD, spinbox);
    unitLab = new QLabel("beats");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::VENT_TACHY_PVC_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 室缓PVC阈值
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("BradyPVC"));
    d_ptr->nameLabs.insert(ECGAlg::VENT_BRADY_PVC_THRESHOLD, label);
    hLayout->addWidget(label, 2);
    spinbox = new SpinBox();
    spinbox->setRange(3, 99);
    spinbox->setStep(1);
    itemID = static_cast<int>(ECGAlg::VENT_BRADY_PVC_THRESHOLD);
    spinbox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinbox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinboxReleased(int, int)));
    hLayout->addWidget(spinbox, 1);
    d_ptr->spinboxs.insert(ECGAlg::VENT_BRADY_PVC_THRESHOLD, spinbox);
    unitLab = new QLabel("beats");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::VENT_BRADY_PVC_THRESHOLD, unitLab);
    layout->addLayout(hLayout);

    // 多形PVC窗宽
    hLayout = new QHBoxLayout();
    label = new QLabel(trs("MultifPVC"));
    d_ptr->nameLabs.insert(ECGAlg::MULTIF_PVC_WINDOW, label);
    hLayout->addWidget(label, 2);
    spinbox = new SpinBox();
    spinbox->setRange(3, 31);
    spinbox->setStep(1);
    itemID = static_cast<int>(ECGAlg::MULTIF_PVC_WINDOW);
    spinbox->setProperty("Item", qVariantFromValue(itemID));
    connect(spinbox, SIGNAL(valueChange(int, int)), this, SLOT(onSpinboxReleased(int, int)));
    hLayout->addWidget(spinbox, 1);
    d_ptr->spinboxs.insert(ECGAlg::MULTIF_PVC_WINDOW, spinbox);
    unitLab = new QLabel("beats");
    hLayout->addWidget(unitLab, 1, Qt::AlignLeft);
    d_ptr->unitLabs.insert(ECGAlg::MULTIF_PVC_WINDOW, unitLab);
    layout->addLayout(hLayout);

    d_ptr->loadOptions();
}

void ArrhythmiaThresholdMenu::onSpinboxReleased(int value, int scale)
{
    SpinBox *spinbox = qobject_cast<SpinBox *>(sender());
    ECGAlg::ARRPara item = (ECGAlg::ARRPara)spinbox->property("Item").toInt();
    ecgParam.setARRThreshold(item, value * scale);
}

void ArrhythmiaThresholdMenu::pauseChangeSlot(int index)
{
    ecgParam.setARRThreshold(ECGAlg::PAUSE_THRESHOLD, index);
}

void ArrhythmiaThresholdMenuPrivate::loadOptions()
{
    spinboxs[ECGAlg::ASYS_THRESHOLD]->setValue(5);
    pauseCbo->setCurrentIndex(0);
    spinboxs[ECGAlg::TACHY_THRESHOLD]->setValue(205);
    spinboxs[ECGAlg::BRADY_THRESHOLD]->setValue(75);
    spinboxs[ECGAlg::EXTRETACHY_THRESHOLD]->setValue(205);
    spinboxs[ECGAlg::EXTREBRADY_THRESHOLD]->setValue(55);
    spinboxs[ECGAlg::VENT_TACHY_THRESHOLD]->setValue(130);
    spinboxs[ECGAlg::VENT_BRADY_THRESHOLD]->setValue(40);
    spinboxs[ECGAlg::VENT_TACHY_PVC_THRESHOLD]->setValue(6);
    spinboxs[ECGAlg::VENT_BRADY_PVC_THRESHOLD]->setValue(5);
    spinboxs[ECGAlg::MULTIF_PVC_WINDOW]->setValue(7);
}
