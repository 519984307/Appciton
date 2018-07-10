/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#include "AlarmLimitMenuContent.h"
#include "LanguageManager.h"
#include "TableView.h"
#include "AlarmLimitModel.h"
#include <QBoxLayout>
#include <QHeaderView>

class AlarmLimitMenuContentPrivate
{
public:
    AlarmLimitMenuContentPrivate()
        : model(NULL)
    {}

    void loadoptions();

    AlarmLimitModel *model;
};

void AlarmLimitMenuContentPrivate::loadoptions()
{
}

AlarmLimitMenuContent::AlarmLimitMenuContent()
    : MenuContent(trs("AlarmLimitMenu"), trs("AlarmLimitMenuDesc")),
      d_ptr(new AlarmLimitMenuContentPrivate())
{
}

AlarmLimitMenuContent::~AlarmLimitMenuContent()
{
    delete d_ptr;
}

void AlarmLimitMenuContent::readyShow()
{
}

void AlarmLimitMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QBoxLayout *layout = new QVBoxLayout(this);

    TableView *table = new TableView();
    layout->addWidget(table);

    d_ptr->model = new AlarmLimitModel();

    // d_ptr->setupAlarmDataInfos();

    layout->addStretch(1);
}
