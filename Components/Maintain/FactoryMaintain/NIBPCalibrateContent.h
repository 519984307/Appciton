/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/

#pragma once
#include "Framework/UI/MenuContent.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/SpinBox.h"
#include <QList>
#include <QHBoxLayout>
#include <QLabel>

class CalibrateSetItem : public QWidget
{
public:
    CalibrateSetItem() : QWidget()
    {
        title = new QLabel();
        value = new QLabel();
        range = new SpinBox();
        btn = new Button();

        QHBoxLayout *manlayout = new QHBoxLayout();
        manlayout->setSpacing(2);
        manlayout->setMargin(0);

        manlayout->addWidget(title);             // 标题
        manlayout->addWidget(value);             // 设置的压力值
        manlayout->addWidget(range);             // 校准点的值
        manlayout->addWidget(btn);               // 校准按钮

        setLayout(manlayout);

        setFocusPolicy(Qt::StrongFocus);
        setFocusProxy(btn);
    }

public:
    QLabel *title;
    QLabel *value;
    SpinBox *range;
    Button *btn;
};

class NIBPCalibrateContentPrivate;
class NIBPCalibrateContent : public MenuContent
{
    Q_OBJECT

public:
    /**
     * @brief getInstance
     * @return
     */
    static NIBPCalibrateContent *getInstance();

    ~NIBPCalibrateContent();

    /**
     * @brief init
     */
    void init(void);

protected:
    /**
     * @brief layoutExec
     */
    virtual void layoutExec(void);
    void timerEvent(QTimerEvent *ev);

    void hideEvent(QHideEvent *e);

private slots:
    /**
     * @brief onBtn1Calibrated  校准点1按钮
     */
    void onBtn1Calibrated(void);
    /**
     * @brief onBtn2Calibrated  校准点2按钮
     */
    void onBtn2Calibrated(void);

    void inCalibrateMode(void);

    /**
     * @brief onOverpressureReleased 过压保护开关
     */
    void onOverpressureReleased(int);

private:
    NIBPCalibrateContent();

    NIBPCalibrateContentPrivate *const d_ptr;
};


