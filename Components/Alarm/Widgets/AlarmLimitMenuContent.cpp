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
#include "TableHeaderView.h"
#include "AlarmLimitModel.h"
#include <QBoxLayout>
#include <QHeaderView>
#include "ParamManager.h"
#include "ParamInfo.h"
#include "AlarmConfig.h"
#include "IConfig.h"
#include "PatientManager.h"
#include "IBPParam.h"
#include "TableViewItemDelegate.h"
#include <Button.h>
#include <QDebug>

#define TABLE_ROW_NUM 6

class AlarmLimitMenuContentPrivate
{
public:
    AlarmLimitMenuContentPrivate()
        : model(NULL), table(NULL),
          prevBtn(NULL), nextBtn(NULL)
    {}

    void loadoptions();

    AlarmLimitModel *model;
    TableView *table;
    Button *prevBtn;
    Button *nextBtn;
};

void AlarmLimitMenuContentPrivate::loadoptions()
{
    QList<ParamID> pids;
    paramManager.getParams(pids);
    QList<AlarmDataInfo> infos;
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        SubParamID subId = static_cast<SubParamID>(i);
        ParamID pid = paramInfo.getParamID(subId);

        if (pid == PARAM_IBP && systemManager.isSupport(PARAM_IBP))
        {
            IBPPressureName pressName1 = ibpParam.getEntitle(IBP_INPUT_1);
            IBPPressureName pressName2 = ibpParam.getEntitle(IBP_INPUT_2);
            IBPPressureName curPressName = ibpParam.getPressureName(subId);
            if (curPressName != pressName1 && curPressName != pressName2)
            {
                continue;
            }
        }

        if (pids.contains(pid))
        {
            AlarmDataInfo info;
            info.paramID = pid;
            info.subParamID = subId;
            info.status = alarmConfig.isLimitAlarmEnable(subId);
            UnitType unit  = paramManager.getSubParamUnit(pid, subId);
            info.limitConfig = alarmConfig.getLimitAlarmConfig(subId, unit);
            int alarmLev = 0;
            currentConfig.getNumAttr(QString("AlarmSource|%1|%2").arg(patientManager.getTypeStr())
                                     .arg(paramInfo.getSubParamName(subId, true)),
                                     "Prio", alarmLev);
            info.alarmLevel = alarmLev;

            infos.append(info);
        }
    }

    model->setupAlarmDataInfos(infos);
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
    d_ptr->loadoptions();
}

void AlarmLimitMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QBoxLayout *layout = new QVBoxLayout(this);

    TableView *table = new TableView();

    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    TableHeaderView *horizontalHeader = new TableHeaderView(Qt::Horizontal);
    table->setHorizontalHeader(horizontalHeader);
    horizontalHeader->setResizeMode(QHeaderView::ResizeToContents);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setShowGrid(false);

    connect(table, SIGNAL(rowClicked(int)), this, SLOT(onRowClicked(int)));
    connect(table, SIGNAL(selectRowChanged(int)), this, SLOT(onSelectRowChanged(int)));

    layout->addWidget(table);

    d_ptr->model = new AlarmLimitModel();

    table->setModel(d_ptr->model);

    table->viewport()->installEventFilter(d_ptr->model);

    table->setFixedHeight(d_ptr->model->getHeaderHeightHint()
                          + d_ptr->model->getRowHeightHint() * TABLE_ROW_NUM);

    d_ptr->table = table;

    d_ptr->table->setItemDelegate(new TableViewItemDelegate(this));


    QBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addStretch(1);
    Button *btn = new Button("Prev");
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setMinimumWidth(100);
    hlayout->addWidget(btn);
    connect(btn, SIGNAL(clicked(bool)), this, SLOT(onbtnClick()));
    d_ptr->prevBtn = btn;

    btn = new Button("Next");
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setMinimumWidth(100);
    hlayout->addWidget(btn);
    connect(btn, SIGNAL(clicked(bool)), this, SLOT(onbtnClick()));
    d_ptr->nextBtn = btn;

    hlayout->addStretch(1);

    layout->addLayout(hlayout);

    layout->addStretch(1);
}

void AlarmLimitMenuContent::onbtnClick()
{
    bool focusPrevBtn = false;
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->prevBtn)
    {
        d_ptr->table->scrollToPreviousPage();
    }
    else if (btn == d_ptr->nextBtn)
    {
        d_ptr->table->scrollToNextPage();
        if (!d_ptr->table->hasNextPage())
        {
            focusPrevBtn = true;
        }
    }

    d_ptr->prevBtn->setEnabled(d_ptr->table->hasPreivousPage());
    d_ptr->nextBtn->setEnabled(d_ptr->table->hasNextPage());
    if (focusPrevBtn)
    {
        d_ptr->prevBtn->setFocus();
    }
}

void AlarmLimitMenuContent::onRowClicked(int row)
{
    qDebug() << Q_FUNC_INFO << row;
    d_ptr->model->editRowData(row);
}

void AlarmLimitMenuContent::onSelectRowChanged(int row)
{
    int editRow = d_ptr->model->curEditRow();
    if (editRow >= 0 && editRow != row)
    {
        d_ptr->model->stopEditRow();
    }
}
