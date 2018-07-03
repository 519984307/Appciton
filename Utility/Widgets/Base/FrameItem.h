/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/2
 **/



#pragma once
#include <QAbstractButton>

class Frame;
class FrameItemPrivate;
class FrameItem : public QAbstractButton
{
    Q_OBJECT
public:
    explicit FrameItem(const QString &text = NULL, Frame *parent = NULL);
    ~FrameItem();

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *ev);

    const QScopedPointer<FrameItemPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(FrameItem)
};
