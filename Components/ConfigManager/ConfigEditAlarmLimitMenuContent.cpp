/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/19
 **/
#include "ConfigEditAlarmLimitMenuContent.h"
#include "Framework/Language/LanguageManager.h"
#include "TableView.h"
#include "TableHeaderView.h"
#include "AlarmLimitModel.h"
#include <QBoxLayout>
#include <QHeaderView>
#include "ParamManager.h"
#include "ParamInfo.h"
#include "AlarmConfig.h"
#include "ConfigManager.h"
#include "PatientManager.h"
#include "IBPParam.h"
#include "TableViewItemDelegate.h"
#include <Button.h>
#include <QDebug>
#include "ConfigEditAlarmLimitModel.h"
#include "SystemManager.h"

#define TABLE_ROW_NUM 6

class ConfigEditAlarmLimitMenuContentPrivate
{
public:
    explicit ConfigEditAlarmLimitMenuContentPrivate(Config *const config)
        : model(NULL), table(NULL),
          prevBtn(NULL), nextBtn(NULL),
          config(config)
    {
    }

    void loadoptions();

    ConfigEditAlarmLimitModel *model;
    TableView *table;
    Button *prevBtn;
    Button *nextBtn;
    Config *config;
    QList<AlarmDataInfo> infos;
};

void ConfigEditAlarmLimitMenuContentPrivate::loadoptions()
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

            QString prefix = "AlarmSource|";
            prefix += paramInfo.getSubParamName(subId, true);
            prefix += "|";
            prefix += Unit::getSymbol(unit);
            prefix += "|";

            QString highPrefix = prefix + "High";
            int v = 0;
            config->getNumValue(highPrefix, v);
            info.limitConfig.highLimit = v;

            v = 0;
            config->getNumAttr(highPrefix, "Min", v);
            info.limitConfig.minHighLimit = v;

            v = 0;
            config->getNumAttr(highPrefix, "Max", v);
            info.limitConfig.maxHighLimit = v;

            v = 0;
            QString lowPrefix = prefix + "Low";
            config->getNumValue(lowPrefix, v);
            info.limitConfig.lowLimit = v;

            v = 0;
            config->getNumAttr(lowPrefix, "Min", v);
            info.limitConfig.minLowLimit = v;

            v = 0;
            config->getNumAttr(lowPrefix, "Max", v);
            info.limitConfig.maxLowLimit = v;

            v = 1;
            config->getNumValue(prefix + "Scale", v);
            info.limitConfig.scale = v;

            v = 0;
            config->getNumValue(prefix + "Step", v);
            info.limitConfig.step = v;

            int alarmLev = 0;
            config->getNumAttr(QString("AlarmSource|%1")
                               .arg(paramInfo.getSubParamName(subId, true)),
                               "Prio", alarmLev);
            info.alarmLevel = alarmLev;

            int alarmStatus = 0;
            config->getNumAttr(QString("AlarmSource|%1")
                               .arg(paramInfo.getSubParamName(subId, true)),
                               "Enable", alarmStatus);
            info.status = alarmStatus;

            infos.append(info);
        }
    }
    this->infos = infos;
    model->setupAlarmDataInfos(infos, false);
    model->setEachPageRowCount(TABLE_ROW_NUM);
}

ConfigEditAlarmLimitMenuContent::ConfigEditAlarmLimitMenuContent(Config *const config)
    : MenuContent(trs("AlarmLimitMenu"), trs("AlarmLimitMenuDesc")),
      d_ptr(new ConfigEditAlarmLimitMenuContentPrivate(config))
{
}

ConfigEditAlarmLimitMenuContent::~ConfigEditAlarmLimitMenuContent()
{
    delete d_ptr;
}

void ConfigEditAlarmLimitMenuContent::setItemFocus(const QString &param)
{
    // 增加报警设置链接功能代码
    int focusIndex = 0;
    QString focusName = param;

    if (!focusName.isEmpty())
    {
        for (int i = 0; i < d_ptr->infos.count(); i++)
        {
            SubParamID subId = d_ptr->infos.at(i).subParamID;
            if (focusName == paramInfo.getSubParamName(subId, true))
            {
                focusIndex = i;
                break;
            }
        }
    }

    QModelIndex index = d_ptr->table->model()->index(focusIndex, 0);
    d_ptr->table->scrollTo(index, QAbstractItemView::PositionAtCenter);

    bool isOnlyToRead = configManager.isReadOnly();
    if (!isOnlyToRead)
    {
        d_ptr->table->selectRow(focusIndex);
    }
}

void ConfigEditAlarmLimitMenuContent::readyShow()
{
    d_ptr->loadoptions();

    // 获取控件是否失能标志位
    bool isOnlyToRead = configManager.isReadOnly();
    d_ptr->table->setEnabled(!isOnlyToRead);
}

void ConfigEditAlarmLimitMenuContent::layoutExec()
{
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

    d_ptr->model = new ConfigEditAlarmLimitModel(d_ptr->config);

    table->setModel(d_ptr->model);

    table->viewport()->installEventFilter(d_ptr->model);

    table->setFixedHeight(d_ptr->model->getHeaderHeightHint()
                          + d_ptr->model->getRowHeightHint() * TABLE_ROW_NUM);

    d_ptr->table = table;

    d_ptr->table->setItemDelegate(new TableViewItemDelegate(this));


    QBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addStretch(1);
    Button *btn = new Button(trs("Prev"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setMinimumWidth(100);
    hlayout->addWidget(btn);
    connect(btn, SIGNAL(clicked(bool)), this, SLOT(onbtnClick()));
    d_ptr->prevBtn = btn;

    btn = new Button(trs("Next"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setMinimumWidth(100);
    hlayout->addWidget(btn);
    connect(btn, SIGNAL(clicked(bool)), this, SLOT(onbtnClick()));
    d_ptr->nextBtn = btn;

    hlayout->addStretch(1);

    layout->addLayout(hlayout);

    layout->addStretch(1);
}

void ConfigEditAlarmLimitMenuContent::setShowParam(const QVariant &param)
{
    setItemFocus(param.toString());
}

void ConfigEditAlarmLimitMenuContent::onbtnClick()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == d_ptr->prevBtn)
    {
        d_ptr->table->scrollToPreviousPage();
    }
    else if (btn == d_ptr->nextBtn)
    {
        d_ptr->table->scrollToNextPage();
    }
}

void ConfigEditAlarmLimitMenuContent::onRowClicked(int row)
{
    qDebug() << Q_FUNC_INFO << row;
    d_ptr->model->editRowData(row);
}

void ConfigEditAlarmLimitMenuContent::onSelectRowChanged(int row)
{
    int editRow = d_ptr->model->curEditRow();
    if (editRow >= 0 && editRow != row)
    {
        d_ptr->model->stopEditRow();
    }
}

