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

class ShortTrendItemPrivate;
class ShortTrendItem : public QWidget
{
    Q_OBJECT
public:
    enum TrendDuration
    {
        TREND_DURATION_30M,
        TREND_DURATION_60M,
        TREND_DURATION_120M,
        TREND_DURATION_240M,
        TREND_DURATION_480M,
    };

    explicit ShortTrendItem(const QString &name, QWidget *parent = NULL);
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
    void setTrendDuration(TrendDuration duration);

    /**
     * @brief getTrendDuration get the trend duration
     * @return the trend duration
     */
    TrendDuration getTrendDuration() const;


protected:
    /* reimplement */
    void paintEvent(QPaintEvent *ev);

    /* reimplement */
    void resizeEvent(QResizeEvent *ev);

private:
    ShortTrendItemPrivate * const d_ptr;
};
