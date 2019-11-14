/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/
#pragma once
#include "Framework/UI/MenuContent.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditGeneralMenuContentPrivate;
class ConfigEditGeneralMenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditGeneralMenuContent(ConfigEditMenuWindow * const cmw);
    ~ConfigEditGeneralMenuContent();

protected:
    /**
     * @brief readyShow
     */
    virtual void readyShow();
    /**
     * @brief layoutExec
     */
    virtual void layoutExec();

private slots:
    /**
     * @brief onBtnReleasedChanged
     */
    void onBtnReleasedChanged();

private:
    ConfigEditGeneralMenuContentPrivate *const d_ptr;
};


