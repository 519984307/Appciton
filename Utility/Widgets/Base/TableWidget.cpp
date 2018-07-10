/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#include "TableWidget.h"


class TableWidgetPrivate
{
public:
    TableWidgetPrivate() {}
};

TableWidget::TableWidget(QWidget *parent)
    :QTableWidget(parent),
      d_ptr(new TableWidgetPrivate)
{
}

TableWidget::~TableWidget()
{
    delete d_ptr;
}
