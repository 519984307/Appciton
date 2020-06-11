/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/11
 **/

#pragma once
#include <QWidget>
#include <QScopedPointer>


class COMeasureResultWidgetPrivate;
class COMeasureResultWidget : public QWidget
{
    Q_OBJECT
public:
    explicit COMeasureResultWidget(QWidget *parent = NULL);
    ~COMeasureResultWidget();

    /**
     * @brief setChecked set the widget in check state
     * @param check
     */
    void setChecked(bool check);

    /**
     * @brief isChecked check whether the widget is in checked state
     * @return true when in check state, otherwise, false
     */
    bool isChecked() const;

signals:
    void chechedStateChanged(bool checked);

protected:
    /* override */
    void resizeEvent(QResizeEvent *ev);

    /* override */
    void paintEvent(QPaintEvent *ev);

    /* override */
    void keyPressEvent(QKeyEvent *ev);

    /* override */
    void keyReleaseEvent(QKeyEvent *ev);

    /* override */
    void mousePressEvent(QMouseEvent *ev);

    /* override */
    void mouseReleaseEvent(QMouseEvent *ev);


private:
    const QScopedPointer<COMeasureResultWidgetPrivate> pimpl;
};
