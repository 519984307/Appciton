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

class StandyWindowPrivate;
class StandyWindow : public QDialog
{
    Q_OBJECT
public:
    StandyWindow();
    ~StandyWindow();

protected:
    /* reimplment */
    void paintEvent(QPaintEvent *ev);

    /* reimplment */
    void keyPressEvent(QKeyEvent *ev);

    /* reimplment */
    void mousePressEvent(QMouseEvent *ev);

private:
    StandyWindowPrivate *const d_ptr;
};
