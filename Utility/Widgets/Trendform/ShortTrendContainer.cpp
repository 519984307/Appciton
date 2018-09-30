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
};

ShortTrendContainer::ShortTrendContainer()
    : IWidget("ShortTrendContainer"), d_ptr(new ShortTrendContainerPrivate())
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    d_ptr->layout = new QVBoxLayout(this);
    d_ptr->layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->layout->setSpacing(0);

    for (int i = SUB_PARAM_HR_PR; i < SUB_PARAM_NR; ++i)
    {
        trendDataStorageManager.registerShortTrend(static_cast<SubParamID>(i));
    }
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
    while (d_ptr->trendItems.count() < num)
    {
        ShortTrendItem *item = new ShortTrendItem();
        d_ptr->trendItems.append(item);
        d_ptr->layout->addWidget(item, 1);
        item->setTrendDuration(d_ptr->duration);
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

    if (subParamIDs[0] == SUB_PARAM_NIBP_SYS
            || subParamIDs[0] == SUB_PARAM_NIBP_DIA
            || subParamIDs[0] == SUB_PARAM_NIBP_MAP)
    {
        item->setNibpTrend(true);
    }
    else
    {
        item->setNibpTrend(false);
    }

    ParamID parmID = paramInfo.getParamID(subParamIDs[0]);
    if (subParamIDs.count() == 1)
    {
        // this is the first subparam, use the first param's data range and color
        item->setWaveColor(colorManager.getColor(paramInfo.getParamName(parmID)));
        item->setTrendName(paramInfo.getSubParamName(subParamIDs[0]));
        // use the limit alarm range as the data range
        UnitType unit = paramManager.getSubParamUnit(parmID, subParamIDs[0]);
        LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subParamIDs[0], unit);
        item->setValueRange(config.maxHighLimit, config.minLowLimit, config.scale);
    }
    else
    {
        // if the trend item contain more than 1 sub param, use the last sub param's param name
        // as trend name
        if (parmID == PARAM_IBP)
        {
            item->setTrendName(paramInfo.getIBPPressName(subParamIDs[0]));
        }
        else
        {
            item->setTrendName(paramInfo.getParamName(parmID));
        }
    }
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
