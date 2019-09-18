/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/6
 **/

#pragma once
#include "Dialog.h"
#include <QScopedPointer>
#include <QModelIndex>
#include "ParamDefine.h"

class OxyCRGEventWindowPrivate;
class OxyCRGEventWindow : public Dialog
{
    Q_OBJECT
public:
    static OxyCRGEventWindow *getInstance();
    ~OxyCRGEventWindow();

    /**
     * @brief waveWidgetTrend1 趋势1显示波形设置
     * @param isRR
     */
    void waveWidgetTrend1(bool isRR);

    /**
     * @brief waveWidgetCompressed 当前显示压缩波形
     * @param id
     */
    void waveWidgetCompressed(WaveformID id);

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
    void showEvent(QShowEvent *ev);
    void timerEvent(QTimerEvent *ev);

private slots:
    void waveInfoReleased(QModelIndex index);
    void waveInfoReleased(int index);
    void eventListReleased(void);
    void leftMoveEvent(void);
    void rightMoveEvent(void);
    void printReleased(void);
    void setReleased(void);

private:
    OxyCRGEventWindow();
    QScopedPointer<OxyCRGEventWindowPrivate> d_ptr;
};
