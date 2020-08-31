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
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QMap>
#include "IConfig.h"
#include "SoundManager.h"
#include "NightModeManager.h"
#include "SystemManager.h"
#include "SystemDefine.h"
#include "ConfigManagerInterface.h"
#include "SoundManager.h"

class NightModeWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_SCREEN_BRIGHTNESS,
        ITEM_CBO_HEART_BEAT_VOLUME,
        ITEM_CBO_KEYPRESS_VOLUME_NUM,
        ITEM_CBO_NIBP_COMPLETED_TIPS,
        ITEM_CBO_ALARM_VOLUME,
        ITEM_CBO_STOP_NIBP_MEASURE
    };
    NightModeWindowPrivate()
        : enterNightMode(NULL), minAlarmVol(3)
    {
    }
    /**
     * @brief loadOptions
     */
    void loadOptions(void);

    QMap <MenuItem, ComboBox*> combos;
    Button *enterNightMode;
    int minAlarmVol;    // min alarm volume
};

void NightModeWindowPrivate::loadOptions()
{
    int index = 0;
    systemConfig.getNumValue("NightMode|ScreenBrightness", index);
    combos[ITEM_CBO_SCREEN_BRIGHTNESS]->setCurrentIndex(index);

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
    systemConfig.getNumValue("Alarms|MinimumAlarmVolume", minAlarmVol);
    systemConfig.getNumValue("NightMode|AlarmVolume", index);
    index -= minAlarmVol;  // calculate cur index
    index = index < 0 ? 0 : index;
    combos[ITEM_CBO_ALARM_VOLUME]->setCurrentIndex(index);

    index = 0;
    systemConfig.getNumValue("NightMode|StopNIBPMeasure", index);
    combos[ITEM_CBO_STOP_NIBP_MEASURE]->setCurrentIndex(index);

    index = nightModeManager.nightMode();
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
    : Dialog(),
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
    setFixedSize(480, 480);

    QLabel *label;
    ComboBox *comboBox;
    Button *btn;
    int comboIndex = 0;

    // screen brightness
    label = new QLabel(trs("SystemBrightness"));
    glayout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = BRT_LEVEL_0; i < BRT_LEVEL_NR; i++)
    {
        comboBox->addItem(QString::number(i + 1));
    }
    comboIndex = static_cast<int>(NightModeWindowPrivate::
                                  ITEM_CBO_SCREEN_BRIGHTNESS);
    comboBox->setProperty("Item",
                          qVariantFromValue(comboIndex));
    connect(comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onComboBoxIndexChanged(int)));
    glayout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NightModeWindowPrivate::
                         ITEM_CBO_SCREEN_BRIGHTNESS, comboBox);

    // heart beat volume
    label = new QLabel(trs("ECGQRSToneVolume"));
    glayout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << QString::number(SoundManager::VOLUME_LEV_1)
                       << QString::number(SoundManager::VOLUME_LEV_2)
                       << QString::number(SoundManager::VOLUME_LEV_3)
                       << QString::number(SoundManager::VOLUME_LEV_4)
                       << QString::number(SoundManager::VOLUME_LEV_MAX));
    comboIndex = static_cast<int>(NightModeWindowPrivate::
                                  ITEM_CBO_HEART_BEAT_VOLUME);
    comboBox->setProperty("Item",
                          qVariantFromValue(comboIndex));
    connect(comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onComboBoxIndexChanged(int)));
    glayout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NightModeWindowPrivate::
                         ITEM_CBO_HEART_BEAT_VOLUME, comboBox);
    // establish the connection between @itemFocusChanged and @onPopupListItemFocusChanged
    connect(comboBox, SIGNAL(itemFoucsIndexChanged(int)),
            this, SLOT(onPopupListItemFocusChanged(int)));

    // key press volume
    label = new QLabel(trs("ToneVolume"));
    glayout->addWidget(label , d_ptr->combos.count() , 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       <<trs("Off")
                       <<QString::number(SoundManager::VOLUME_LEV_1)
                       <<QString::number(SoundManager::VOLUME_LEV_2)
                       <<QString::number(SoundManager::VOLUME_LEV_3)
                       <<QString::number(SoundManager::VOLUME_LEV_4)
                       <<QString::number(SoundManager::VOLUME_LEV_5));
    glayout->addWidget(comboBox , d_ptr->combos.count() , 1);
    comboIndex = static_cast<int>(NightModeWindowPrivate::
                                  ITEM_CBO_KEYPRESS_VOLUME_NUM);
    comboBox->setProperty("Item" , qVariantFromValue(comboIndex));
    connect(comboBox , SIGNAL(currentIndexChanged(int)) , this , SLOT(onComboBoxIndexChanged(int)));
    connect(comboBox, SIGNAL(itemFoucsIndexChanged(int)), this, SLOT(onPopupListItemFocusChanged(int)));
    d_ptr->combos.insert(NightModeWindowPrivate::
                         ITEM_CBO_KEYPRESS_VOLUME_NUM , comboBox);

    // nibp completed tips
    label = new QLabel(trs("NIBPCompleteTone"));
    glayout->addWidget(label , d_ptr->combos.count() , 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("OFF")
                       << trs("ON"));
    glayout->addWidget(comboBox , d_ptr->combos.count() , 1);
    comboIndex = static_cast<int>(NightModeWindowPrivate::
                                  ITEM_CBO_NIBP_COMPLETED_TIPS);
    comboBox->setProperty("Item" , qVariantFromValue(comboIndex));
    connect(comboBox , SIGNAL(currentIndexChanged(int)) , this , SLOT(onComboBoxIndexChanged(int)));
    connect(comboBox, SIGNAL(itemFoucsIndexChanged(int)), this , SLOT(onPopupListItemFocusChanged(int)));
    d_ptr->combos.insert(NightModeWindowPrivate::
                         ITEM_CBO_NIBP_COMPLETED_TIPS , comboBox);

    // night mode alarm volume
    label = new QLabel(trs("SystemAlarmVolume"));
    glayout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    systemConfig.getNumValue("Alarms|MinimumAlarmVolume", d_ptr->minAlarmVol);
    for (int i = d_ptr->minAlarmVol; i <= SoundManager::VOLUME_LEV_5; i++)
    {
        comboBox->addItem(QString::number(i));
    }
    comboIndex = static_cast<int>(NightModeWindowPrivate::ITEM_CBO_ALARM_VOLUME);
    comboBox->setProperty("Item", qVariantFromValue(comboIndex));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    glayout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NightModeWindowPrivate::ITEM_CBO_ALARM_VOLUME, comboBox);

    // stop nibp measure
    label = new QLabel(trs("StopNIBPMeasure"));
    glayout->addWidget(label , d_ptr->combos.count() , 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("No")
                       << trs("Yes"));
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
        nightModeManager.setBrightness(static_cast<BrightnessLevel>(index));
        break;
    case NightModeWindowPrivate::ITEM_CBO_HEART_BEAT_VOLUME:
        node = "HeartBeatVolume";
        nightModeManager.setSoundVolume(SoundManager::SOUND_TYPE_HEARTBEAT,
                                        static_cast<SoundManager::VolumeLevel>(index));
        break;
    case NightModeWindowPrivate::ITEM_CBO_KEYPRESS_VOLUME_NUM:
        node = "KeyPressVolume";
        nightModeManager.setSoundVolume(SoundManager::SOUND_TYPE_NOTIFICATION,
                                        static_cast<SoundManager::VolumeLevel>(index));
        break;
    case NightModeWindowPrivate::ITEM_CBO_NIBP_COMPLETED_TIPS:
        node = "NIBPCompletedTips";
        nightModeManager.setSoundVolume(SoundManager::SOUND_TYPE_NIBP_COMPLETE,
                                        static_cast<SoundManager::VolumeLevel>(index));
        break;
    case NightModeWindowPrivate::ITEM_CBO_ALARM_VOLUME:
        index = index + d_ptr->minAlarmVol;
        node = "AlarmVolume";
        nightModeManager.setSoundVolume(SoundManager::SOUND_TYPE_ALARM, static_cast<SoundManager::VolumeLevel>(index));
        break;
    case NightModeWindowPrivate::ITEM_CBO_STOP_NIBP_MEASURE:
        node = "StopNIBPMeasure";
        nightModeManager.setNibpStopMeasure(static_cast<bool>(index));
        break;
    }
    systemConfig.setNumValue(QString("NightMode|%1").arg(node), index);
}

void NightModeWindow::OnBtnReleased()
{
    int index = nightModeManager.nightMode();
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
    nightModeManager.setNightMode(!nightModeManager.nightMode());
}

void NightModeWindow::onPopupListItemFocusChanged(int volume)
{
    ComboBox *w = qobject_cast<ComboBox*>(sender());

    int orignalVolume = 0;
    if (w == d_ptr->combos[NightModeWindowPrivate::ITEM_CBO_HEART_BEAT_VOLUME])
    {
        soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT, static_cast<SoundManager::VolumeLevel>(volume));
        soundManager.heartBeatTone();
        ConfigManagerInterface *configInterface = ConfigManagerInterface::getConfigManager();
        if (configInterface)
        {
            configInterface->getCurConfig().getNumValue("ECG|QRSVolume", orignalVolume);
        }
        soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT,
                               static_cast<SoundManager::VolumeLevel>(orignalVolume));
    }
    else if (w == d_ptr->combos[NightModeWindowPrivate::ITEM_CBO_NIBP_COMPLETED_TIPS])
    {
        if (volume == 1)
        {
            soundManager.setNIBPCompleteTone(true);
            soundManager.nibpCompleteTone();
            systemConfig.getNumValue("PrimaryCfg|NIBP|CompleteTone", orignalVolume);
            soundManager.setNIBPCompleteTone(static_cast<bool>(orignalVolume));
        }
    }
    else if (w == d_ptr->combos[NightModeWindowPrivate::ITEM_CBO_KEYPRESS_VOLUME_NUM])
    {
        soundManager.setVolume(SoundManager::SOUND_TYPE_NOTIFICATION , static_cast<SoundManager::VolumeLevel>(volume));
        soundManager.keyPressTone();
        systemConfig.getNumValue("General|KeyPressVolume", orignalVolume);
        soundManager.setVolume(SoundManager::SOUND_TYPE_NOTIFICATION,
                               static_cast<SoundManager::VolumeLevel>(orignalVolume));
    }
}
