/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/30
 **/

#pragma once
#include <IWidget.h>
#include <QMap>

class TopBarWidgetPrivate;
class TopBarWidget : public IWidget
{
public:
    static TopBarWidget &constrution(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TopBarWidget;
        }
        return *_selfObj;
    }
    static TopBarWidget *_selfObj;
    ~TopBarWidget();

    bool addWidget(IWidget *win);

    void refreshLayout();

    QColor getTopBarBlackGroundColor();

    IWidget *findWidget(QString);

protected:
    void paintEvent(QPaintEvent *);

private:
    TopBarWidget();
    TopBarWidgetPrivate *d_ptr;
    void _layout();
};

#define topBarWidget (TopBarWidget::constrution())
