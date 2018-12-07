/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/30
 **/

#include "TrendGraphSetWindow.h"
#include "DropList.h"
#include <QVBoxLayout>
#include "Button.h"
#include "LanguageManager.h"
#include "TrendDataSymbol.h"
#include "IConfig.h"
#include "ParamManager.h"
#include "ScrollArea.h"
#include "AlarmConfig.h"
#include "IBPParam.h"
#include <QPainter>
#include <QRect>
#include "TrendGraphWindow.h"
#include "ThemeManager.h"

#define ITEM_HEIGHT     (themeManger.getAcceptableControlHeight())
#define ITEM_WIDTH      150

TrendGraphSetWindow *TrendGraphSetWindow::selfObj = NULL;

class TrendGraphSetWindowPrivate
{
public:
    TrendGraphSetWindowPrivate();

    void trendRulerLayout(void);
    void clearRulerLayout(void);
    void loadOptions();
public:
    TrendGroup trendGroup;                 // 当前趋势组
    QList<RulerItem *> itemList;
    Button *allAutoBtn;
    DropList *trendGroupList;
    DropList *timeIntervalList;
    DropList *waveNumList;
    QVBoxLayout *rulerLayout;
    ScrollArea *mScrollArea;
};
TrendGraphSetWindow::~TrendGraphSetWindow()
{
}

TrendGroup TrendGraphSetWindow::getTrendGroup()
{
    return d_ptr->trendGroup;
}

void TrendGraphSetWindow::upDateTrendGroup()
{
    d_ptr->clearRulerLayout();
    d_ptr->trendRulerLayout();
}

void TrendGraphSetWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);

    upDateTrendGroup();
    d_ptr->loadOptions();
}

bool TrendGraphSetWindow::focusNextPrevChild(bool next)
{
    bool status = QDialog::focusNextPrevChild(next);
    QWidget *cur = focusWidget();
    if (d_ptr->mScrollArea->isAncestorOf(cur))
    {
        d_ptr->mScrollArea->ensureWidgetVisible(cur);
    }
   return  status;
}

void TrendGraphSetWindow::allAutoReleased()
{
    RulerItem *item = NULL;
    int count = d_ptr->itemList.count();
    for (int i = 0; i < count; i ++)
    {
        item = d_ptr->itemList.at(i);
        item->combo->setCurrentIndex(1);
        item->downRuler->setEnabled(false);
        item->upRuler->setEnabled(false);
        SubParamID subID = item->sid;
        item->downRuler->setEnabled(false);
        item->upRuler->setEnabled(false);
        TrendGraphWindow::getInstance()->setSubWidgetAutoRuler(subID, true);
    }
}

void TrendGraphSetWindow::trendGroupReleased(int g)
{
    QString prefix = "TrendGraph|TrendGroup";
    systemConfig.setNumValue(prefix, g);
    d_ptr->trendGroup = (TrendGroup)g;
    upDateTrendGroup();
    TrendGraphWindow::getInstance()->updateTrendGraph();
}

void TrendGraphSetWindow::timeIntervalReleased(int timeInterval)
{
    QString prefix = "TrendGraph|TimeInterval";
    systemConfig.setNumValue(prefix, timeInterval);
    TrendGraphWindow::getInstance()->timeIntervalChange(timeInterval);
}

void TrendGraphSetWindow::waveNumberReleased(int num)
{
    QString prefix = "TrendGraph|WaveNumber";
    systemConfig.setNumValue(prefix, num);
    TrendGraphWindow::getInstance()->waveNumberChange(num + 1);
}

void TrendGraphSetWindow::onComboBoxChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    if (combo)
    {
        int id = combo->property("Combo").toInt();
        RulerItem *item = d_ptr->itemList.at(id);

        SubParamID subID = item->sid;
        if (index == 0)
        {
            item->downRuler->setEnabled(true);
            item->upRuler->setEnabled(true);
        }
        else
        {
            item->downRuler->setEnabled(false);
            item->upRuler->setEnabled(false);
        }
        TrendGraphWindow::getInstance()->setSubWidgetAutoRuler(subID, index);
    }
}

void TrendGraphSetWindow::upDownRulerChange(int, int)
{
    SpinBox *spb = qobject_cast<SpinBox *>(sender());
    if (spb)
    {
        int id = spb->property("Ruler").toInt();
        RulerItem *item = d_ptr->itemList.at(id / 2);
        TrendGraphWindow::getInstance()->setSubWidgetRulerLimit(item->sid, item->downRuler->getValue(),
                                                item->upRuler->getValue(), item->downRuler->getScale());
    }
}

TrendGraphSetWindow::TrendGraphSetWindow()
    : Window(), d_ptr(new TrendGraphSetWindowPrivate())
{
    setWindowTitle(trs("TrendGraphSet"));
    QHBoxLayout *hTitleLayout = new QHBoxLayout();

    QLabel *l = new QLabel(trs("Parameter"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    hTitleLayout->addWidget(l);

    l = new QLabel(trs("AutoRuler"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    hTitleLayout->addWidget(l);

    l = new QLabel(trs("DownRuler"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    hTitleLayout->addWidget(l);

    l = new QLabel(trs("UpRuler"));
    l->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    l->setAlignment(Qt::AlignCenter);
    hTitleLayout->addWidget(l);

    QWidget *rulerWidget = new QWidget();
    rulerWidget->setFocusPolicy(Qt::NoFocus);

    ScrollArea *mScrollArea = new ScrollArea();
    mScrollArea->setWidgetResizable(true);
    mScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScrollArea->setFloatbarPolicy(ScrollArea::FloatBarNotShow);
    mScrollArea->setFocusPolicy(Qt::NoFocus);
    mScrollArea->setFrameShape(QFrame::NoFrame);
    mScrollArea->setFixedHeight(300);

    QString name;
    int count = d_ptr->itemList.count();
    d_ptr->rulerLayout = new QVBoxLayout();
    rulerWidget->setLayout(d_ptr->rulerLayout);
    for (int i = 0; i < count; i ++)
    {
        RulerItem *item = NULL;
        item = d_ptr->itemList.at(i);
        name = paramInfo.getSubParamName(item->sid);
        if (item->sid == SUB_PARAM_ETCO2)
        {
            item->label->setText("CO2(Et/Fi)");
        }
        else if (item->sid == SUB_PARAM_T1)
        {
            item->label->setText("T1/T2");
        }
        else
        {
            item->label->setText(trs(name));
        }
        item->label->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
        item->combo->addItem(trs("Off"));
        item->combo->addItem(trs("On"));
        item->combo->setCurrentIndex(1);
        item->combo->setProperty("Combo", qVariantFromValue(i));
        item->combo->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
        connect(item->combo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(onComboBoxChanged(int)));

        item->downRuler->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
        item->downRuler->setProperty("Ruler", qVariantFromValue(i * 2));
        item->downRuler->setEnabled(false);
        connect(item->downRuler, SIGNAL(valueChange(int, int)),
                this, SLOT(upDownRulerChange(int, int)));

        item->upRuler->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
        item->upRuler->setProperty("Ruler", qVariantFromValue(i * 2 + 1));
        item->upRuler->setEnabled(false);
        connect(item->upRuler, SIGNAL(valueChange(int, int)),
                this, SLOT(upDownRulerChange(int, int)));
    }
    d_ptr->loadOptions();
    mScrollArea->setWidget(rulerWidget);
    d_ptr->mScrollArea = mScrollArea;

    d_ptr->allAutoBtn = new Button(trs("AllAuto"));
    d_ptr->allAutoBtn->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->allAutoBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->allAutoBtn, SIGNAL(released()), this, SLOT(allAutoReleased()));

    QString prefix = "TrendGraph|";
    int index = 0;

    QString groupPrefix = prefix + "TrendGroup";
    systemConfig.getNumValue(groupPrefix, index);
    d_ptr->trendGroupList = new DropList(trs("TrendGroup"));
    d_ptr->trendGroupList->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->trendGroupList->addItem("Resp");
    if (systemManager.isSupport(CONFIG_IBP))
    {
        d_ptr->trendGroupList->addItem("IBP");
    }
    if (systemManager.isSupport(CONFIG_AG))
    {
        d_ptr->trendGroupList->addItem("AG");
    }
    d_ptr->trendGroupList->setCurrentIndex(index);
    connect(d_ptr->trendGroupList, SIGNAL(currentIndexChanged(int)), this, SLOT(trendGroupReleased(int)));

    index = 0;
    QString intervalPrefix = prefix + "TimeInterval";
    systemConfig.getNumValue(intervalPrefix, index);
    d_ptr->timeIntervalList = new DropList(trs("TimeInterval"));
    d_ptr->timeIntervalList->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    for (int i = 0; i < RESOLUTION_RATIO_NR; i ++)
    {
        d_ptr->timeIntervalList->addItem(TrendDataSymbol::convert((ResolutionRatio)i));
    }
    d_ptr->timeIntervalList->setCurrentIndex(index);
    connect(d_ptr->timeIntervalList, SIGNAL(currentIndexChanged(int)), this, SLOT(timeIntervalReleased(int)));

    index = 0;
    QString numPrefix = prefix + "WaveNumber";
    systemConfig.getNumValue(numPrefix, index);
    d_ptr->waveNumList = new DropList(trs("WaveNumber"));
    d_ptr->waveNumList->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->waveNumList->addItems(QStringList()
                                 << "1"
                                 << "2"
                                 << "3"
                                );
    d_ptr->waveNumList->setCurrentIndex(index);
    connect(d_ptr->waveNumList, SIGNAL(currentIndexChanged(int)), this, SLOT(waveNumberReleased(int)));
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(d_ptr->allAutoBtn);
    hLayout->addWidget(d_ptr->trendGroupList);
    hLayout->addWidget(d_ptr->timeIntervalList);
    hLayout->addWidget(d_ptr->waveNumList);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(10);
    mainLayout->setMargin(10);
    mainLayout->addLayout(hTitleLayout);
    mainLayout->addWidget(mScrollArea);
    mainLayout->addLayout(hLayout);
    setWindowLayout(mainLayout);

    upDateTrendGroup();
}

TrendGraphSetWindowPrivate::TrendGraphSetWindowPrivate()
    : trendGroup(TREND_GROUP_RESP), allAutoBtn(NULL),
      trendGroupList(NULL), timeIntervalList(NULL),
      waveNumList(NULL), rulerLayout(NULL), mScrollArea(NULL)
{
    int value = 0;
    QString prefix = "TrendGraph|TrendGroup";
    systemConfig.getNumValue(prefix, value);
    trendGroup = static_cast<TrendGroup>(value);

    RulerItem *item = NULL;
    QList<ParamID> pids;
    paramManager.getParams(pids);
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        switch (i)
        {
        case SUB_PARAM_HR_PR:
        case SUB_PARAM_SPO2:
        case SUB_PARAM_RR_BR:
        case SUB_PARAM_NIBP_SYS:
        case SUB_PARAM_ART_SYS:
        case SUB_PARAM_PA_SYS:
        case SUB_PARAM_CVP_MAP:
        case SUB_PARAM_LAP_MAP:
        case SUB_PARAM_RAP_MAP:
        case SUB_PARAM_ICP_MAP:
        case SUB_PARAM_AUXP1_SYS:
        case SUB_PARAM_AUXP2_SYS:
        case SUB_PARAM_CO_CO:
        case SUB_PARAM_CO_CI:
        case SUB_PARAM_CO_TB:
        case SUB_PARAM_ETCO2:
        case SUB_PARAM_ETN2O:
        case SUB_PARAM_ETAA1:
        case SUB_PARAM_ETAA2:
        case SUB_PARAM_ETO2:
        case SUB_PARAM_T1:
            break;
        default:
            continue;
        }

        ParamID id = paramInfo.getParamID((SubParamID)i);
        if (-1 != pids.indexOf(id))
        {
            item = new RulerItem(id, (SubParamID)i);
            itemList.append(item);
        }
    }
}

void TrendGraphSetWindowPrivate::trendRulerLayout()
{
    int count = itemList.count();
    for (int i = 0; i < count; i ++)
    {
        RulerItem *item = NULL;
        item = itemList.at(i);
        if (trendGroup == TREND_GROUP_RESP)
        {
            switch (item->sid)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_RR_BR:
            case SUB_PARAM_NIBP_SYS:
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_T1:
                break;
            default:
                continue;
            }
        }
        else if (trendGroup == TREND_GROUP_IBP)
        {
            SubParamID ibp1;
            SubParamID ibp2;
            ibpParam.getSubParamID(ibp1, ibp2);
            if (item->sid != ibp1 && item->sid != ibp2 &&
                    item->sid != SUB_PARAM_HR_PR && item->sid != SUB_PARAM_SPO2 &&
                    item->sid != SUB_PARAM_NIBP_SYS && item->sid != SUB_PARAM_T1)
            {
                continue;
            }
        }
        else if (trendGroup == TREND_GROUP_AG)
        {
            switch (item->sid)
            {
            case SUB_PARAM_HR_PR:
            case SUB_PARAM_SPO2:
            case SUB_PARAM_NIBP_SYS:
            case SUB_PARAM_T1:
            case SUB_PARAM_ETCO2:
            case SUB_PARAM_ETN2O:
            case SUB_PARAM_ETAA1:
            case SUB_PARAM_ETAA2:
            case SUB_PARAM_ETO2:
                break;
            default:
                continue;
            }
        }
        rulerLayout->addWidget(item);
        item->setVisible(true);
        item->setEnabled(true);
    }
}

void TrendGraphSetWindowPrivate::clearRulerLayout()
{
    int count = rulerLayout->count();
    for (int i = 0; i < count; i ++)
    {
        QLayoutItem *item = rulerLayout->takeAt(0);
        RulerItem *widget = qobject_cast<RulerItem *>(item->widget());
        if (widget != NULL)
        {
            widget->setVisible(false);
        }
    }
}

void TrendGraphSetWindowPrivate::loadOptions()
{
    RulerItem *item;
    int count = itemList.count();
    for (int i = 0; i < count; i++)
    {
        item = itemList.at(i);
        if (NULL == item)
        {
            continue;
        }

        SubParamID subID = item->sid;
        ParamID id = item->pid;
        UnitType type = paramManager.getSubParamUnit(id, subID);
        ParamRulerConfig config = alarmConfig.getParamRulerConfig(subID, type);

        item->downRuler->setScale(config.scale);
        item->downRuler->setRange(config.minDownRuler, config.upRuler - 1);
        item->downRuler->setValue(config.downRuler);
        item->downRuler->setStep(1);

        item->upRuler->setScale(config.scale);
        item->upRuler->setRange(config.downRuler + 1, config.maxUpRuler);
        item->upRuler->setValue(config.upRuler);
        item->upRuler->setStep(1);
    }
}
