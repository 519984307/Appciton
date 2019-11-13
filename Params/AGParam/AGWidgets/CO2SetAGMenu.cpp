/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/


#include "CO2SetAGMenu.h"
#include "CO2Symbol.h"
#include "ConfigManager.h"
#include "FontManager.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Utility/Unit.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QGridLayout>

enum CO2SetType
{
    OPERATION_MODE = 0,
    UNIT_SETUP,
    APNEA_ALARM,
    O2_COMPEN,
    N2O_COMPEN,
    CO2_SET_MAX
};

class CO2SetAGMenuPrivate
{
public:
    ComboBox *combos[CO2_SET_MAX];
    QLabel *labels[CO2_SET_MAX];
};

CO2SetAGMenu::~CO2SetAGMenu()
{
}

CO2SetAGMenu::CO2SetAGMenu(): Dialog(), d_ptr(new CO2SetAGMenuPrivate())
{
    setWindowTitle(trs("CO2OfAGSetUp"));

    QStringList typeName;
    typeName.append("OperationMode");
    typeName.append("UnitSetUp");
    typeName.append("ApneaAlarm");
    typeName.append("O2Compensation");
    typeName.append("N2OCompensation");

    QStringList comboList[CO2_SET_MAX];
    for (int i = 0; i < OPERATION_MODE_NR; i++)
    {
        comboList[OPERATION_MODE].append(CO2Symbol::convert(CO2OperationMode(i)));
    }

    comboList[UNIT_SETUP].append(Unit::getSymbol(UNIT_PERCENT));
    comboList[UNIT_SETUP].append(Unit::getSymbol(UNIT_MMHG));
    comboList[UNIT_SETUP].append(Unit::getSymbol(UNIT_KPA));

    for (int i = 0; i < CO2_APNEA_TIME_NR; i++)
    {
        comboList[APNEA_ALARM].append(trs(CO2Symbol::convert(CO2ApneaTime(i))));
    }

    for (int i = 0; i < O2_COMPEN_NR; i++)
    {
        comboList[O2_COMPEN].append(trs(CO2Symbol::convert(O2Compensation(i))));
    }

    for (int i = 0; i < N2O_COMPEN_NR; i++)
    {
        comboList[N2O_COMPEN].append(trs(CO2Symbol::convert(N2OCompensation(i))));
    }

    int typeIndex[CO2_SET_MAX] = {0};
    for (int i = 0; i < CO2_SET_MAX; i++)
    {
        int index = 0;
        currentConfig.getNumValue(QString("AG|%1").arg(typeName.at(i)), index);
        if (i == UNIT_SETUP)
        {
            if (index == UNIT_PERCENT)
            {
                index = 0;
            }
            else if (index == UNIT_MMHG)
            {
                index = 1;
            }
            else if (index == UNIT_KPA)
            {
                index = 2;
            }
            else
            {
                index = 2;
            }
        }
        typeIndex[i] = index;
    }
    QGridLayout *gl = new QGridLayout();
    gl->setContentsMargins(10, 10, 10, 10);
    for (int i = 0; i < CO2_SET_MAX; i++)
    {
        d_ptr->labels[i] = new QLabel(trs(typeName.at(i)));
        d_ptr->labels[i]->setFont(fontManager.textFont(fontManager.getFontSize(2)));
        d_ptr->combos[i] = new ComboBox();
        d_ptr->combos[i]->addItems(comboList[i]);
        d_ptr->combos[i]->setCurrentIndex(typeIndex[i]);
        d_ptr->combos[i]->setProperty("comboId", qVariantFromValue(i));
        d_ptr->combos[i]->setFont(fontManager.textFont(fontManager.getFontSize(2)));
        gl->addWidget(d_ptr->labels[i], i + 1, 0);
        gl->addWidget(d_ptr->combos[i], i + 1, 1);
        connect(d_ptr->combos[i], SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    }
    setWindowLayout(gl);
}

void CO2SetAGMenu::onComboIndexChanged(int index)
{
    ComboBox *combos = qobject_cast<ComboBox *>(sender());
    int typeIndex = combos->property("comboId").toInt();
    QString typeName("");
    switch ((CO2SetType)typeIndex)
    {
    case OPERATION_MODE:
        typeName = "OperationMode";
        break;
    case UNIT_SETUP:
        typeName = "UnitSetUp";
        if (index == 0)
        {
            index = UNIT_PERCENT;
        }
        else if (index == 1)
        {
            index = UNIT_MMHG;
        }
        else if (index == 2)
        {
            index = UNIT_KPA;
        }
        else
        {
            index = UNIT_KPA;
        }
        break;
    case APNEA_ALARM:
        typeName = "ApneaAlarm";
        break;
    case O2_COMPEN:
        typeName = "O2Compensation";
        break;
    case N2O_COMPEN:
        typeName = "N2OCompensation";
        break;
    case CO2_SET_MAX:
        typeName = "";
        break;
    }
    if (!typeName.isEmpty())
    {
        currentConfig.setNumValue(QString("AG|%1").arg(typeName), index);
    }
}


