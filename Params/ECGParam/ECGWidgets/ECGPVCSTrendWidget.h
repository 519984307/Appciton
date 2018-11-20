/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/7
 **/


#pragma once
#include "TrendWidget.h"
#include "ECGDefine.h"

class QLabel;
class ECGPVCSTrendWidget: public TrendWidget
{
    Q_OBJECT

public:
    // 刷新PVCS的值。
    void setPVCSValue(int16_t pvcs);

    // 是否发生报警
    void isAlarm(bool isAlarm);

    // 显示参数值
    void showValue(void);

    ECGPVCSTrendWidget();
    ~ECGPVCSTrendWidget();
public:
    virtual void doRestoreNormalStatus();
    virtual QList<SubParamID> getShortTrendSubParams() const;

protected:
    virtual void setTextSize(void);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_pvcsValue;
    QString _pvcsString;
    bool _isAlarm;
};
