/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/5
 **/

#pragma once

#include "Framework/UI/MenuContent.h"

class DataReviewMenuContentPrivate;
class DataReviewMenuContent : public MenuContent
{
    Q_OBJECT
public:
    DataReviewMenuContent();
    ~DataReviewMenuContent();

protected:
    /* reimplment */
    void layoutExec();

    /* reimplment */
    void readyShow();

private slots:
    /**
     * @brief onBtnRelease  按钮触发槽函数
     */
    void onBtnReleased(void);

private:
    DataReviewMenuContentPrivate *const d_ptr;
};
