/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/23
 **/

#pragma once

#include <QAbstractTableModel>
#include "TrendDataDefine.h"
#include "IBPDefine.h"
#include "TrendDataStorageManager.h"

class TrendTableModelPrivate;
class TrendTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TrendTableModel(QObject *parent = NULL);
    ~TrendTableModel();

    /* reimplement */
    int columnCount(const QModelIndex &parent) const;

    /* reimplement */
    int rowCount(const QModelIndex &parent) const;

    /* reimplement */
    QVariant data(const QModelIndex &index, int role) const;

    /* reimplement */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /* reimplement */
    bool eventFilter(QObject *obj, QEvent *ev);

    int getHeaderHeightHint();

    int getRowHeightHint();

    void setTimeInterval(int t);

    void loadCurParam(int trendGroup);

    void setHistoryDataPath(QString path);

    void setHistoryData(bool flag);

    void updateData();

    void leftPage(int &curSecCol);

    void rightPage(int &curSecCol);

    void leftMoveEvent(int &curSecCol);

    void rightMoveEvent(int &curSecCol);

    bool getDataTimeRange(unsigned &start, unsigned &end);

    void displayDataTimeRange(unsigned &start, unsigned &end);

    void printTrendData(unsigned startTime, unsigned endTime);

    /**
     * @brief getTrendDataPack  get trend data pack
     * @return  trend data pack (read only)
     */
    const QVector<BlockEntry> &getBlockEntryList();

    /**
     * @brief getColumnCount  每次显示的最大固定列数
     */
    unsigned getColumnCount(void) const;

    /**
     * @brief getCurIndexInfo  获取当前model索引信息
     * @param curIndex 当前索引
     * @param totalIndex 总索引
     */
    void getCurIndexInfo(unsigned &curIndex, unsigned &totalIndex) const;

    /**
     * @brief getCurTableTime 获取当前表格时间
     * @return
     */
    QString getCurTableDate();

protected:
    void timerEvent(QTimerEvent *ev);

private:
    TrendTableModelPrivate * const d_ptr;
};
