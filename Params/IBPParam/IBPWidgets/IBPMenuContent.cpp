/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/10
 **/

#include "IBPMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IBPSymbol.h"
#include "IConfig.h"
#include "IBPParam.h"
#include "NumberInput.h"
#include "IMessageBox.h"
#include "Button.h"

class IBPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ENTITLE_1 = 1,
        ITEM_CBO_ENTITLE_2,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_FILTER_MODE,
        ITEM_CBO_SENSITIVITY,
        ITEM_CBO_CALIB_ZERO,
        ITEM_CBO_CALIBRATION
    };

    IBPMenuContentPrivate() {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> buttons;
    IBPPressureName ibp1;
    IBPPressureName ibp2;
};

void IBPMenuContentPrivate::loadOptions()
{
    ibp1 = ibpParam.getEntitle(IBP_INPUT_1);
    ibp2 = ibpParam.getEntitle(IBP_INPUT_2);
    combos[ITEM_CBO_ENTITLE_1]->setCurrentIndex(ibp1);
    combos[ITEM_CBO_ENTITLE_2]->setCurrentIndex(ibp2);
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(ibpParam.getSweepSpeed());
    combos[ITEM_CBO_FILTER_MODE]->setCurrentIndex(ibpParam.getFilter());
    combos[ITEM_CBO_SENSITIVITY]->setCurrentIndex(ibpParam.getSensitivity());
}

IBPMenuContent::IBPMenuContent()
    : MenuContent(trs("IBPMenu"), trs("IBPMenuDesc")),
      d_ptr(new IBPMenuContentPrivate)
{
}

IBPMenuContent::~IBPMenuContent()
{
    delete d_ptr;
}

void IBPMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void IBPMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    Button *button;
    int itemID;

    // 标名选择1
    label = new QLabel(trs("ChannelPressureEntitle1"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << IBPSymbol::convert(IBP_PRESSURE_ART)
                       << IBPSymbol::convert(IBP_PRESSURE_PA)
                       << IBPSymbol::convert(IBP_PRESSURE_CVP)
                       << IBPSymbol::convert(IBP_PRESSURE_LAP)
                       << IBPSymbol::convert(IBP_PRESSURE_RAP)
                       << IBPSymbol::convert(IBP_PRESSURE_ICP)
                       << IBPSymbol::convert(IBP_PRESSURE_AUXP1)
                       << IBPSymbol::convert(IBP_PRESSURE_AUXP2)
                      );
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_1);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_1, comboBox);

    // 标名选择2
    label = new QLabel(trs("ChannelPressureEntitle2"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << IBPSymbol::convert(IBP_PRESSURE_ART)
                       << IBPSymbol::convert(IBP_PRESSURE_PA)
                       << IBPSymbol::convert(IBP_PRESSURE_CVP)
                       << IBPSymbol::convert(IBP_PRESSURE_LAP)
                       << IBPSymbol::convert(IBP_PRESSURE_RAP)
                       << IBPSymbol::convert(IBP_PRESSURE_ICP)
                       << IBPSymbol::convert(IBP_PRESSURE_AUXP1)
                       << IBPSymbol::convert(IBP_PRESSURE_AUXP2)
                      );
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2, comboBox);

    // 波形速度
    label = new QLabel(trs("IBPSweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_62_5)
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_125)
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_250)
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_500)
                      );
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED, comboBox);

    // 滤波模式
    label = new QLabel(trs("FilterMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << IBPSymbol::convert(IBP_FILTER_MODE_0)
                       << IBPSymbol::convert(IBP_FILTER_MODE_1)
                      );
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_FILTER_MODE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_FILTER_MODE, comboBox);

    // 灵敏度
    label = new QLabel(trs("Sensitivity"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_HIGH))
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_MID))
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_LOW))
                      );
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_SENSITIVITY);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(IBPMenuContentPrivate::ITEM_CBO_SENSITIVITY, comboBox);

    // 校零
    label = new QLabel(trs("IBPZeroCalib"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button(trs("IBPZeroStart"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_CALIB_ZERO);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(IBPMenuContentPrivate::ITEM_CBO_CALIB_ZERO, button);

    // 校准
    label = new QLabel(trs("ServiceCalibrate"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button(trs("80"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(IBPMenuContentPrivate::ITEM_CBO_CALIBRATION);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(IBPMenuContentPrivate::ITEM_CBO_CALIBRATION, button);

    layout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count(), 1);
}

void IBPMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        IBPMenuContentPrivate::MenuItem item
            = (IBPMenuContentPrivate::MenuItem) box->property("Item").toInt();
        switch (item)
        {
        case IBPMenuContentPrivate::ITEM_CBO_ENTITLE_1:
        {
            if (index == static_cast<int>(d_ptr->ibp2))
            {
                box = d_ptr->combos.value(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2);
                d_ptr->ibp2 = d_ptr->ibp1;
                box->blockSignals(true);
                box->setCurrentIndex(static_cast<int>(d_ptr->ibp2));
                box->blockSignals(false);
            }
            d_ptr->ibp1 = (IBPPressureName)index;
            ibpParam.setEntitle((IBPPressureName)index, IBP_INPUT_1);
            break;
        }
        case IBPMenuContentPrivate::ITEM_CBO_ENTITLE_2:
        {
            if (index == static_cast<int>(d_ptr->ibp1))
            {
                box = d_ptr->combos.value(IBPMenuContentPrivate::ITEM_CBO_ENTITLE_1);
                d_ptr->ibp1 = d_ptr->ibp2;
                box->blockSignals(true);
                box->setCurrentIndex(static_cast<int>(d_ptr->ibp1));
                box->blockSignals(false);
            }
            d_ptr->ibp2 = (IBPPressureName)index;
            ibpParam.setEntitle((IBPPressureName)index, IBP_INPUT_2);
            break;
        }
        case IBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
            ibpParam.setSweepSpeed((IBPSweepSpeed)index);
            break;
        case IBPMenuContentPrivate::ITEM_CBO_FILTER_MODE:
            ibpParam.setFilter((IBPFilterMode)index);
            break;
        case IBPMenuContentPrivate::ITEM_CBO_SENSITIVITY:
            ibpParam.setSensitivity((IBPSensitivity)index);
            break;
        default:
            break;
        }
    }
}

void IBPMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        IBPMenuContentPrivate::MenuItem item
            = (IBPMenuContentPrivate::MenuItem) button->property("Item").toInt();
        switch (item)
        {
        case IBPMenuContentPrivate::ITEM_CBO_CALIB_ZERO:
            ibpParam.zeroCalibration(IBP_INPUT_1);
            break;
        case IBPMenuContentPrivate::ITEM_CBO_CALIBRATION:
        {
            NumberInput numberPad;
            numberPad.setTitleBarText(trs("ServiceCalibrate"));
            numberPad.setMaxInputLength(7);
            numberPad.setInitString(button->text());
            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                quint16 value = text.toShort(&ok);
                if (ok)
                {
                    if (value >= 80 && value <= 300)
                    {
                        button->setText(text);
                        ibpParam.setCalibration(IBP_INPUT_1, value);
                    }
                    else
                    {
                        IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "80-300", QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        default:
            break;
        }
    }
}
