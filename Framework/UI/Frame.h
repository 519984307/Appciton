/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/


#pragma once
#include <QWidget>


class FramePrivate;
class FrameItem;
class Frame : public QWidget
{
 Q_OBJECT
public:
    explicit Frame(QWidget *parent = NULL);
    ~Frame();


    /**
     * @brief addItem add a frame item and take the ownership of the item
     * @param item the frame item
     * @return ture if the item has not been added, otherwise, return false
     */
    bool addItem(FrameItem *item);

    /**
     * @brief itemCount get the number of the frame item
     * @return the number;
     */
    int itemCount() const;

    /**
     * @brief indexOf get the index of the frame item
     * @param item frame item
     * @return return the item index when found, otherwise, return -1
     */
    int indexOf(FrameItem *item) const;

private:
    const QScopedPointer<FramePrivate> d_ptr;
};
