/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by tongzhou fang  fangtongzhou@blmed.cn, 2019/3/20
 **/

#ifndef TESTNIBPSTATE_H
#define TESTNIBPSTATE_H

#include <QString>
#include <QtTest>
#include "NIBPState.h"
#include "MockNIBPStateMachine.h"
class NIBPStateTest : public QObject
{
    Q_OBJECT

public:
    NIBPStateTest();

private Q_SLOTS:
    void testGetID_data();
    void testGetID();
    void testGetStateMachine_data();
    void testGetStateMachine();
};

#endif
