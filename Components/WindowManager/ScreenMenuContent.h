/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/24
 **/

#include "MenuContent.h"

class  ScreenMenuContentPrivate;
class ScreenMenuContent : public MenuContent
{
    Q_OBJECT
public:
    ScreenMenuContent();
    ~ScreenMenuContent();

protected:
    /* reimplement */
    void readyShow();

    /* reimplement */
    void layoutExec();

    void hideEvent(QHideEvent *ev);

private slots:
    void onComboxIndexChanged(int index);
    void onBtnClick();

private:
    ScreenMenuContentPrivate * const d_ptr;
};
