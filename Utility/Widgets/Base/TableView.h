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

    /* reimplement */
    QSize sizeHint() const;

    /**
     * @brief scrollTonextPage show the next page of items
     */
    void scrollToNextPage();

    /**
     * @brief scrollToPreviousPage show the previous page of items
     */
    void scrollToPreviousPage();

    /**
     * @brief scrollToAssignedPage 显示指定行所在的页面
     */
    void scrollToAssignedPage(int row);

    /**
     * @brief hasPreivousPage check whether has next page
     * @return
     */
    bool hasPreivousPage();

    /**
     * @brief hasNextPage check whether has preivous page
     * @return
     */
    bool hasNextPage();

    /* reimplement */
    void setModel(QAbstractItemModel *model);

    /**
     * @brief getPageInfo 获取页码信息
     * @param curPage     当前页码
     * @param totalPage   总页数
     */
    void getPageInfo(int &curPage, int &totalPage);

signals:
    /* emit when the row is clicked */
    void rowClicked(int row);

    /* emit when the select row changed */
    void selectRowChanged(int row);

    /* emit when the item is clicked */
    void itemClicked(QModelIndex item);

protected:
    /* reimplement */
    // void mouseMoveEvent(QMouseEvent *ev);

    /* reimplement */
    void keyPressEvent(QKeyEvent *ev);

    /* reimplement */
    void keyReleaseEvent(QKeyEvent *ev);

    /* reimplement */
    void mousePressEvent(QMouseEvent *ev);

    /* reimplement */
    void mouseReleaseEvent(QMouseEvent *ev);

    /* reimplement */
    void focusInEvent(QFocusEvent *ev);

    /* reimplement */
    void focusOutEvent(QFocusEvent *ev);

private slots:
    /* need to perform some check when the table is focus out, if the table don't have focus any more,
     * it would update the selection and select nothing
     */
    void checkAfterFocusOut();

    /**
     * @brief onSpanChanged handle the row span changed event
     * @param index the item's index
     */
    void onSpanChanged(const QModelIndex &index);

    /**
     * @brief onModelToBeReset 数据复位前准备
     */
    void onModelToBeReset();

    /**
     * @brief onModelReset 处理model复位后，焦点位置
     */
    void onModelReset();

private:
    TableViewPrivate *const d_ptr;
};

