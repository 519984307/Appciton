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
#include "Button.h"
#include "ComboBox.h"
#include <QLabel>
#include <QGridLayout>
#include "LanguageManager.h"
#include "RecorderManager.h"
#include "WindowManager.h"
#include "IConfig.h"
#include "LayoutManager.h"
#include "ECGParam.h"
#include "IBPParam.h"
#include "RESPSymbol.h"
#include "SPO2Symbol.h"
#include "CO2Symbol.h"
#include "ContinuousPageGenerator.h"
#include "TimeManager.h"

#define STOP_PRINT_TIMEOUT          (100)

class PrintSettingMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_PRINT_TIME,
        ITEM_CBO_PRINT_SPEED,
    };

    PrintSettingMenuContentPrivate()
        : printTimeCbo(NULL),
          printSpeedCbo(NULL),
          printTimerId(-1),
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
    void wavesUpdate(QList<int> &waveIDs, QStringList &waveNames);

    QList<ComboBox *> selectWaves;
    QList<int> waveIDs;
    QStringList waveNames;
    ComboBox *printTimeCbo;
    ComboBox *printSpeedCbo;

    int printTimerId;
    int waitTimerId;
    bool isWait;
    int timeoutNum;
};

void PrintSettingMenuContentPrivate::loadOptions()
{
    PrintTime timeSec = recorderManager.getPrintTime();
    printTimeCbo->setCurrentIndex(timeSec);

    PrintSpeed speed = recorderManager.getPrintSpeed();
    printSpeedCbo->setCurrentIndex(speed);

    // update wave
    QList<int> updatedWaveIDs;
    wavesUpdate(updatedWaveIDs, waveNames);

    if (waveIDs != updatedWaveIDs)
    {
        waveIDs = updatedWaveIDs;
        for (int i = 0; i < PRINT_WAVE_NUM; i++)
        {
            ComboBox *combo = selectWaves.at(i);
            combo->blockSignals(true);
            combo->clear();
            combo->addItem(trs("Off"));
            foreach(QString name, waveNames)
            {
                combo->addItem(name);
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
    int item = 0;
    int index = 0;
    int lastColumn = 2;

    // select wave
    d_ptr->wavesUpdate(d_ptr->waveIDs, d_ptr->waveNames);
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
            combo->addItem(name);
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
    d_ptr->printTimeCbo = combo;
    combo->addItems(QStringList()
                    << trs(PrintSymbol::convert(PRINT_TIME_CONTINOUS))
                    << trs(PrintSymbol::convert(PRINT_TIME_EIGHT_SEC))
                   );
    combo->setProperty("comboItem", qVariantFromValue(item));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
    glayout->addWidget(combo, index, lastColumn);
    index++;
    item++;

    // print speed
    label = new QLabel(trs("PrintSpeed"));
    glayout->addWidget(label, index, 0);

    combo = new ComboBox;
    d_ptr->printSpeedCbo = combo;
    combo->addItems(QStringList()
                    << PrintSymbol::convert(PRINT_SPEED_125)
                    << PrintSymbol::convert(PRINT_SPEED_250)
                    << PrintSymbol::convert(PRINT_SPEED_500)
                   );
    combo->setProperty("comboItem", qVariantFromValue(item));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
    glayout->addWidget(combo, index, lastColumn);
    index++;

    glayout->setRowStretch(index, 1);
}

void PrintSettingMenuContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->printTimerId == ev->timerId())
    {
        if (!recorderManager.isPrinting() || d_ptr->timeoutNum == 10)
        {
            if (!recorderManager.isPrinting())
            {
                recorderManager.addPageGenerator(new ContinuousPageGenerator(timeManager.getCurTime()));
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
    int item = combo->property("comboItem").toInt();
    switch (item)
    {
    case PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_SPEED:
        recorderManager.setPrintSpeed((PrintSpeed)index);
        break;
    case PrintSettingMenuContentPrivate::ITEM_CBO_PRINT_TIME:
        recorderManager.setPrintTime((PrintTime)index);
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

void PrintSettingMenuContentPrivate::wavesUpdate(QList<int> &waveIDs, QStringList &waveNames)
{
    waveIDs.clear();
    waveNames.clear();

    // ecg
    int index = 0;
    currentConfig.getNumValue("ECG|Ecg1Wave", index);
    ECGLead ecgLead = static_cast<ECGLead>(index);
    WaveformID waveID = ecgParam.leadToWaveID(ecgLead);
    waveIDs.append(waveID);
    waveNames.append(ECGSymbol::convert(ecgLead, ecgParam.getLeadConvention()));

    ECGLeadMode leadMode = ecgParam.getLeadMode();
    if (leadMode == ECG_LEAD_MODE_5
            || leadMode == ECG_LEAD_MODE_12)
    {
        int index = 0;
        currentConfig.getNumValue("ECG|Ecg2Wave", index);
        ECGLead ecgLead = static_cast<ECGLead>(index);
        WaveformID waveID = ecgParam.leadToWaveID(ecgLead);
        waveIDs.append(waveID);
        waveNames.append(ECGSymbol::convert(ecgLead, ecgParam.getLeadConvention()));
    }

    if (systemManager.isSupport(CONFIG_RESP))
    {
        // resp
        waveIDs.append(WAVE_RESP);
        waveNames.append(paramInfo.getParamWaveformName(WAVE_RESP));
    }

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        // spo2
        waveIDs.append(WAVE_SPO2);
        waveNames.append(paramInfo.getParamWaveformName(WAVE_SPO2));
    }

    if (systemManager.isSupport(CONFIG_IBP))
    {
        // ibp
        IBPPressureName ibpTitle = ibpParam.getEntitle(IBP_INPUT_1);
        waveID = ibpParam.getWaveformID(ibpTitle);
        waveIDs.append(waveID);
        waveNames.append(IBPSymbol::convert(ibpTitle));

        ibpTitle = ibpParam.getEntitle(IBP_INPUT_2);
        waveID = ibpParam.getWaveformID(ibpTitle);
        waveIDs.append(waveID);
        waveNames.append(IBPSymbol::convert(ibpTitle));
    }

    if (systemManager.isSupport(CONFIG_CO2))
    {
        // co2
        waveIDs.append(WAVE_CO2);
        waveNames.append(paramInfo.getParamWaveformName(WAVE_CO2));
    }
}
