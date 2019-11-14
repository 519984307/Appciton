/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/20
 **/

#pragma once
#include "OxyCRGTrendWaveWidget.h"
#include "Framework/Utility/RingBuff.h"

class OxyCRGRESPWaveWidgetPrivate;
class OxyCRGRESPWaveWidget : public OxyCRGTrendWaveWidget
{
    Q_OBJECT
public:
    explicit OxyCRGRESPWaveWidget(const QString &waveName);
    ~OxyCRGRESPWaveWidget();

    /* reimplment */
    void setDataRate(int rate);

protected:
    /* reimplment */
    void paintEvent(QPaintEvent *e);

private:
    Q_DECLARE_PRIVATE(OxyCRGRESPWaveWidget)
};
