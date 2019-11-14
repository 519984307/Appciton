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
#include "Framework/UI/Dialog.h"
#include "ParamInfo.h"

class TrendGraphWindowPrivate;
class TrendGraphWindow : public Dialog
{
    Q_OBJECT
public:
    static TrendGraphWindow *getInstance();
    ~TrendGraphWindow();

public:
    /**
     * @brief setSubWidgetRulerLimit 设置子窗口上线限标尺
     * @param index 设置窗口的索引
     * @param down
     * @param up
     * @param scale
     */
    void setSubWidgetRulerLimit(int index, int down, int up, int scale);

    /**
     * @brief setAllParamAutoRuler 当前参数全部设为自动标尺
     */
    void setAllParamAutoRuler(void);

    /**
     * @brief updateTrendGraph 更新趋势组
     */
    void updateTrendGraph();

    /**
     * @brief timeIntervalChange 设置趋势图时间间隔
     * @param timeInterval 时间间隔
     */
    void timeIntervalChange(int timeInterval);

    /**
     * @brief waveNumberChange 设置趋势图显示波形数
     * @param num 波形数目
     */
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

protected:
    void timerEvent(QTimerEvent *ev);
    void showEvent(QShowEvent *ev);

private slots:
    void onButtonReleased(void);

private:
    TrendGraphWindow();
    QScopedPointer<TrendGraphWindowPrivate> d_ptr;
};
