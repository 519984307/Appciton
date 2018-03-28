#include <QVBoxLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "LabelButton.h"
#include "FactoryDataRecord.h"
#include "Debug.h"
#include "IButton.h"
#include "FactoryWindowManager.h"
#include "IConfig.h"
#include "SystemManager.h"

FactoryDataRecord *FactoryDataRecord::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactoryDataRecord::FactoryDataRecord() : PopupWidget()
{
    layoutExec();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void FactoryDataRecord::layoutExec()
{
    int submenuW = factoryWindowManager.getSubmenuWidth();
    int submenuH = factoryWindowManager.getSubmenuHeight();
    setFixedSize(submenuW, submenuH);

    setTitleBarText(trs("DataRecord"));

    int itemW = submenuW - 200;
    int fontsize = 15;
    int btnWidth = itemW / 2;

//    QVBoxLayout *labelLayout = new QVBoxLayout();
//    labelLayout->setContentsMargins(50, 0, 50, 0);
//    labelLayout->setSpacing(10);
//    labelLayout->setAlignment(Qt::AlignTop);

    _ECG = new IComboList(trs("ECG"));
    _ECG->setFont(fontManager.textFont(fontsize));
    _ECG->setLabelAlignment(Qt::AlignCenter);
    _ECG->label->setFixedSize(btnWidth, ITEM_H);
    _ECG->combolist->setFixedSize(btnWidth, ITEM_H);
    _ECG->addItem(trs("Off"));
    _ECG->addItem(trs("On"));
    connect(_ECG->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_ECGReleased(int)));
    contentLayout->addWidget(_ECG);

    _SPO2 = new IComboList(trs("SPO2"));
    _SPO2->setFont(fontManager.textFont(fontsize));
    _SPO2->label->setFixedSize(btnWidth, ITEM_H);
    _SPO2->setLabelAlignment(Qt::AlignCenter);
    _SPO2->combolist->setFixedSize(btnWidth, ITEM_H);
    _SPO2->addItem(trs("Off"));
    _SPO2->addItem(trs("On"));
    connect(_SPO2->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_SPO2Released(int)));
    contentLayout->addWidget(_SPO2);

    _NIBP = new IComboList(trs("NIBP"));
    _NIBP->setFont(fontManager.textFont(fontsize));
    _NIBP->label->setFixedSize(btnWidth, ITEM_H);
    _NIBP->setLabelAlignment(Qt::AlignCenter);
    _NIBP->combolist->setFixedSize(btnWidth, ITEM_H);
    _NIBP->addItem(trs("Off"));
    _NIBP->addItem(trs("On"));
    connect(_NIBP->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_NIBPReleased(int)));
    contentLayout->addWidget(_NIBP);

    _TEMP = new IComboList(trs("TEMP"));
    _TEMP->setFont(fontManager.textFont(fontsize));
    _TEMP->label->setFixedSize(btnWidth, ITEM_H);
    _TEMP->setLabelAlignment(Qt::AlignCenter);
    _TEMP->combolist->setFixedSize(btnWidth, ITEM_H);
    _TEMP->addItem(trs("Off"));
    _TEMP->addItem(trs("On"));
    connect(_TEMP->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_TEMPReleased(int)));
    contentLayout->addWidget(_TEMP);

    _battery = new IComboList(trs("BatteryInfo"));
    _battery->setFont(fontManager.textFont(fontsize));
    _battery->label->setFixedSize(btnWidth, ITEM_H);
    _battery->setLabelAlignment(Qt::AlignCenter);
    _battery->combolist->setFixedSize(btnWidth, ITEM_H);
    _battery->addItem(trs("Off"));
    _battery->addItem(trs("On"));
    connect(_battery->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_batteryReleased(int)));
    contentLayout->addWidget(_battery);

    _PDCommLog = new IComboList(trs("PDCommData"));
    _PDCommLog->setFont(fontManager.textFont(fontsize));
    _PDCommLog->label->setFixedSize(btnWidth, ITEM_H);
    _PDCommLog->setLabelAlignment(Qt::AlignCenter);
    _PDCommLog->combolist->setFixedSize(btnWidth, ITEM_H);
    _PDCommLog->addItem(trs("Off"));
    _PDCommLog->addItem(trs("On"));
    connect(_PDCommLog->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_pdCommLog(int)));
    contentLayout->addWidget(_PDCommLog);

    QLabel *_note = new QLabel(trs("RecordedDataUsesCPU"));
    _note->setAlignment(Qt::AlignCenter);
    _note->setFixedHeight(TITLE_H);
    _note->setFont(fontManager.textFont(fontsize));
    contentLayout->addWidget(_note);

    contentLayout->addStretch();

    contentLayout->setSpacing(10);

    setCloseBtnTxt("");
    setCloseBtnPic(QImage("/usr/local/iDM/icons/main.png"));
    setCloseBtnColor(Qt::transparent);
}

void FactoryDataRecord::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusNextPrevChild(false);
            return;
        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            return;
        default:
            break;
    }

    PopupWidget::keyPressEvent(e);
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void FactoryDataRecord::readyShow()
{
    _ECG->setFocus();

    int value = 0;
    machineConfig.getNumValue("Record|ECG", value);
    _ECG->setCurrentIndex(value);

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        QString str;
        machineConfig.getStrValue("SPO2", str);
        if (str == "BLM_TS3")
        {
            machineConfig.getNumValue("Record|SPO2", value);
            _SPO2->setCurrentIndex(value);
            _SPO2->show();
        }
        else
        {
            _SPO2->hide();
        }

    }
    else
    {
        _SPO2->hide();
    }

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        machineConfig.getNumValue("Record|NIBP", value);
        _NIBP->setCurrentIndex(value);
        _NIBP->show();
    }
    else
    {
        _NIBP->hide();
    }

    if (systemManager.isSupport(CONFIG_TEMP))
    {
        machineConfig.getNumValue("Record|TEMP", value);
        _TEMP->setCurrentIndex(value);
        _TEMP->show();
    }
    else
    {
        _TEMP->hide();
    }

    machineConfig.getNumValue("Record|Battery", value);
    _battery->setCurrentIndex(value);

    machineConfig.getNumValue("Record|PDCommLog", value);
    _PDCommLog->setCurrentIndex(value);
}

/**************************************************************************************************
 * ecg 12槽函数。
 *************************************************************************************************/
void FactoryDataRecord::_ECGReleased(int index)
{
    machineConfig.setNumValue("Record|ECG", index);
    machineConfig.saveToDisk();
}

/**************************************************************************************************
 * TEMP槽函数。
 *************************************************************************************************/
void FactoryDataRecord::_TEMPReleased(int index)
{
    machineConfig.setNumValue("Record|TEMP", index);
    machineConfig.saveToDisk();
}

/**************************************************************************************************
 * SPO2槽函数。
 *************************************************************************************************/
void FactoryDataRecord::_SPO2Released(int index)
{
    machineConfig.setNumValue("Record|SPO2", index);
    machineConfig.saveToDisk();
}

/**************************************************************************************************
 * NIBP槽函数。
 *************************************************************************************************/
void FactoryDataRecord::_NIBPReleased(int index)
{
    machineConfig.setNumValue("Record|NIBP", index);
    machineConfig.saveToDisk();
}

/**************************************************************************************************
 * battery槽函数。
 *************************************************************************************************/
void FactoryDataRecord::_batteryReleased(int index)
{
    machineConfig.setNumValue("Record|Battery", index);
    machineConfig.saveToDisk();
}

/**************************************************************************************************
 * PD Comm Log槽函数。
 *************************************************************************************************/
void FactoryDataRecord::_pdCommLog(int index)
{
    machineConfig.setNumValue("Record|PDCommLog", index);
    machineConfig.saveToDisk();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryDataRecord::~FactoryDataRecord()
{

}



