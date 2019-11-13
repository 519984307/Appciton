/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

class FactoryDataRecordContentPrivate;
class FactoryDataRecordContent : public MenuContent
{
    Q_OBJECT
public:
    FactoryDataRecordContent();
    ~FactoryDataRecordContent();

protected:
    virtual void readyShow();
    virtual void layoutExec();

private slots:
    /**
     * @brief onComboBoxIndexChanged
     * @param index
     */
    void onComboBoxIndexChanged(int index);

private:
    FactoryDataRecordContentPrivate *const d_ptr;
};
