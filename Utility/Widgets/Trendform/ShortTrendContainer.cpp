/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/29
 **/


#include "ShortTrendContainer.h"
#include <QVBoxLayout>
#include "ShortTrendItem.h"
#include "ParamInfo.h"
#include "ColorManager.h"
#include "ParamManager.h"
#include "AlarmConfig.h"
#include "TrendDataStorageManager.h"
#include "Framework/Language/LanguageManager.h"
#include "ShortTrendWindow.h"
#include "WindowManager.h"

class ShortTrendContainerPrivate
{
public:
    ShortTrendContainerPrivate()
        : layout(NULL),
          duration(SHORT_TREND_DURATION_60M)
    {}

    QVBoxLayout *layout;
    ShortTrendDuration duration;
    QList<ShortTrendItem *> trendItems;
    QList<SubParamID> defaultTrendList;
};

ShortTrendContainer::ShortTrendContainer()
    : IWidget("ShortTrendContainer"), d_ptr(new ShortTrendContainerPrivate())
{
    d_ptr->layout = new QVBoxLayout(this);
    d_ptr->layout->setContentsMargins(4, 4, 4, 4);
    d_ptr->layout->setSpacing(0);

    for (int i = SUB_PARAM_HR_PR; i < SUB_PARAM_NR; ++i)
    {
        trendDataStorageManager.registerShortTrend(static_cast<SubParamID>(i));
    }
    connect(this, SIGNAL(released()), this, SLOT(onReleased()));
    connect(&colorManager, SIGNAL(paletteChanged(ParamID)), this, SLOT(onPaletteChanged(ParamID)));
}

ShortTrendContainer::~ShortTrendContainer()
{
    delete d_ptr;
    for (int i = SUB_PARAM_HR_PR; i < SUB_PARAM_NR; ++i)
    {
        trendDataStorageManager.unRegisterShortTrend(static_cast<SubParamID>(i));
    }
}

void ShortTrendContainer::setTrendItemNum(int num)
{
    qDeleteAll(d_ptr->trendItems);
    d_ptr->trendItems.clear();
    while (d_ptr->trendItems.count() < num)
    {
        ShortTrendItem *item = new ShortTrendItem();
        d_ptr->trendItems.append(item);
        d_ptr->layout->addWidget(item, 1);
        item->setTrendDuration(d_ptr->duration);
        connect(&trendDataStorageManager, SIGNAL(newTrendDataArrived(ShortTrendInterval)),
                item, SLOT(onNewTrendDataArrived(ShortTrendInterval)));
    }

    if (num > 0)
    {
        // the last item need to draw the time label
        d_ptr->trendItems.last()->enableDrawingTimeLabel(true);
    }
}

int ShortTrendContainer::getTrendNum() const
{
    return d_ptr->trendItems.count();
}

void ShortTrendContainer::addSubParamToTrendItem(int trendindex, QList<SubParamID> subParamIDs)
{
    if (trendindex >= d_ptr->trendItems.count() || trendindex < 0)
    {
        return;
    }

    if (subParamIDs.isEmpty())
    {
        clearTrendItemSubParam(trendindex);
        return;
    }

    ShortTrendItem *item = d_ptr->trendItems.at(trendindex);
    item->setSubParamList(subParamIDs);

    if ((subParamIDs[0] == SUB_PARAM_NIBP_SYS
            || subParamIDs[0] == SUB_PARAM_NIBP_DIA
            || subParamIDs[0] == SUB_PARAM_NIBP_MAP) && !item->isNibpTrend())
    {
        item->setNibpTrend(true);
        connect(&trendDataStorageManager, SIGNAL(newNibpDataReceived()),
                item, SLOT(onNewNibpMeasurementData()));
    }
    else if (item->isNibpTrend())
    {
        disconnect(&trendDataStorageManager, SIGNAL(newNibpDataReceived()),
                item, SLOT(onNewNibpMeasurementData()));
        item->setNibpTrend(false);
    }

    ParamID parmID = paramInfo.getParamID(subParamIDs[0]);
    item->setWaveColor(colorManager.getColor(paramInfo.getParamName(parmID)));

    if (subParamIDs.count() == 1)
    {
        // only one sub param, use the sub parameter's name
        item->setTrendName(trs(paramInfo.getSubParamName(subParamIDs[0])));
    }
    else
    {
        // use the parameter's name
        if (parmID == PARAM_IBP)
        {
            item->setTrendName(paramInfo.getIBPPressName(subParamIDs[0]));
        }
        else
        {
            item->setTrendName(paramInfo.getParamName(parmID));
        }
    }

    // 寻找是否存在与之一样的短趋势，有这设置与之一样的数据范围
    for (int i = 0; i < d_ptr->trendItems.count(); i++)
    {
        if (i == trendindex)
        {
            continue;
        }
        if (item->getSubParamList() == d_ptr->trendItems.at(i)->getSubParamList())
        {
            short max = 0, min = 0, scale = 0;
            d_ptr->trendItems.at(i)->getValueRange(max, min, scale);
            item->setValueRange(max, min, scale);
            return;
        }
    }
    // use the limit alarm range as the data range
    UnitType unit = paramManager.getSubParamUnit(parmID, subParamIDs[0]);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subParamIDs[0], unit);
    item->setValueRange(config.maxHighLimit, config.minLowLimit, config.scale);
}

void ShortTrendContainer::clearTrendItemSubParam(int trendindex)
{
    if (trendindex >= d_ptr->trendItems.count() || trendindex < 0)
    {
        return;
    }

    d_ptr->trendItems.at(trendindex)->setSubParamList(QList<SubParamID>());
}

void ShortTrendContainer::setTrendDuration(ShortTrendDuration duration)
{
    if (duration == d_ptr->duration)
    {
        return;
    }

    d_ptr->duration = duration;

    QList<ShortTrendItem *>::Iterator iter;
    for (iter = d_ptr->trendItems.begin(); iter != d_ptr->trendItems.end(); ++iter)
    {
        (*iter)->setTrendDuration(duration);
    }
}

ShortTrendDuration ShortTrendContainer::getTrendDuration() const
{
    return d_ptr->duration;
}

void ShortTrendContainer::getShortTrendList(QList<SubParamID> &ids)
{
    ids.clear();
    QList<ShortTrendItem *>::ConstIterator iter = d_ptr->trendItems.constBegin();
    for (; iter!= d_ptr->trendItems.constEnd(); iter++)
    {
        ids.append((*iter)->getSubParamList().at(0));
    }
}

void ShortTrendContainer::getDefaultTrendList(QList<SubParamID> &defaultTrendList)
{
    defaultTrendList.clear();
    defaultTrendList = d_ptr->defaultTrendList;
}

void ShortTrendContainer::updateDefautlTrendList()
{
    getShortTrendList(d_ptr->defaultTrendList);
}

void ShortTrendContainer::onReleased()
{
    ShortTrendWindow win(this);
    windowManager.showWindow(&win, WindowManager::ShowBehaviorModal);
}

void ShortTrendContainer::onPaletteChanged(ParamID id)
{
    QList<ShortTrendItem *>::iterator iter = d_ptr->trendItems.begin();
    for (; iter != d_ptr->trendItems.end(); ++iter)
    {
        QList<SubParamID> subParams = (*iter)->getSubParamList();
        ParamID param = paramInfo.getParamID(subParams.at(0));
        if (param == PARAM_DUP_ECG)
        {
            param = PARAM_ECG;
        }
        if (param == PARAM_DUP_RESP)
        {
            param = PARAM_RESP;
        }
        if (param == id)
        {
            (*iter)->setWaveColor(colorManager.getColor(paramInfo.getParamName(param)));
            (*iter)->updateBlackground();
        }
    }
}
