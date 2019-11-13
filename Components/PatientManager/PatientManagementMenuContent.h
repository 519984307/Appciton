/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/10
 **/

#pragma once

#include "Framework/UI/MenuContent.h"

class PatientManagementMenuContentPrivate;
class PatientManagementMenuContent : public MenuContent
{
    Q_OBJECT
public:
    PatientManagementMenuContent();
    ~PatientManagementMenuContent();

protected:
    /* reimplment */
    void layoutExec();

    /*reimplment*/
    void showEvent(QShowEvent *ev);

private slots:
    /**
     * @brief onBtnReleased
     */
    void onBtnReleased(void);

private:
    PatientManagementMenuContentPrivate *const d_ptr;
};
