/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/17
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class WiredNetworkMaintainMenuContentPrivate;
class WiredNetworkMaintainMenuContent : public MenuContent
{
    Q_OBJECT
public:
    WiredNetworkMaintainMenuContent();
    ~WiredNetworkMaintainMenuContent();

protected:
    /* reimplement */
    virtual void readyShow();

    /* reimplement */
    virtual void layoutExec();

private slots:
    void onComboBoxIndexChanged(int index);
    void onButtonReleased(void);

private:
    friend class WiredNetworkMaintainMenuContentPrivate;
    WiredNetworkMaintainMenuContentPrivate * const d_ptr;
};
