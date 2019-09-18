/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/1
 **/

#pragma once
#include "Dialog.h"
#include <QScopedPointer>
#include <QModelIndex>
#include "Param.h"

class EventWindowPrivate;
class EventWindow : public Dialog
{
    Q_OBJECT
public:
    static EventWindow *getInstance();
    ~EventWindow();

    /**
     * @brief setWaveSpeed 设置波形速度
     */
    void setWaveSpeed(int);

    /**
     * @brief setWaveGain 设置ECG波形增益
     */
    void setWaveGain(int);

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

    void findEventIndex(SubParamID id, unsigned time);
protected:
    void showEvent(QShowEvent *ev);
    void timerEvent(QTimerEvent *ev);

private slots:
    void waveInfoReleased(QModelIndex index);
    void waveInfoReleased(int index);
    void eventTypeSelect(int);
    void eventListPrintReleased(void);
    void upPageReleased(void);
    void downPageReleased(void);
    void eventListReleased(void);
    void leftMoveCoordinate(void);
    void rightMoveCoordinate(void);
    void leftMoveEvent(void);
    void rightMoveEvent(void);
    void printRelease(void);
    void setReleased(void);
    void upReleased(void);
    void downReleased(void);

private:
    EventWindow();
    QScopedPointer<EventWindowPrivate> d_ptr;
};
