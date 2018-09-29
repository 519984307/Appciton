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

class PrintSettingMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_PRINT_TIME,
        ITEM_CBO_PRINT_SPEED,
    };

    PrintSettingMenuContentPrivate()
        : clearPrintTask(NULL),
          printTime(NULL),
          printSpeed(NULL)
    {
    }
    /**
     * @brief loadOptions
     */
    void loadOptions(void);

    Button *clearPrintTask;
    QList<ComboBox *> selectWaves;
    QList<int> waveIDs;
    QStringList waveNames;
    ComboBox *printTime;
    ComboBox *printSpeed;
};

void PrintSettingMenuContentPrivate::loadOptions()
{
    PrintTime timeSec = recorderManager.getPrintTime();
    printTime->setCurrentIndex(timeSec);

    PrintSpeed speed = recorderManager.getPrintSpeed();
    printSpeed->setCurrentIndex(speed);

    int index[PRINT_WAVE_NUM] = {0};
    QSet<int> waveCboIds;
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        QString path;
        path = QString("Print|SelectWave%1").arg(i + 1);
        systemConfig.getNumValue(path, index[i]);
        if (index[i] > WAVE_NONE && index[i] < WAVE_NR)
        {
            waveCboIds.insert(index[i]);
        }
    }
    if (waveCboIds.size() < PRINT_WAVE_NUM)
    {
        for (int i = 0; i < PRINT_WAVE_NUM; i++)
        {
            QString path;
            path = QString("Print|SelectWave%1").arg(i + 1);
            systemConfig.setNumValue(path, waveIDs.at(i));
        }
    }
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        int count = 1;
        foreach(int id, waveIDs)
        {
            if (index[i] == id)
            {
                break;
            }
            count++;
        }

        if (count < selectWaves[i]->count())
        {
            selectWaves[i]->setCurrentIndex(count);
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

    ComboBox *combo;
    Button *btn;
    QLabel *label;
    int item = 0;
    int index = 0;
    int lastColumn = 2;

    // select wave
    QStringList waveNames;
    d_ptr->waveIDs = layoutManager.getDisplayedWaveformIDs();
    waveNames = layoutManager.getDisplayedWaveformLabels();
    d_ptr->waveNames = waveNames;
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        QString comboName = QString("%1%2").arg(trs("Wave")).arg(i + 1);
        QLabel *label = new QLabel(trs(comboName));
        glayout->addWidget(label, index, 0);
        ComboBox *combo = new ComboBox;
        d_ptr->selectWaves.append(combo);
        combo->addItem(trs("Off"));
        foreach(QString name, waveNames)
        {
            combo->addItem(name);
        }
        if (waveNames.size() <= i)
        {
            combo->setEnabled(false);
        }
        else
        {
            combo->setCurrentIndex(i + 1);
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
    d_ptr->printTime = combo;
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
    d_ptr->printSpeed = combo;
    combo->addItems(QStringList()
                    << PrintSymbol::convert(PRINT_SPEED_125)
                    << PrintSymbol::convert(PRINT_SPEED_250)
                    << PrintSymbol::convert(PRINT_SPEED_500)
                   );
    combo->setProperty("comboItem", qVariantFromValue(item));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxIndexChanged(int)));
    glayout->addWidget(combo, index, lastColumn);
    index++;

    // clear print task
    btn = new Button(trs("ClearPrintTask"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onClearBtnReleased()));
    glayout->addWidget(btn, index, lastColumn);
    index++;

    glayout->setRowStretch(index, 1);
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
    ComboBox *cmbList = qobject_cast<ComboBox *>(sender());
    if (!cmbList)
    {
        return;
    }

    int item = cmbList->property("comboItem").toInt();
    QSet<QString> wavenames;
    int count = 0;
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        ComboBox *curCmbList = d_ptr->selectWaves[i];
        if (curCmbList->currentIndex() != 0) // not in off state
        {
            wavenames.insert(curCmbList->currentText());
            count++;
        }
    }

    QString path;
    path = QString("Print|SelectWave%1").arg(item + 1);

    // 寻找打印波形对应的波形ID号
    QString curWaveName = cmbList->currentText();
    int waveIndex = 0;
    foreach(QString waveName, d_ptr->waveNames)
    {
        if (curWaveName == waveName)
        {
            break;
        }
        waveIndex++;
    }
    systemConfig.setNumValue(path, d_ptr->waveIDs.at(waveIndex));

    if (wavenames.size() == count)
    {
        // no duplicated wavenames
        return;
    }

    // have duplicated wavenames, select another wavename for the duplicate combolist
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        ComboBox *curCmbList = d_ptr->selectWaves[i];
        if (curCmbList != cmbList
                && curCmbList->currentIndex() != 0
                && curCmbList->currentText() == waveName)
        {
            int curIndex = curCmbList->currentIndex();
            for (int j = 1; j < curCmbList->count(); j++)
            {
                if (j == curIndex)
                {
                    continue;
                }

                QString curWaveName = curCmbList->itemText(j);
                if (!wavenames.contains(curWaveName))
                {
                    curCmbList->blockSignals(true);
                    curCmbList->setCurrentIndex(j);
                    QString path;
                    path = QString("Print|SelectWave%1").arg(i + 1);

                    // 寻找打印波形对应的波形ID号
                    int waveIndex = 0;
                    foreach(QString waveName, d_ptr->waveNames)
                    {
                        if (curWaveName == waveName)
                        {
                            break;
                        }
                        waveIndex++;
                    }
                    systemConfig.setNumValue(path, d_ptr->waveIDs.at(waveIndex));

                    curCmbList->blockSignals(false);
                    return;
                }
            }

            break;
        }
    }
}

void PrintSettingMenuContent::onClearBtnReleased()
{
}
