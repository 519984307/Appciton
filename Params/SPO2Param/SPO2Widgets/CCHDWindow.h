/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/11
 **/

#include "Framework/UI/Dialog.h"

class CCHDWindowPrivate;
class CCHDWindow : public Dialog
{
    Q_OBJECT
public:
    CCHDWindow();
    ~CCHDWindow();

protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *ev);

private slots:
    void onButtonReleased();

private:
    CCHDWindowPrivate * const d_ptr;
};
