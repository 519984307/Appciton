/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/7
 **/
#include "HemodynamicReviewWindow.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/TableView.h"
#include "Framework/UI/TableViewItemDelegate.h"
#include "Framework/UI/TableHeaderView.h"
#include "Framework/UI/ThemeManager.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include "HemodynamicDefine.h"
#include "HemodynamicDataModel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "HemodynamicManager.h"
#include <QDateTime>
#include "WindowManager.h"
#include "HemodynamicWindow.h"

#define MAX_ROW_COUNT           28
#define MAX_COLUMN_COUNT        12
#define MED_ROW_COUNT           8           // 一屏有多少行
#define INPUT_VALUE_COUNT       10          // 输入值有多少

class HemodynaimcReviewWindowPrivate
{
public:
    HemodynaimcReviewWindowPrivate()
        : refUnitFlag(0),
          selectColumn(0),
          curVScroller(0),
          up(NULL),
          down(NULL),
          left(NULL),
          right(NULL),
          keyReturn(NULL),
          reference(NULL),
          model(NULL),
          tableView(NULL),
          moveCount(0),
          rowMoveCount(0)
    {
        dataInfos.clear();
        HeaderInfos.time.clear();
        HeaderInfos.unitRange = "";
        HeaderInfos.valueType = "";
        rangList.clear();
        unitList.clear();
    }
    int refUnitFlag;
    int selectColumn;
    int curVScroller;

    Button *up;
    Button *down;
    Button *left;
    Button *right;
    Button *keyReturn;
    Button *reference;

    HemodynamicDataModel *model;
    TableView *tableView;

    QList<ReviewDataInfo> dataInfos;
    QList<ReviewDataInfo> colorInfos;
    ReviewDataInfo HeaderInfos;
    int moveCount;
    int rowMoveCount;
    QStringList  rangList;
    QStringList  unitList;
};

HemodynaimcReviewWindow::HemodynaimcReviewWindow()
    : Dialog(),
      d_ptr(new HemodynaimcReviewWindowPrivate)
{
    layoutExec();
    updateReviewTable();
}

HemodynaimcReviewWindow *HemodynaimcReviewWindow::getInstance()
{
    static HemodynaimcReviewWindow *instance = NULL;
    if (!instance)
    {
        instance = new HemodynaimcReviewWindow;
    }
    return instance;
}

HemodynaimcReviewWindow::~HemodynaimcReviewWindow()
{
    delete d_ptr;
}

void HemodynaimcReviewWindow::layoutExec()
{
    setWindowTitle(trs("ReviewWindow"));

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(5);
    vlayout->setAlignment(Qt::AlignTop);

    d_ptr->tableView = new TableView(this);
    d_ptr->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    HemodynamicDataModel *model = new HemodynamicDataModel;
    d_ptr->tableView->setItemDelegate(new TableViewItemDelegate(this));
    d_ptr->tableView->verticalHeader()->setVisible(false);
    TableHeaderView *hl = new TableHeaderView(Qt::Horizontal);
    d_ptr->tableView->setHorizontalHeader(hl);
    hl->setResizeMode(QHeaderView::ResizeToContents);
    d_ptr->tableView->setFocusPolicy(Qt::ClickFocus);
    d_ptr->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->tableView->setSelectionBehavior(QAbstractItemView::SelectColumns);
    d_ptr->tableView->setModel(model);
    vlayout->addWidget(d_ptr->tableView);
    d_ptr->tableView->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    d_ptr->model = model;
    d_ptr->tableView->setFixedHeight(d_ptr->model->getHeaderHeightHint()
                                     + d_ptr->model->getRowHeightHint() * model->getRowCount());


    for (int i = 0; i < model->getRowCount(); i++)
    {
        d_ptr->tableView->setRowHeight(i, d_ptr->model->getRowHeightHint());
    }

    Button *button;
    QHBoxLayout *hlyout = new QHBoxLayout;

    button = new Button(trs("PrimitiveCalculation"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReturnReleased()));
    d_ptr->keyReturn = button;
    hlyout->addWidget(button);

    button = new Button(trs("ReferenceRange"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReferenceReleased()));
    d_ptr->reference = button;
    hlyout->addWidget(button);

    button = new Button();
    button->setButtonStyle(Button::ButtonIconOnly);
    button->setIcon(QIcon("/usr/local/nPM/icons/ArrowUp.png"));
    connect(button, SIGNAL(released()), this, SLOT(onBtnUpReleased()));
    d_ptr->up = button;
    hlyout->addWidget(button);

    button = new Button();
    button->setButtonStyle(Button::ButtonIconOnly);
    button->setIcon(QIcon("/usr/local/nPM/icons/ArrowDown.png"));
    connect(button, SIGNAL(released()), this, SLOT(onBtnDownReleased()));
    d_ptr->down = button;
    hlyout->addWidget(button);

    button = new Button();
    button->setButtonStyle(Button::ButtonIconOnly);
    button->setIcon(QIcon("/usr/local/nPM/icons/ArrowLeft.png"));
    connect(button, SIGNAL(released()), this, SLOT(onBtnLeftReleased()));
    d_ptr->left = button;
    hlyout->addWidget(button);

    button = new Button();
    button->setButtonStyle(Button::ButtonIconOnly);
    button->setIcon(QIcon("/usr/local/nPM/icons/ArrowRight.png"));
    connect(button, SIGNAL(released()), this, SLOT(onBtnRightReleased()));
    d_ptr->right = button;
    hlyout->addWidget(button);

    vlayout->addLayout(hlyout);
    vlayout->addStretch(1);
    setFixedSize(themeManager.defaultWindowSize());
    setWindowLayout(vlayout);
}


void HemodynaimcReviewWindow::updateReviewTable()
{
    QList<HemodynamicInfo> reviewData =
        hemodynamicManager.getReviewData();
    QList<ReviewDataInfo> dataInfos;
    QList<ReviewDataInfo> colorInfos;
    ReviewDataInfo info;
    ReviewDataInfo colorInfo;

    info.time.clear();
    int dataCloumn = (reviewData.length() + COLUMN_NUM_NR - 2 - 1) /
                     (COLUMN_NUM_NR - 2) * (COLUMN_NUM_NR - 2);
    for (int i = 0; i < dataCloumn; i++)
    {
        info.time.append("");
        colorInfo.time.append("#FFFFFF");
    }
    colorInfo.valueType = "#FFFFFF";
    colorInfo.unitRange = "#FFFFFF";

    for (int i = 0; i < MAX_ROW_COUNT; i++)
    {
        if (i == 0)
        {
            info.valueType = trs("InputValue");
        }
        else if (i < INPUT_VALUE_COUNT)
        {
            info.valueType = HemodynamicSymbol::
                             convert((HemodynamicParam)(i - 1));
        }
        else if (i == INPUT_VALUE_COUNT)
        {
            info.valueType = trs("OutputValue");
        }
        else
        {
            info.valueType = HemodynamicSymbol::
                             convert((HemodynamicOutput)(i - 11));
        }
        dataInfos.append(info);
        colorInfos.append(colorInfo);
    }

    for (int i = 0; i < MAX_ROW_COUNT; i++)
    {
        d_ptr->unitList.append("");
        if (i != 0 && i < INPUT_VALUE_COUNT)
        {
            d_ptr->unitList[i] = dataInfos[i].unitRange = HemodynamicSymbol::
                                 convertUnit((HemodynamicParam)(i - 1));
        }
        else if (i > INPUT_VALUE_COUNT)
        {
            d_ptr->unitList[i] = dataInfos[i].unitRange = HemodynamicSymbol::
                                 convertUnit((HemodynamicOutput)(i - 11));
        }
    }

    for (int i = 0; i < MAX_ROW_COUNT; i++)
    {
        if (i <= INPUT_VALUE_COUNT)
        {
            d_ptr->rangList.append("");
        }
        else if (i > INPUT_VALUE_COUNT)
        {
            d_ptr->rangList.append(
                HemodynamicSymbol::
                convertRange((HemodynamicOutput)(i - 11)));
        }
    }

    info.valueType =  trs("InOutValue");
    info.unitRange =  trs("TEMPUnit");


    HemodynamicInfo singleData;
    QString text;
    float value;

    for (int i = 0; i < reviewData.length(); i ++)
    {
        singleData = reviewData.at(reviewData.length() - i - 1);
        info.time[i] = QDateTime
                       ::fromTime_t(singleData.calcTime).toString("MM.dd hh:mm");

        for (int m = 0; m  < HEMODYNAMIC_PARAM_NR; m ++)
        {
            value = singleData.calcInput[m];
            if (value == InvData())
            {
                text = InvStr();
            }
            else
            {
                text = QString::number(singleData.calcInput[m], 'f', hemodynamicManager.inputAccuracy[m]);
            }
            dataInfos[m + 1].time[i] = text;
            colorInfos[m + 1].time[i] = "#FFFFFF";
        }
        for (int n = 0; n < HEMODYNAMIC_OUTPUT_NR; n ++)
        {
            value = singleData.calcOutput[n];
            QString color;
            if (value == InvData())
            {
                text = InvStr();
            }
            else
            {
                text = QString::number(singleData.calcOutput[n], 'f', hemodynamicManager.outputAccuracy[n]);
                color = "#FFFFFF";
            }
            if (text != InvStr() && (n < HEMODYNAMIC_OUTPUT_LVSW || n > HEMODYNAMIC_OUTPUT_BSA))
            {
                if (value < hemodynamicManager.outputLimit[n].low)
                {
                    text = text + DOWN_ARROW;
                    color = "#D2C314";
                }
                else if (value > hemodynamicManager.outputLimit[n].high)
                {
                    text = text + UP_ARROW;
                    color = "#D2C314";
                }
                else
                {
                    color = "#FFFFFF";
                }
            }
            dataInfos[n + 1 + INPUT_VALUE_COUNT].time[i] = text;
            colorInfos[n + 1 + INPUT_VALUE_COUNT].time[i] = color;
        }
    }
    d_ptr->HeaderInfos = info;
    d_ptr->dataInfos = dataInfos;
    d_ptr->colorInfos = colorInfos;

    d_ptr->model->setHeadInfos(info);

    dataInfos.clear();
    for (int i = 0; i < INPUT_VALUE_COUNT; i++)
    {
        dataInfos.append(d_ptr->dataInfos[i]);
    }
    d_ptr->rowMoveCount = MED_ROW_COUNT;
    d_ptr->model->setDynamicInfos(dataInfos);
    d_ptr->model->setDynamicColorInfos(colorInfos);
    d_ptr->tableView->selectColumn(2);
}


void HemodynaimcReviewWindow::showEvent(QShowEvent *ev)
{
    Q_UNUSED(ev)
    updateReviewTable();
}



void HemodynaimcReviewWindow::onBtnUpReleased()
{
    int count = d_ptr->rowMoveCount;
    if (count <= MED_ROW_COUNT)
    {
        return;
    }

    QModelIndex modelIndex = d_ptr->tableView->currentIndex();
    int column = modelIndex.column();

    QList<ReviewDataInfo> infos;
    int subCount = count - MED_ROW_COUNT - MED_ROW_COUNT;
    if (subCount < 0)
    {
        return;
    }
    for (int i = 0; i < MED_ROW_COUNT; i++)
    {
        infos.append(d_ptr->dataInfos[subCount + i]);
    }
    d_ptr->model->setDynamicInfos(infos);

    infos.clear();
    for (int i = 0; i < MED_ROW_COUNT; i++)
    {
        infos.append(d_ptr->colorInfos[subCount + i]);
    }
    d_ptr->model->setDynamicColorInfos(infos);

    d_ptr->rowMoveCount -= MED_ROW_COUNT;

    d_ptr->tableView->selectColumn(column);
}

void HemodynaimcReviewWindow::onBtnDownReleased()
{
    int count = d_ptr->rowMoveCount;
    if (count >= MAX_ROW_COUNT)
    {
        return;
    }

    QModelIndex modelIndex = d_ptr->tableView->currentIndex();
    int column = modelIndex.column();

    QList<ReviewDataInfo> infos;
    for (int i = 0; i < MED_ROW_COUNT; i++)
    {
        if (i + count >= MAX_ROW_COUNT)
        {
            ReviewDataInfo info;
            info.time.clear();
            info.unitRange = "";
            info.valueType = "";
            infos.append(info);
        }
        else
        {
            infos.append(d_ptr->dataInfos[i + count]);
        }
    }
    d_ptr->model->setDynamicInfos(infos);

    infos.clear();
    for (int i = 0; i < MED_ROW_COUNT; i++)
    {
        if (i + count >= MAX_ROW_COUNT)
        {
            ReviewDataInfo info;
            info.time.clear();
            info.unitRange = "";
            info.valueType = "";
            infos.append(info);
        }
        else
        {
            infos.append(d_ptr->colorInfos[i + count]);
        }
    }
    d_ptr->model->setDynamicColorInfos(infos);

    d_ptr->rowMoveCount += MED_ROW_COUNT;

    d_ptr->tableView->selectColumn(column);
}

void HemodynaimcReviewWindow::onBtnRightReleased()
{
    QModelIndex modelIndex = d_ptr->tableView->currentIndex();
    int column = modelIndex.column();

    if (column < 2)
    {
        d_ptr->tableView->selectColumn(2);
        return;
    }

    if (column >= hemodynamicManager.getReviewData().length() + 1)
    {
        return;
    }

    bool isRestart = false;
    if (column >= COLUMN_NUM_NR - 1)
    {
        if (d_ptr->dataInfos.at(1).time.count() <= COLUMN_NUM_NR - 2)
        {
            return;
        }
        else
        {
            isRestart = true;
            d_ptr->moveCount++;
        }
    }
    else
    {
        d_ptr->tableView->selectColumn(column + 1);
        return;
    }

    if (isRestart)
    {
        QList<ReviewDataInfo> dataInfos = d_ptr->dataInfos;

        for (int i = 0; i < dataInfos.count(); i++)
        {
            dataInfos[i].time.clear();
            int count = d_ptr->dataInfos.at(1).time.count();
            int moveCount = d_ptr->moveCount;
            for (int j = moveCount * 4; j < count; j++)
            {
                QString str = d_ptr->dataInfos[i].time[j];
                dataInfos[i].time.append(str);
            }
        }

        d_ptr->model->setDynamicInfos(dataInfos);
        d_ptr->tableView->selectColumn(2);
    }
}

void HemodynaimcReviewWindow::onBtnReturnReleased()
{
    QModelIndex modelIndex = d_ptr->tableView->currentIndex();
    int column = modelIndex.column();

    hemodynamicManager.setCalcValue(
        hemodynamicManager.
        getReviewData().
        at(hemodynamicManager.
           getReviewData().length() - column + 1));
    HemodynamicWindow::getInstance()->updateData();
    hide();
}

void HemodynaimcReviewWindow::onBtnReferenceReleased()
{
    ReviewDataInfo infos = d_ptr->HeaderInfos;
    bool isRange = false;

    QModelIndex modelIndex = d_ptr->tableView->currentIndex();
    int column = modelIndex.column();

    if (d_ptr->reference->text() == trs("ReferenceRange"))
    {
        infos.unitRange = trs("TEMPUnit");
    }
    else
    {
        isRange = true;
        infos.unitRange = trs("ReferenceRange");
    }
    d_ptr->reference->setText(infos.unitRange);
    d_ptr->model->setHeadInfos(infos);

    int count = d_ptr->rowMoveCount;
    if (count < MED_ROW_COUNT)
    {
        return;
    }

    QStringList unitRangeList;
    if (isRange)
    {
        unitRangeList = d_ptr->rangList;
    }
    else
    {
        unitRangeList = d_ptr->unitList;
    }
    for (int i = 0; i < MAX_ROW_COUNT; i++)
    {
        d_ptr->dataInfos[i].unitRange = unitRangeList[i];
    }


    QList<ReviewDataInfo> dataInfo;
    for (int i = 0; i < MED_ROW_COUNT; i++)
    {
        if (i + count - MED_ROW_COUNT >= MAX_ROW_COUNT)
        {
            ReviewDataInfo info;
            info.time.clear();
            info.unitRange = "";
            info.valueType = "";
            dataInfo.append(info);
        }
        else
        {
            dataInfo.append(d_ptr->dataInfos[count - MED_ROW_COUNT + i]);
        }
    }
    d_ptr->model->setDynamicInfos(dataInfo);

    dataInfo.clear();
    for (int i = 0; i < MED_ROW_COUNT; i++)
    {
        if (i + count - MED_ROW_COUNT >= MAX_ROW_COUNT)
        {
            ReviewDataInfo info;
            info.time.clear();
            info.unitRange = "";
            info.valueType = "";
            dataInfo.append(info);
        }
        else
        {
            dataInfo.append(d_ptr->colorInfos[count - MED_ROW_COUNT + i]);
        }
    }
    d_ptr->model->setDynamicColorInfos(dataInfo);

    d_ptr->tableView->selectColumn(column);
}

void HemodynaimcReviewWindow::onBtnLeftReleased()
{
    QModelIndex modelIndex = d_ptr->tableView->currentIndex();
    int column = modelIndex.column();
    int moveCount = d_ptr->moveCount;

    if (column < 2)
    {
        d_ptr->tableView->selectColumn(2);
        return;
    }

    if (column > 2)
    {
        d_ptr->tableView->selectColumn(column - 1);
        return;
    }

    if (moveCount == 0)
    {
        d_ptr->tableView->selectColumn(2);
        return;
    }

    d_ptr->moveCount--;
    QList<ReviewDataInfo> dataInfos = d_ptr->dataInfos;

    for (int i = 0; i < dataInfos.count(); i++)
    {
        dataInfos[i].time.clear();
        int count = d_ptr->dataInfos.at(1).time.count();
        int moveCount = d_ptr->moveCount;
        for (int j = moveCount * 4; j < count; j++)
        {
            QString str = d_ptr->dataInfos[i].time[j];
            dataInfos[i].time.append(str);
        }
    }

    d_ptr->model->setDynamicInfos(dataInfos);
    d_ptr->tableView->selectColumn(COLUMN_NUM_NR - 1);
}

















