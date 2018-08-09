/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/20
 **/

#pragma once
#include "Window.h"
#include "IBPDefine.h"
#include "TrendDataDefine.h"

class TrendTableWindowPrivate;
class TrendTableWindow : public Window
{
    Q_OBJECT
public:
    static TrendTableWindow *getInstance();
    ~TrendTableWindow();

public:
    /**
     * @brief setTimeInterval 设置事件间隔
     * @param t
     */
    void setTimeInterval(int t);

    /**
     * @brief setTrendGroup 设置趋势组
     * @param g
     */
    void setTrendGroup(int g);

    /**
     * @brief setHistoryDataPath 设置历史回顾数据路径
     * @param path
     */
    void setHistoryDataPath(QString path);

    /**
     * @brief setHistoryData 设置是否窗口显示为历史数据
     * @param flag
     */
    void setHistoryData(bool flag);

    /**
     * @brief printTrendData 打印趋势数据
     * @param startTime 打印的开始时间
     * @param endTime 打印的结束时间
     */
    void printTrendData(unsigned startTime, unsigned endTime);

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void leftReleased(void);
    void rightReleased(void);
    void upReleased(void);
    void downReleased(void);
    void leftMoveEvent(void);
    void rightMoveEvent(void);
    void printWidgetRelease(void);
    void trendDataSetReleased(void);

private:
    TrendTableWindow();
    QScopedPointer<TrendTableWindowPrivate> d_ptr;
};

