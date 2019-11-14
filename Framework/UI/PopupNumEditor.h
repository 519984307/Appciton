/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/16
 **/

#pragma once
#include <QWidget>
#include "ItemEditInfo.h"

class PopupNumEditorPrivate;
/**
 * @brief The PopupNumEditor class
 *        create a value editor to edit the value, this widget is popup type and will
 *        deleted when hide. Mainly use to value of item in the view. Support edit
 *        int and float value.
 */
class PopupNumEditor : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief PopupNumEditor
     * @param showHint  if show hint
     */
    PopupNumEditor();
    ~PopupNumEditor();

    /* set the edit value Area global geometry */
    void setEditValueGeometry(const QRect &r);

    /* set the edit item infos */
    void setEditInfo(const ItemEditInfo &info);

    /* reimplement */
    void paintEvent(QPaintEvent *ev);

    /* reimplement */
    QSize sizeHint() const;

protected:
    /* reimplement */
    void keyPressEvent(QKeyEvent *ev);
    /* reimplement */
    void keyReleaseEvent(QKeyEvent *ev);
    /* reimplement */
    void mousePressEvent(QMouseEvent *ev);
    /* reimplement */
    void mouseReleaseEvent(QMouseEvent *ev);
    /* reimplement */
    void showEvent(QShowEvent *ev);

signals:
    /* emit when edit value changed */
    void valueChanged(int value);

private slots:
    /**
     * @brief timeOutExec
     */
    void timeOutExec();

private:
    PopupNumEditorPrivate *const d_ptr;
    friend class PopupNumEditorPrivate;
};
