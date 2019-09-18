/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/17
 **/
#include "ConfigEditNIBPMenuContent.h"
#include <QMap>
#include <QLabel>
#include "LanguageManager.h"
#include "ComboBox.h"
#include "NIBPSymbol.h"
#include "NIBPDefine.h"
#include <QGridLayout>
#include "PatientManager.h"
#include "Button.h"
#include "ParamInfo.h"
#include "ParamDefine.h"
#include "ConfigManager.h"
#include "SpinBox.h"
#include "IConfig.h"
#include "NIBPParam.h"

class ConfigEditNIBPMenuContentPrivate
{
public:
    explicit ConfigEditNIBPMenuContentPrivate(Config *const config, PatientType type = PATIENT_TYPE_ADULT);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    Config *const config;
    SpinBox *initCuffSpb;
    QStringList initCuffStrs;
    QLabel *initCuffUnitLbl;
    PatientType curType;
};

ConfigEditNIBPMenuContentPrivate
    ::ConfigEditNIBPMenuContentPrivate(Config *const config, PatientType type)
    : config(config)
    , initCuffSpb(NULL)
    , initCuffUnitLbl(NULL)
    , curType(type)
{
}

ConfigEditNIBPMenuContent::ConfigEditNIBPMenuContent(Config *const config, PatientType type):
    MenuContent(trs("NIBPMenu"),
                trs("NIBPMenuDesc")),
    d_ptr(new ConfigEditNIBPMenuContentPrivate(config, type))
{
}

ConfigEditNIBPMenuContent::~ConfigEditNIBPMenuContent()
{
    delete d_ptr;
}

void ConfigEditNIBPMenuContentPrivate::loadOptions()
{
    UnitType defUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
    UnitType unit = nibpParam.getUnit();
    PatientType type = curType;
    int start = 0, end = 0;
    if (type == PATIENT_TYPE_ADULT)
    {
        start = 80;
        end = 240;
    }
    else if (type == PATIENT_TYPE_PED)
    {
        start = 80;
        end = 200;
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        start = 60;
        end = 140;
    }
    initCuffStrs.clear();
    for (int i = start; i <= end; i += 10)
    {
        if (unit == defUnit)
        {
            initCuffStrs.append(QString::number(i));
        }
        else
        {
            initCuffStrs.append(Unit::convert(unit, defUnit, i));
        }
    }
    initCuffSpb->setStringList(initCuffStrs);

    if (unit == defUnit)
    {
        initCuffUnitLbl->setText(Unit::getSymbol(UNIT_MMHG));
    }
    else
    {
        initCuffUnitLbl->setText(Unit::getSymbol(UNIT_KPA));
    }

    // init the 'initVal'
    int initVal = 0;
    config->getNumValue("NIBP|InitialCuffInflation", initVal);
    initCuffSpb->setValue(initVal);
}

void ConfigEditNIBPMenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();
    d_ptr->initCuffSpb->setEnabled(!isOnlyToRead);
}

void ConfigEditNIBPMenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_NIBP_SYS, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

void ConfigEditNIBPMenuContent::onSpinBoxReleased(int value)
{
    d_ptr->config->setNumValue("NIBP|InitialCuffInflation", value);
}

void ConfigEditNIBPMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    int count = 0;

     // initial cuff
    label = new QLabel(trs("NIBPInitialPressure"));
    layout->addWidget(label, count, 0);
    d_ptr->initCuffSpb = new SpinBox();
    d_ptr->initCuffSpb->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_STRING);
    connect(d_ptr->initCuffSpb, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxReleased(int)));
    QHBoxLayout *hLayout = new QHBoxLayout();
    d_ptr->initCuffUnitLbl = new QLabel("mmHg");
    hLayout->addWidget(d_ptr->initCuffSpb);
    hLayout->addWidget(d_ptr->initCuffUnitLbl);
    layout->addLayout(hLayout, count, 1);

    // 添加报警设置链接
    count = count + 1;
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, count, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(count + 1, 1);
}
