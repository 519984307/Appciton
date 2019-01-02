/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/11
 **/

#pragma once

#include <QDialog>

class StandbyWindow : public QDialog
{
    Q_OBJECT
public:
    StandbyWindow();
    ~StandbyWindow();

protected:
    /* reimplement */
    void paintEvent(QPaintEvent *ev);

    /* reimplement */
    void keyReleaseEvent(QKeyEvent *ev);

    /* reimplement */
    void mouseReleaseEvent(QMouseEvent *ev);

    /* reimplement */
    void hideEvent(QHideEvent *ev);
};
