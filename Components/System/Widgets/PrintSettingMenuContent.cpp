/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/19
 **/

#include "PrintSettingMenuContent.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QGridLayout>
#include "RecorderManager.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "LayoutManager.h"
#include "ECGParam.h"
#include "IBPParam.h"
#include "RESPSymbol.h"
#include "SPO2Symbol.h"
#include "CO2Symbol.h"
#include "ContinuousPageGenerator.h"
#include "TimeManager.h"
#include "CO2Param.h"
#include <QTimerEvent>
#include <QSet>

#define STOP_PRINT_TIMEOUT          (100)

class PrintSettingMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_PRINT_TIME,
        ITEM_CBO_PRINT_SPEED,
        ITEM_CBO_PRINT_PHY_ALARM,
        ITEM_CBO_PRINT_CODEMARKER,
        ITEM_CBO_PRINT_NIBP,
        ITEM_CBO_PRINT_WAVE_FREEZE
    };

    PrintSettingMenuContentPrivate()
        : printTimerId(-1),
          waitTimerId(-1),
          isWait(false),
          timeoutNum(0)
    {
    }
    /**
     * @brief loadOptions
     */
    void loadOptions(void);

    /**
     * @brief wavesUpdate 波形更新
     * @param waveIDs  获取更新的波形id
     * @param waveNames  获取更新的波形名字
     */
    void wavesUpdate(QList<int> *waveIDs, QStringList *waveNames);

    QList<ComboBox *> selectWaves;
    QList<int> waveIDs;
    QStringList waveNames;
    QMap<MenuItem, ComboBox *> combos;

    int printTimerId;
    int waitTimerId;
    bool isWait;
    int timeoutNum;
};

void PrintSettingMenuContentPrivate::loadOptions()
{
    int index = 0;
    index = static_cast<int>(recorderManager.getPrintTime());
    combos[ITEM_CBO_PRINT_TIME]->setCurrentIndex(index);

    index = static_cast<int>(recorderManager.getPrintSpeed());
    combos[ITEM_CBO_PRINT_SPEED]->setCurrentIndex(index);

    systemConfig.getNumValue("Print|PhysiologicalAlarm", index);
    combos[ITEM_CBO_PRINT_PHY_ALARM]->setCurrentIndex(index);

    systemConfig.getNumValue("Print|CoderMarker", index);
    combos[ITEM_CBO_PRINT_CODEMARKER]->setCurrentIndex(index);

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        systemConfig.getNumValue("Print|NIBPReading", index);
        combos[ITEM_CBO_PRINT_NIBP]->setCurrentIndex(index);
    }

    systemConfig.getNumValue("Print|WaveFreeze", index);
    combos[ITEM_CBO_PRINT_WAVE_FREEZE]->setCurrentIndex(index);

    // update wave
    wavesUpdate(&waveIDs, &waveNames);

    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        ComboBox *combo = selectWaves.at(i);
        combo->blockSignals(true);
        combo->clear();
        combo->addItem(trs("Off"));
        foreach(QString name, waveNames)
        {
            combo->addItem(trs(name));
        }
        combo->blockSignals(false);
        // 如果波形数量小于选择打印波形combo时，失能多余的选择打印波形combo
        if (waveNames.size() <= i)
        {
            combo->setEnabled(false);
        }
        else
        {
            combo->setEnabled(true);
        }
    }

    int offCount = 0;
    QSet<int> waveCboIds;
    int savedWaveIds[PRINT_WAVE_NUM] = {0};

    // 搜集当前波形菜单中所有选择的波形id
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        QString path;
        path = QString("Print|SelectWave%1").arg(i + 1);
        systemConfig.getNumValue(path, savedWaveIds[i]);

        if (savedWaveIds[i] < WAVE_NR)
        {
            waveCboIds.insert(savedWaveIds[i]);
        }

        if (savedWaveIds[i] == WAVE_NONE)
        {
            offCount++;
        }
    }

    // 如果出现重复选择项，重新按照当前显示波形序列更新打印波形id
    if (offCount < PRINT_WAVE_NUM - 1)
    {
        if ((waveCboIds.size()) < PRINT_WAVE_NUM)
        {
            int idCount = waveIDs.count();
            for (int i = 0; i < PRINT_WAVE_NUM; i++)
            {
                QString path;
                path = QString("Print|SelectWave%1").arg(i + 1);
                int waveId = WAVE_NONE;
                if (i < idCount)
                {
                    waveId = waveIDs.at(i);
                }
                systemConfig.setNumValue(path, waveId);
                // 更新打印的波形ids
                savedWaveIds[i] = waveId;
            }
        }
    }

    // 选择波形id对应的菜单索引
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        selectWaves[i]->blockSignals(true);
        // 选择空波形的索引
        if (savedWaveIds[i] == WAVE_NONE)
        {
            selectWaves[i]->setCurrentIndex(0);
            selectWaves[i]->blockSignals(false);
            continue;
        }

        // 由于波形选择框中第一个选择项是空波形item，所以如果找到waveId的
        // 索引，需要在其基础上加1成为当前波形选择框的索引
        int cboIndex = waveIDs.indexOf(savedWaveIds[i]);
        if (cboIndex >= 0)
        {
            cboIndex++;
        }

        // 范围之外的波形选择更新为WAVE_NONE
        if (cboIndex == -1)
        {
            selectWaves[i]->setCurrentIndex(0);
            // 及时更新在配置文件中
            QString path;
            path = QString("Print|SelectWave%1").arg(i + 1);
            systemConfig.setNumValue(path, static_cast<int>(WAVE_NONE));
            selectWaves[i]->blockSignals(false);
            continue;
        }

        if (cboIndex < selectWaves[i]->count() && cboIndex >= 0)
        {
            selectWaves[i]->setCurrentIndex(cboIndex);
            selectWaves[i]->blockSignals(false);
        }
    }
}

PrintSettingMenuContent::PrintSettingMenuContent()
    : MenuContent(trs("PrintSettingMenu"),
                  trs("PrintSettingMenuDesc")),
      d_ptr(new PrintSettingMenuContentPrivate)
{
    connect(&co2Param, SIGNAL(connectStatusUpdated(bool)), this, SLOT(onConnectedStatusChanged()));
}

PrintSettingMenuContent::~PrintSettingMenuContent()
{
    delete d_ptr;
}

void PrintSettingMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void PrintSettingMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);
    glayout->setHorizontalSpacing(0);

    ComboBox *combo;
    QLabel *label;
    int itemID;
    int index = 0;
    int lastColumn = 2;

    // select wave
    d_ptr->wavesUpdate(&d_ptr->waveIDs, &d_ptr->waveNames);
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        QString comboName = QString("%1%2").arg(trs("Wave")).arg(i + 1);
        QLabel *label = new QLabel(trs(comboName));
        glayout->addWidget(label, index, 0);
        ComboBox *combo = new ComboBox;
        d_ptr->selectWaves.append(combo);
        combo->addItem(trs("Off"));
        foreach(QString name, d_ptr->waveNames)
        {
            combo->addItem(trs(name));
        }
        if (d_ptr->waveNames.size() <= i)
        {
            combo->setEnabled(false);
        }
        else
        {
            combo->blockSignals(true);
            combo->setCurrentIndex(i + 1);
            combo->blockSignals(false);
        }
        combo->setProperty("comboItem", qVariantFromValue(i));
        connect(combo, SIGNAL(currentIndexChanged(QString)), this, SLOT(onSelectWaveChanged(QString)));
        glayout->addWidget(combo, index, lastColumn);
        index++;
    }

    // print time
    label = new QLabel(trs("PrintTime"));
    glayout->addWidget(label, index, 0);

    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(PrintSymbol::convert(PRINT_TIME_CONTINOUS))
                    << trs(PrintSymbol::convert(PRINT_TIME_EIGHT_SEC)));
    itemID = static_cast<int>(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_TIME);
    combo->setProperty("comboItem", qVariantFromValue(itemID));
    d_ptr->combos.insert(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_TIME, combo);
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
    glayout->addWidget(combo, index, lastColumn);
    index++;

    // print speed
    label = new QLabel(trs("PrintSpeed"));
    glayout->addWidget(label, index, 0);

    combo = new ComboBox;
    combo->addItems(QStringList()
                    << PrintSymbol::convert(PRINT_SPEED_125)
                    << PrintSymbol::convert(PRINT_SPEED_250)
                    << PrintSymbol::convert(PRINT_SPEED_500));
    itemID = static_cast<int>(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_SPEED);
    combo->setProperty("comboItem", qVariantFromValue(itemID));
    d_ptr->combos.insert(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_SPEED, combo);
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
    glayout->addWidget(combo, index, lastColumn);
    index++;

    // phy alarm
    label = new QLabel(trs("PhyAlarmTriggerPrint"));
    glayout->addWidget(label, index, 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("Enable"));
    itemID = static_cast<int>(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_PHY_ALARM);
    combo->setProperty("comboItem", qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
    glayout->addWidget(combo, index, lastColumn);
    d_ptr->combos.insert(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_PHY_ALARM, combo);
    index++;

    // code marker
    label = new QLabel(trs("CodeMarkerTriggerPrint"));
    glayout->addWidget(label, index, 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("Enable"));
    itemID = static_cast<int>(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_CODEMARKER);
    combo->setProperty("comboItem", qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
    glayout->addWidget(combo, index, lastColumn);
    d_ptr->combos.insert(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_CODEMARKER, combo);
    index++;

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        // NIBP Reading
        label = new QLabel(trs("NIBPReadingTriggerPrint"));
        glayout->addWidget(label, index, 0);
        combo = new ComboBox();
        combo->addItems(QStringList()
                        << trs("Disable")
                        << trs("Enable"));
        itemID = static_cast<int>(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_NIBP);
        combo->setProperty("comboItem", qVariantFromValue(itemID));
        connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
        glayout->addWidget(combo, index, lastColumn);
        d_ptr->combos.insert(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_NIBP, combo);
        index++;
    }

    // Wave Freeze
    label = new QLabel(trs("WaveFreezeTriggerPrint"));
    glayout->addWidget(label, index, 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("Disable")
                    << trs("Enable"));
    itemID = static_cast<int>(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_WAVE_FREEZE);
    combo->setProperty("comboItem", qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
    glayout->addWidget(combo, index, lastColumn);
    d_ptr->combos.insert(PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_WAVE_FREEZE, combo);
}

void PrintSettingMenuContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->printTimerId == ev->timerId())
    {
        if (!recorderManager.isPrinting() || d_ptr->timeoutNum == 10)
        {
            if (!recorderManager.isPrinting() && !recorderManager.getPrintStatus())
            {
                recorderManager.addPageGenerator(new ContinuousPageGenerator());
            }
            killTimer(d_ptr->printTimerId);
            d_ptr->printTimerId = -1;
            d_ptr->timeoutNum = 0;
        }
        d_ptr->timeoutNum++;
    }
    else if (d_ptr->waitTimerId == ev->timerId())
    {
        d_ptr->printTimerId = startTimer(STOP_PRINT_TIMEOUT);
        killTimer(d_ptr->waitTimerId);
        d_ptr->waitTimerId = -1;
        d_ptr->isWait = false;
    }
}

void PrintSettingMenuContent::onComboxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    if (!combo)
    {
        return;
    }
    PrintSettingMenuContentPrivate::MenuItem
    item = static_cast<PrintSettingMenuContentPrivate::MenuItem>(combo->property("comboItem").toInt());
    switch (item)
    {
    case PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_SPEED:
        recorderManager.setPrintSpeed((PrintSpeed)index);
        break;
    case PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_TIME:
        recorderManager.setPrintTime((PrintTime)index);
        break;
    case PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_PHY_ALARM:
        systemConfig.setNumValue("Print|PhysiologicalAlarm", index);
        break;
    case PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_CODEMARKER:
        systemConfig.setNumValue("Print|CoderMarker", index);
        break;
    case PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_NIBP:
        systemConfig.setNumValue("Print|NIBPReading", index);
        break;
    case PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_WAVE_FREEZE:
        systemConfig.setNumValue("Print|WaveFreeze", index);
        break;
    default:
        break;
    }
}

void PrintSettingMenuContent::onSelectWaveChanged(const QString &waveName)
{
    ComboBox *curWaveCbo = qobject_cast<ComboBox *>(sender());
    if (!curWaveCbo)
    {
        return;
    }

    // 如果选择的是关闭item，则保存完配置数据直接退出
    int item = curWaveCbo->property("comboItem").toInt();
    QString path = QString("Print|SelectWave%1").arg(item + 1);
    if (curWaveCbo->currentIndex() == 0)
    {
        systemConfig.setNumValue(path, static_cast<int>(WAVE_NONE));
        if (recorderManager.isPrinting() && !d_ptr->isWait)
        {
            recorderManager.stopPrint();
            d_ptr->waitTimerId = startTimer(2000);
            d_ptr->isWait = true;
        }
        return;
    }

    // 保存当前选择的波形ID
    int waveIndex;
    QString curWaveName = curWaveCbo->currentText();
    waveIndex = d_ptr->waveNames.indexOf(curWaveName);
    if (waveIndex >= 0 && waveIndex < d_ptr->waveNames.count())
    {
        systemConfig.setNumValue(path, d_ptr->waveIDs.at(waveIndex));
    }
    if (recorderManager.isPrinting() && !d_ptr->isWait)
    {
        recorderManager.stopPrint();
        d_ptr->waitTimerId = startTimer(2000);
        d_ptr->isWait = true;
    }

    // 收集当前所有选择菜单选择的波形id
    // 选择的波形id互不匹配时，直接退出该代码块
    int count = 0;
    QSet<QString> wavenames;
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        ComboBox *curcurWaveCbo = d_ptr->selectWaves[i];
        if (curcurWaveCbo->currentIndex() != 0)
        {
            wavenames.insert(curcurWaveCbo->currentText());
            count++;
        }
    }
    if (wavenames.size() == count)
    {
        return;
    }

    // 当选择了相同波形id时，将旧的选择的波形id替换为空
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        ComboBox *selectWaveCbo = d_ptr->selectWaves[i];
        // 剔除当前item为0的选项
        if (selectWaveCbo->currentIndex() == 0)
        {
            continue;
        }

        // 剔除当前波形控件与选择波形控件相同的选项
        if (selectWaveCbo == curWaveCbo)
        {
            continue;
        }

        // 匹配到相同波形id时，将旧的选择项替换为空，并退出轮询
        if (selectWaveCbo->currentText() == waveName)
        {
            selectWaveCbo->blockSignals(true);
            selectWaveCbo->setCurrentIndex(0);
            selectWaveCbo->blockSignals(false);
            QString path = QString("Print|SelectWave%1").arg(i + 1);
            systemConfig.setNumValue(path, static_cast<int>(WAVE_NONE));
            break;
        }
    }
}

void PrintSettingMenuContent::onConnectedStatusChanged()
{
    d_ptr->loadOptions();
    // 当co2模块在打印过程中移除时，菜单会及时更新为关闭选项，此时调用对应槽函数
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        QString path;
        path = QString("Print|SelectWave%1").arg(i + 1);
        int waveId = WAVE_NONE;
        systemConfig.getNumValue(path, waveId);
        if (waveId == WAVE_NONE)
        {
            onSelectWaveChanged(trs("Off"));
        }
    }
}

void PrintSettingMenuContentPrivate::wavesUpdate(QList<int> *waveIDs, QStringList *waveNames)
{
    waveIDs->clear();
    waveNames->clear();

    // ecg
    int index = 0;
    currentConfig.getNumValue("ECG|Ecg1Wave", index);
    ECGLead ecgLead = static_cast<ECGLead>(index);
    WaveformID waveID = ecgParam.leadToWaveID(ecgLead);
    waveIDs->append(waveID);
    waveNames->append(ECGSymbol::convert(ecgLead, ecgParam.getLeadConvention()));

    ECGLeadMode leadMode = ecgParam.getLeadMode();
    if (leadMode == ECG_LEAD_MODE_5
            || leadMode == ECG_LEAD_MODE_12)
    {
        int index = 0;
        currentConfig.getNumValue("ECG|Ecg2Wave", index);
        ECGLead ecgLead = static_cast<ECGLead>(index);
        WaveformID waveID = ecgParam.leadToWaveID(ecgLead);
        waveIDs->append(waveID);
        waveNames->append(ECGSymbol::convert(ecgLead, ecgParam.getLeadConvention()));
    }

    if (systemManager.isSupport(CONFIG_RESP))
    {
        // resp
        waveIDs->append(WAVE_RESP);
        waveNames->append(trs(paramInfo.getParamWaveformName(WAVE_RESP)));
    }

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        // spo2
        waveIDs->append(WAVE_SPO2);
        waveNames->append(trs(paramInfo.getParamWaveformName(WAVE_SPO2)));
    }

    // add ibp waveform when the module is connected to the host
    if (systemManager.isSupport(CONFIG_IBP) && ibpParam.isConnected())
    {
        // ibp
        IBPLabel ibpTitle = ibpParam.getEntitle(IBP_CHN_1);
        waveID = ibpParam.getWaveformID(ibpTitle);
        waveIDs->append(waveID);
        waveNames->append(IBPSymbol::convert(ibpTitle));

        ibpTitle = ibpParam.getEntitle(IBP_CHN_2);
        waveID = ibpParam.getWaveformID(ibpTitle);
        waveIDs->append(waveID);
        waveNames->append(IBPSymbol::convert(ibpTitle));
    }

    // add CO2 waveform when the module is connected to the host
    if (co2Param.isConnected())
    {
        // co2
        waveIDs->append(WAVE_CO2);
        waveNames->append(trs(paramInfo.getParamWaveformName(WAVE_CO2)));
    }
}
