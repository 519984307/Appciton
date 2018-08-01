/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/27
 **/
#include "FreezeWindow.h"
#include <QLabel>
#include <QList>
#include <QHBoxLayout>
#include <QGridLayout>
#include "Button.h"
#include "ComboBox.h"
#include <QKeyEvent>
#include "LanguageManager.h"
#include "WindowManager.h"
#include "FreezeManager.h"
#include "FreezePageGenerator.h"
#include "RecorderManager.h"

#define RECORD_FREEZE_WAVE_NUM 3
class FreezeWindowPrivate
{
public:
    FreezeWindowPrivate();
    ComboBox *combos[RECORD_FREEZE_WAVE_NUM];
    Button *leftBtn;
    Button *rightBtn;
    Button *printBtn;
    QList<int> waveIDs;
};

FreezeWindowPrivate::FreezeWindowPrivate()
    : leftBtn(NULL),
      rightBtn(NULL),
      printBtn(NULL)
{
    for (int i = 0; i < RECORD_FREEZE_WAVE_NUM; i++)
    {
        combos[i] = NULL;
    }
    waveIDs.clear();
}

FreezeWindow::FreezeWindow()
    : Window(),
      d_ptr(new FreezeWindowPrivate)
{
    setWindowTitle(trs("Freeze"));

    QStringList waveNames;
    windowManager.getDisplayedWaveformIDsAndLabels(
        d_ptr->waveIDs, waveNames);
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QHBoxLayout *hl;

    hl = new QHBoxLayout;
    for (int i = 0; i < RECORD_FREEZE_WAVE_NUM; i++)
    {
        QString comboName = QString("%1%2").arg(trs("Wave")).arg(i + 1);
        QLabel *label = new QLabel(trs(comboName));
        hl->addWidget(label);
        ComboBox *combo = new ComboBox;
        d_ptr->combos[i] = combo;
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
        hl->addWidget(combo);
    }
    layout->addLayout(hl, 0, 0);

    hl = new QHBoxLayout;
    d_ptr->leftBtn = new Button;
    d_ptr->leftBtn->setButtonStyle(Button::ButtonIconOnly);
    // d_ptr->leftBtn->setIconSize(QSize(50, 50));
    d_ptr->leftBtn->setIcon(QIcon("/usr/local/nPM/icons/ArrowLeft.png"));
    hl->addWidget(d_ptr->leftBtn, 1);
    connect(d_ptr->leftBtn, SIGNAL(released()), this, SLOT(onBtnClick()));

    d_ptr->rightBtn = new Button;
    d_ptr->rightBtn->setButtonStyle(Button::ButtonIconOnly);
    // d_ptr->rightBtn->setIconSize(QSize(50, 50));
    d_ptr->rightBtn->setIcon(QIcon("/usr/local/nPM/icons/ArrowRight.png"));
    hl->addWidget(d_ptr->rightBtn, 1);
    connect(d_ptr->rightBtn, SIGNAL(released()), this, SLOT(onBtnClick()));

    d_ptr->printBtn = new Button(trs("Print"));
    d_ptr->printBtn->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(d_ptr->printBtn, 1);
    connect(d_ptr->printBtn, SIGNAL(released()), this, SLOT(onBtnClick()));
    d_ptr->printBtn->setEnabled(!!waveNames.size());

    layout->addLayout(hl, 1, 0);
    layout->setRowStretch(2, 1);
    setWindowLayout(layout);
}

FreezeWindow::~FreezeWindow()
{
    delete d_ptr;
}

void FreezeWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);
    QRect rect = windowManager.getMenuArea();

    move(rect.x() + (rect.width() - width()) / 2, rect.y() + rect.height() - height());

    freezeManager.startFreeze();
}

void FreezeWindow::hideEvent(QHideEvent *ev)
{
    Window::hideEvent(ev);
    freezeManager.stopFreeze();
}

void FreezeWindow::onSelectWaveChanged(const QString &waveName)
{
    ComboBox *cmbList = qobject_cast<ComboBox *>(sender());
    if (!cmbList)
    {
        return;
    }

    QSet<QString> wavenames;
    int count = 0;
    for (int i = 0; i < RECORD_FREEZE_WAVE_NUM; i++)
    {
        ComboBox *curCmbList = d_ptr->combos[i];
        if (curCmbList->currentIndex() != 0) // not in off state
        {
            wavenames.insert(curCmbList->currentText());
            count++;
        }
    }

    if (wavenames.size() == count)
    {
        // no duplicated wavenames
        return;
    }

    // have duplicated wavenames, select another wavename for the duplicate combolist
    for (int i = 0; i < RECORD_FREEZE_WAVE_NUM; i++)
    {
        ComboBox *curCmbList = d_ptr->combos[i];
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

                if (!wavenames.contains(curCmbList->itemText(j)))
                {
                    curCmbList->blockSignals(true);
                    curCmbList->setCurrentIndex(j);
                    curCmbList->blockSignals(false);
                    return;
                }
            }

            break;
        }
    }
}

void FreezeWindow::onBtnClick()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->leftBtn)
    {
        if (!freezeManager.isInReviewMode())
        {
            freezeManager.enterReviewMode();
            return;
        }

        int reviewSecond = freezeManager.getCurReviewSecond() - 1;
        if (reviewSecond < 0)
        {
            reviewSecond = 0;
        }
        freezeManager.setCurReviewSecond(reviewSecond);
    }
    else if (btn == d_ptr->rightBtn)
    {
        if (!freezeManager.isInReviewMode())
        {
            freezeManager.enterReviewMode();
            return;
        }

        int reviewSecond = freezeManager.getCurReviewSecond() + 1;
        if (reviewSecond > FreezeManager::MAX_REVIEW_SECOND)
        {
            reviewSecond = FreezeManager::MAX_REVIEW_SECOND;
        }
        freezeManager.setCurReviewSecond(reviewSecond);
    }
    else if (btn == d_ptr->printBtn)
    {
        QList<FreezePageGenerator::FreezeWaveInfo> waveinfos;

        for (int i = 0; i < RECORD_FREEZE_WAVE_NUM; i++)
        {
            int curIndex = d_ptr->combos[i]->currentIndex() - 1;
            if (curIndex < 0)
            {
                continue;
            }

            FreezePageGenerator::FreezeWaveInfo waveinfo;
            waveinfo.id = static_cast<WaveformID>(d_ptr->waveIDs.at(curIndex));
            FreezeDataModel *model = freezeManager.getWaveDataModel(waveinfo.id);
            waveinfo.timestampOfLastSecond = model->timestamp() - model->getReviewStartSecond();
            WaveWidget *w = windowManager.getWaveWidget(waveinfo.id);
            if (w)
            {
                int datasize = w->bufSize();
                int sampleRate = model->getSampleRate();
                if (datasize % sampleRate != 0)
                {
                    // get wave data align to the seconds, datasize should be divided by sample rate without remains.
                    datasize = ((datasize / sampleRate) + 1) * sampleRate;
                }
                waveinfo.data.resize(datasize);
                model->getWaveData(waveinfo.data.data(), datasize);
            }
            waveinfos.append(waveinfo);
        }

        RecordPageGenerator *generator = new FreezePageGenerator(freezeManager.getTrendData(),
                waveinfos);

        recorderManager.addPageGenerator(generator);
    }
}





















