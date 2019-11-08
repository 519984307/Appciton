/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#include "AlarmLimitWindow.h"
#include "LanguageManager.h"
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
#include <QCoreApplication>
#include <QFocusEvent>
#include <QTimer>
#include <SystemManager.h>
#include "MessageBox.h"
#include "WindowManager.h"

#define TABLE_ROW_NUM 7

class AlarmLimitWindowPrivate
{
public:
    explicit AlarmLimitWindowPrivate(const QString &param)
        : model(NULL)
        , table(NULL)
        , prevBtn(NULL)
        , nextBtn(NULL)
        , defaultsBtn(NULL)
        , param(param)
    {
    }

    void loadoptions();

    AlarmLimitModel *model;
    TableView *table;
    Button *prevBtn;
    Button *nextBtn;
    Button *defaultsBtn;
    QList<AlarmDataInfo> infos;
    QString param;
};

void AlarmLimitWindowPrivate::loadoptions()
{
    QList<ParamID> pids = paramManager.getParamIDs();
    QList<AlarmDataInfo> infos;
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        SubParamID subId = static_cast<SubParamID>(i);
        ParamID pid = paramInfo.getParamID(subId);

        if (!systemManager.isSupport(pid))
        {
            continue;
        }

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
            currentConfig.getNumAttr(QString("AlarmSource|%1")
                                     .arg(paramInfo.getSubParamName(subId, true)),
                                     "Prio", alarmLev);
            info.alarmLevel = alarmLev;

            infos.append(info);
        }
    }
    this->infos = infos;
    model->setupAlarmDataInfos(infos);
    model->setEachPageRowCount(TABLE_ROW_NUM);
}

AlarmLimitWindow::AlarmLimitWindow(const QString &param)
    : Dialog()
    , d_ptr(new AlarmLimitWindowPrivate(param))
{
    layoutExec();
    readyShow();
}

AlarmLimitWindow::AlarmLimitWindow()
    : Dialog()
    , d_ptr(new AlarmLimitWindowPrivate(""))
{
    layoutExec();
    readyShow();
}

AlarmLimitWindow::~AlarmLimitWindow()
{
    delete d_ptr;
}

void AlarmLimitWindow::setItemFocus()
{
    // 增加报警设置链接功能代码
    int focusIndex = 0;
    QString focusName = d_ptr->param;

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
    d_ptr->table->scrollToAssignedPage(index.row());

    d_ptr->table->setCurrentIndex(index);
    d_ptr->table->setFocus(Qt::ActiveWindowFocusReason);
}

void AlarmLimitWindow::showEvent(QShowEvent *ev)
{
    Dialog::showEvent(ev);
    QTimer::singleShot(0, this, SLOT(setItemFocus()));
}


void AlarmLimitWindow::readyShow()
{
    d_ptr->loadoptions();
}

void AlarmLimitWindow::layoutExec()
{
    setWindowTitle(trs("AlarmLimit"));
    setFixedSize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());
    setFocusPolicy(Qt::NoFocus);

    QBoxLayout *layout = new QVBoxLayout();

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
    hlayout->setSpacing(20);
    hlayout->addStretch(1);

    Button *btn = new Button(trs("Defaults"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setFixedWidth(150);
    hlayout->addWidget(btn);
    connect(btn, SIGNAL(clicked(bool)), this, SLOT(onDefaultsClick()));
    d_ptr->defaultsBtn = btn;

    btn = new Button(trs("Prev"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setFixedWidth(150);
    hlayout->addWidget(btn);
    connect(btn, SIGNAL(clicked(bool)), this, SLOT(onbtnClick()));
    d_ptr->prevBtn = btn;

    btn = new Button(trs("Next"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setFixedWidth(150);
    hlayout->addWidget(btn);
    connect(btn, SIGNAL(clicked(bool)), this, SLOT(onbtnClick()));
    d_ptr->nextBtn = btn;

    hlayout->addStretch(1);

    layout->addLayout(hlayout);

    setWindowLayout(layout);
}

void AlarmLimitWindow::onbtnClick()
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

void AlarmLimitWindow::onRowClicked(int row)
{
    d_ptr->model->editRowData(row);
}

void AlarmLimitWindow::onSelectRowChanged(int row)
{
    int editRow = d_ptr->model->curEditRow();
    if (editRow >= 0 && editRow != row)
    {
        d_ptr->model->stopEditRow();
    }
}

void AlarmLimitWindow::onDefaultsClick()
{
    QStringList slist;
    slist << trs("No") << trs("Yes");
    MessageBox messageBox(trs("Warn"), trs("SureAllAlarmDefaults"), slist, true);
    if (messageBox.exec() == 1)
    {
        restoreDefaults();
    }
}

void AlarmLimitWindow::restoreDefaults()
{
    Config defaultConfig(configManager.getOriginalConfig(patientManager.getType()));

    QList<ParamID> pids = paramManager.getParamIDs();
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
            // default
            int status = 0;
            defaultConfig.getNumAttr(QString("AlarmSource|%1")
                                     .arg(paramInfo.getSubParamName(subId, true)),
                                     "Enable", status);
            currentConfig.setNumAttr(QString("AlarmSource|%1")
                                     .arg(paramInfo.getSubParamName(subId, true)),
                                     "Enable", status);

            int lev = 0;
            defaultConfig.getNumAttr(QString("AlarmSource|%1")
                                     .arg(paramInfo.getSubParamName(subId, true)),
                                     "Prio", lev);
            currentConfig.setNumAttr(QString("AlarmSource|%1")
                                     .arg(paramInfo.getSubParamName(subId, true)),
                                     "Prio", lev);

            UnitType unit = paramManager.getSubParamUnit(pid, subId);
            int high = 0;
            defaultConfig.getNumValue(QString("AlarmSource|%1|%2|%3")
                                      .arg(paramInfo.getSubParamName(subId, true)).arg(Unit::getSymbol(unit))
                                      .arg("High"), high);
            currentConfig.setNumValue(QString("AlarmSource|%1|%2|%3")
                                      .arg(paramInfo.getSubParamName(subId, true)).arg(Unit::getSymbol(unit))
                                      .arg("High"), high);

            int low = 0;
            defaultConfig.getNumValue(QString("AlarmSource|%1|%2|%3")
                                      .arg(paramInfo.getSubParamName(subId, true)).arg(Unit::getSymbol(unit))
                                      .arg("Low"), low);
            currentConfig.setNumValue(QString("AlarmSource|%1|%2|%3")
                                      .arg(paramInfo.getSubParamName(subId, true)).arg(Unit::getSymbol(unit))
                                      .arg("Low"), low);

            AlarmDataInfo info;
            info.paramID = pid;
            info.subParamID = subId;
            info.status = status;
            alarmConfig.setLimitAlarmEnable(subId, status);
            info.limitConfig = alarmConfig.getLimitAlarmConfig(subId, unit);
            info.alarmLevel = lev;
            info.limitConfig.highLimit = high;
            info.limitConfig.lowLimit = low;
            alarmConfig.setLimitAlarmConfig(subId, unit, info.limitConfig);
            infos.append(info);
        }
    }
    alarmConfig.clearLimitAlarmInfo();
    d_ptr->infos = infos;
    d_ptr->model->setupAlarmDataInfos(infos);
    d_ptr->model->setEachPageRowCount(TABLE_ROW_NUM);
}
