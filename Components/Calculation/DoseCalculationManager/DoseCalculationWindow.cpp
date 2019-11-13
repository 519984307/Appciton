/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/30
 **/
#include "DoseCalculationWindow.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QSignalMapper>
#include "DoseCalculationDefine.h"
#include "DoseCalculationManager.h"
#include "WindowManager.h"
#include "MessageBox.h"
#include "KeyInputPanel.h"
#include "TitrateTableWindow.h"


class DoseCalculationWindowPrivate
{
public:
    DoseCalculationWindowPrivate();

    ComboBox *drugName;              // 药物名称
    ComboBox *patientType;           // 病人类型
    Button *calcParam[CALCULATION_PARAM_NR];
    QLabel *calcParamUnit[CALCULATION_PARAM_NR];
    Button *titrationTable;           // 滴定表
};


DoseCalculationWindowPrivate::DoseCalculationWindowPrivate()
    : drugName(NULL),
      patientType(NULL),
      titrationTable(NULL)

{
    for (int i = 0; i < CALCULATION_PARAM_NR; i++)
    {
        calcParam[i] = NULL;
        calcParamUnit[i] = NULL;
    }
}

DoseCalculationWindow::~DoseCalculationWindow()
{
    delete d_ptr;
}

DoseCalculationWindow *DoseCalculationWindow::getInstance()
{
    static DoseCalculationWindow *instance = NULL;
    if (!instance)
    {
        instance = new DoseCalculationWindow;
    }
    return instance;
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void DoseCalculationWindow::layoutExec()
{
    setWindowTitle(trs("DoseCalculation"));

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(5);

    ComboBox *combo;
    QLabel *label;
    int itemId = 0;
    int index = 0;

    // drug name
    label = new QLabel(trs("DrugName"));
    layout->addWidget(label, index, 0);
    combo = new ComboBox;
    for (int i = 0; i < DRUG_NAME_NR; i++)
    {
        combo->addItem(trs(DoseCalculationSymbol::convert((DrugName)i)));
    }
    layout->addWidget(combo, index, 1);
    index++;
    combo->setProperty("comboItem", qVariantFromValue(itemId));
    itemId++;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdataParamDefault(int)));
    d_ptr->drugName = combo;

    // patient type
    label = new QLabel(trs("PatientType"));
    layout->addWidget(label, index, 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(PatientSymbol::convert(PATIENT_TYPE_ADULT))
                    << trs(PatientSymbol::convert(PATIENT_TYPE_PED))
                    << trs(PatientSymbol::convert(PATIENT_TYPE_NEO)));
    layout->addWidget(combo, index, 1);
    index++;
    combo->setProperty("comboItem", qVariantFromValue(itemId));
    itemId++;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdataParamDefault(int)));
    d_ptr->patientType = combo;

    Button *button;
    for (int i = 0; i < CALCULATION_PARAM_DOSE2; i++)
    {
        label = new QLabel(trs(DoseCalculationSymbol
                               ::convert(static_cast<DoseCalculationParam>(i))));
        layout->addWidget(label, index, 0);
        d_ptr->calcParamUnit[i] = new QLabel(trs(DoseCalculationSymbol::convert(
                doseCalculationManager.getType(),
                DRUG_UNIT_GRAM,
                static_cast<DoseCalculationParam>(i))));
        button = new Button;
        QString str = trs(DoseCalculationSymbol::convert(
                              doseCalculationManager.getType(),
                              doseCalculationManager.getDrugName(),
                              static_cast<DoseCalculationParam>(i)));

        button->setText(str);
        button->setButtonStyle(Button::ButtonTextOnly);
        layout->addWidget(button, index, 1);
        layout->addWidget(d_ptr->calcParamUnit[i], index, 2);
        index++;
        button->setProperty("btnItem", qVariantFromValue(i));
        connect(button, SIGNAL(released()), this, SLOT(onBtnParamReleased()));
        d_ptr->calcParam[i] = button;
    }

    index = 0;
    for (int i = CALCULATION_PARAM_DOSE2; i < CALCULATION_PARAM_NR; i++)
    {
        label = new QLabel(trs(DoseCalculationSymbol
                               ::convert(static_cast<DoseCalculationParam>(i))));
        layout->addWidget(label, index, 4);
        d_ptr->calcParamUnit[i] = new QLabel(trs(DoseCalculationSymbol::convert(
                doseCalculationManager.getType(),
                DRUG_UNIT_GRAM,
                static_cast<DoseCalculationParam>(i))));
        button = new Button;
        QString str = trs(DoseCalculationSymbol::convert(
                              doseCalculationManager.getType(),
                              doseCalculationManager.getDrugName(),
                              static_cast<DoseCalculationParam>(i)));
        button->setText(str);
        button->setButtonStyle(Button::ButtonTextOnly);
        layout->addWidget(button, index, 5);
        layout->addWidget(d_ptr->calcParamUnit[i], index, 6);
        index++;
        button->setProperty("btnItem", qVariantFromValue(i));
        connect(button, SIGNAL(released()), this, SLOT(onBtnParamReleased()));
        d_ptr->calcParam[i] = button;
    }
    button = new Button(trs("TitrationTable"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnTitrateTableReleased()));
    layout->addWidget(button, (CALCULATION_PARAM_DOSE2 + 2), 5);
    button->setEnabled(false);
    d_ptr->titrationTable = button;

    layout->setColumnMinimumWidth(0, 100);
    setWindowLayout(layout);
    setFixedSize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());
}

/**************************************************************************************************
 * 功能： 将界面的药物计算参数的值和单位存储下来。
 *************************************************************************************************/
void DoseCalculationWindow::initDrugParam()
{
    for (int i = 0; i < CALCULATION_PARAM_NR; i ++)
    {
        if (d_ptr->calcParam[i]->text() == "---")
        {
            doseCalculationManager.setDrugParam(i, -1, d_ptr->calcParamUnit[i]->text());
        }
        else
        {
            doseCalculationManager.setDrugParam(i, d_ptr->calcParam[i]->text().toFloat(),
                                                d_ptr->calcParamUnit[i]->text());
        }
    }
}

/**************************************************************************************************
 * 功能： 将存储的参数值和单位更新到控件上。
 *************************************************************************************************/
void DoseCalculationWindow::updateDrugParam()
{
    for (int i = 0; i < CALCULATION_PARAM_NR; i ++)
    {
        if (i <= CALCULATION_PARAM_DOSE4)
        {
            if (doseCalculationManager.getDrugParam(i).value >= 0
                    && doseCalculationManager.getDrugParam(i).value < 10000)
            {
                if ((doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value == 0
                     && i == CALCULATION_PARAM_WEIGHT)
                        || (doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value == 0
                            && i == CALCULATION_PARAM_DOSE3)
                        || (doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value == 0
                            && i == CALCULATION_PARAM_DOSE4))
                {
                    d_ptr->calcParam[i]->setText("");
                }
                else
                {
                    d_ptr->calcParam[i]->setText(QString::number(doseCalculationManager.getDrugParam(i).value, 'f', 2));
                    d_ptr->calcParamUnit[i]->setText(doseCalculationManager.getDrugParam(i).unit);
                }
            }
            else
            {
                if (i == CALCULATION_PARAM_DOSE3 || i == CALCULATION_PARAM_DOSE4)
                {
                    if (doseCalculationManager.getDrugParam(i).value > 0)
                    {
                        d_ptr->calcParam[i]->setText("---");
                    }
                    else
                    {
                        d_ptr->calcParam[i]->setText("");
                    }
                }
                else
                {
                    d_ptr->calcParam[i]->setText("---");
                }
            }
        }
        else
        {
            if (doseCalculationManager.getDrugParam(i).value >= 0
                    && doseCalculationManager.getDrugParam(i).value < 1000)
            {
                d_ptr->calcParam[i]->setText(QString::number(doseCalculationManager.getDrugParam(i).value, 'f', 2));
                d_ptr->calcParamUnit[i]->setText(doseCalculationManager.getDrugParam(i).unit);
            }
            else
            {
                d_ptr->calcParam[i]->setText("---");
            }
        }
    }
}

/**************************************************************************************************
 * 功能： 将病人类型和药物名称对应的默认值设置在控件上并存储下来。
 *************************************************************************************************/
void DoseCalculationWindow::updataParamDefault(void)
{
    for (int j = 0; j < CALCULATION_PARAM_NR; j ++)
    {
        d_ptr->calcParam[j]->setText(DoseCalculationSymbol::convert(doseCalculationManager.getType(),
                                     doseCalculationManager.getDrugName(),
                                     static_cast<DoseCalculationParam>(j)));

        d_ptr->calcParamUnit[j]->setText(DoseCalculationSymbol::convert(doseCalculationManager.getType(),
                                         doseCalculationManager.getDrugUnit(),
                                         static_cast<DoseCalculationParam>(j)));

        doseCalculationManager.setDrugParam(j, d_ptr->calcParam[j]->text().toFloat(), d_ptr->calcParamUnit[j]->text());
    }
}

/**************************************************************************************************
 * 功能： 修改体重操作。
 *************************************************************************************************/
void DoseCalculationWindow::weightHandle(float weight)
{
    if ((weight < 0.1) || (weight > 200))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.1-200",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_WEIGHT, weight);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改药物总量操作。
 *************************************************************************************************/
void DoseCalculationWindow::totalHandle(float total)
{
    if ((total < 0) || (total > 10000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_TOTAL, total);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改溶液体积操作。
 *************************************************************************************************/
void DoseCalculationWindow::volumeHandle(float volume)
{
    if ((volume < 0) || (volume > 10000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_VOLUME, volume);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改药物浓度操作。
 *************************************************************************************************/
void DoseCalculationWindow::concentrationHandle(float concentration)
{
    if ((concentration < 0) || (concentration > 10000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_CONCENTRATION, concentration);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改剂量/min操作。
 *************************************************************************************************/
void DoseCalculationWindow::dose1Handle(float dose1)
{
    if ((dose1 < 0) || (dose1 > 10000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DOSE1, dose1);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改剂量/h操作。
 *************************************************************************************************/
void DoseCalculationWindow::dose2Handle(float dose2)
{
    if ((dose2 < 0) || (dose2 > 10000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DOSE2, dose2);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改剂量/kg/min操作。
 *************************************************************************************************/
void DoseCalculationWindow::dose3Handle(float dose3)
{
    if ((dose3 < 0) || (dose3 > 10000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DOSE3, dose3);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改剂量/kg/h操作。
 *************************************************************************************************/
void DoseCalculationWindow::dose4Handle(float dose4)
{
    if ((dose4 < 0) || (dose4 > 10000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DOSE4, dose4);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改输液速度操作。
 *************************************************************************************************/
void DoseCalculationWindow::infusionRateHandle(float infusionRate)
{
    if ((infusionRate < 0) || (infusionRate > 1000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_INFUSIONRATE, infusionRate);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改滴速操作。
 *************************************************************************************************/
void DoseCalculationWindow::dripRateHandle(float dripRate)
{
    if ((dripRate < 0) || (dripRate > 1000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DRIPRATE, dripRate);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改每滴体积操作。
 *************************************************************************************************/
void DoseCalculationWindow::dropSizeHandle(float dropSize)
{
    if ((dropSize < 0) || (dropSize > 1000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DROPSIZE, dropSize);
        updateDrugParam();
    }
}

/**************************************************************************************************
 * 功能： 修改持续时间操作。
 *************************************************************************************************/
void DoseCalculationWindow::durationHandle(float duration)
{
    if ((duration < 0) || (duration > 1000))
    {
        MessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-999.99",
                              QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DURATION, duration);
        updateDrugParam();
    }
}

/***************************************************************************************************
 * 显示事件
 **************************************************************************************************/
void DoseCalculationWindow::showEvent(QShowEvent *e)
{
    Dialog::showEvent(e);
}


/***************************************************************************************************
 * 药物计算参数设置槽函数
 **************************************************************************************************/
void DoseCalculationWindow::onBtnParamReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    int index = button->property("btnItem").toInt();

    KeyInputPanel numberPad;
    QString rec("[0-9]|[.]");
    numberPad.setBtnEnable(rec);
    QString str(trs(DoseCalculationSymbol::convert(static_cast<DoseCalculationParam>(index))));
    str += "(";
    str += trs("Unit");
    str += ": ";
    str += trs(d_ptr->calcParamUnit[index]->text());
    str += ")";
    numberPad.setWindowTitle(str);
    numberPad.setMaxInputLength(7);
    numberPad.setInitString(d_ptr->calcParam[index]->text());
    if (numberPad.exec())
    {
        QString text = numberPad.getStrValue();
        bool ok = false;
        float value = text.toFloat(&ok);
        if (ok)
        {
            switch (index)
            {
            case CALCULATION_PARAM_WEIGHT:
            {
                if (d_ptr->titrationTable->isEnabled() == false)
                {
                    updataParamDefault();
                    d_ptr->titrationTable->setEnabled(true);
                }
                weightHandle(value);
                break;
            }
            case CALCULATION_PARAM_TOTAL:
            {
                totalHandle(value);
                break;
            }
            case CALCULATION_PARAM_VOLUME:
            {
                volumeHandle(value);
                break;
            }
            case CALCULATION_PARAM_CONCENTRATION:
            {
                concentrationHandle(value);
                break;
            }
            case CALCULATION_PARAM_DOSE1:
            {
                dose1Handle(value);
                break;
            }
            case CALCULATION_PARAM_DOSE2:
            {
                dose2Handle(value);
                break;
            }
            case CALCULATION_PARAM_DOSE3:
            {
                dose3Handle(value);
                break;
            }
            case CALCULATION_PARAM_DOSE4:
            {
                dose4Handle(value);
                break;
            }
            case CALCULATION_PARAM_INFUSIONRATE:
            {
                infusionRateHandle(value);
                break;
            }
            case CALCULATION_PARAM_DRIPRATE:
            {
                dripRateHandle(value);
                break;
            }
            case CALCULATION_PARAM_DROPSIZE:
            {
                dropSizeHandle(value);
                break;
            }
            case CALCULATION_PARAM_DURATION:
            {
                durationHandle(value);
                break;
            }
            }
        }
    }
}

/**************************************************************************************************
 * 刷新参数默认值。
 *************************************************************************************************/
void DoseCalculationWindow::onComboUpdataParamDefault(int index)
{
    ComboBox *combo = dynamic_cast<ComboBox *>(sender());
    int indexType = combo->property("comboItem").toInt();
    if (indexType == 1)
    {
        doseCalculationManager.setType(static_cast<PatientType>(index));
    }
    else if (indexType == 0)
    {
        doseCalculationManager.setDrugName(static_cast<DrugName>(index));
        if (static_cast<DrugName>(index) == DRUG_NAME_DRUGD || static_cast<DrugName>(index) == DRUG_NAME_HEPARIN
                || static_cast<DrugName>(index) == DRUG_NAME_PITOCIN)
        {
            doseCalculationManager.setDrugUnit(DRUG_UNIT_UNIT);
        }
        else if (static_cast<DrugName>(index) == DRUG_NAME_DRUGE)
        {
            doseCalculationManager.setDrugUnit(DRUG_UNIT_MEQ);
        }
        else
        {
            doseCalculationManager.setDrugUnit(DRUG_UNIT_GRAM);
        }
    }

    for (int j = 0; j < CALCULATION_PARAM_NR; j ++)
    {
        d_ptr->calcParam[j]->setText(DoseCalculationSymbol::convert(doseCalculationManager.getType(),
                                     doseCalculationManager.getDrugName(),
                                     static_cast<DoseCalculationParam>(j)));

        d_ptr->calcParamUnit[j]->setText(DoseCalculationSymbol::convert(doseCalculationManager.getType(),
                                         doseCalculationManager.getDrugUnit(),
                                         static_cast<DoseCalculationParam>(j)));

        doseCalculationManager.setDrugParam(j, d_ptr->calcParam[j]->text().toFloat(), d_ptr->calcParamUnit[j]->text());
    }
}

/**************************************************************************************************
 * 功能： 打开滴定表窗口。
 *************************************************************************************************/
void DoseCalculationWindow::onBtnTitrateTableReleased()
{
//    titrateTableWidget.updateTitrateTableData();
//    titrateTableWidget.autoShow();
    windowManager.showWindow(TitrateTableWindow::getInstance(),
                             WindowManager::ShowBehaviorModal);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
DoseCalculationWindow::DoseCalculationWindow()
    : Dialog(),
      d_ptr(new DoseCalculationWindowPrivate)
{
    setFixedSize(800, 580);
    layoutExec();
    initDrugParam();
}
