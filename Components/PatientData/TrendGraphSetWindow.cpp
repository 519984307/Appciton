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
#include <QVBoxLayout>
#include "Framework/UI/Button.h"
#include "Framework/UI/DropList.h"
#include "Framework/UI/ScrollArea.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Language/LanguageManager.h"
#include "TrendDataSymbol.h"
#include "IConfig.h"
#include "ParamManager.h"
#include "AlarmConfig.h"
#include "IBPParam.h"
#include <QPainter>
#include <QRect>
#include "TrendGraphWindow.h"
#include "TrendGraphConfig.h"

#define ITEM_HEIGHT                 (themeManager.getAcceptableControlHeight())
#define ITEM_WIDTH                  150
#define INIT_ITEM_NUM               3
#define WINDOW_HEIGHT               400

TrendGraphSetWindow *TrendGraphSetWindow::selfObj = NULL;

class TrendGraphSetWindowPrivate
{
public:
    TrendGraphSetWindowPrivate();

public:
    TrendGroup trendGroup;                 // 当前趋势组
    QList<RulerItem *> itemList;
    Button *allAutoBtn;
    DropList *trendGroupList;
    DropList *timeIntervalList;
    DropList *waveNumList;
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
}

void TrendGraphSetWindow::setCurParam(QList<SubParamID> subList)
{
    for (int i = 0; i < d_ptr->itemList.count(); i++)
    {
        RulerItem *item = d_ptr->itemList.at(i);
        if (i < subList.count())
        {
            SubParamID sid = subList.at(i);
            item->sid = sid;
            ParamID pid = paramInfo.getParamID(sid);
            UnitType type = paramManager.getSubParamUnit(pid, sid);
            ParamRulerConfig config = TrendGraphConfig::getParamRulerConfig(sid, type);
            QString name = paramInfo.getSubParamName(sid);
            if (sid == SUB_PARAM_ETCO2)
            {
                item->label->setText(QString("%1(%2/%3)")
                                     .arg(trs("CO2")).arg(trs("Et")).arg(trs("Fi")));
            }
            else if (sid == SUB_PARAM_T1)
            {
                item->label->setText(QString("%1/%2")
                                     .arg(trs("T1")).arg(trs("T2")));
            }
            else if (sid == SUB_PARAM_NIBP_SYS)
            {
                item->label->setText(trs("NIBP"));
            }
            else
            {
                item->label->setText(trs(name));
            }
            int autoRuler = 0;
            QString prefix = "TrendGraph|Ruler|";
            prefix += paramInfo.getSubParamName(sid, true);
            systemConfig.getNumAttr(prefix, "Auto", autoRuler);
            item->combo->setCurrentIndex(autoRuler);

            item->downRuler->setScale(config.scale);
            item->downRuler->setRange(config.minDownRuler, config.upRuler - 1);
            item->downRuler->setValue(config.downRuler);
            item->downRuler->setStep(1);

            item->upRuler->setScale(config.scale);
            item->upRuler->setRange(config.downRuler + 1, config.maxUpRuler);
            item->upRuler->setValue(config.upRuler);
            item->upRuler->setStep(1);
            item->setVisible(true);
            continue;
        }
        item->setVisible(false);
    }
}

void TrendGraphSetWindow::showEvent(QShowEvent *ev)
{
    Dialog::showEvent(ev);
}

void TrendGraphSetWindow::allAutoReleased()
{
    int count = d_ptr->itemList.count();
    for (int i = 0; i < count; i ++)
    {
        RulerItem *item = d_ptr->itemList.at(i);
        item->combo->setCurrentIndex(1);
        item->downRuler->setEnabled(false);
        item->upRuler->setEnabled(false);
        item->downRuler->setEnabled(false);
        item->upRuler->setEnabled(false);
    }
    TrendGraphWindow::getInstance()->setAllParamAutoRuler();
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
        QString prefix = "TrendGraph|Ruler|";
        prefix += paramInfo.getSubParamName(item->sid, true);
        systemConfig.setNumAttr(prefix, "Auto", index);
    }
}

void TrendGraphSetWindow::upDownRulerChange(int, int)
{
    SpinBox *spb = qobject_cast<SpinBox *>(sender());
    if (spb)
    {
        int id = spb->property("Ruler").toInt();
        RulerItem *item = d_ptr->itemList.at(id / 2);
        if (id % 2)     // 标尺上限改变时,修改标尺下限的范围
        {
            int min;
            int max;
            item->downRuler->getRange(&min, &max);
            max = item->upRuler->getValue() - 1;
            item->downRuler->setRange(min, max);
        }
        else            // 标尺下限改变时,修改标尺上限的范围
        {
            int min;
            int max;
            item->upRuler->getRange(&min, &max);
            min = item->downRuler->getValue() + 1;
            item->upRuler->setRange(min, max);
        }
        TrendGraphWindow::getInstance()->setSubWidgetRulerLimit(id / 2, item->downRuler->getValue(),
                item->upRuler->getValue(), item->downRuler->getScale());
    }
}

TrendGraphSetWindow::TrendGraphSetWindow()
    : Dialog(), d_ptr(new TrendGraphSetWindowPrivate())
{
    setFixedHeight(WINDOW_HEIGHT);
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

    int count = d_ptr->itemList.count();
    QVBoxLayout *rulerLayout = new QVBoxLayout();
    for (int i = 0; i < count; i ++)
    {
        RulerItem *item = NULL;
        item = d_ptr->itemList.at(i);
        rulerLayout->addWidget(item);
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
    rulerLayout->addStretch(0);

    d_ptr->allAutoBtn = new Button(trs("AllAuto"));
    d_ptr->allAutoBtn->setFixedSize(ITEM_WIDTH, ITEM_HEIGHT);
    d_ptr->allAutoBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d_ptr->allAutoBtn, SIGNAL(released()), this, SLOT(allAutoReleased()));

    QString prefix = "TrendGraph|";
    int index = 0;

    QString groupPrefix = prefix + "TrendGroup";
    systemConfig.getNumValue(groupPrefix, index);
    d_ptr->trendGroupList = new DropList(trs("TrendGroup"));
    d_ptr->trendGroupList->setEnabled(false);       // 暂时置灰趋势组选择功能(目前不支持IBP,AG参数功能)
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
        if (i == RESOLUTION_RATIO_NIBP)
        {
            continue;
        }
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
                                 << "3");
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
    mainLayout->addLayout(rulerLayout);
    mainLayout->addLayout(hLayout);
    setWindowLayout(mainLayout);

    upDateTrendGroup();
}

TrendGraphSetWindowPrivate::TrendGraphSetWindowPrivate()
    : trendGroup(TREND_GROUP_RESP), allAutoBtn(NULL),
      trendGroupList(NULL), timeIntervalList(NULL),
      waveNumList(NULL)
{
    int value = 0;
    QString prefix = "TrendGraph|TrendGroup";
    systemConfig.getNumValue(prefix, value);
    trendGroup = static_cast<TrendGroup>(value);

    for (int i = 0; i < INIT_ITEM_NUM; ++i)
    {
        RulerItem *item = new RulerItem();
        itemList.append(item);
    }
}
