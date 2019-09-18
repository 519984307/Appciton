/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/7
 **/

#pragma once
#include "Dialog.h"
#include <QScopedPointer>

class HistoryDataReviewWindowPrivate;
class HistoryDataReviewWindow : public Dialog
{
    Q_OBJECT
public:
    static HistoryDataReviewWindow* getInstance();
    ~HistoryDataReviewWindow();

    /**
     * @brief setHistoryReviewIndex 设置调用的历史数据所在的索引
     * @param index 索引位置
     * @param timeStr 历史数据时间字符串
     */
    void setHistoryReviewIndex(int index, QString timeStr);

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void onButtonReleased(void);

private:
    HistoryDataReviewWindow();
    QScopedPointer<HistoryDataReviewWindowPrivate> d_ptr;
};
