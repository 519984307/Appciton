#include "ConfigEditGeneralMenu.h"
#include "ConfigEditMenuGrp.h"
#include "LabelButton.h"
#include "KeyBoardPanel.h"


#define MAX_CONFIG_NAME_LEN 64

class ConfigEditGeneralMenuPrivate {
public:
    ConfigEditGeneralMenuPrivate()
        :configName(NULL){}
    LabelButton *configName;
};

ConfigEditGeneralMenu::ConfigEditGeneralMenu()
    :SubMenu(trs("General")), d_ptr(new ConfigEditGeneralMenuPrivate())
{
    setDesc(trs("GeneralSettings"));
    startLayout();
}

ConfigEditGeneralMenu::~ConfigEditGeneralMenu()
{

}

void ConfigEditGeneralMenu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW -  5;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    d_ptr->configName = new LabelButton(trs("ConfigName"));
    d_ptr->configName->setFont(fontManager.textFont(fontSize));
    //d_ptr->configName->setValue(trs("confignametest"));
    d_ptr->configName->label->setFixedSize(labelWidth, ITEM_H);
    d_ptr->configName->button->setFixedSize(btnWidth, ITEM_H);
    connect(d_ptr->configName->button, SIGNAL(released()), this, SLOT(editConfigName()));
    mainLayout->addWidget(d_ptr->configName);
}

void ConfigEditGeneralMenu::readyShow()
{
    d_ptr->configName->button->setText(configEditMenuGrp.getCurrentEditConfigName());
}

void ConfigEditGeneralMenu::editConfigName()
{
    KeyBoardPanel namePanel;
    namePanel.setTitleBarText(trs("SetConfigName"));
    namePanel.setInitString(configEditMenuGrp.getCurrentEditConfigName());
    namePanel.setMaxInputLength(MAX_CONFIG_NAME_LEN);

    QString regKeyStr("[a-zA-Z]|[0-9]|_");
    namePanel.setBtnEnable(regKeyStr);

    if (1 == namePanel.exec())
    {
        QString oldStr = d_ptr->configName->button->text();
        QString text = namePanel.getStrValue();

        if(oldStr != text)
        {
            configEditMenuGrp.setCurrentEditConfigName(text);
            d_ptr->configName->button->setText(text);

        }
    }
}
