/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/18
 **/

#pragma once
#include <QListView>

class ListsViewPrivate;
class ListView : public QListView
{
    Q_OBJECT
public:
    explicit ListView(QWidget *parent = NULL);
    ~ListView();

protected:
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);
    void paintEvent(QPaintEvent *ev);

private:
    ListsViewPrivate * const d_ptr;
};
