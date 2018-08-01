/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/

#include "MenuContent.h"

class MonitorInfoContentPrivate;
class MonitorInfoContent : public MenuContent
{
    Q_OBJECT
public:
    MonitorInfoContent();
    ~MonitorInfoContent();

protected:
    virtual void readyShow();
    virtual void layoutExec();

private slots:
    /**
     * @brief onBtnReleasedChanged
     */
    void onBtnReleasedChanged();

private:
    MonitorInfoContentPrivate *const d_ptr;
};


