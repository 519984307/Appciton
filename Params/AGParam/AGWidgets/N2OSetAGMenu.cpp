/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/


#include "N2OSetAGMenu.h"
#include "IComboList.h"
#include "UnitManager.h"
#include "CO2Symbol.h"
#include "ConfigManager.h"
#include "MenuManager.h"
#include "FontManager.h"
#include "ComboBox.h"
#include "LanguageManager.h"

enum N2OSetType
{
    OPERATION_MODE = 0,
    N2O_SET_MAX
};

class N2OSetAGMenuPrivate
{
public:
    ComboBox *combos[N2O_SET_MAX];
    QLabel *labels[N2O_SET_MAX];
};

N2OSetAGMenu::~N2OSetAGMenu()
{
}

N2OSetAGMenu::N2OSetAGMenu(): Dialog(), d_ptr(new N2OSetAGMenuPrivate())
{
    setWindowTitle(trs("N2OOfAGSetUp"));

    QStringList typeName;
    typeName.append("OperationMode");

    QStringList comboList[N2O_SET_MAX];
    for (int i = 0; i < OPERATION_MODE_NR; i++)
    {
        comboList[OPERATION_MODE].append(CO2Symbol::convert(CO2OperationMode(i)));
    }

    int typeIndex[N2O_SET_MAX] = {0};
    for (int i = 0; i < N2O_SET_MAX; i++)
    {
        int index = 0;
        currentConfig.getNumValue(QString("AG|%1").arg(typeName.at(i)), index);
        typeIndex[i] = index;
    }
    QGridLayout *gl = new QGridLayout();
    gl->setContentsMargins(10, 10, 10, 10);
    for (int i = 0; i < N2O_SET_MAX; i++)
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

void N2OSetAGMenu::onComboIndexChanged(int index)
{
    ComboBox *combos = qobject_cast<ComboBox *>(sender());
    int typeIndex = combos->property("comboId").toInt();
    QString typeName("");
    switch ((N2OSetType)typeIndex)
    {
    case OPERATION_MODE:
        typeName = "OperationMode";
        break;
    case N2O_SET_MAX:
        typeName = "";
        break;
    }
    if (!typeName.isEmpty())
    {
        currentConfig.setNumValue(QString("AG|%1").arg(typeName), index);
    }
}

