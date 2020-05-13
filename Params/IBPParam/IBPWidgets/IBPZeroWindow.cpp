/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/5/13
 **/

#include "IBPZeroWindow.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include "IBPParam.h"
#include <QGridLayout>
#include <QLabel>
#include <QTimerEvent>
#include <QBitArray>

#define ZERO_QUERY_INTERVAL   100   // ms
#define ZERO_TIMEOUT_DURATION   5000    // ms

class IBPZeroWindowPrivate
{
public:
    IBPZeroWindowPrivate()
        : chnCbo(NULL),
          zeroBtn(NULL),
          infoLbl(NULL),
          zeroChannel(IBP_CHN_1),
          timeElapsed(0),
          timerID(-1),
          zeroReply(IBP_CHN_NR),
          zeroResult(IBP_CHN_NR)
    {}

    /**
     * @brief getZeroResult get the zero result
     * @return the result string
     */
    QString getZeroResult() const
    {
        QString result;

        if (zeroChannel == IBP_CHN_NR)
        {
            if (zeroResult.count(true) == IBP_CHN_NR)
            {
                /* all success */
                result = trs("ZeroSuccessfully");
            }
            else if (zeroResult.count(false) == IBP_CHN_NR)
            {
                /* all fail */
                result = trs("ZeroFailed");
            }
            else
            {
                QString s = "IBP ";
                QString f = "IBP ";
                bool sComma = false;
                bool fComma = false;
                for (int i = 0; i < IBP_CHN_NR; i++)
                {
                    if (zeroResult.count(i))
                    {
                        if (sComma)
                        {
                            s += ", ";
                        }
                        s += QString::number(i + 1);
                        sComma = true;
                    }
                    else
                    {
                        if (fComma)
                        {
                            f += ", ";
                        }
                        f += QString::number(i + 1);
                        fComma = true;
                    }
                }

                result = QString("%1 %2; %3 %4").arg(s).arg(trs("ZeroSuccessfully"))
                                                .arg(f).arg(trs("ZeroFailed"));
            }
        }
        else
        {
            if (zeroResult.at(zeroChannel))
            {
                result = QString("IBP %1 %2").arg(zeroChannel + 1).arg(trs("ZeroSuccessfully"));
            }
            else
            {
                result = trs("ZeroFailed");
            }
        }
        return result;
    }

    ComboBox *chnCbo;   /* channel selection combo */
    Button *zeroBtn;    /* zero button */
    QLabel *infoLbl;    /* zero info label */
    IBPChannel zeroChannel;     /* Zero all channels when the value is IBP_CHN_NR */
    int timeElapsed;    /* zero time elapsed */
    int timerID;        /* zero timer ID */
    QBitArray zeroReply;
    QBitArray zeroResult;
};

IBPZeroWindow::IBPZeroWindow()
    : Dialog(),
      pimpl(new IBPZeroWindowPrivate())
{
    setWindowTitle(trs("IBPZero"));
    QGridLayout *layout = new QGridLayout();
    layout->setMargin(10);

    QLabel *label = new QLabel(trs("ZeroChannel"));
    layout->addWidget(label, 0, 0);
    ComboBox *combo = new ComboBox();
    QStringList items;
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        items.append(QString("IBP %1").arg(i + 1));
    }
    items.append(trs("All"));
    combo->addItems(items);
    layout->addWidget(combo, 0, 1);
    pimpl->chnCbo = combo;

    Button *btn = new Button(trs("Start"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, 1, 1);
    connect(btn, SIGNAL(released()), this, SLOT(startZero()));
    pimpl->zeroBtn = btn;

    label = new QLabel();
    layout->addWidget(label, 2, 0, 1, 2);
    pimpl->infoLbl = label;

    setWindowLayout(layout);

    setMinimumWidth(480);
    setMinimumHeight(240);
}

IBPZeroWindow::~IBPZeroWindow()
{
}

void IBPZeroWindow::timerEvent(QTimerEvent *ev)
{
    Dialog::timerEvent(ev);
    if (ev->timerId() == pimpl->timerID)
    {
        pimpl->timeElapsed += ZERO_QUERY_INTERVAL;
        if (pimpl->timeElapsed >= ZERO_TIMEOUT_DURATION)
        {
            pimpl->timeElapsed = 0;
            killTimer(pimpl->timerID);
            pimpl->timerID = -1;
            pimpl->infoLbl->setText(pimpl->getZeroResult());
            pimpl->zeroBtn->setEnabled(true);
        }
        else
        {
            bool allChannel =  pimpl->zeroChannel == IBP_CHN_NR;
            /* check reply */
            for (int i = 0; i < IBP_CHN_NR; i++)
            {
                if (allChannel || i == pimpl->zeroChannel)
                {
                    IBPChannel chn = static_cast<IBPChannel>(i);
                    bool reply = ibpParam.hasIBPZeroReply(chn);
                    if (reply)
                    {
                        pimpl->zeroReply.setBit(i, true);
                        pimpl->zeroResult.setBit(i, ibpParam.getLastZeroResult(chn));
                    }
                }
            }

            /* check complete */
            if ((allChannel && pimpl->zeroReply.count(true))
                 || (!allChannel && pimpl->zeroReply.at(pimpl->zeroChannel)))
            {
                pimpl->infoLbl->setText(pimpl->getZeroResult());
                pimpl->zeroBtn->setEnabled(true);
                pimpl->timeElapsed = 0;
                killTimer(pimpl->timerID);
                pimpl->timerID = -1;
            }
        }
    }
}

void IBPZeroWindow::startZero()
{
    pimpl->zeroBtn->setEnabled(false);
    pimpl->infoLbl->setText(trs("IBPZeroing"));
    pimpl->zeroReply.fill(false);

    int index = pimpl->chnCbo->currentIndex();
    pimpl->zeroChannel = static_cast<IBPChannel>(index);
    if (pimpl->zeroChannel == IBP_CHN_NR)
    {
        for (int i = 0; i < IBP_CHN_NR; i++)
        {
            ibpParam.zeroChannel(static_cast<IBPChannel>(i));
        }
    }
    else
    {
        ibpParam.zeroChannel(pimpl->zeroChannel);
    }

    pimpl->timeElapsed = 0;
    pimpl->timerID = startTimer(ZERO_QUERY_INTERVAL);
}
