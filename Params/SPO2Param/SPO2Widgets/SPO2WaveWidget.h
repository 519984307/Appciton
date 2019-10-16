/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/27
 **/


#pragma once
#include "WaveWidget.h"
#include "SPO2Define.h"

class WaveWidgetLabel;

class PopupList;
class SPO2WaveWidget : public WaveWidget
{
    Q_OBJECT

public:
    void setNotify(bool enable, QString str = " ");

    SPO2WaveWidget(const QString &waveName, const QString &title);
    ~SPO2WaveWidget();

    bool waveEnable();

protected:
    // 窗体大小调整事件
    virtual void resizeEvent(QResizeEvent *e);
    virtual void focusInEvent(QFocusEvent */*e*/);

    /* reimplment */
    void loadConfig(void);

private:
//    WaveWidgetLabel *_name;
    WaveWidgetLabel *_notify;
    /**
     * @brief _adjustLabelLayout  调整标签布局
     */
    void _adjustLabelLayout(void);
};
