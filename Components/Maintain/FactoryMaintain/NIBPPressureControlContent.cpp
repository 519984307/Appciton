/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "ComboBox.h"
#include "NIBPPressureControlContent.h"
#include "NIBPParam.h"
#include "NIBPSymbol.h"
#include "PatientManager.h"
#include "SpinBox.h"
#include "NIBPRepairMenuWindow.h"
#include "Button.h"
#include "MenuWindow.h"
#include <QLayout>
#include "NIBPProviderIFace.h"
#include "NIBPServiceStateDefine.h"

class NIBPPressureControlContentPrivate
{
public:
    NIBPPressureControlContentPrivate();
    SpinBox *chargePressure;          // 设定充气压力值
    ComboBox *patientType;           // 病人类型
    Button *inflateBtn;             // 充气、放气控制按钮
    QLabel *value;
    int inflatePressure;               //  充气压力值
    int patientVaulue;                  //  病人类型
    bool inflateSwitch;                 //  充气、放气标志
    bool pressureControlFlag;          //  进入模式标志
};

NIBPPressureControlContentPrivate::NIBPPressureControlContentPrivate()
    : chargePressure(NULL),
      patientType(NULL),
      inflateBtn(NULL),
      value(NULL),
      inflatePressure(0),
      patientVaulue(0),
      inflateSwitch(0),
      pressureControlFlag(false)
{
}

// 压力控制模式
/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void NIBPPressureControlContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QHBoxLayout *hl = new QHBoxLayout();
    QLabel *l = new QLabel(trs("ServicePressure"));
    hl->addWidget(l);

    d_ptr->value = new QLabel();
    d_ptr->value->setText(InvStr());
    hl->addWidget(d_ptr->value);

    l = new QLabel();
    l->setText(Unit::getSymbol(nibpParam.getUnit()));
    hl->addWidget(l);

    layout->addLayout(hl, 0, 0);

    hl = new QHBoxLayout();
    l = new QLabel(trs("ServicePatientType"));
    hl->addWidget(l);
    d_ptr->patientType = new ComboBox();
    d_ptr->patientType->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_ADULT)));
    d_ptr->patientType->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_PED)));
    d_ptr->patientType->addItem(trs(PatientSymbol::convert(PATIENT_TYPE_NULL)));
    connect(d_ptr->patientType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(patientInflate(int)));
    hl->addWidget(d_ptr->patientType);
    layout->addLayout(hl, 1, 0);

    hl = new QHBoxLayout();
    l = new QLabel(trs("ServiceChargePressure"));
    hl->addWidget(l);
    d_ptr->chargePressure = new SpinBox();
//    d_ptr->chargePressure->setSuffix(Unit::getSymbol(UNIT_MMHG));
//    d_ptr->chargePressure->setSuffixSpace(2);
//    d_ptr->chargePressure->setMode(ISPIN_MODE_INT);
    d_ptr->chargePressure->setRange(50, 300);
    d_ptr->chargePressure->setValue(250);
    d_ptr->chargePressure->setStep(5);
    connect(d_ptr->chargePressure, SIGNAL(valueChange(int, int)), this, SLOT(pressureChange(int)));
    hl->addWidget(d_ptr->chargePressure);
    layout->addLayout(hl, 2, 0);


    hl = new QHBoxLayout();
    l = new QLabel(trs("ServiceInflate"));
    hl->addWidget(l);
    d_ptr->inflateBtn = new Button();
    d_ptr->inflateBtn->setText(trs("ServiceInflate"));
    d_ptr->inflateBtn->setButtonStyle(Button::ButtonTextOnly);
    d_ptr->inflatePressure = true;
    connect(d_ptr->inflateBtn, SIGNAL(released()), this, SLOT(inflateBtnReleased()));
    hl->addWidget(d_ptr->inflateBtn);
    layout->addLayout(hl, 3, 0);

    layout->setRowStretch(4, 1);
}

/**************************************************************************************************
 * 初始化。
 *************************************************************************************************/
void NIBPPressureControlContent::init(void)
{
    d_ptr->value->setText(InvStr());
    d_ptr->chargePressure->setValue(250);
//    d_ptr->chargePressure->disable(false);
    d_ptr->chargePressure->setEnabled(true);
    btnSwitch(true);

    d_ptr->patientVaulue = 0;
    d_ptr->patientType->setCurrentIndex(d_ptr->patientVaulue);

    nibpParam.provider().servicePressureProtect(true);

    d_ptr->patientType->setDisabled(false);
    d_ptr->inflateBtn->setEnabled(true);

    d_ptr->pressureControlFlag = false;
}

/**************************************************************************************************
 * 进入压力控制指令。
 *************************************************************************************************/
bool NIBPPressureControlContent::focusNextPrevChild(bool next)
{
    init();

    nibpParam.switchState(NIBP_SERVICE_PRESSURECONTROL_STATE);

    MenuContent::focusNextPrevChild(next);

    return next;
}

/**************************************************************************************************
 * 病人类型，充气值改变指令
 *************************************************************************************************/
void NIBPPressureControlContent::changeReleased(void)
{
    if (!d_ptr->inflatePressure)
    {
        deflateReleased();
    }
}

/**************************************************************************************************
 * 充气、放气控制按钮。
 *************************************************************************************************/
void NIBPPressureControlContent::inflateBtnReleased()
{
    if (d_ptr->pressureControlFlag)
    {
        if (d_ptr->inflatePressure)
        {
            inflateReleased();
            btnSwitch(false);
        }
        else
        {
            deflateReleased();
        }
    }
    else
    {
        NIBPRepairMenuWindow::getInstance()->messageBox();
    }
}

/**************************************************************************************************
 * 充气槽函数。
 *************************************************************************************************/
void NIBPPressureControlContent::inflateReleased()
{
//    PatientType type = (PatientType)(d_ptr->patientType->currentIndex());
//    if ( type == PATIENT_TYPE_NULL )
    int index = d_ptr->patientType->currentIndex();
    if (d_ptr->patientType->itemText(index) == trs(PatientSymbol::convert(PATIENT_TYPE_NULL)))
    {
        nibpParam.provider().servicePressureProtect(false);
    }
    else
    {
        nibpParam.provider().servicePressureProtect(true);
    }

    nibpParam.provider().setPatientType((unsigned char)((PatientType)d_ptr->patientVaulue));

    int value = d_ptr->chargePressure->getValue();
    d_ptr->inflatePressure = value;

    nibpParam.provider().servicePressureinflate(d_ptr->inflatePressure);
}

/**************************************************************************************************
 * 放气槽函数。
 *************************************************************************************************/
void NIBPPressureControlContent::deflateReleased(void)
{
    nibpParam.provider().servicePressuredeflate();
}

/**************************************************************************************************
 * 充气压力值设定。
 *************************************************************************************************/
void NIBPPressureControlContent::pressureChange(int value)
{
//    UnitType unit = nibpParam.getUnit();
//    if (unit == UNIT_KPA)
//    {
//        int i = 0;
//        Unit::convert(UNIT_MMHG, i, unit, value.toDouble());
//        _inflate_pressure = i;
//        return;
//    }
    if (d_ptr->pressureControlFlag)
    {
        changeReleased();
        d_ptr->inflatePressure = value;
    }
    else
    {
        NIBPRepairMenuWindow::getInstance()->messageBox();
    }
}

/**************************************************************************************************
 * 病人类型槽函数。
 *************************************************************************************************/
void NIBPPressureControlContent::patientInflate(int index)
{
    if (d_ptr->pressureControlFlag)
    {
        changeReleased();
//        PatientType type = (PatientType)index;
//        if( type == PATIENT_TYPE_NULL )
        if (d_ptr->patientType->itemText(index) == trs(PatientSymbol::convert(PATIENT_TYPE_NULL)))
        {
            nibpParam.provider().servicePressureProtect(false);
        }
        else
        {
            nibpParam.provider().servicePressureProtect(true);
            d_ptr->patientVaulue = index;
        }
    }
    else
    {
        NIBPRepairMenuWindow::getInstance()->messageBox();
    }
}

/**************************************************************************************************
 * 压力值。
 *************************************************************************************************/
void NIBPPressureControlContent::setCuffPressure(int pressure)
{
    if (pressure == -1)
    {
        d_ptr->value->setText(InvStr());
    }
    else
    {
        UnitType unit = nibpParam.getUnit();
        if (unit == UNIT_MMHG)
        {
            d_ptr->value->setNum(pressure);
            return;
        }

        QString str = Unit::convert(unit, UNIT_MMHG, pressure);
        d_ptr->value->setText(str);
    }
}

/**************************************************************************************************
 * 进入模式应答。
 *************************************************************************************************/
void NIBPPressureControlContent::unPacket(bool flag)
{
    d_ptr->pressureControlFlag = flag;
    if (flag)
    {
        d_ptr->value->setText(InvStr());
        d_ptr->patientType->setDisabled(false);
//        d_ptr->chargePressure->disable(false);
        d_ptr->chargePressure->setEnabled(true);
        d_ptr->inflateBtn->setEnabled(true);
    }
    else
    {
        d_ptr->value->setText("999");
        d_ptr->patientType->setDisabled(true);
//        d_ptr->chargePressure->disable(true);
        d_ptr->chargePressure->setEnabled(false);
        d_ptr->inflateBtn->setEnabled(false);
        QLayout *layout = getMenuWindow()->getWindowLayout();
        QWidget *lastWin = layout->itemAt(layout->count() - 1)->widget();
        lastWin->setFocus();
    }
}

/**************************************************************************************************
 * 充气、放气按钮切换的控制。
 *************************************************************************************************/
void NIBPPressureControlContent::btnSwitch(bool inflate)
{
    if (inflate)
    {
        d_ptr->inflateBtn->setText(trs("ServiceInflate"));
        d_ptr->inflatePressure = true;
    }
    else
    {
        d_ptr->inflateBtn->setText(trs("ServiceDeflate"));
        d_ptr->inflatePressure = false;
    }
}


NIBPPressureControlContent *NIBPPressureControlContent::getInstance()
{
    static NIBPPressureControlContent *instance = NULL;
    if (!instance)
    {
        instance = new NIBPPressureControlContent();
    }
    return instance;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPPressureControlContent::NIBPPressureControlContent()
    : MenuContent(trs("ServicePressureControl"),
                  trs("ServicePressureControlDesc")),
      d_ptr(new NIBPPressureControlContentPrivate)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPPressureControlContent::~NIBPPressureControlContent()
{
    delete d_ptr;
}

