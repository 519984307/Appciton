/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/27
 **/

#pragma once
#include "Window.h"
#include "ParamInfo.h"

class TrendGraphWindowPrivate;
class TrendGraphWindow : public Window
{
    Q_OBJECT
public:
    static TrendGraphWindow *getInstance();
    ~TrendGraphWindow();

public:
    void setSubWidgetRulerLimit(SubParamID id, int down, int up, int scale);

    void updateTrendGraph();

    void timeIntervalChange(int timeInterval);

    void waveNumberChange(int num);

    /**
     * @brief setHistoryDataPath 设置历史回顾数据的文件路径
     * @param path 文件路径
     */
    void setHistoryDataPath(QString path);

    /**
     * @brief setHistoryData 此刻的趋势表是否为历史数据
     * @param flag 标志
     */
    void setHistoryData(bool flag);

private slots:
    void onButtonReleased(void);

private:
    TrendGraphWindow();
    QScopedPointer<TrendGraphWindowPrivate> d_ptr;
};
