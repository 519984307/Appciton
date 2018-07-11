/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#include <QTableView>

class TableViewPrivate;
class TableView : public QTableView
{
    Q_OBJECT
public:
    explicit TableView(QWidget *parent = NULL);
    ~TableView();
};

