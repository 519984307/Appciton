/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/17
 **/

#include "MenuContent.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditNIBPMenuContentPrivate;
class ConfigEditNIBPMenuContent : public MenuContent
{
    Q_OBJECT
public:
    explicit ConfigEditNIBPMenuContent(Config *const config);
    ~ConfigEditNIBPMenuContent();

protected:
    virtual void readyShow();
    virtual void layoutExec();

private slots:
    /**
     * @brief onComboIndexChanged
     * @param index
     */
    void  onComboIndexChanged(int index);

private:
    ConfigEditNIBPMenuContentPrivate *const d_ptr;
};

