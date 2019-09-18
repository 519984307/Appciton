/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/4/2
 **/

#include "NurseCallManagerInterface.h"
#include "stddef.h"

static NurseCallManagerInterface *curNurseCallManagerInterface = NULL;

NurseCallManagerInterface *NurseCallManagerInterface::registerNurseCallManagerInterface(NurseCallManagerInterface *interface)
{
    NurseCallManagerInterface *oldHandle = curNurseCallManagerInterface;
    curNurseCallManagerInterface = interface;
    return oldHandle;
}

NurseCallManagerInterface *NurseCallManagerInterface::getNurseCallManagerInterface()
{
    return curNurseCallManagerInterface;
}
