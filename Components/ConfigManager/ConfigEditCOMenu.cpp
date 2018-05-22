//////////////////////////////////////////////
#include "ConfigEditCOMenu.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "IComboList.h"
#include "LabelButton.h"
#include "ISpinBox.h"
#include "LanguageManager.h"
#include "SetWidget.h"
#include "PublicMenuManager.h"
#include "NumberInput.h"
#include "IMessageBox.h"
#include "NumberInput.h"
#include "COParam.h"
#include "ConfigManager.h"
#include "Config.h"
#include "ConfigEditMenuGrp.h"

ConfigCOMenu *ConfigCOMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ConfigCOMenu::ConfigCOMenu() : SubMenu(trs("C.O.")),
                               _ductRatio(NULL),
                               _inputMode(NULL),
                               _injectionTemp(NULL),
                               _injectionVolumn(NULL),
                               _measureMode(NULL),
                               _start(NULL),
                              _measureSta(CO_INST_START)
{
    setDesc(trs("C.O.Desc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ConfigCOMenu::~ConfigCOMenu()
{

}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void ConfigCOMenu::layoutExec()
{
    int submenuW = publicMenuManager.getSubmenuWidth();
    int submenuH = publicMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    Config *config = configEditMenuGrp.getCurrentEditConfig();
    float index  = 0;

    config->getNumValue("CO|ratdio", index);

    _ductRatio = new LabelButton(trs("CORatio"));
    _ductRatio->setFont(defaultFont());
    _ductRatio->button->setText(QString::number(index));
    _ductRatio->label->setFixedSize(labelWidth, ITEM_H);
    _ductRatio->button->setFixedSize(btnWidth, ITEM_H);
    connect(_ductRatio->button, SIGNAL(realReleased()), this, SLOT(_ductRatioReleased()));
    mainLayout->addWidget(_ductRatio);

    config->getNumValue("CO|InjectionTempSource", index);
    _inputMode = new IComboList(trs("InjectionTempSource"));
    _inputMode->setFont(defaultFont());
    _inputMode->addItem(trs("Auto"));
    _inputMode->addItem(trs("Manual"));
    _inputMode->label->setFixedSize(labelWidth, ITEM_H);
    _inputMode->combolist->setFixedSize(btnWidth, ITEM_H);
    _inputMode->combolist->setCurrentIndex(index);
    connect(_inputMode, SIGNAL(currentIndexChanged(int)), this, SLOT(_inputModeSlot(int)));
    mainLayout->addWidget(_inputMode);

    config->getNumValue("CO|InjectionTemp", index);
    _injectionTemp = new LabelButton(trs("InjectionTemp"));
    _injectionTemp->setFont(defaultFont());
    _injectionTemp->button->setText(QString::number(index));
    _injectionTemp->label->setFixedSize(labelWidth, ITEM_H);
    _injectionTemp->button->setFixedSize(btnWidth, ITEM_H);
    _injectionTemp->button->setEnabled(false);
    _injectionTemp->button->setFocusPolicy(Qt::NoFocus);
    connect(_injectionTemp->button, SIGNAL(realReleased()), this, SLOT(_injectionTempReleased()));
    mainLayout->addWidget(_injectionTemp);

    config->getNumValue("CO|InjectionVolumn", index);
    _injectionVolumn = new LabelButton(trs("InjectionVolumn"));
    _injectionVolumn->setFont(defaultFont());
    _injectionVolumn->button->setText(QString::number(index));
    _injectionVolumn->label->setFixedSize(labelWidth, ITEM_H);
    _injectionVolumn->button->setFixedSize(btnWidth, ITEM_H);
    connect(_injectionVolumn->button, SIGNAL(realReleased()), this, SLOT(_injectionVolumnReleased()));
    mainLayout->addWidget(_injectionVolumn);

     config->getNumValue("CO|MeasureMode", index);
    _measureMode = new IComboList(trs("MeasureMode"));
    _measureMode->setFont(defaultFont());
    _measureMode->addItem(trs("Manual"));
    _measureMode->addItem(trs("Auto"));
    _measureMode->label->setFixedSize(labelWidth, ITEM_H);
    _measureMode->combolist->setFixedSize(btnWidth, ITEM_H);
    _measureMode->combolist->setCurrentIndex(index);
    connect(_measureMode, SIGNAL(currentIndexChanged(int)), this, SLOT(_inputModeSlot(int)));
    mainLayout->addWidget(_measureMode);

    _start = new IButton(trs("COStart"));
    _start->setFont(defaultFont());
    _start->setFixedSize(btnWidth, ITEM_H);
    _start->setBorderEnabled(true);
    connect(_start, SIGNAL(realReleased()), this, SLOT(_startReleased()));
    mainLayout->addWidget(_start, 0, Qt::AlignRight);

}

/**************************************************************************************************
 * CO系数设置槽函数。
 *************************************************************************************************/
void ConfigCOMenu::_ductRatioReleased()
{
    NumberInput numberPad;
    numberPad.setTitleBarText(trs("CORatio"));
    numberPad.setMaxInputLength(5);
    numberPad.setInitString(_ductRatio->button->text());
    Config *config = configEditMenuGrp.getCurrentEditConfig();


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
                _ductRatio->button->setText(text);
                config->setNumValue("CO|ratdio",value);/*保存数据*/
            }
            else
            {
                IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.001-0.999", QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }

        }
    }
}

/**************************************************************************************************
 * 注射液温度来源设置槽函数。
 *************************************************************************************************/
void ConfigCOMenu::_inputModeSlot(int index)
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();

    if (index == CO_TI_MODE_MANUAL)
    {
        _injectionTemp->button->setEnabled(true);
        _injectionTemp->button->setFocusPolicy(Qt::StrongFocus);

    }
    else if (index == CO_TI_MODE_AUTO)
    {
        _injectionTemp->button->setEnabled(false);
        _injectionTemp->button->setFocusPolicy(Qt::NoFocus);
        config->setNumValue("CO|InjectionTempSource",index);/*保存数据*/
        //coParam.setTempSource((COTiMode)index);
    }
}

/**************************************************************************************************
 * 手动设置注射液温度槽函数。
 *************************************************************************************************/
void ConfigCOMenu::_injectionTempReleased()
{
    NumberInput numberPad;
    numberPad.setTitleBarText(trs("InjectionTemp"));
    numberPad.setMaxInputLength(4);
    numberPad.setInitString(_injectionTemp->button->text());
    Config *config = configEditMenuGrp.getCurrentEditConfig();

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
                _injectionTemp->button->setText(text);
                //coParam.setTempSource(CO_TI_MODE_MANUAL, actualValue);
                config->setNumValue("CO|InjectionTempSource",value);/*保存数据*/
            }
            else
            {
                IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.0-27.0", QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }

        }
    }
}

/**************************************************************************************************
 * Manually set injection volumn slot funtion.
 *************************************************************************************************/
void ConfigCOMenu::_injectionVolumnReleased()
{
    NumberInput numberPad;
    numberPad.setTitleBarText(trs("InjectionVolumn"));
    numberPad.setMaxInputLength(3);
    numberPad.setInitString(_injectionVolumn->button->text());
     Config *config = configEditMenuGrp.getCurrentEditConfig();
    if (numberPad.exec())
    {
        QString text = numberPad.getStrValue();
        bool ok = false;
        unsigned int value = text.toInt(&ok);
        if (ok)
        {
            if (value >= 1 && value <= 200)
            {
                _injectionVolumn->button->setText(text);
               // coParam.setInjectionVolumn((unsigned char)value);
                config->setNumValue("CO|InjectionTempSource",value);/*保存数据*/
            }
            else
            {
                IMessageBox messageBox(trs("Prompt"), trs("InvalidInput") + "0.0-27.0", QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }

        }
    }
}

/**************************************************************************************************
 * 开始测量槽函数。
 *************************************************************************************************/
void ConfigCOMenu::_startReleased(void)
{
    coParam.measureCtrl(_measureSta);
    if (_measureSta == CO_INST_START)
    {
        _start->setText(trs("Cancel"));
        _measureSta = CO_INST_END;
    }
    else if (_measureSta == CO_INST_END)
    {
        _start->setText(trs("COStart"));
        _measureSta = CO_INST_START;
    }
}

