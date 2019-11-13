/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/30
 **/

#pragma once
#include <QAbstractButton>
#include <QScopedPointer>

class DropListPrivate;
class DropList : public QAbstractButton
{
    Q_OBJECT
public:
    explicit DropList(const QString &text = QString(), QWidget *parent = NULL);
    ~DropList();

    void addItem(const QString &text);
    void addItems(const QStringList &textList);

    /* reimplement */
    QSize sizeHint() const;

signals:
    void currentIndexChanged(int);

public slots:
    void setCurrentIndex(int);

protected:
    /* reimplement */
    void keyPressEvent(QKeyEvent *ev);
    /* reimplement */
    void keyReleaseEvent(QKeyEvent *ev);
    /* reimplement */
    void hideEvent(QHideEvent *ev);
    /* reimplement */
    void paintEvent(QPaintEvent *ev);

    void mousePressEvent(QMouseEvent *e);

private slots:
    void showPopup();

private:
    QScopedPointer<DropListPrivate> d_ptr;
};
