/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#include "TableView.h"

TableView::TableView(QWidget *parent)
    : QTableView(parent)
{
    setFrameStyle(QFrame::NoFrame);
}

TableView::~TableView()
{
}
