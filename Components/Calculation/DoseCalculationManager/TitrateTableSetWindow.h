/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/1
 **/
#pragma once
#include "Framework/UI/Dialog.h"


class TitrateTableSetWindowPrivate;
class TitrateTableSetWindow : public Dialog
{
    Q_OBJECT
public:
    static TitrateTableSetWindow* getInstance();
    ~TitrateTableSetWindow();
    /**
     * @brief layoutExec
     */
    void layoutExec();

protected:
    void showEvent(QShowEvent *e);

private slots:
    /**
     * @brief onBtnYesReleased
     */
    void onBtnYesReleased(void);
private:
    TitrateTableSetWindow();

    TitrateTableSetWindowPrivate *const d_ptr;
};


