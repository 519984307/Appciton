/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/10
 **/


#pragma once
#include "IWidget.h"
#include <QLabel>
#include <QList>
#include <QTimer>

//提示信息栏
class PromptInfoBarWidget : public IWidget
{
    Q_OBJECT

public:
    //返回对象本身
    static PromptInfoBarWidget &getSelf();

    // 设置报警提示信息。
    void display(QString &info);

    // 构造与析构
    explicit PromptInfoBarWidget(const QString &name);
    ~PromptInfoBarWidget();
protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void onTimeout();

private:
    void _drawText(void);

    QString _text;
    QTimer *_timer;
    QList<QString> _stringList;
};
#define promptInfoBarWidget (PromptInfoBarWidget::getSelf())
