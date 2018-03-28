#include "DoseCalculationWidget.h"
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "IButton.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "NumberInput.h"
#include "DoseCalculationManager.h"
#include "IMessageBox.h"
#include "TitrateTableWidget.h"

DoseCalculationWidget *DoseCalculationWidget::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
DoseCalculationWidget::~DoseCalculationWidget()
{

}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void DoseCalculationWidget::layoutExec()
{
    setFixedWidth(windowManager.getPopMenuWidth());
    setFixedHeight(windowManager.getPopMenuHeight());

    int  submenuW = windowManager.getPopMenuWidth();

    setTitleBarText(trs("DoseCalculation"));

    int itemW = submenuW - 20;

    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 4;
    int labelWidth = btnWidth;

    _drugName = new IComboList(trs("DrugName"));
    _drugName->setFont(fontManager.textFont(fontSize));
    for(int i = 0; i < DRUG_NAME_NR; i ++)
    {
        _drugName->addItem(trs(DoseCalculationSymbol::convert((DrugName)i)));
    }
    _drugName->label->setFixedSize(labelWidth/4*3 - labelWidth/5, _itemH);
    _drugName->combolist->setFixedSize(btnWidth + labelWidth/5, _itemH);
    _drugName->label->setAlignment(Qt::AlignLeft);
    _drugName->combolist->setCurrentIndex(doseCalculationManager.getDrugName());
    QHBoxLayout *hlayoutName = new QHBoxLayout();
    hlayoutName->addWidget(_drugName);
    hlayoutName->addStretch();

    _patientType = new IComboList(trs("PatientType"));
    _patientType->setFont(fontManager.textFont(fontSize));
    _patientType->addItem(trs(DoseCalculationSymbol::convert(PATIENT_TYPE_ADULT)));
    _patientType->addItem(trs(DoseCalculationSymbol::convert(PATIENT_TYPE_PED)));
    _patientType->addItem(trs(DoseCalculationSymbol::convert(PATIENT_TYPE_NEO)));
    _patientType->label->setFixedSize(labelWidth/4*3 - labelWidth/5, _itemH);
    _patientType->combolist->setFixedSize(btnWidth + labelWidth/5, _itemH);
    _patientType->label->setAlignment(Qt::AlignLeft);
    _patientType->combolist->setCurrentIndex(doseCalculationManager.getType());
    QHBoxLayout *hlayoutType= new QHBoxLayout();
    hlayoutType->addWidget(_patientType);
    hlayoutType->addStretch();

    connect(_drugName, SIGNAL(currentIndexChanged(int)), this, SLOT(_updataParamDefault(int)));
    connect(_patientType, SIGNAL(currentIndexChanged(int)), this, SLOT(_updataParamDefault(int)));

    _titrationTable = new IButton(trs("TitrationTable"));
    _titrationTable->setFixedSize(btnWidth, _itemH);
    _titrationTable->setFont(fontManager.textFont(fontSize));
    _titrationTable->setBorderEnabled(true);
    _titrationTable->setEnabled(false);

    connect(_titrationTable, SIGNAL(realReleased()), this, SLOT(_titrateTableReleased()));

    _signalMapper = new QSignalMapper(this);
    QHBoxLayout *hlayout[CALCULATION_PARAM_NR];
    QVBoxLayout *fristColumnlayout = new QVBoxLayout();
    QVBoxLayout *secondColumnLayout = new QVBoxLayout();
    fristColumnlayout->addLayout(hlayoutName);
    fristColumnlayout->addLayout(hlayoutType);
    for(int j = 0; j < CALCULATION_PARAM_NR; j ++)
    {
        _calcParam[j] = new LabelButton(trs(DoseCalculationSymbol::convert(static_cast<DoseCalculationParam>(j))));
        _calcParam[j]->setFont(fontManager.textFont(fontSize));
        _calcParam[j]->label->setFixedSize(labelWidth/4*3 - labelWidth/5, _itemH);
        _calcParam[j]->button->setFixedSize(btnWidth/5*2 + labelWidth/5, _itemH);
        _calcParam[j]->label->setAlignment(Qt::AlignLeft);
        _calcParam[j]->setValue(DoseCalculationSymbol::convert(doseCalculationManager.getType(),
                                                               doseCalculationManager.getDrugName(),
                                                               static_cast<DoseCalculationParam>(j)));
        _calcParamUnit[j] = new QLabel(DoseCalculationSymbol::convert(doseCalculationManager.getType(),
                                                                      DRUG_UNIT_GRAM,
                                                                      static_cast<DoseCalculationParam>(j)));
        _calcParamUnit[j]->setFont(fontManager.textFont(fontSize));
        _calcParamUnit[j]->setFixedSize(btnWidth/5*3, _itemH);
        connect(_calcParam[j]->button, SIGNAL(realReleased()), _signalMapper, SLOT(map()));
        _signalMapper->setMapping(_calcParam[j]->button, j);

        hlayout[j] = new QHBoxLayout();
        hlayout[j]->addWidget(_calcParam[j]);
        hlayout[j]->addWidget(_calcParamUnit[j]);
        hlayout[j]->addStretch();

        if(j <= CALCULATION_PARAM_DOSE1)
        {
            fristColumnlayout->addLayout(hlayout[j]);
        }
        else
        {
            secondColumnLayout->addLayout(hlayout[j]);
        }
    }

    connect(_signalMapper, SIGNAL(mapped(int)), this, SLOT(_btnParamReleased(int)));

    QHBoxLayout *lastRowLayout = new QHBoxLayout();
    lastRowLayout->addStretch();
    lastRowLayout->addWidget(_titrationTable);
    lastRowLayout->setContentsMargins(0,0,30,0);

    QHBoxLayout *HLayout = new QHBoxLayout();
    HLayout->addLayout(fristColumnlayout);
    HLayout->addStretch();
    HLayout->addLayout(secondColumnLayout);
    HLayout->addStretch();
    HLayout->setContentsMargins(20,0,0,0);

    contentLayout->addLayout(HLayout);
    contentLayout->addStretch();
    contentLayout->addLayout(lastRowLayout);
    contentLayout->setSpacing(5);
    contentLayout->addStretch();
}

void DoseCalculationWidget::initDrugParam()
{
    for(int i = 0; i < CALCULATION_PARAM_NR; i ++)
    {
        if(_calcParam[i]->button->text() == "---")
        {
            doseCalculationManager.setDrugParam(i, -1, _calcParamUnit[i]->text());
        }
        else
        {
            doseCalculationManager.setDrugParam(i, _calcParam[i]->button->text().toFloat(), _calcParamUnit[i]->text());
        }
    }
}

void DoseCalculationWidget::updateDrugParam()
{

    for(int i = 0; i < CALCULATION_PARAM_NR; i ++)
    {
        if (i <= CALCULATION_PARAM_DOSE4)
        {
            if(doseCalculationManager.getDrugParam(i).value >= 0 && doseCalculationManager.getDrugParam(i).value < 10000)
            {
                if ((doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value == 0 && i == CALCULATION_PARAM_WEIGHT)
                        || (doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value == 0 && i == CALCULATION_PARAM_DOSE3)
                        || (doseCalculationManager.getDrugParam(CALCULATION_PARAM_WEIGHT).value == 0 && i == CALCULATION_PARAM_DOSE4))
                {
                    _calcParam[i]->setValue("");
                }
                else
                {
                    _calcParam[i]->button->setText(QString::number(doseCalculationManager.getDrugParam(i).value,'f', 2));
                    _calcParamUnit[i]->setText(doseCalculationManager.getDrugParam(i).unit);
                }
            }
            else
            {
                if (i == CALCULATION_PARAM_DOSE3 || i == CALCULATION_PARAM_DOSE4)
                {
                    if (doseCalculationManager.getDrugParam(i).value > 0)
                    {
                        _calcParam[i]->setValue("---");
                    }
                    else
                    {
                        _calcParam[i]->setValue("");
                    }
                }
                else
                {
                    _calcParam[i]->setValue("---");
                }
            }
        }
        else
        {
            if(doseCalculationManager.getDrugParam(i).value >= 0 && doseCalculationManager.getDrugParam(i).value < 1000)
            {
                _calcParam[i]->button->setText(QString::number(doseCalculationManager.getDrugParam(i).value,'f', 2));
                _calcParamUnit[i]->setText(doseCalculationManager.getDrugParam(i).unit);
            }
            else
            {
                _calcParam[i]->setValue("---");
            }
        }
    }
}

void DoseCalculationWidget::updataParamDefault(void)
{
    for(int j = 0; j < CALCULATION_PARAM_NR; j ++)
    {

        _calcParam[j]->setValue(DoseCalculationSymbol::convert(doseCalculationManager.getType(),
                                                               doseCalculationManager.getDrugName(),
                                                               static_cast<DoseCalculationParam>(j)));

        _calcParamUnit[j]->setText(DoseCalculationSymbol::convert(doseCalculationManager.getType(),
                                                                  doseCalculationManager.getDrugUnit(),
                                                                  static_cast<DoseCalculationParam>(j)));

        doseCalculationManager.setDrugParam(j, _calcParam[j]->button->text().toFloat(), _calcParamUnit[j]->text());
    }
}

void DoseCalculationWidget::weightHandle(float weight)
{
    if((weight < 0.1) || (weight > 200))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.1-200", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_WEIGHT, weight);
        updateDrugParam();
    }
}

void DoseCalculationWidget::totalHandle(float total)
{
    if((total < 0) || (total > 10000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_TOTAL, total);
        updateDrugParam();
    }
}

void DoseCalculationWidget::volumeHandle(float volume)
{
    if((volume < 0) || (volume > 10000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_VOLUME, volume);
        updateDrugParam();
    }
}

void DoseCalculationWidget::concentrationHandle(float concentration)
{
    if((concentration < 0) || (concentration > 10000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_CONCENTRATION, concentration);
        updateDrugParam();
    }
}

void DoseCalculationWidget::dose1Handle(float dose1)
{
    if((dose1 < 0) || (dose1 > 10000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DOSE1, dose1);
        updateDrugParam();
    }
}

void DoseCalculationWidget::dose2Handle(float dose2)
{
    if((dose2 < 0) || (dose2 > 10000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DOSE2, dose2);
        updateDrugParam();
    }
}

void DoseCalculationWidget::dose3Handle(float dose3)
{
    if((dose3 < 0) || (dose3 > 10000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DOSE3, dose3);
        updateDrugParam();
    }
}

void DoseCalculationWidget::dose4Handle(float dose4)
{
    if((dose4 < 0) || (dose4 > 10000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-9999.99", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DOSE4, dose4);
        updateDrugParam();
    }
}

void DoseCalculationWidget::infusionRateHandle(float infusionRate)
{
    if((infusionRate < 0) || (infusionRate > 1000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-999.99", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_INFUSIONRATE, infusionRate);
        updateDrugParam();
    }
}

void DoseCalculationWidget::dripRateHandle(float dripRate)
{
    if((dripRate < 0) || (dripRate > 1000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-999.99", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DRIPRATE, dripRate);
        updateDrugParam();
    }
}

void DoseCalculationWidget::dropSizeHandle(float dropSize)
{
    if((dropSize < 0) || (dropSize > 1000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-999.99", QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        doseCalculationManager.doseCalcAction(CALCULATION_PARAM_DROPSIZE, dropSize);
        updateDrugParam();
    }
}

void DoseCalculationWidget::durationHandle(float duration)
{
    if((duration < 0) || (duration > 1000))
    {
        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0-999.99", QStringList(trs("EnglishYESChineseSURE")));
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
void DoseCalculationWidget::showEvent(QShowEvent *e)
{
    PopupWidget::showEvent(e);

    // 居中显示。
    QRect r = windowManager.getMenuArea();
//    move(r.x() + (r.width() - width()) / 2, r.y() + (2 * r.height() / 3 - height()));
    move(r.x() + (r.width() - width()) / 2, r.y() + (r.height() - height()) / 2);
}

void DoseCalculationWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusPreviousChild();
            break;
        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;
        default:
            break;
    }

    QWidget::keyPressEvent(e);
}

/***************************************************************************************************
 * 药物计算参数设置槽函数
 **************************************************************************************************/
void DoseCalculationWidget::_btnParamReleased(int index)
{
    NumberInput numberPad;
    numberPad.setTitleBarText(trs(DoseCalculationSymbol::convert(static_cast<DoseCalculationParam>(index))));
    numberPad.setMaxInputLength(7);
    numberPad.setInitString(_calcParam[index]->button->text());
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
                if (_titrationTable->isEnabled() == false)
                {
                    updataParamDefault();
                    _titrationTable->setEnabled(true);
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
void DoseCalculationWidget::_updataParamDefault(int index)
{
    IComboList* btn = dynamic_cast<IComboList*>(sender());
    if (btn->label->text() == trs("PatientType"))
    {
        doseCalculationManager.setType(static_cast<PatientType>(index));

    }
    else if (btn->label->text() == trs("DrugName"))
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

    for(int j = 0; j < CALCULATION_PARAM_NR; j ++)
    {

        _calcParam[j]->setValue(DoseCalculationSymbol::convert(doseCalculationManager.getType(),
                                                               doseCalculationManager.getDrugName(),
                                                               static_cast<DoseCalculationParam>(j)));

        _calcParamUnit[j]->setText(DoseCalculationSymbol::convert(doseCalculationManager.getType(),
                                                                  doseCalculationManager.getDrugUnit(),
                                                                  static_cast<DoseCalculationParam>(j)));

        doseCalculationManager.setDrugParam(j, _calcParam[j]->button->text().toFloat(), _calcParamUnit[j]->text());
    }
}

void DoseCalculationWidget::_titrateTableReleased()
{
    titrateTableWidget.updateTitrateTableData();
    titrateTableWidget.autoShow();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
DoseCalculationWidget::DoseCalculationWidget(): PopupWidget()
{
    layoutExec();
    initDrugParam();
}
