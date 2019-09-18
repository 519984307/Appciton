/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/7
 **/
#pragma once
#include "Dialog.h"

struct ReviewDataInfo
{
    QString valueType;
    QString unitRange;
    QStringList time;
};

class HemodynaimcReviewWindowPrivate;
class HemodynaimcReviewWindow : public Dialog
{
    Q_OBJECT
public:
    static HemodynaimcReviewWindow *getInstance();
    ~HemodynaimcReviewWindow();

    /**
     * @brief layoutExec
     */
    void layoutExec(void);
    /**
     * @brief updateReviewTable
     */
    void updateReviewTable(void);

protected:
    void showEvent(QShowEvent *ev);

private slots:
    /**
     * @brief onBtnUpReleased
     */
    void onBtnUpReleased(void);
    /**
     * @brief onBtnDownReleased
     */
    void onBtnDownReleased(void);
    /**
     * @brief onBtnLeftReleased
     */
    void onBtnLeftReleased(void);
    /**
     * @brief onBtnRightReleased
     */
    void onBtnRightReleased(void);
    /**
     * @brief onBtnReturnReleased
     */
    void onBtnReturnReleased(void);
    /**
     * @brief onBtnReferenceReleased
     */
    void onBtnReferenceReleased(void);

private:
    HemodynaimcReviewWindow();
    HemodynaimcReviewWindowPrivate *const d_ptr;
};

