/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#pragma once

#include <QTableWidget>

class TableWidgetPrivate;
class TableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = NULL);
    ~TableWidget();

private:
    const TableWidgetPrivate *d_ptr;
};
