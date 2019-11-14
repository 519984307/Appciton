/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/6/29
 **/

#include "ScrollArea.h"
#include "FloatScrollBar.h"
#include "KineticScroller/qscrollareakineticscroller.h"

class ScrollAreaPrivate
{
    Q_DECLARE_PUBLIC(ScrollArea)
public:
    explicit ScrollAreaPrivate(ScrollArea *const q_ptr);

    /**
     * @brief init initialize the widget
     */
    void init();

    void setupScrollBarPolicy();

    ScrollArea *const q_ptr;
    FloatScrollBar *scrollBar;
    QScrollAreaKineticScroller *scroller;
    ScrollArea::FloatBarPolicy scrollBarPolicy;
    ScrollArea::ScrollDirection direction;
};
