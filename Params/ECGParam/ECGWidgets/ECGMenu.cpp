#include "ECGMenu.h"
#include "ECGParam.h"
#include "IComboList.h"
#include "LabelButton.h"
#include "LanguageManager.h"
#include "SystemManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "MenuManager.h"
#include "WindowManager.h"
#include "IConfig.h"

ECGMenu *ECGMenu::_selfObj = NULL;

/**************************************************************************************************
 * 导联模式改变。
 *************************************************************************************************/
void ECGMenu::_leadModeSlot(int index)
{
    ecgParam.setLeadMode((ECGLeadMode)index);
    QTimer::singleShot(0, this, SLOT(_loadOptionsInSlot()));
//    _loadOptions();
}

/**************************************************************************************************
 * Filter mode change
 *************************************************************************************************/
void ECGMenu::_filterModeSlot(int index)
{
    ecgParam.setFilterMode(index);
}

/**************************************************************************************************
 * 滤波改变。
 *************************************************************************************************/
void ECGMenu::_notchSlot(int index)
{
    ecgParam.setNotchFilter(index);
}

/**************************************************************************************************
 * 波形速度改变。
 *************************************************************************************************/
void ECGMenu::_sweepSpeedSlot(int index)
{
    ecgParam.setSweepSpeed((ECGSweepSpeed)index);
}

/**************************************************************************************************
 * 起搏模式改变。
 *************************************************************************************************/
void ECGMenu::_paceMakerSlot(int index)
{
    ecgParam.setPacermaker((ECGPaceMode)index);
}

/**************************************************************************************************
 * 12L起搏模式改变。
 *************************************************************************************************/
void ECGMenu::_12LpaceMakerSlot(int index)
{
    ecgParam.set12LPacermaker((ECGPaceMode)index);
}

/**************************************************************************************************
 * 心跳音改变。
 *************************************************************************************************/
void ECGMenu:: _qrsVolumSlot(int index)
{
    ecgParam.setQRSToneVolume(index);
}

/**************************************************************************************************
 * 初始化添加选项值。
 *************************************************************************************************/
void ECGMenu::_addFixedOptions(void)
{
    //filter mode
    _filterMode->addItem(trs(ECGSymbol::convert(ECG_FILTERMODE_MONITOR)));
    _filterMode->addItem(trs(ECGSymbol::convert(ECG_FILTERMODE_DIAGNOSTIC)));
    _filterMode->addItem(trs(ECGSymbol::convert(ECG_FILTERMODE_SURGERY)));
    _filterMode->addItem(trs(ECGSymbol::convert(ECG_FILTERMODE_ST)));

    //notch filter
    _notch->addItem(trs(ECGSymbol::convert(ECG_NOTCH_OFF1)));
    _notch->addItem(trs(ECGSymbol::convert(ECG_NOTCH_50HZ)));
    _notch->addItem(trs(ECGSymbol::convert(ECG_NOTCH_60HZ)));
    _notch->addItem(trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ)));

    // Lead Mode
    _leadMode->addItem(trs(ECGSymbol::convert(ECG_LEAD_MODE_3)));
    _leadMode->addItem(trs(ECGSymbol::convert(ECG_LEAD_MODE_5)));
    _leadMode->addItem(trs(ECGSymbol::convert(ECG_LEAD_MODE_12)));

    // pace检测。
    _pacemaker->addItem(trs(ECGSymbol::convert(ECG_PACE_OFF)));
    _pacemaker->addItem(trs(ECGSymbol::convert(ECG_PACE_ON)));

    // pace检测。
    if (_12Lpacemaker)
    {
        _12Lpacemaker->addItem(trs(ECGSymbol::convert(ECG_PACE_OFF)));
        _12Lpacemaker->addItem(trs(ECGSymbol::convert(ECG_PACE_ON)));
    }

    // 波形速度。
    _sweepSpeed->addItem(ECGSymbol::convert(ECG_SWEEP_SPEED_125));
    _sweepSpeed->addItem(ECGSymbol::convert(ECG_SWEEP_SPEED_250));
    _sweepSpeed->addItem(ECGSymbol::convert(ECG_SWEEP_SPEED_500));

    //心跳音
    _qrsTone->addItem(trs("Off"));
    for (int i = SOUND_VOL_1; i <= SOUND_VOL_MAX; i++)
    {
        _qrsTone->addItem(QString::number(i));
    }
}

/**************************************************************************************************
 * 延时调用载入配置的槽函数。
 *************************************************************************************************/
void ECGMenu::_loadOptionsInSlot(void)
{
    _loadOptions();
}

/**************************************************************************************************
 * 载入当前配置。
 *************************************************************************************************/
void ECGMenu::_loadOptions(void)
{
    //lead mode
    _leadMode->setCurrentIndex(ecgParam.getLeadMode());

    //滤波和陷波
    _filterMode->setCurrentItem(trs(ECGSymbol::convert(ecgParam.getFilterMode())));

    _notch->setCurrentItem(trs(ECGSymbol::convert(ecgParam.getNotchFilter())));

    // 起搏。
    _pacemaker->setCurrentItem(trs(ECGSymbol::convert(ecgParam.getPacermaker())));

    // 起搏。
    if (_12Lpacemaker)
    {
        _12Lpacemaker->setCurrentItem(trs(ECGSymbol::convert(ecgParam.get12LPacermaker())));
    }

    // 波速。
    _sweepSpeed->setCurrentIndex(ecgParam.getSweepSpeed());

    //心跳音量
    _qrsTone->setCurrentIndex(ecgParam.getQRSToneVolume());
}

/**************************************************************************************************
 * 显示前确定配置。
 *************************************************************************************************/
void ECGMenu::readyShow(void)
{
    _loadOptions();
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void ECGMenu::layoutExec(void)
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    _leadMode = new IComboList(trs("ECGLeadMode"));               // 导联模式。
    _leadMode->setFont(defaultFont());
    _leadMode->label->setFixedSize(labelWidth, ITEM_H);
    _leadMode->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_leadMode, SIGNAL(currentIndexChanged(int)), this, SLOT(_leadModeSlot(int)));
    mainLayout->addWidget(_leadMode);

    _filterMode = new IComboList(trs("FilterMode"));
    _filterMode->setFont(defaultFont());
    _filterMode->label->setFixedSize(labelWidth, ITEM_H);
    _filterMode->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_filterMode, SIGNAL(currentIndexChanged(int)), this, SLOT(_filterModeSlot(int)));
    mainLayout->addWidget(_filterMode);

    _notch = new IComboList(trs("SupervisorNotchFilter"));               // 滤波。
    _notch->setFont(defaultFont());
    _notch->label->setFixedSize(labelWidth, ITEM_H);
    _notch->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_notch, SIGNAL(currentIndexChanged(int)), this, SLOT(_notchSlot(int)));
    mainLayout->addWidget(_notch);

    _pacemaker = new IComboList(trs("ECGPacemaker"));             // 起搏。
    _pacemaker->setFont(defaultFont());
    _pacemaker->label->setFixedSize(labelWidth, ITEM_H);
    _pacemaker->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_pacemaker, SIGNAL(currentIndexChanged(int)), this, SLOT(_paceMakerSlot(int)));
    mainLayout->addWidget(_pacemaker);

    int ecg12lead_enable = 0;
    machineConfig.getNumValue("ECG12LEADEnable", ecg12lead_enable);
    if (ecg12lead_enable)
    {
        _12Lpacemaker = new IComboList(trs("ECG12LPacemaker"));             // ECG 12L起搏。
        _12Lpacemaker->setFont(defaultFont());
        _12Lpacemaker->label->setFixedSize(labelWidth, ITEM_H);
        _12Lpacemaker->combolist->setFixedSize(btnWidth, ITEM_H);
        connect(_12Lpacemaker, SIGNAL(currentIndexChanged(int)), this, SLOT(_12LpaceMakerSlot(int)));
        mainLayout->addWidget(_12Lpacemaker);
    }

    _sweepSpeed = new IComboList(trs("ECGSweepSpeed"));           // 波形速度。
    _sweepSpeed->setFont(defaultFont());
    _sweepSpeed->label->setFixedSize(labelWidth, ITEM_H);
    _sweepSpeed->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_sweepSpeed, SIGNAL(currentIndexChanged(int)), this, SLOT(_sweepSpeedSlot(int)));
    mainLayout->addWidget(_sweepSpeed);

    QString qrsTitle = trs("ECGQRSPRToneVolume");
    if (!systemManager.isSupport(CONFIG_SPO2))
    {
        qrsTitle = trs("ECGQRSToneVolume");
    }
    _qrsTone = new IComboList(qrsTitle);           // 心跳音量。
    _qrsTone->setFont(defaultFont());
    _qrsTone->label->setFixedSize(labelWidth, ITEM_H);
    _qrsTone->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_qrsTone, SIGNAL(currentIndexChanged(int)), this, SLOT(_qrsVolumSlot(int)));
    mainLayout->addWidget(_qrsTone);

    _addFixedOptions();


#ifndef CONFIG_ECG_TEST
    _notch->setVisible(false);
#endif
}

/**************************************************************************************************
 * 刷新各个选项。
 *************************************************************************************************/
void ECGMenu::refresh(void)
{
    if (!isVisible())
    {
        return;
    }

    _loadOptions();
}

/**************************************************************************************************
 * 设置ECG导联模式。
 *************************************************************************************************/
void ECGMenu::setECGLeadMode(int leadMode)
{
    _leadModeSlot(leadMode);

    return;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGMenu::ECGMenu() : SubMenu(trs("ECGMenu"))
{
    setDesc(trs("ECGMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGMenu::~ECGMenu()
{

}
