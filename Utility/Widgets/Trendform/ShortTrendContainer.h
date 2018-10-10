/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/29
 **/

#pragma once
#include "IWidget.h"
#include "ParamDefine.h"
#include "TrendDataDefine.h"

class ShortTrendContainerPrivate;
class ShortTrendContainer : public IWidget
{
    Q_OBJECT
public:
    ShortTrendContainer();

    ~ShortTrendContainer();

    /**
     * @brief setTrendItemNum set the trend number this short trend container can hold
     * @param num the short trend number
     */
    void setTrendItemNum(int num);

    /**
     * @brief getTrendItemNum get the trend number
     * @return
     */
    int getTrendNum() const;

    /**
     * @brief addSubParamToTrendItem add subpraram to the short trend
     * @param trendindex the trend index
     * @param subParamIDs the new subParamIDs which will be show trend in the trend item
     */
    void addSubParamToTrendItem(int trendindex, QList<SubParamID> subParamIDs);

    /**
     * @brief clearTrendItemSubParam clear subParam in the short trend item
     * @note After clear the subParam, the trend item will no draww any trend graph
     * @param trendindex the trend index
     */
    void clearTrendItemSubParam(int trendindex);

    /**
     * @brief setTrendDuration set the trend duration
     * @note all the trend item will be configureated to the same duration
     * @param duration the duration
     */
    void setTrendDuration(ShortTrendDuration duration);

    /**
     * @brief getTrendDuration the the trend duration
     * @return the duration of all the short trend item
     */
    ShortTrendDuration getTrendDuration() const;

    /**
     * @brief getShortTrendList 获得当前短趋势列表
     * @param shortTrendList
     */
    void getShortTrendList(QStringList &shortTrendList);

    /**
     * @brief getSubParamList 获得对应的子参数列表
     * @param id
     * @param paraList
     */
    void getSubParamList(SubParamID id, QList<SubParamID> &paraList);

private slots:
    void onReleased();
private:
    ShortTrendContainerPrivate *const d_ptr;
};
