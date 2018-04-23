#include <QHBoxLayout>
#include <QVBoxLayout>
#include "SystemMenu.h"
#include "SystemManager.h"
#include "LanguageManager.h"
#include "SoundManager.h"
#include "IComboList.h"
#include "LabelButton.h"
#include "MenuManager.h"
#include "SystemManager.h"
#include "PrintManager.h"
#include "ECGParam.h"
#include "IConfig.h"
#include "PrintManager.h"
#include "MenuManager.h"
#include "ConfigMaintainMenuGrp.h"
#ifdef Q_WS_QWS
#include "TSCalibrationWindow.h"
#include <QWSServer>
#endif

SystemMenu *SystemMenu::_selfObj = NULL;

/**************************************************************************************************
 * 报警音量槽函数。
 *************************************************************************************************/
void SystemMenu::_audioVolumeSlot(int index)
{
    int volume = _audioVolume->itemText(index).toInt();
    soundManager.setVolume(SoundManager::SOUND_TYPE_ALARM, (SoundManager::VolumeLevel) volume);
}

/**************************************************************************************************
 * 报警音量槽函数。
 *************************************************************************************************/
void SystemMenu::_errorToneVolumeSlot(int index)
{
    soundManager.setVolume(SoundManager::SOUND_TYPE_ERROR, (SoundManager::VolumeLevel)(index + 1));
}

/**************************************************************************************************
 * 屏幕亮度槽函数。
 *************************************************************************************************/
void SystemMenu::_brightnessSlot(int index)
{
    systemManager.setBrightness((BrightnessLevel)index);
}

/**************************************************************************************************
 * 设置打印速度。
 *************************************************************************************************/
void SystemMenu::_printSpeedSlot(int index)
{
    printManager.setPrintSpeed((PrintSpeed)index);
}

/**************************************************************************************************
 * 设置打印波形数量。
 *************************************************************************************************/
void SystemMenu::_printWaveformNumSlot(int index)
{
    printManager.setPrintWaveNum(index + 1);
}

/**************************************************************************************************
 * 打印配置。
 *************************************************************************************************/
void SystemMenu::_printCfgSlot(void)
{
    printManager.enablePrinterSpeed(printManager.getPrintSpeed());
    printManager.requestPrint(PRINT_TYPE_REVIEW, PRINT_LAYOUT_ID_SUPERVISOR_CONFIG);
}

void SystemMenu::_configManagerSlot()
{
    configMaintainMenuGrp.initializeSubMenu();
    configMaintainMenuGrp.popup();
}

#ifdef Q_WS_QWS
/**
 * @brief SystemMenu::_touchScreenCalSlot touch screen calibration slot
 */
void SystemMenu::_touchScreenCalSlot()
{
    QWSServer::instance()->closeMouse();
    TSCalibrationWindow calWin;
    calWin.exec();
    QWSServer::instance()->openMouse();
}
#endif

/**************************************************************************************************
 * 载入当前配置。
 *************************************************************************************************/
void SystemMenu::_loadOptions(void)
{
    _audioVolume->setCurrentItem(QString::number((int)soundManager.
                                                 getVolume(SoundManager::SOUND_TYPE_ALARM)));
    _screenBrightness->setCurrentIndex(systemManager.getBrightness());
    _printSpeed->setCurrentIndex(printManager.getPrintSpeed());
    _printWaveformNum->setCurrentIndex(printManager.getPrintWaveNum() - 1);
}

/**************************************************************************************************
 * 显示前确定配置。
 *************************************************************************************************/
void SystemMenu::readyShow(void)
{
    _loadOptions();
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void SystemMenu::layoutExec(void)
{
    QFont font = defaultFont();
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    // 报警音量。
    _audioVolume = new IComboList(trs("SystemAlarmVolume"));
    _audioVolume->setFont(font);
    int index = 0;
    superConfig.getNumValue("Alarm|MinimumAlarmVolume", index);
    for (int i = index + 1; i <= SoundManager::VOLUME_LEV_MAX; i++)
    {
        _audioVolume->addItem(QString::number(i));
    }
    connect(_audioVolume, SIGNAL(currentIndexChanged(int)), this, SLOT(_audioVolumeSlot(int)));
    _audioVolume->label->setFixedSize(labelWidth, ITEM_H);
    _audioVolume->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_audioVolume);

    // 报警音量。
//    _promptVolume = new IComboList(trs("Prompt Volume"));
//    _promptVolume->setFont(font);
//    for (int i = SOUND_VOL_0; i < SOUND_VOL_5; i++)
//    {
//        _promptVolume->addItem(QString::number(i + 1));
//    }
//    connect(_promptVolume, SIGNAL(currentIndexChanged(int)), this, SLOT(_promptVolumeSlot(int)));
//    _promptVolume->label->setFixedSize(labelWidth, ITEM_H);
//    _promptVolume->combolist->setFixedSize(btnWidth, ITEM_H);
//    _promptVolume->setCurrentIndex(2);
//    mainLayout->addWidget(_promptVolume);

    // 错误报警音量。
//    _errorToneVolume = new IComboList(trs("Error Tone Volume"));
//    _errorToneVolume->setFont(font);
//    for (int i = SOUND_VOL_0; i < SOUND_VOL_5; i++)
//    {
//        _errorToneVolume->addItem(QString::number(i + 1));
//    }
//    connect(_errorToneVolume, SIGNAL(currentIndexChanged(int)), this, SLOT(_errorToneVolumeSlot(int)));
//    _errorToneVolume->label->setFixedSize(labelWidth, ITEM_H);
//    _errorToneVolume->combolist->setFixedSize(btnWidth, ITEM_H);
//    _errorToneVolume->setCurrentIndex(4);
//    mainLayout->addWidget(_errorToneVolume);

    // 屏幕亮度。
    _screenBrightness = new IComboList(trs("SystemBrightness"));
    _screenBrightness->setFont(font);
    for (int i = BRT_LEVEL_0; i <= BRT_LEVEL_9; i++)
    {
        _screenBrightness->addItem(QString::number(i + 1));
    }
    connect(_screenBrightness, SIGNAL(currentIndexChanged(int)), this, SLOT(_brightnessSlot(int)));
    _screenBrightness->label->setFixedSize(labelWidth, ITEM_H);
    _screenBrightness->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_screenBrightness);

    // 打印速度。
    _printSpeed = new IComboList(trs("PrintSpeed"));
    _printSpeed->setFont(font);
    for (int i = PRINT_SPEED_250; i < PRINT_SPEED_NR; i++)
    {
        _printSpeed->addItem(PrintSymbol::convert((PrintSpeed)i));
    }
    connect(_printSpeed, SIGNAL(currentIndexChanged(int)), this, SLOT(_printSpeedSlot(int)));
    _printSpeed->label->setFixedSize(labelWidth, ITEM_H);
    _printSpeed->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_printSpeed);

    // 打印波形数量。
    _printWaveformNum = new IComboList(trs("PrintNumOfTraces"));
    _printWaveformNum->setFont(font);
    for (int i = 1; i < 5; i++)
    {
        _printWaveformNum->addItem(QString::number(i));
    }
    connect(_printWaveformNum, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_printWaveformNumSlot(int)));
    _printWaveformNum->label->setFixedSize(labelWidth, ITEM_H);
    _printWaveformNum->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_printWaveformNum);

    // 打印配置
    _printCfg = new LabelButton("");
    _printCfg->setFont(font);
    _printCfg->setValue(trs("SupervisorPrintConfig"));
    connect(_printCfg->button, SIGNAL(realReleased()), this, SLOT(_printCfgSlot()));
    _printCfg->label->setFixedSize(labelWidth, ITEM_H);
    _printCfg->button->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_printCfg);

    // config Manager
    _configManager = new LabelButton("");
    _configManager->setFont(font);
    _configManager->setValue(trs("ConfigManager"));
    connect(_configManager->button, SIGNAL(realReleased()), this, SLOT(_configManagerSlot()));
    _configManager->label->setFixedSize(labelWidth, ITEM_H);
    _configManager->button->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_configManager);

#ifdef Q_WS_QWS
    // touch screen calibration
    _touchScreenCal= new LabelButton("");
    _touchScreenCal->setFont(font);
    _touchScreenCal->setValue(trs("TouchScreenCalibration"));
    connect(_touchScreenCal->button, SIGNAL(realReleased()), this, SLOT(_touchScreenCalSlot()));
    _touchScreenCal->label->setFixedSize(labelWidth, ITEM_H);
    _touchScreenCal->button->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_touchScreenCal);
#endif
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SystemMenu::SystemMenu() : SubMenu(trs("SystemMenu"))
{
    setDesc(trs("SystemMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SystemMenu::~SystemMenu()
{

}
