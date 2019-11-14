/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/4
 **/

#pragma once
#include <QWidget>
#include <QScopedPointer>

class PopupMoveEditorPrivate;
class PopupMoveEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PopupMoveEditor(const QString &text);
    ~PopupMoveEditor();

    /* set the editor Area global geometry */
    void setEditorGeometry(const QRect &r);

    /* reimplement */
    void paintEvent(QPaintEvent *ev);

    /* reimplement */
    QSize sizeHint() const;

signals:
    void leftMove(void);
    void rightMove(void);

protected:
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void showEvent(QShowEvent *ev);
private slots:
    /**
     * @brief timeOutExec
     */
    void timeOutExec();

private:
    QScopedPointer<PopupMoveEditorPrivate> d_ptr;
};
