#include "FreezeTableWidget.h"
#include <QScrollBar>
#include <QHeaderView>

FreezeTableWidget::FreezeTableWidget(QAbstractItemModel * model) : _freezeColumnCount(1)
{

    setStyleSheet("QTableView { border: none;"
                  "background-color: rgb(255, 255, 255, 255);"
                  "selection-background-color: rgb(65, 105, 225, 255);"
                  "selection-color: black;"
                  "font: 10pt;}");
    horizontalHeader()->setStyleSheet("QHeaderView {font: 10pt;}");
    setModel(model);
    _frozenTableView = new QTableView(this);
    _model = new QStandardItemModel();
    _model = model;

    init();

    connect(horizontalHeader(),SIGNAL(sectionResized(int,int,int)), this,
            SLOT(updateSectionWidth(int,int,int)));
    connect(verticalHeader(),SIGNAL(sectionResized(int,int,int)), this,
            SLOT(updateSectionHeight(int,int,int)));

    connect(_frozenTableView->verticalScrollBar(), SIGNAL(valueChanged(int)),
            verticalScrollBar(), SLOT(setValue(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
            _frozenTableView->verticalScrollBar(), SLOT(setValue(int)));

}

FreezeTableWidget::~FreezeTableWidget()
{

}

void FreezeTableWidget::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    updateFrozenTableGeometry();
}

QModelIndex FreezeTableWidget::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

    if (cursorAction == MoveLeft && current.column() > 0
            && visualRect(current).topLeft().x() < _frozenTableView->columnWidth(0) ){
          const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x()
                               - _frozenTableView->columnWidth(0);
          horizontalScrollBar()->setValue(newValue);
    }
    return current;
}

void FreezeTableWidget::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{

    if (current.column() < previous.column() && current.column() > 0
            && visualRect(current).topLeft().x() < _frozenTableView->columnWidth(0) * 2 ){
          const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x()
                               - _frozenTableView->columnWidth(0) * 2;
          horizontalScrollBar()->setValue(newValue);
    }
    else if (current.column() > previous.column() && current.column() > 0
             && visualRect(current).topRight().x() > _frozenTableView->columnWidth(0) * 6)
    {
        const int newValue = 2 + horizontalScrollBar()->value() + visualRect(current).topRight().x() - _frozenTableView->columnWidth(0) * 6;
        horizontalScrollBar()->setValue(newValue);
    }
}

void FreezeTableWidget::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    if (index.column() > 0)
        QTableView::scrollTo(index, hint);
}

void FreezeTableWidget::updateSectionWidth(int logicalIndex, int /*oldSize*/, int newSize)
{
    if (logicalIndex == 0){
          _frozenTableView->setColumnWidth(0, newSize);
          updateFrozenTableGeometry();
    }
}

void FreezeTableWidget::updateSectionHeight(int logicalIndex, int /*oldSize*/, int newSize)
{
    _frozenTableView->setRowHeight(logicalIndex, newSize);
}

void FreezeTableWidget::setFreezeColumn(int count)
{
    _freezeColumnCount = count;
}

void FreezeTableWidget::init()
{
    _frozenTableView->setModel(_model);
    _frozenTableView->setFocusPolicy(Qt::NoFocus);
    _frozenTableView->verticalHeader()->hide();
    _frozenTableView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);

    viewport()->stackUnder(_frozenTableView);
//! [init part1]

//! [init part2]
    _frozenTableView->setStyleSheet("QTableView { border: none;"
                                   "background-color: rgb(142, 222, 33, 255);"
                                   "selection-background-color: rgb(153, 153, 153, 255)}"); //for demo purposes
    _frozenTableView->horizontalHeader()->setStyleSheet("QHeaderView {font: 10pt;}");
    _frozenTableView->setSelectionMode(QAbstractItemView::NoSelection);
    _frozenTableView->horizontalHeader()->setClickable(false);
//    _frozenTableView->setSelectionModel(selectionModel());
    for (int col = _freezeColumnCount; col < model()->columnCount(); ++col)
    {
        _model->removeColumn(col);
//          _frozenTableView->setColumnHidden(col, true);
    }

    _frozenTableView->setColumnWidth(0, columnWidth(0) );

    _frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _frozenTableView->show();

    updateFrozenTableGeometry();

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    _frozenTableView->setVerticalScrollMode(ScrollPerPixel);
}

void FreezeTableWidget::updateFrozenTableGeometry()
{
    for (int i = 0; i < _freezeColumnCount; i ++)
    {
        _frozenTableView->setColumnWidth(i, columnWidth(i));
    }
    _frozenTableView->setGeometry(verticalHeader()->width() + frameWidth(),
                                 frameWidth(), columnWidth(0) * _freezeColumnCount,
                                 viewport()->height()+horizontalHeader()->height());
}
