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
     * @brief hasPreivousPage check whether has next page
     * @return
     */
    bool hasPreivousPage();

    /**
     * @brief hasNextPage check whether has preivous page
     * @return
     */
    bool hasNextPage();

protected:
    /* reimplement */
    // void mouseMoveEvent(QMouseEvent *ev);

    /* reimplement */
    void keyPressEvent(QKeyEvent *ev);

    /* reimplement */
    void keyReleaseEvent(QKeyEvent *ev);

    /* reimplement */
    void focusInEvent(QFocusEvent *ev);

    /* reimplement */
    void focusOutEvent(QFocusEvent *ev);

private:
    TableViewPrivate *const d_ptr;
};

