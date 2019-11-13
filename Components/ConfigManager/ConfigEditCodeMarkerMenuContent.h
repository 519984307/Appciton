/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/19
 **/

#pragma once
#include "Framework/UI/MenuContent.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditCodeMarkerMenuContentPrivate;
class ConfigEditCodeMarkerMenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditCodeMarkerMenuContent(Config *const config);
    ~ConfigEditCodeMarkerMenuContent();

protected:
    virtual void readyShow();
    virtual void layoutExec();
    virtual void hideEvent(QHideEvent *ev);
private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);

private:
    ConfigEditCodeMarkerMenuContentPrivate *const d_ptr;
};

