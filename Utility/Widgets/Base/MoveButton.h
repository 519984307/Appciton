/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/5
 **/

#pragma once
#include "Button.h"
#include <QScopedPointer>

class MoveButtonPrivate;
class MoveButton : public Button
{
    Q_OBJECT
public:
    explicit MoveButton(const QString &text = QString());
    ~MoveButton();

signals:
    void leftMove(void);
    void rightMove(void);

protected:
    void paintEvent(QPaintEvent *ev);
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);
    void mousePressEvent(QMouseEvent *ev);

private:
    QScopedPointer<MoveButtonPrivate> d_ptr;
};
