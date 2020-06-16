/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once
#include "TrendWidget.h"
#include <QScopedPointer>

class COTrendWidgetPrivate;
class COTrendWidget: public TrendWidget
{
    Q_OBJECT
public:
    COTrendWidget();
    ~COTrendWidget();

public:
    /**
     * @brief setMeasureResult set the displayed co and ci value
     * @param co cardiac output
     * @param ci cardiac index
     * @param t the measure time
     * @note
     * The co and ci value should has been scaled by 10
     */
    void setMeasureResult(short co, short ci, unsigned t);

    /**
     * @brief setTBData set the display TB value
     * @param tb tb value
     * @note
     * The value should have been scaled by 10
     */
    void setTb(short tb);

    QList<SubParamID> getShortTrendSubParams() const;

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    const QScopedPointer<COTrendWidgetPrivate> pimpl;
};
