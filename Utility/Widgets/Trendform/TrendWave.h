/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/6/13
 **/

#pragma once
#include "IWidget.h"
#include "ParamDefine.h"
#include "TrendDataDefine.h"

class TrendWavePrivate;
class TrendWave : public IWidget
{
    Q_OBJECT
public:
    explicit TrendWave(const QString &name , QWidget* parent = NULL);
    ~TrendWave();

    void addSubParam(SubParamID id);

    /* reimplment */
    void updateWidgetConfig();

protected:
    /* reimplment */
    void resizeEvent(QResizeEvent *e);

    /* reimplment */
    void paintEvent(QPaintEvent *e);

protected slots:
    void onNewTrendDataArrived(unsigned timeStamp);

    void onPaletteChanged(ParamID param);

    void onClearTrendData();

    void updateRange(SubParamID subParam);

private:
    TrendWavePrivate * const d_ptr;
};
