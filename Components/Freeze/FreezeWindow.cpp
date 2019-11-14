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
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/MoveButton.h"
#include "Framework/Language/LanguageManager.h"
#include <QKeyEvent>
#include "FreezeManager.h"
#include "FreezePageGenerator.h"
#include "RecorderManager.h"
#include "EventStorageManager.h"
#include "LayoutManager.h"
#include "WaveWidget.h"
#include "TimeManager.h"
#include <QDebug>
#include <TrendCache.h>
#include "WindowManager.h"

#define STOP_PRINT_TIMEOUT          (100)

#define RECORD_FREEZE_WAVE_NUM 3
class FreezeWindowPrivate
{
public:
    FreezeWindowPrivate();
    ComboBox *combos[RECORD_FREEZE_WAVE_NUM];
    MoveButton *waveMoveBtn;
    Button *printBtn;
    QList<int> waveIDs;
    int printTimerId;
    int waitTimerId;
    bool isWait;
    int timeoutNum;
    RecordPageGenerator *generator;
};

FreezeWindowPrivate::FreezeWindowPrivate()
    : waveMoveBtn(NULL),
      printBtn(NULL),
      printTimerId(-1),
      waitTimerId(-1),
      isWait(false),
      timeoutNum(0),
      generator(NULL)
{
    for (int i = 0; i < RECORD_FREEZE_WAVE_NUM; i++)
    {
        combos[i] = NULL;
    }
    waveIDs.clear();
}

FreezeWindow::FreezeWindow()
    : Dialog(),
      d_ptr(new FreezeWindowPrivate)
{
    setWindowTitle(trs("Freeze"));

    QStringList waveNames;
    d_ptr->waveIDs = layoutManager.getDisplayedWaveformIDs();
    waveNames = layoutManager.getDisplayedWaveformLabels();
    QGridLayout *layout = new QGridLayout();
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
    d_ptr->waveMoveBtn = new MoveButton(trs("Scroll"));
    d_ptr->waveMoveBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->waveMoveBtn, SIGNAL(leftMove()), this, SLOT(leftMoveWaveformSlot()));
    connect(d_ptr->waveMoveBtn, SIGNAL(rightMove()), this, SLOT(rightMoveWaveformSlot()));
    hl->addWidget(d_ptr->waveMoveBtn, 2);

    d_ptr->printBtn = new Button(trs("Print"));
    d_ptr->printBtn->setButtonStyle(Button::ButtonTextOnly);
    hl->addWidget(d_ptr->printBtn, 1);
    connect(d_ptr->printBtn, SIGNAL(released()), this, SLOT(onBtnClick()));
    d_ptr->printBtn->setEnabled(!!waveNames.size());

    layout->addLayout(hl, 1, 0);
    layout->setRowStretch(2, 1);
    setWindowLayout(layout);
    // 加大宽度，使得英文‘Waveform Move’可以完全显示
    setFixedWidth(480);
}

FreezeWindow::~FreezeWindow()
{
    delete d_ptr;
}

void FreezeWindow::showEvent(QShowEvent *ev)
{
    Dialog::showEvent(ev);
    QRect rect = layoutManager.getMenuArea();

    move(rect.x() + (rect.width() - width()) / 2, rect.y() + rect.height() - height());

    unsigned currentTime = timeManager.getCurTime();
    eventStorageManager.triggerWaveFreezeEvent(currentTime);

    // 将开始冻结打印接口放在触发波形冻结事件后面，这样该接口内部的趋势表数据缓冲容器可以及时获取有效缓冲数据
    freezeManager.startFreeze();

    // 更新打印按键状态
    if (recorderManager.isConnected())
    {
        d_ptr->printBtn->setEnabled(true);
    }
    else
    {
        d_ptr->printBtn->setEnabled(false);
    }
}

void FreezeWindow::hideEvent(QHideEvent *ev)
{
    Dialog::hideEvent(ev);
    freezeManager.stopFreeze();
    windowManager.closeAllWidows();
}

void FreezeWindow::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->printTimerId == ev->timerId())
    {
        if (!recorderManager.isPrinting() || d_ptr->timeoutNum == 10)  // 1000ms超时处理
        {
            if (!recorderManager.isPrinting() && !recorderManager.getPrintStatus())
            {
                recorderManager.addPageGenerator(d_ptr->generator);
            }
            else
            {
                d_ptr->generator->deleteLater();
                d_ptr->generator = NULL;
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
        if (curCmbList->currentIndex() != 0)  // not in off state
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
    if (btn == d_ptr->printBtn)
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
            WaveWidget *w = layoutManager.getDisplayedWaveWidget(waveinfo.id);
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

        if (recorderManager.isPrinting() && !d_ptr->isWait)
        {
            if (generator->getPriority() <= recorderManager.getCurPrintPriority())
            {
                generator->deleteLater();
            }
            else
            {
                recorderManager.stopPrint();
                d_ptr->generator = generator;
                d_ptr->waitTimerId = startTimer(2000);  // 等待2000ms
                d_ptr->isWait = true;
            }
        }
        else if (!recorderManager.getPrintStatus())
        {
            recorderManager.addPageGenerator(generator);
        }
        else
        {
            generator->deleteLater();
        }
    }
}

void FreezeWindow::leftMoveWaveformSlot()
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

void FreezeWindow::rightMoveWaveformSlot()
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





















