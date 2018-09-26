/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/25
 **/


#pragma once
#include "IWidget.h"
#include "AlarmInfoWindow.h"

class AlarmInfoPopListVIew;
class AlarmTechInfoBarWidget : public IWidget
{
    Q_OBJECT

public:
    // 清除界面。
    void clear(void);

    // 设置报警提示信息。
    void display(AlarmInfoNode &node);

    //返回对象本身
    static AlarmTechInfoBarWidget &getSelf();

    // 构造与析构。
    explicit AlarmTechInfoBarWidget(const QString &name);
    ~AlarmTechInfoBarWidget();

    void updateList();

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void _releaseHandle(IWidget *);
    void _alarmListHide();

private:
    void _drawBackground(void);
    void _drawText(void);

    AlarmType _type;
    AlarmPriority _alarmPriority;
    unsigned _pauseTime;
    QString _text;
    bool _latch;
    bool _acknowledge;

    AlarmInfoWindow *_alarmWindow;
    AlarmType _alarmType;
};
#define alarmTechInfoBarWin (AlarmTechInfoBarWidget::getSelf())
