/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/27
 **/
#pragma once

#include "Window.h"

class FreezeWindowPrivate;
class FreezeWindow : public Window
{
    Q_OBJECT
public:
    FreezeWindow();
    ~FreezeWindow();

protected:
    virtual void showEvent(QShowEvent *ev);
    virtual void hideEvent(QHideEvent *ev);
    // virtual void keyPressEvent(QKeyEvent *e);

private slots:
    /**
     * @brief onSelectWaveChanged
     * @param waveName
     */
    void onSelectWaveChanged(const QString &waveName);
    /**
     * @brief onBtnClick
     */
    void onBtnClick();

private:
    FreezeWindowPrivate *const d_ptr;
};

