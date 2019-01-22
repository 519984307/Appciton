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

    /* reimplment */
    void updateWidgetConfig();

protected:
    // 窗体大小调整事件
    virtual void resizeEvent(QResizeEvent *e);
    virtual void focusInEvent(QFocusEvent */*e*/);

private slots:
    /**
     * @brief onTimeout  time out handle
     */
    void onTimeout(void);

private:
    void _loadConfig(void);
//    WaveWidgetLabel *_name;
    WaveWidgetLabel *_notify;
};
