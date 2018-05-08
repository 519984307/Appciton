#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "IComboList.h"
#include "NIBPPressureControl.h"
#include "Debug.h"
#include "NIBPParam.h"
#include "NIBPSymbol.h"
#include "PatientManager.h"
#include "ISpinBox.h"
#include "NIBPRepairMenuManager.h"


NIBPPressureControl *NIBPPressureControl::_selfObj = NULL;

//压力控制模式

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPPressureControl::layoutExec()
{
    int submenuW = nibpRepairMenuManager.getSubmenuWidth();
    int submenuH = nibpRepairMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int pressuresize = fontManager.getFontSize(5);
    int Service_H = 50;
    int btnWidth = itemW / 2.5;

    QHBoxLayout *labelLayout = new QHBoxLayout();
    QLabel *l = new QLabel(trs("ServicePressure"));
    l->setFixedSize(btnWidth, Service_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(pressuresize));
    labelLayout->addWidget(l);

    _value = new QLabel();
    _value->setFixedSize(btnWidth, Service_H);
    _value->setAlignment(Qt::AlignCenter);
    _value->setFont(fontManager.textFont(pressuresize));
    _value->setText(InvStr());
    labelLayout->addWidget(_value);

    l = new QLabel();
    l->setText(Unit::getSymbol(nibpParam.getUnit()));
    l->setFixedSize(50, Service_H);
    l->setAlignment(Qt::AlignCenter);
    l->setFont(fontManager.textFont(fontSize));
    labelLayout->addWidget(l);
    mainLayout->addLayout(labelLayout);

    _patientType = new IComboList(trs("ServicePatientType"));
    _patientType->label->setFixedSize(btnWidth+30, ITEM_H);
    _patientType->setLabelAlignment(Qt::AlignLeft);
    _patientType->combolist->setFixedSize(btnWidth, ITEM_H);
    _patientType->setFont(fontManager.textFont(fontSize));
    _patientType->setFocusPolicy(Qt::StrongFocus);
    _patientType->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_ADULT)));
    _patientType->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_PED)));
//    _patientType->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_NEO)));
    _patientType->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_NULL)));
    connect(_patientType->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_patientInflate(int)));
    mainLayout->addWidget(_patientType, 0, Qt::AlignRight);

    _chargePressure = new ISpinBox(trs("ServiceChargePressure"));
    _chargePressure->setFixedHeight(ITEM_H);
    _chargePressure->setLabelWidth(btnWidth+50);
    _chargePressure->setValueWidth(btnWidth);
    _chargePressure->setLabelAlignment(Qt::AlignLeft);
    _chargePressure->setSuffix(Unit::getSymbol(UNIT_MMHG));
    _chargePressure->setSuffixSpace(2);
    _chargePressure->setFont(fontManager.textFont(fontSize));
    _chargePressure->setMode(ISPIN_MODE_INT);
    _chargePressure->setRange(50, 300);
    _chargePressure->setValue(250);
    _chargePressure->setStep(5);
    connect(_chargePressure, SIGNAL(valueChange(QString,int)), this, SLOT(_pressureChange(QString)));
    mainLayout->addWidget(_chargePressure, 0, Qt::AlignRight);

    _InflateBtn= new LButtonEx();
    _InflateBtn->setText(trs("ServiceInflate"));
    _inflatePressure = true;
    _InflateBtn->setFont(fontManager.textFont(fontSize));
    _InflateBtn->setFixedSize(btnWidth, ITEM_H);
    connect(_InflateBtn, SIGNAL(realReleased()), this, SLOT(_InflateBtnReleased()));
    mainLayout->addWidget(_InflateBtn, 0, Qt::AlignRight);
}

/**************************************************************************************************
 * 初始化。
 *************************************************************************************************/
void NIBPPressureControl::init(void)
{
    _value->setText(InvStr());
    _chargePressure->setValue(250);
    _chargePressure->disable(false);
    _chargePressure->setEnabled(true);
    btnSwitch(true);

    _patientVaulue = 0;
    _patientType->setCurrentIndex(_patientVaulue);

    nibpParam.provider().servicePressureProtect(true);

    _patientType->setDisabled(false);
    _InflateBtn->setEnabled(true);

    _pressureControlFlag = false;
}

/**************************************************************************************************
 * 进入压力控制指令。
 *************************************************************************************************/
void NIBPPressureControl::focusFirstItem()
{
    init();

    nibpParam.switchState(NIBP_SERVICE_PRESSURECONTROL_STATE);

    SubMenu::focusFirstItem();
}

/**************************************************************************************************
 * 病人类型，充气值改变指令
 *************************************************************************************************/
void NIBPPressureControl::_changeReleased(void)
{
    if(!_inflatePressure)
    {
        _deflateReleased();
    }
}

/**************************************************************************************************
 * 充气、放气控制按钮。
 *************************************************************************************************/
void NIBPPressureControl::_InflateBtnReleased()
{
    if (_pressureControlFlag)
    {
        if(_inflatePressure)
        {
            _inflateReleased();
            btnSwitch(false);
        }
        else
        {
            _deflateReleased();
        }
    }
    else
    {
        nibpRepairMenuManager.messageBox();
    }
}

/**************************************************************************************************
 * 充气槽函数。
 *************************************************************************************************/
void NIBPPressureControl::_inflateReleased()
{
//    PatientType type = (PatientType)(_patientType->currentIndex());
//    if ( type == PATIENT_TYPE_NULL )
    int index = _patientType->currentIndex();
    if (_patientType->itemText(index) == trs(PatientSymbol::convert(PATIENT_TYPE_NULL)))
    {
        nibpParam.provider().servicePressureProtect(false);
    }
    else
    {
        nibpParam.provider().servicePressureProtect(true);
    }

    nibpParam.provider().setPatientType((unsigned char)((PatientType)_patientVaulue));

    QString value = _chargePressure->getText();
    _inflatePressure = value.toInt();

    nibpParam.provider().servicePressureinflate(_inflatePressure);
}

/**************************************************************************************************
 * 放气槽函数。
 *************************************************************************************************/
void NIBPPressureControl::_deflateReleased(void)
{
    nibpParam.provider().servicePressuredeflate();
}

/**************************************************************************************************
 * 充气压力值设定。
 *************************************************************************************************/
void NIBPPressureControl::_pressureChange(QString value)
{
//    UnitType unit = nibpParam.getUnit();
//    if (unit == UNIT_KPA)
//    {
//        int i = 0;
//        Unit::convert(UNIT_MMHG, i, unit, value.toDouble());
//        _inflate_pressure = i;
//        return;
//    }
    if (_pressureControlFlag)
    {
        _changeReleased();
        _inflatePressure = value.toInt();
    }
    else
    {
        nibpRepairMenuManager.messageBox();
    }
}

/**************************************************************************************************
 * 病人类型槽函数。
 *************************************************************************************************/
void NIBPPressureControl::_patientInflate(int index)
{
    if (_pressureControlFlag)
    {
        _changeReleased();
//        PatientType type = (PatientType)index;
//        if( type == PATIENT_TYPE_NULL )
        if (_patientType->itemText(index) == trs(PatientSymbol::convert(PATIENT_TYPE_NULL)))
        {
            nibpParam.provider().servicePressureProtect(false);
        }
        else
        {
            nibpParam.provider().servicePressureProtect(true);
            _patientVaulue = index;
        }
    }
    else
    {
        nibpRepairMenuManager.messageBox();
    }
}

/**************************************************************************************************
 * 压力值。
 *************************************************************************************************/
void NIBPPressureControl::setCuffPressure(int pressure)
{
    if (pressure == -1)
    {
        _value->setText(InvStr());
    }
    else
    {
        UnitType unit = nibpParam.getUnit();
        if (unit == UNIT_MMHG)
        {
            _value->setNum(pressure);
            return;
        }

        QString str = Unit::convert(unit, UNIT_MMHG, pressure);
        _value->setText(str);
    }
}

/**************************************************************************************************
 * 进入模式应答。
 *************************************************************************************************/
void NIBPPressureControl::unPacket(bool flag)
{
    _pressureControlFlag = flag;
    if( flag )
    {
        _value->setText(InvStr());
        _patientType->setDisabled(false);
        _chargePressure->disable(false);
        _chargePressure->setEnabled(true);
        _InflateBtn->setEnabled(true);
    }
    else
    {
        _value->setText("999");
        _patientType->setDisabled(true);
        _chargePressure->disable(true);
        _chargePressure->setEnabled(false);
        _InflateBtn->setEnabled(false);
        QWidget *lastWin = mainLayout->itemAt(mainLayout->count() - 1)->widget();
        lastWin->setFocus();
    }
}

/**************************************************************************************************
 * 充气、放气按钮切换的控制。
 *************************************************************************************************/
void NIBPPressureControl::btnSwitch(bool inflate)
{
    if(inflate)
    {
        _InflateBtn->setText(trs("ServiceInflate"));
        _inflatePressure = true;
    }
    else
    {
        _InflateBtn->setText(trs("ServiceDeflate"));
        _inflatePressure = false;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPPressureControl::NIBPPressureControl() : SubMenu(trs("ServicePressureControl"))
{
    setDesc(trs("NIBPPressureControl"));

    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPPressureControl::~NIBPPressureControl()
{

}

