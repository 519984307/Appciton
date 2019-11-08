/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/17
 **/

#include "WiredNetworkMaintainMenuContent.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IConfig.h"
#include "Button.h"
#include "KeyInputPanel.h"

#define MAX_IP_STRING_LENGTH 15

class WiredNetworkMaintainMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ADDRESS_TYPE,
        ITEM_BTN_IP_ADDRESS,
        ITEM_BTN_SUBNET_MASK,
        ITEM_BTN_GATEWAY
    };

    explicit WiredNetworkMaintainMenuContentPrivate(WiredNetworkMaintainMenuContent *const q_ptr)
        : q_ptr(q_ptr), addressTypeCbx(NULL)
    {}

    void loadOptions();

    WiredNetworkMaintainMenuContent *const q_ptr;
    ComboBox *addressTypeCbx;
    QMap<MenuItem, Button *> buttons;
};

void WiredNetworkMaintainMenuContentPrivate::loadOptions()
{
    QString tmpStr;
    systemConfig.getStrValue("WiredWork|IpAddress", tmpStr);
    if (tmpStr.isNull())
    {
        buttons[ITEM_BTN_IP_ADDRESS]->setText("0.0.0.0");
    }
    else
    {
        buttons[ITEM_BTN_IP_ADDRESS]->setText(tmpStr);
    }
    tmpStr.clear();

    systemConfig.getStrValue("WiredWork|SubnetMask", tmpStr);
    if (tmpStr.isNull())
    {
        buttons[ITEM_BTN_SUBNET_MASK]->setText("0.0.0.0");
    }
    else
    {
        buttons[ITEM_BTN_SUBNET_MASK]->setText(tmpStr);
    }
    tmpStr.clear();

    systemConfig.getStrValue("WiredWork|GateWay", tmpStr);
    if (tmpStr.isNull())
    {
        buttons[ITEM_BTN_GATEWAY]->setText("255.255.255.0");
    }
    else
    {
        buttons[ITEM_BTN_GATEWAY]->setText(tmpStr);
    }
    tmpStr.clear();

    systemConfig.getStrValue("WiredWork|AddressType", tmpStr);
    if (tmpStr.toInt() >=  addressTypeCbx->count())
    {
        addressTypeCbx->setCurrentIndex(0);
    }
    else
    {
        addressTypeCbx->setCurrentIndex(tmpStr.toInt());
        if (tmpStr.toInt())
        {
            q_ptr->onComboBoxIndexChanged(tmpStr.toInt());
        }
    }
    tmpStr.clear();
}

WiredNetworkMaintainMenuContent::WiredNetworkMaintainMenuContent()
    : MenuContent(trs("WiredWorkMaintainMenu"), trs("WiredWorkMaintainMenuDesc")),
      d_ptr(new WiredNetworkMaintainMenuContentPrivate(this))
{
}

WiredNetworkMaintainMenuContent::~WiredNetworkMaintainMenuContent()
{
    delete d_ptr;
}

void WiredNetworkMaintainMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void WiredNetworkMaintainMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    QLabel *label;
    Button *button;
    int itemID;

    // addressType
    label = new QLabel(trs("AddressType"));
    layout->addWidget(label, 0, 0);
    d_ptr->addressTypeCbx = new ComboBox;
    d_ptr->addressTypeCbx->addItems(QStringList()
                                 << trs("Static")
                                 << trs("DHCP"));
    itemID = static_cast<int>(WiredNetworkMaintainMenuContentPrivate::ITEM_CBO_ADDRESS_TYPE);
    d_ptr->addressTypeCbx->setProperty("Item", qVariantFromValue(itemID));
    connect(d_ptr->addressTypeCbx, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(d_ptr->addressTypeCbx, 0, 1);

    // ip address
    label = new QLabel(trs("IpAddress"));
    layout->addWidget(label, 1 + d_ptr->buttons.count(), 0);
    button = new Button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_IP_ADDRESS);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, 1 + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_IP_ADDRESS, button);

    // subnet mask
    label = new QLabel(trs("SubnetMask"));
    layout->addWidget(label, 1 + d_ptr->buttons.count(), 0);
    button = new Button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_SUBNET_MASK);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, 1 + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_SUBNET_MASK, button);

    // gateway
    label = new QLabel(trs("GateWay"));
    layout->addWidget(label, 1 + d_ptr->buttons.count(), 0);
    button = new Button;
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_GATEWAY);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, 1 + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_GATEWAY, button);

    layout->setRowStretch(1 + d_ptr->buttons.count(), 1);
}

void WiredNetworkMaintainMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        WiredNetworkMaintainMenuContentPrivate::MenuItem item
            = (WiredNetworkMaintainMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case WiredNetworkMaintainMenuContentPrivate::ITEM_CBO_ADDRESS_TYPE:
            systemConfig.setNumValue("WiredWork|AddressType", index);
            d_ptr->buttons[WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_IP_ADDRESS]->setEnabled(!index);
            d_ptr->buttons[WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_SUBNET_MASK]->setEnabled(!index);
            d_ptr->buttons[WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_GATEWAY]->setEnabled(!index);
            break;
        default:
            break;
        }
    }
}

static bool isIpStrValid(const QString &ipStr)
{
    QString pattern = "^(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                      "(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                      "(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                      "(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])$";
    QRegExp reg(pattern);
    return reg.exactMatch(ipStr);
}

void WiredNetworkMaintainMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        QString string;
        KeyInputPanel englishPanel;
        englishPanel.setMaxInputLength(MAX_IP_STRING_LENGTH);
        englishPanel.setInitString(button->text());
        englishPanel.setSpaceEnable(false);
        englishPanel.setBtnEnable("[0-9.]");
        englishPanel.setCheckValueHook(isIpStrValid);

        WiredNetworkMaintainMenuContentPrivate::MenuItem item
            = (WiredNetworkMaintainMenuContentPrivate::MenuItem)button->property("Item").toInt();
        switch (item)
        {
        case WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_IP_ADDRESS:
            englishPanel.setWindowTitle(trs("IpAddress"));
            string = "IpAddress";
            break;
        case WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_SUBNET_MASK:
            englishPanel.setWindowTitle(trs("GateWay"));
            string = "GateWay";
            break;
        case WiredNetworkMaintainMenuContentPrivate::ITEM_BTN_GATEWAY:
            englishPanel.setWindowTitle(trs("SubnetMask"));
            string = "SubnetMask";
            break;
        default:
            break;
        }

        if (englishPanel.exec())
        {
            QString text = englishPanel.getStrValue();
            systemConfig.setStrValue(QString("WiredWork|%1").arg(string), text);
            button->setText(text);
        }
    }
}
