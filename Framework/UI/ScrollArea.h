/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/6/29
 **/

#pragma once
#include <QScrollArea>

class ScrollAreaPrivate;
class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    enum FloatBarPolicy {
        FloatBarNotShow,                    // not show
        FloatBarShowForAWhile,              // show for a while
        FloatBarShowForAWhileWhenNeeded,    // show when the widget is higher
                                            // than the area, but for a while
        FloatBarShowForever,                // show forever
        FloatBarShowForeverWhenNeeded,      // show forever when the widget is
                                            // higher than the area
    };

    enum ScrollDirection {
        ScrollHorizontal = 0x01,
        ScrollVertical = 0x02,
        ScrollBothDirection = ScrollHorizontal|ScrollVertical,
    };

    explicit ScrollArea(QWidget *parent = NULL);
    ~ScrollArea();

    /**
     * @brief setFloatbarPolicy set the float bar display policy,
     *                          default policy is @FolatBarShowForAWhile
     * @param policy the policy
     */
    void setFloatbarPolicy(FloatBarPolicy policy);

    /**
     * @brief setOverShot set the scroll area support overshot or not
     * @param enable
     */
    void setOverShot(bool enable);

    /**
     * @brief updateFloatBar update the scroll bar status
     */
    void updateFloatBar();

    /**
     * @brief setScrollDirection set the scroll direction
     * @param dir direction
     */
    void setScrollDirection(ScrollDirection dir);

protected:
    /* reimplement */
    void resizeEvent(QResizeEvent *ev);
    /* reimplement */
    void showEvent(QShowEvent *ev);
    /* reimplement */
    void scrollContentsBy(int dx, int dy);

    /* reimplement */
    bool viewportEvent(QEvent *ev);

    ScrollArea(ScrollAreaPrivate *d, QWidget *parent);

    QScopedPointer<ScrollAreaPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(ScrollArea)
};
