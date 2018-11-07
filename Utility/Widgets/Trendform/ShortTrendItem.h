/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/26
 **/

#pragma once
#include <QWidget>
#include <ParamDefine.h>
#include <QList>
#include <TrendDataDefine.h>

class ShortTrendItemPrivate;
class ShortTrendItem : public QWidget
{
    Q_OBJECT
public:
    explicit ShortTrendItem(const QString &name = QString(), QWidget *parent = NULL);
    ~ShortTrendItem();

    /**
     * @brief getTrendName get the trend name
     * @return the trend name
     */
    QString getTrendName() const;

    /**
     * @brief setTrendName set the trend name
     * @param name the trend name
     */
    void setTrendName(const QString &name);

    /**
     * @brief getSubParamList get the sub param list
     * @return a list of the subparam
     */
    QList<SubParamID> getSubParamList() const;

    /**
     * @brief setSubParamList get the sub param id of the this trend item draw
     * @param subParams
     */
    void setSubParamList(const QList<SubParamID> subParams);

    /**
     * @brief setValueRange set the value range
     * @param max the maximum value
     * @param min the minimum value
     * @param scale the value scale
     */
    void setValueRange(short max, short min, short scale = 1);

    /**
     * @brief getValueRange get the trend value's range
     * @param max store the maximum value
     * @param min store the minimum value
     * @param scale store the value scale
     */
    void getValueRange(short &max, short &min, short &scale) const;

    /**
     * @brief setTrendDuration set the trend duration
     * @param duration the duration
     */
    void setTrendDuration(ShortTrendDuration duration);

    /**
     * @brief getTrendDuration get the trend duration
     * @return the trend duration
     */
    ShortTrendDuration getTrendDuration() const;

    /**
     * @brief setWaveColor set the wave color
     * @param color the wave's color
     */
    void setWaveColor(const QColor &color);

    /**
     * @brief getWaveColor get the wave color
     * @return the wave color
     */
    QColor getWaveColor() const;

    /**
     * @brief enableDrawingTimeLabel enable draw the time label on the bottom of the trend graph
     * @param enable
     */
    void enableDrawingTimeLabel(bool enable);

    /**
     * @brief isDrawingTimeLabel check whether is drawing the time label or not
     * @return  true if enable, otherwise, return false
     */
    bool isDrawingTimeLabel() const;

    /**
     * @brief setNibpTrend need to set to true if current trend is NIBP trend
     * @param enable
     */
    void setNibpTrend(bool enable);

    /**
     * @brief isNibpTrend check wheter current trend is nibp trend
     * @return  true if current trend is nibp trend, otherwise, return false
     */
    bool isNibpTrend() const;

public slots:
    void onNewTrendDataArrived(ShortTrendInterval intreval);

    /**
     * @brief onNewNibpMeasurementData handle the new NIBP measurement data
     * @note the nibp data might be recored at the timestamp that can't be divided by the interval.
     *       when this case happened, the trend storage manager will emit a signal to tell the short
     *       trend item to reload the nibp data.
     * @param interval the signale interval
     */
    void onNewNibpMeasurementData();

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *ev);

    /* reimplement */
    void resizeEvent(QResizeEvent *ev);

private:
    ShortTrendItemPrivate * const d_ptr;
};
