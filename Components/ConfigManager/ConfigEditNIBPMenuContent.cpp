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

class ConfigEditNIBPMenuContentPrivate
{
public:
    explicit ConfigEditNIBPMenuContentPrivate(Config *const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    Config *const config;
    SpinBox *initCuffSpb;
};

ConfigEditNIBPMenuContentPrivate
    ::ConfigEditNIBPMenuContentPrivate(Config *const config)
    :config(config), initCuffSpb(NULL)
{
}

ConfigEditNIBPMenuContent::ConfigEditNIBPMenuContent(Config *const config):
    MenuContent(trs("NIBPMenu"),
                trs("NIBPMenuDesc")),
    d_ptr(new ConfigEditNIBPMenuContentPrivate(config))
{
}

ConfigEditNIBPMenuContent::~ConfigEditNIBPMenuContent()
{
    delete d_ptr;
}

void ConfigEditNIBPMenuContentPrivate::loadOptions()
{
    PatientType type = patientManager.getType();
    int initVal;
    if (type == PATIENT_TYPE_ADULT)
    {
        initCuffSpb->setRange(120, 280);
    }
    else if (type == PATIENT_TYPE_PED)
    {
        initCuffSpb->setRange(80, 250);
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        initCuffSpb->setRange(60, 140);
    }
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
    label = new QLabel(trs("NIBPInitialCuff"));
    layout->addWidget(label, count, 0);
    d_ptr->initCuffSpb = new SpinBox();
    d_ptr->initCuffSpb->setStep(10);
    connect(d_ptr->initCuffSpb, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxReleased(int)));
    QHBoxLayout *hLayout = new QHBoxLayout();
    label = new QLabel("mmHg");
    hLayout->addWidget(d_ptr->initCuffSpb);
    hLayout->addWidget(label);
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
