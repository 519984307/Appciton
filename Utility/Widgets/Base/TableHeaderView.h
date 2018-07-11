/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/11
 **/

#pragma once

#include <QHeaderView>

class TableHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    explicit TableHeaderView(Qt::Orientation orientation, QWidget *parent = NULL);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
};
