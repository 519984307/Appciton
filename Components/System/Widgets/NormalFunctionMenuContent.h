/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#pragma once
#include "Framework/UI/MenuContent.h"
#include "SoundManager.h"

class NormalFunctionMenuContentPrivate;
class NormalFunctionMenuContent : public MenuContent
{
    Q_OBJECT
public:
    NormalFunctionMenuContent();
    ~NormalFunctionMenuContent();

protected:
    /* reimplment */
    void readyShow();

    /* reimplement */
    void layoutExec();

    /* reimplement */
    void setShowParam(const QVariant &val);

private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);
    /**
     * @brief onBtnReleasd
     */
    void onBtnReleasd(void);

    /**
     * @brief onPopupListItemFocusChanged
     * @param volume
     */
    void onPopupListItemFocusChanged(int volume);

    void nightModeHandle(bool isNightMode);

private:
    NormalFunctionMenuContentPrivate * const d_ptr;
};
