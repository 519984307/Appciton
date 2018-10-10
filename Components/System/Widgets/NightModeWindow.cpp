/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/30
 **/

#include "NightModeWindow.h"
#include <QGridLayout>
#include <QLabel>
#include "ComboBox.h"
#include <QMap>
#include "Button.h"
#include "IConfig.h"
#include "SoundManager.h"
#include "NightModeManager.h"

class NightModeWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_SCREEN_BRIGHTNESS,
        ITEM_CBO_ALARM_VOLUME,
        ITEM_CBO_HEART_BEAT_VOLUME,
        ITEM_CBO_KEYPRESS_VOLUME_NUM,
        ITEM_CBO_NIBP_COMPLETED_TIPS,
        ITEM_CBO_STOP_NIBP_MEASURE
    };
    NightModeWindowPrivate()
                : enterNightMode(NULL)
    {
    }
    /**
     * @brief loadOptions
     */
    void loadOptions(void);

    QMap <MenuItem, ComboBox*> combos;
    Button *enterNightMode;
};

void NightModeWindowPrivate::loadOptions()
{
    int index = 0;
    systemConfig.getNumValue("NightMode|ScreenBrightness", index);
    combos[ITEM_CBO_SCREEN_BRIGHTNESS]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("NightMode|AlarmVolume", index);
    combos[ITEM_CBO_ALARM_VOLUME]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("NightMode|HeartBeatVolume", index);
    combos[ITEM_CBO_HEART_BEAT_VOLUME]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("NightMode|KeyPressVolume", index);
    combos[ITEM_CBO_KEYPRESS_VOLUME_NUM]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("NightMode|NIBPCompletedTips", index);
    combos[ITEM_CBO_NIBP_COMPLETED_TIPS]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("NightMode|StopNIBPMeasure", index);
    combos[ITEM_CBO_STOP_NIBP_MEASURE]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("NightMode|EnterNightMode", index);
    QString path;
    if (!index)
    {
        path = trs("EnterNightMode");
    }
    else
    {
        path = trs("ExitNightMode");
    }
    enterNightMode->setText(path);
}

NightModeWindow::NightModeWindow()
                          : Window(),
                            d_ptr(new NightModeWindowPrivate)
{
    layoutExec();
    readyShow();
}

NightModeWindow::~NightModeWindow()
{
    delete d_ptr;
}

void NightModeWindow::readyShow()
{
    d_ptr->loadOptions();
}

void NightModeWindow::layoutExec()
{
    setWindowTitle(trs("NightMode"));

    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);
    setFixedWidth(480);

    QLabel *label;
    ComboBox *comboBox;
    Button *btn;
    int comboIndex = 0;

    // screen brightness
    label = new QLabel(trs("SystemBrightness"));
    glayout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << QString::number(BRT_LEVEL_1)
                       << QString::number(BRT_LEVEL_2)
                       << QString::number(BRT_LEVEL_3)
                       << QString::number(BRT_LEVEL_4)
                       << QString::number(BRT_LEVEL_5)
                       << QString::number(BRT_LEVEL_6)
                       << QString::number(BRT_LEVEL_7)
                       << QString::number(BRT_LEVEL_8)
                       << QString::number(BRT_LEVEL_9)
                       << QString::number(BRT_LEVEL_10)
                      );
    comboIndex = static_cast<int>(NightModeWindowPrivate::
                                  ITEM_CBO_SCREEN_BRIGHTNESS);
    comboBox->setProperty("Item",
                          qVariantFromValue(comboIndex));
    connect(comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onComboBoxIndexChanged(int)));
    glayout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NightModeWindowPrivate::
                         ITEM_CBO_SCREEN_BRIGHTNESS, comboBox);

    // alarm volume
    label = new QLabel(trs("SystemAlarmVolume"));
    glayout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << QString::number(SoundManager::VOLUME_LEV_1)
                       << QString::number(SoundManager::VOLUME_LEV_2)
                       << QString::number(SoundManager::VOLUME_LEV_3)
                       << QString::number(SoundManager::VOLUME_LEV_4)
                       << QString::number(SoundManager::VOLUME_LEV_5)
                      );
    comboIndex = static_cast<int>(NightModeWindowPrivate::
                                  ITEM_CBO_ALARM_VOLUME);
    comboBox->setProperty("Item",
                          qVariantFromValue(comboIndex));
    connect(comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onComboBoxIndexChanged(int)));
    glayout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NightModeWindowPrivate::
                         ITEM_CBO_ALARM_VOLUME, comboBox);

    // heart beat volume
    label = new QLabel(trs("SystemHeartBeat"));
    glayout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << QString::number(BRT_LEVEL_1)
                       << QString::number(BRT_LEVEL_2)
                       << QString::number(BRT_LEVEL_3)
                       << QString::number(BRT_LEVEL_4)
                       << QString::number(BRT_LEVEL_5)
                       << QString::number(BRT_LEVEL_6)
                       << QString::number(BRT_LEVEL_7)
                       << QString::number(BRT_LEVEL_8)
                       << QString::number(BRT_LEVEL_9)
                      );
    comboIndex = static_cast<int>(NightModeWindowPrivate::
                                  ITEM_CBO_HEART_BEAT_VOLUME);
    comboBox->setProperty("Item",
                          qVariantFromValue(comboIndex));
    connect(comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onComboBoxIndexChanged(int)));
    glayout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NightModeWindowPrivate::
                         ITEM_CBO_HEART_BEAT_VOLUME, comboBox);

    // key press volume
    label = new QLabel(trs("KeyPressVolume"));
    glayout->addWidget(label , d_ptr->combos.count() , 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       <<QString::number(SoundManager::VOLUME_LEV_0)
                       <<QString::number(SoundManager::VOLUME_LEV_1)
                       <<QString::number(SoundManager::VOLUME_LEV_2)
                       <<QString::number(SoundManager::VOLUME_LEV_3)
                       <<QString::number(SoundManager::VOLUME_LEV_4)
                       <<QString::number(SoundManager::VOLUME_LEV_5)
                       );
    glayout->addWidget(comboBox , d_ptr->combos.count() , 1);
    comboIndex = static_cast<int>(NightModeWindowPrivate::
                                  ITEM_CBO_KEYPRESS_VOLUME_NUM);
    comboBox->setProperty("Item" , qVariantFromValue(comboIndex));
    connect(comboBox , SIGNAL(currentIndexChanged(int)) , this , SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(NightModeWindowPrivate::
                         ITEM_CBO_KEYPRESS_VOLUME_NUM , comboBox);

    // nibp completed tips
    label = new QLabel(trs("NIBPCompletedTips"));
    glayout->addWidget(label , d_ptr->combos.count() , 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Closed")
                       << trs("Opened")
                       );
    glayout->addWidget(comboBox , d_ptr->combos.count() , 1);
    comboIndex = static_cast<int>(NightModeWindowPrivate::
                                  ITEM_CBO_NIBP_COMPLETED_TIPS);
    comboBox->setProperty("Item" , qVariantFromValue(comboIndex));
    connect(comboBox , SIGNAL(currentIndexChanged(int)) , this , SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(NightModeWindowPrivate::
                         ITEM_CBO_NIBP_COMPLETED_TIPS , comboBox);

    // stop nibp measure
    label = new QLabel(trs("StopNIBPMeasure"));
    glayout->addWidget(label , d_ptr->combos.count() , 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes")
                       );
    glayout->addWidget(comboBox , d_ptr->combos.count() , 1);
    comboIndex = static_cast<int>(NightModeWindowPrivate::
                                  ITEM_CBO_STOP_NIBP_MEASURE);
    comboBox->setProperty("Item" , qVariantFromValue(comboIndex));
    connect(comboBox , SIGNAL(currentIndexChanged(int)) , this , SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(NightModeWindowPrivate::
                         ITEM_CBO_STOP_NIBP_MEASURE , comboBox);

    // enter night mode
    btn = new Button(trs("EnterNightMode"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    glayout->addWidget(btn , d_ptr->combos.count() , 1);
    connect(btn, SIGNAL(released()), this, SLOT(OnBtnReleased()));
    d_ptr->enterNightMode = btn;

    glayout->setRowStretch(d_ptr->combos.count() + 1, 1);
    setWindowLayout(glayout);
}

void NightModeWindow::onComboBoxIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }
    ComboBox *combo = qobject_cast<ComboBox*>(sender());
    int indexType = combo->property("Item").toInt();
    QString node;
    switch (indexType)
    {
        case NightModeWindowPrivate::ITEM_CBO_SCREEN_BRIGHTNESS:
        node = "ScreenBrightness";
        index = combo->itemText(index).toInt();
        break;
        case NightModeWindowPrivate::ITEM_CBO_ALARM_VOLUME:
        node = "AlarmVolume";
        break;
        case NightModeWindowPrivate::ITEM_CBO_HEART_BEAT_VOLUME:
        node = "HeartBeatVolume";
        break;
        case NightModeWindowPrivate::ITEM_CBO_KEYPRESS_VOLUME_NUM:
        node = "KeyPressVolume";
        break;
        case NightModeWindowPrivate::ITEM_CBO_NIBP_COMPLETED_TIPS:
        node = "NIBPCompletedTips";
        break;
        case NightModeWindowPrivate::ITEM_CBO_STOP_NIBP_MEASURE:
        node = "StopNIBPMeasure";
        break;
    }
    systemConfig.setNumValue(QString("NightMode|%1").arg(node), index);
}

void NightModeWindow::OnBtnReleased()
{
    int index = 0;
    systemConfig.getNumValue("NightMode|EnterNightMode", index);
    QString name;
    if (index)
    {
        name = trs("EnterNightMode");
    }
    else
    {
        name = trs("ExitNightMode");
    }
    d_ptr->enterNightMode->setText(name);
    nightModeManager.setNightMode();
}
