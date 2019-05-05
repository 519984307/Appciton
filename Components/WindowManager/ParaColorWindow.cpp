/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/11
 **/

#include "ParaColorWindow.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include <QList>
#include "SoundManager.h"
#include "SystemManager.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "ColorManager.h"
#include "WindowManager.h"

class ParaColorWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ECG_COLOR = 0,
        ITEM_CBO_SPO2_COLOR,
        ITEM_CBO_RESP_COLOR,
        ITEM_CBO_NIBP_COLOR,
        ITEM_CBO_CO2_COLOR,
        ITEM_CBO_TEMP_COLOR,
        ITEM_CBO_AG_COLOR,
        ITEM_CBO_IBP_COLOR,
    };

    ParaColorWindowPrivate() {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QStringList colorList;
};

void ParaColorWindowPrivate::loadOptions()
{
    QString color;
    QString nodePath;
    QStringList strList = QStringList()
                          << "ECGColor"
                          << "SPO2Color"
                          << "RESPColor"
                          << "NIBPColor"
                          << "CO2Color"
                          << "TEMPColor"
                          << "AGColor"
                          << "IBPColor";

    for (int i = 0; i < strList.count(); i++)
    {
        color.clear();
        nodePath = QString("Display|%1").arg(strList.at(i));
        currentConfig.getStrValue(nodePath, color);
        MenuItem item = static_cast<MenuItem>(i);
        QMap<MenuItem, ComboBox *>::ConstIterator comboIter = combos.find(item);
        if (comboIter == combos.constEnd())
        {
            continue;
        }
        combos[item]->setCurrentIndex(colorList.indexOf(color));
    }
}

ParaColorWindow::ParaColorWindow()
    : Dialog(),
      d_ptr(new ParaColorWindowPrivate)
{
    QString color;
    currentConfig.getStrValue("Display|AllColors", color);
    d_ptr->colorList = color.split(',', QString::KeepEmptyParts);
    setWindowTitle(trs("ParameterColorDesc"));
    setFixedSize(480, windowManager.getPopWindowHeight());
    layoutExec();
}

ParaColorWindow::~ParaColorWindow()
{
    delete d_ptr;
}

void ParaColorWindow::showEvent(QShowEvent *ev)
{
    d_ptr->loadOptions();
    Dialog::showEvent(ev);
}

void ParaColorWindow::layoutExec()
{
    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(10);
    layout->setMargin(10);
    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // ECG
    label = new QLabel(trs("ECG"));
    label->setFixedWidth(120);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < d_ptr->colorList.count(); i ++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    itemID = static_cast<int>(ParaColorWindowPrivate::ITEM_CBO_ECG_COLOR);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ParaColorWindowPrivate::ITEM_CBO_ECG_COLOR, comboBox);

    // SPO2
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        label = new QLabel(trs("SPO2"));
        comboBox = new ComboBox();
        for (int i = 0; i < d_ptr->colorList.count(); i ++)
        {
            comboBox->addItem(trs(d_ptr->colorList.at(i)));
        }
        itemID = static_cast<int>(ParaColorWindowPrivate::ITEM_CBO_SPO2_COLOR);
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ParaColorWindowPrivate::ITEM_CBO_SPO2_COLOR, comboBox);
    }

    // NIBP
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        label = new QLabel(trs("NIBP"));
        comboBox = new ComboBox();
        for (int i = 0; i < d_ptr->colorList.count(); i ++)
        {
            comboBox->addItem(trs(d_ptr->colorList.at(i)));
        }
        itemID = static_cast<int>(ParaColorWindowPrivate::ITEM_CBO_NIBP_COLOR);
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ParaColorWindowPrivate::ITEM_CBO_NIBP_COLOR, comboBox);
    }

    // CO2
    if (systemManager.isSupport(CONFIG_CO2))
    {
        label = new QLabel(trs("CO2"));
        comboBox = new ComboBox();
        for (int i = 0; i < d_ptr->colorList.count(); i ++)
        {
            comboBox->addItem(trs(d_ptr->colorList.at(i)));
        }
        itemID = static_cast<int>(ParaColorWindowPrivate::ITEM_CBO_CO2_COLOR);
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ParaColorWindowPrivate::ITEM_CBO_CO2_COLOR, comboBox);
    }

    // RESP
    if (systemManager.isSupport(CONFIG_RESP))
    {
        label = new QLabel(trs("RESP"));
        comboBox = new ComboBox();
        for (int i = 0; i < d_ptr->colorList.count(); i ++)
        {
            comboBox->addItem(trs(d_ptr->colorList.at(i)));
        }
        itemID = static_cast<int>(ParaColorWindowPrivate::ITEM_CBO_RESP_COLOR);
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ParaColorWindowPrivate::ITEM_CBO_RESP_COLOR, comboBox);
    }

    // TEMP
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        label = new QLabel(trs("TEMP"));
        comboBox = new ComboBox();
        for (int i = 0; i < d_ptr->colorList.count(); i ++)
        {
            comboBox->addItem(trs(d_ptr->colorList.at(i)));
        }
        itemID = static_cast<int>(ParaColorWindowPrivate::ITEM_CBO_TEMP_COLOR);
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

        layout->addWidget(label, d_ptr->combos.count(), 0);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ParaColorWindowPrivate::ITEM_CBO_TEMP_COLOR, comboBox);
    }

    // AG color
    if (systemManager.isSupport(CONFIG_AG))
    {
        label = new QLabel(trs("AG"));
        comboBox = new ComboBox;
        for (int i = 0; i < d_ptr->colorList.count(); i++)
        {
            comboBox->addItem(trs(d_ptr->colorList.at(i)));
        }
        itemID = ParaColorWindowPrivate::ITEM_CBO_AG_COLOR;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ParaColorWindowPrivate::ITEM_CBO_AG_COLOR, comboBox);
    }

    // IBP color
    if (systemManager.isSupport(CONFIG_IBP))
    {
        label = new QLabel(trs("IBP"));
        comboBox = new ComboBox;
        for (int i = 0; i < d_ptr->colorList.count(); i++)
        {
            comboBox->addItem(trs(d_ptr->colorList.at(i)));
        }
        itemID = ParaColorWindowPrivate::ITEM_CBO_IBP_COLOR;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ParaColorWindowPrivate::ITEM_CBO_IBP_COLOR, comboBox);
    }

    layout->setRowStretch(d_ptr->combos.count(), 1);

    setWindowLayout(layout);
}

void ParaColorWindow::onComboBoxIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    QString strPath;
    QString dupPath;
    ParamID id = PARAM_NONE;
    if (box)
    {
        ParaColorWindowPrivate::MenuItem item
                = (ParaColorWindowPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case ParaColorWindowPrivate::ITEM_CBO_ECG_COLOR:
        {
            strPath = "Display|ECGColor";
            id = PARAM_ECG;
            dupPath = "Display|ECGDUPColor";
            break;
        }
        case ParaColorWindowPrivate::ITEM_CBO_SPO2_COLOR:
        {
            strPath = "Display|SPO2Color";
            id = PARAM_SPO2;
            break;
        }
        case ParaColorWindowPrivate::ITEM_CBO_NIBP_COLOR:
        {
            strPath = "Display|NIBPColor";
            id = PARAM_NIBP;
            break;
        }
        case ParaColorWindowPrivate::ITEM_CBO_CO2_COLOR:
        {
            strPath = "Display|CO2Color";
            id = PARAM_CO2;
            break;
        }
        case ParaColorWindowPrivate::ITEM_CBO_RESP_COLOR:
        {
            strPath = "Display|RESPColor";
            id = PARAM_RESP;
            break;
        }
        case ParaColorWindowPrivate::ITEM_CBO_TEMP_COLOR:
        {
            strPath = "Display|TEMPColor";
            id = PARAM_TEMP;
            break;
        }
        case ParaColorWindowPrivate::ITEM_CBO_AG_COLOR:
        {
            strPath = "Display|AGColor";
            id = PARAM_AG;
            break;
        }
        case ParaColorWindowPrivate::ITEM_CBO_IBP_COLOR:
        {
            strPath = "Display|IBPColor";
            id = PARAM_IBP;
            break;
        }
        }
        currentConfig.setStrValue(strPath, d_ptr->colorList.at(index));
        if (id == PARAM_ECG)
        {
            currentConfig.setStrValue(dupPath, d_ptr->colorList.at(index));
        }
        colorManager.updateColorPalatte(id);
    }
}

