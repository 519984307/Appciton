/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/10/30
 **/

#include "NIBPMaintainMgrInterface.h"
#include "NIBPRepairMenuWindow.h"

NIBPMaintainMgrInterface *NIBPMaintainMgrInterface::getNIBPMaintainMgr()
{
    return NIBPRepairMenuWindow::getInstance();
}
