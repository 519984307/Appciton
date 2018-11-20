/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/25
 **/
#include "NIBPRepairMenuWindow.h"
#include "NIBPProviderIFace.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPState.h"
#include "MessageBox.h"
#include "NIBPRepairMenuWindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QPainter>
#include <QBitmap>
#include <QScrollArea>
#include <QScrollBar>
#include "ColorManager.h"
#include "FontManager.h"

#include "NIBPParam.h"
#include "NIBPCalibrateContent.h"
#include "NIBPZeroPointContent.h"
#include "NIBPManometerContent.h"
#include "NIBPPressureControlContent.h"


class NIBPRepairMenuWindowPrivate
{
public:
    NIBPRepairMenuWindowPrivate();

    MessageBox *messageBoxWait;
    MessageBox *messageBoxError;

    bool replyFlag;                     // 进入维护模式标志
    bool repairError;                   // 维护模式错误
};

NIBPRepairMenuWindowPrivate::NIBPRepairMenuWindowPrivate()
    : messageBoxWait(NULL),
      messageBoxError(NULL),
      replyFlag(false),
      repairError(false)
{
}

NIBPRepairMenuWindow *NIBPRepairMenuWindow::getInstance()
{
    static NIBPRepairMenuWindow *instance = NULL;
    if (!instance)
    {
        instance = new NIBPRepairMenuWindow();
        instance->addMenuContent(NIBPCalibrateContent::getInstance());
        instance->addMenuContent(NIBPManometerContent::getInstance());
        instance->addMenuContent(NIBPZeroPointContent::getInstance());
        instance->addMenuContent(NIBPPressureControlContent::getInstance());
    }
    return instance;
}

/**************************************************************************************************
 * 功能：初始化。
 *************************************************************************************************/
void NIBPRepairMenuWindow::init()
{
//    NIBPCalibrateContent::getInstance()->init();
//    NIBPManometerContent::getInstance()->init();
//    NIBPPressureControlContent::getInstance()->init();
//    NIBPZeroPointContent::getInstance()->init();

    d_ptr->repairError = false;
    d_ptr->replyFlag = false;

    // 进入服务模式。
    nibpParam.changeMode(NIBP_STATE_MACHINE_SERVICE);
}

/**************************************************************************************************
 * 进入维护模式的应答。
 *************************************************************************************************/
void NIBPRepairMenuWindow::unPacket(bool flag)
{
    d_ptr->replyFlag = flag;
    if (flag == false)
    {
        messageBox();
    }
}

/**************************************************************************************************
 * 返回列表。
 *************************************************************************************************/
void NIBPRepairMenuWindow::returnMenu()
{
    NIBPZeroPointContent::getInstance()->init();
}

/**************************************************************************************************
 * 提示框显示。
 *************************************************************************************************/
void NIBPRepairMenuWindow::messageBox(void)
{
    d_ptr->messageBoxWait->exec();
}

/**************************************************************************************************
 * 警告框显示。
 *************************************************************************************************/
void NIBPRepairMenuWindow::warnShow(bool enable)
{
    // 模块硬件出错
    if (nibpParam.isErrorDisable())
    {
//        warn->setText(trs("NIBPModuleError"));
    }
    // 模块通信错误
    else
    {
//        warn->setText(trs("NIBPServiceModuleErrorQuitTryAgain"));
        if (enable)
        {
            if (d_ptr->messageBoxWait != NULL && d_ptr->messageBoxWait->isVisible())
            {
                d_ptr->messageBoxWait->close();
            }
            d_ptr->messageBoxError->exec();
        }
        else
        {
            if (d_ptr->messageBoxError != NULL && d_ptr->messageBoxError->isVisible())
            {
                d_ptr->messageBoxError->close();
            }
        }
    }

    if (enable)
    {
//        closeStrongFoucs();
    }
//    warn->setVisible(enable);
    d_ptr->repairError = enable;
}


void NIBPRepairMenuWindow::closeSlot()
{
    // 转换到状态，发送退出服务模式
    nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_REPAIR_RETURN, NULL, 0);

//    menuManager.returnPrevious();
}

bool NIBPRepairMenuWindow::getRepairError(void)
{
    return d_ptr->repairError;
}

/***************************************************************************************************
 * 隐藏事件
 **************************************************************************************************/
void NIBPRepairMenuWindow::hideEvent(QHideEvent *event)
{
    nibpParam.handleNIBPEvent(NIBP_EVENT_SERVICE_REPAIR_RETURN, NULL, 0);
    // 进入服务模式。
    nibpParam.changeMode(NIBP_STATE_MACHINE_MONITOR);
    MenuWindow::hideEvent(event);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPRepairMenuWindow::NIBPRepairMenuWindow()
    : MenuWindow(),
      d_ptr(new NIBPRepairMenuWindowPrivate)
{
    setWindowTitle(trs("NIBPRepairMenuWindow"));
    d_ptr->repairError = false;
    d_ptr->replyFlag = false;
    d_ptr->messageBoxWait = new MessageBox(trs("Warn"), trs("NIBPServiceWaitAgain"),
                                            QStringList(trs("EnglishYESChineseSURE")));
    d_ptr->messageBoxError = new MessageBox(trs("Warn"), trs("NIBPServiceModuleErrorQuitTryAgain"),
            QStringList(trs("EnglishYESChineseSURE")));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPRepairMenuWindow::~NIBPRepairMenuWindow()
{
    delete d_ptr;
}
