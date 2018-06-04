#include "FreezeWidget.h"
#include "IComboList.h"
#include "LanguageManager.h"
#include <QBoxLayout>
#include "WindowManager.h"
#include "FontManager.h"
#include <QSet>
#include "FreezeManager.h"
#include <QKeyEvent>

#define RECORD_FREEZE_WAVE_NUM 3
class FreezeWidgetPrivate
{
public:
    FreezeWidgetPrivate()
        :leftBtn(NULL),
          rightBtn(NULL),
          printBtn(NULL)
    {
        for(int i = 0; i< RECORD_FREEZE_WAVE_NUM; i++)
        {
            comboLists[i] = NULL;
        }
    }

    IComboList* comboLists[RECORD_FREEZE_WAVE_NUM];
    IButton *leftBtn;
    IButton *rightBtn;
    IButton *printBtn;
    QList<int> waveIDs;
};

FreezeWidget::FreezeWidget()
    :PopupWidget(), d_ptr(new FreezeWidgetPrivate())
{
    setTitleBarText(trs("Freeze"));

    QStringList waveNames;
    windowManager.getDisplayedWaveformIDsAndLabels(d_ptr->waveIDs, waveNames);

    QFont font = fontManager.textFont(fontManager.getFontSize(1));

    contentLayout->addStretch(1);
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setMargin(4);
    hlayout->setSpacing(10);
    hlayout->addStretch(1);
    for(int i = 0; i< RECORD_FREEZE_WAVE_NUM; i++)
    {
        QString comboName = QString("%1%2").arg(trs("Wave")).arg(i + 1);
        IComboList *cmbList = new IComboList(comboName);
        d_ptr->comboLists[i] = cmbList;
        cmbList->setFont(font);
        cmbList->addItem(trs("Off"));
        foreach (QString name, waveNames) {
            cmbList->addItem(name);
        }
        cmbList->label->setFixedHeight(ITEM_H);
        cmbList->combolist->setFixedHeight(ITEM_H);

        if(waveNames.size()<=i)
        {
            cmbList->combolist->setEnabled(false);
        }
        else
        {
            //set teh current wave in order
            cmbList->setCurrentIndex(i+1);
        }

        connect(cmbList, SIGNAL(currentIndexChanged(QString)), this, SLOT(onSelectWaveChanged(QString)));

        hlayout->addWidget(cmbList);
    }
    hlayout->addStretch(1);
    contentLayout->addLayout(hlayout, 1);

    hlayout = new QHBoxLayout();
    hlayout->setMargin(4);
    hlayout->setSpacing(10);

    d_ptr->leftBtn = new IButton;
    d_ptr->leftBtn->setFixedHeight(ITEM_H);
    d_ptr->leftBtn->setPicture(QImage("/usr/local/nPM/icons/ArrowLeft.png"));
    d_ptr->leftBtn->setFixedWidth(100);
    connect(d_ptr->leftBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    hlayout->addWidget(d_ptr->leftBtn);

    d_ptr->rightBtn = new IButton;
    d_ptr->rightBtn->setFixedHeight(ITEM_H);
    d_ptr->rightBtn->setPicture(QImage("/usr/local/nPM/icons/ArrowRight.png"));
    d_ptr->rightBtn->setFixedWidth(100);
    connect(d_ptr->rightBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    hlayout->addWidget(d_ptr->rightBtn);

    d_ptr->printBtn = new IButton(trs("Print"));
    d_ptr->printBtn->setFont(font);
    d_ptr->printBtn->setFixedHeight(ITEM_H);
    d_ptr->printBtn->setFixedWidth(100);
    connect(d_ptr->printBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    if(waveNames.size() == 0)
    {
        d_ptr->printBtn->setEnabled(false);
    }
    hlayout->addWidget(d_ptr->printBtn);

    contentLayout->addLayout(hlayout, 1);

    this->setFixedSize(windowManager.getPopMenuWidth() / 2, ITEM_H * 4);
}

FreezeWidget::~FreezeWidget()
{

}

void FreezeWidget::showEvent(QShowEvent *ev)
{
    PopupWidget::showEvent(ev);
    QRect rect = windowManager.getMenuArea();

    //move to bottom
    move(rect.x() + (rect.width() - width()) / 2, rect.y() + rect.height() - height());

    freezeManager.startFreeze();
}

void FreezeWidget::hideEvent(QHideEvent *ev)
{
    PopupWidget::hideEvent(ev);
    freezeManager.stopFreeze();
}

void FreezeWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusPreviousChild();
            break;
        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;
        default:
            break;
    }

    QWidget::keyPressEvent(e);
}

void FreezeWidget::onSelectWaveChanged(const QString &waveName)
{
    IComboList *cmbList = qobject_cast<IComboList *>(sender());
    if(!cmbList)
    {
        return;
    }

    QSet<QString> wavenames;
    int count;
    for(int i = 0; i< RECORD_FREEZE_WAVE_NUM; i++)
    {
        IComboList *curCmbList = d_ptr->comboLists[i];
        if(curCmbList->currentIndex() != 0) // not in off state
        {
            wavenames.insert(curCmbList->currentText());
            count ++;
        }
    }

    if(wavenames.size() == 0)
    {
        d_ptr->printBtn->setEnabled(false);
    }
    else
    {
        d_ptr->printBtn->setEnabled(true);
    }

    if(wavenames.size() == count)
    {
        //no duplicated wavenames
        return;
    }

    //have duplicated wavenames, select another wavename for the duplicate combolist
    for(int i = 0; i < RECORD_FREEZE_WAVE_NUM; i++)
    {
        IComboList *curCmbList = d_ptr->comboLists[i];
        if(curCmbList != cmbList
                && curCmbList->currentIndex() != 0
                && curCmbList->currentText() == waveName)
        {
            int curIndex = curCmbList->currentIndex();
            for(int j = 1; j < curCmbList->count(); j++)
            {
                if(j == curIndex)
                {
                    continue;
                }

                if(!wavenames.contains(curCmbList->itemText(j)))
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

void FreezeWidget::onBtnClick()
{
    IButton *btn = qobject_cast<IButton *>(sender());
    if(btn == d_ptr->leftBtn)
    {
        if(!freezeManager.isInReviewMode())
        {
            freezeManager.enterReviewMode();
            return;
        }

        int reviewSecond = freezeManager.getCurReviewSecond() - 1;
        if(reviewSecond < 0)
        {
            reviewSecond = 0;
        }
        freezeManager.setCurReviewSecond(reviewSecond);
    }
    else if(btn == d_ptr->rightBtn)
    {
        if(!freezeManager.isInReviewMode())
        {
            freezeManager.enterReviewMode();
            return;
        }

        int reviewSecond = freezeManager.getCurReviewSecond() + 1;
        if(reviewSecond > FreezeManager::MAX_REVIEW_SECOND)
        {
            reviewSecond = FreezeManager::MAX_REVIEW_SECOND;
        }
        freezeManager.setCurReviewSecond(reviewSecond);
    }
    else if(btn == d_ptr->printBtn)
    {

    }
}
