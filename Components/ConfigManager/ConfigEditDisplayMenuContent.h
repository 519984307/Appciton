/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/18
 **/
#pragma once
#include "Framework/UI/MenuContent.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditDisplayMenuContentPrivate;
class ConfigEditDisplayMenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditDisplayMenuContent(QStringList colorList, Config * const config);
    ~ConfigEditDisplayMenuContent();

protected:
    virtual void readyShow();
    virtual void layoutExec();

private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);

private:
    ConfigEditDisplayMenuContentPrivate *const d_ptr;
};
