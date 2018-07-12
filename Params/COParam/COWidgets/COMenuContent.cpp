/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/10
 **/

#include "COMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include "Button.h"
#include <QGridLayout>
#include <QList>
#include "COSymbol.h"
#include "SoundManager.h"
#include "SystemManager.h"
#include "COParam.h"
#include "IConfig.h"
#include "NumberInput.h"
#include "IMessageBox.h"

class COMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_CO_RATIO = 1,
        ITEM_CBO_INJECT_TEMP_SOURCE,
        ITEM_CBO_INJECTION_TEMP,
        ITEM_CBO_INJECTION_VOLUMN,
        ITEM_CBO_MEASURE_CONTROL,
    };

    COMenuContentPrivate() : measureSta(CO_INST_START){}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> buttons;
    COInstCtl measureSta;              // 测量状态
};

void COMenuContentPrivate::loadOptions()
{
    buttons[ITEM_CBO_CO_RATIO]->setText(QString::number((double)coParam.getCORatio() / 1000));
    combos[ITEM_CBO_INJECT_TEMP_SOURCE]->setCurrentIndex(coParam.getTempSource());
    if (combos[ITEM_CBO_INJECT_TEMP_SOURCE]->currentIndex() == CO_TI_MODE_AUTO)
    {
        buttons[ITEM_CBO_INJECTION_TEMP]->setEnabled(false);
    }
    else
    {
        buttons[ITEM_CBO_INJECTION_TEMP]->setEnabled(true);
    }
    buttons[ITEM_CBO_INJECTION_TEMP]->setText(QString::number((double)coParam.getInjectionTemp() / 10));
    buttons[ITEM_CBO_INJECTION_VOLUMN]->setText(QString::number(coParam.getInjectionVolumn()));
    buttons[ITEM_CBO_MEASURE_CONTROL]->setText(trs(COSymbol::convert(coParam.getMeasureCtrl())));
}

COMenuContent::COMenuContent()
    : MenuContent(trs("COMenu"), trs("COMenuDesc")),
      d_ptr(new COMenuContentPrivate)
{

}

COMenuContent::~COMenuContent()
{
    delete d_ptr;
}

void COMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void COMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    Button *button;
    QLabel *label;
    int itemID;

    // Ratio
    label = new QLabel(trs("CORatio"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button("0.542");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_CO_RATIO);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_CBO_CO_RATIO, button);

    // temp source
    label = new QLabel(trs("InjectionTempSource"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(COSymbol::convert(CO_TI_MODE_AUTO))
                       << trs(COSymbol::convert(CO_TI_MODE_MANUAL))
                       );
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_INJECT_TEMP_SOURCE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->combos.insert(COMenuContentPrivate::ITEM_CBO_INJECT_TEMP_SOURCE, comboBox);

    // injection temp
    label = new QLabel(trs("InjectionTemp"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button("20");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP, button);

    // injection volumn
    label = new QLabel(trs("InjectionVolumn"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button("10");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_INJECTION_VOLUMN);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_CBO_INJECTION_VOLUMN, button);

    // measure contrl
    label = new QLabel(trs("MeasureControl"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button(trs("COStart"));
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_MEASURE_CONTROL);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_CBO_MEASURE_CONTROL, button);

    layout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count(), 1);
}

void COMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        COMenuContentPrivate::MenuItem item
                = (COMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case COMenuContentPrivate::ITEM_CBO_INJECT_TEMP_SOURCE:
        {
            if (index == (int)CO_TI_MODE_MANUAL)
            {
                d_ptr->buttons.value(COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP)->setEnabled(true);
            }
            else if (index == (int)CO_TI_MODE_AUTO)
            {
                d_ptr->buttons.value(COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP)->setEnabled(false);
            }
            int temp = d_ptr->buttons[COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP]->text().toFloat() * 10;
            coParam.setTempSource((COTiMode)index, temp);
            break;
        }
        default:
            break;
        }
    }

}

void COMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        COMenuContentPrivate::MenuItem item
                 = (COMenuContentPrivate::MenuItem) button->property("Item").toInt();
        switch (item) {
        case COMenuContentPrivate::ITEM_CBO_CO_RATIO:
        {

            NumberInput numberPad;
            numberPad.setTitleBarText(trs("CORatio"));
            numberPad.setMaxInputLength(5);
            numberPad.setInitString(button->text());
            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                float value = text.toFloat(&ok);
                unsigned short actualValue = value * 1000;
                if (ok)
                {
                    if (actualValue >= 1 && actualValue <= 999)
                    {
                        button->setText(text);
                        coParam.setCORatio(actualValue);
                    }
                    else
                    {
                        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.001-0.999", QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }

                }
            }
            break;
        }
        case COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP:
        {
            NumberInput numberPad;
            numberPad.setTitleBarText(trs("InjectionTemp"));
            numberPad.setMaxInputLength(4);
            numberPad.setInitString(button->text());
            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                float value = text.toFloat(&ok);
                unsigned short actualValue = value * 10;
                if (ok)
                {
                    if (actualValue <= 270)
                    {
                        button->setText(text);
                        coParam.setTempSource(CO_TI_MODE_MANUAL, actualValue);
                    }
                    else
                    {
                        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.0-27.0", QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }

                }
            }
            break;
        }
        case COMenuContentPrivate::ITEM_CBO_INJECTION_VOLUMN:
        {
            NumberInput numberPad;
            numberPad.setTitleBarText(trs("InjectionVolumn"));
            numberPad.setMaxInputLength(3);
            numberPad.setInitString(button->text());
            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                unsigned int value = text.toInt(&ok);
                if (ok)
                {
                    if (value >= 1 && value <= 200)
                    {
                        button->setText(text);
                        coParam.setInjectionVolumn((unsigned char)value);
                    }
                    else
                    {
                        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "1-200", QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }

                }
            }
            break;
        }
        case COMenuContentPrivate::ITEM_CBO_MEASURE_CONTROL:
        {
            coParam.measureCtrl(d_ptr->measureSta);
            if (d_ptr->measureSta == CO_INST_START)
            {
                button->setText(trs("COEnd"));
                d_ptr->measureSta = CO_INST_END;
            }
            else if (d_ptr->measureSta == CO_INST_END)
            {
                button->setText(trs("COStart"));
                d_ptr->measureSta = CO_INST_START;
            }
            break;
        }
        default:
            break;
        }
    }
}
