/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/25
 **/

#pragma once
#include "Framework/UI/MenuContent.h"

enum FactoryContentTestType
{
    FACTORY_CONTENT_TEST_LIGHT,
    FACTORY_CONTENT_TEST_SOUND,
    FACTORY_CONTENT_TEST_KEY,
    FACTORY_CONTENT_TEST_PRINT,
#ifdef OUTPUT_TESTPACKET_THROUGH_USB_CONTENT
    FACTORY_CONTENT_TEST_USB_HS,
    FACTORY_CONTENT_TEST_USB_FS,
#endif
#ifndef HIDE_WIFI_FUNCTION
    FACTORY_CONTENT_TEST_WIFI,
#endif
#ifndef HIDE_WIRED_NETWORK_FUNCTION
    FACTORY_CONTENT_TEST_ENTHERNET,
#endif
    FACTORY_CONTENT_TEST_BATTERY,
    FACTORY_CONTENT_TEST_NR
};

class FactoryTestMenuContentPrivate;
class FactoryTestMenuContent : public MenuContent
{
    Q_OBJECT
public:
    FactoryTestMenuContent();
    ~FactoryTestMenuContent();

protected:
    virtual void layoutExec();

    virtual void readyShow();

private slots:
    void onBtnReleased();

private:
    FactoryTestMenuContentPrivate *const d_ptr;
};

