#pragma once

#include <QTableView>
#include <QStandardItemModel>


class FreezeTableWidget : public QTableView
{
    Q_OBJECT

public:
    FreezeTableWidget(QAbstractItemModel *model);
    ~FreezeTableWidget();

    void setFreezeColumn(int);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void scrollTo (const QModelIndex & index, ScrollHint hint = EnsureVisible);

private slots:
      void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
      void updateSectionHeight(int logicalIndex, int oldSize, int newSize);

private:
    void init();
    void updateFrozenTableGeometry();

private:
    QTableView *_frozenTableView;
    QAbstractItemModel *_model;
    int _freezeColumnCount;
};
