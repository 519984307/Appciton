/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/12
 **/

#pragma once
#include "TrendWidget.h"

class QLabel;
class O2TrendWidget : public TrendWidget
{
    Q_OBJECT
public:
    // 刷新O2的值
    void setO2Value(int16_t o2);

    // 刷新趋势参数上下限
    void updateLimit(void);

    // 是否报警
    void isAlarm(bool flag);

    // 显示
    void showValue(void);

    // 构造与析构
    O2TrendWidget();
    ~O2TrendWidget();

    QList <SubParamID> getShortTrendSubParams() const;

    virtual void doRestoreNormalStatus();

    /**
     * @brief updatePalette update the palette
     * @param pal palette
     */
    virtual void updatePalette(const QPalette &pal);

protected:
    virtual void setTextSize();

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_o2Value;
    bool _isAlarm;
    QString _o2String;
};
