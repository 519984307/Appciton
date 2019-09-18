/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#pragma once
#include <QString>
#include <QStringList>
#include "ParamInfo.h"
#include "BaseDefine.h"
#include <QTimer>

#define PARAM_UPDATE_TIMEOUT    (5000)      //  趋势数据刷新超时5sec

class Param : public QObject
{
    Q_OBJECT

public:
    // 获取参数的ID。
    ParamID getParamID(void);
    const QString &getName(void);

    // 使能与禁用。
    virtual void enable(void);
    virtual void disable(void);
    bool isEnabled(void);

    virtual bool isConnected(void);

    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);
    /**
     * @brief exitDemo exit the demo mode, the subclass should inherit this interface to do
     *                 the clear stuff when exit the demo mode. The default implement do nothing
     */
    virtual void exitDemo(void);

    // 获取当前的波形控件名称。
    virtual void getAvailableWaveforms(QStringList &waveforms, QStringList &waveformShowName,
                                       int flag = 0);
    virtual void getTrendWindow(QString &trendWin);
    virtual void getWaveWindow(QString &waveWin);

    // 获取子参数值
    virtual short getSubParamValue(SubParamID id);

    // check whether a subparam is avaliable, always true in default implement
    virtual bool isSubParamAvaliable(SubParamID id);

    // 显示子参数值
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 刷新参数上下限
    virtual void updateSubParamLimit(SubParamID /*id*/){}

    // 构造与析构。
    explicit Param(ParamID id);
    virtual ~Param();

    /**
     * @brief getShortTrendList 获得短趋势参数列表
     * @return
     */
    virtual QList<SubParamID> getShortTrendList(){return QList<SubParamID>();}

protected slots:
    virtual void paramUpdateTimeout() {}

protected:
    QTimer *paramUpdateTimer;

private slots:
    virtual void onPaletteChanged(ParamID id)
    {
        Q_UNUSED(id)
    }

private:
    ParamID _paramID;
    QString _paramName;
    bool _isEnable;
};
