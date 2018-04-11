#include "NIBPRepair.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "LabelButton.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QPainter>
#include <QBitmap>
#include <QScrollArea>
#include <QScrollBar>
#include "ColorManager.h"
#include "SubMenu.h"
#include "Debug.h"
#include "FontManager.h"

#include "NIBPCalibrate.h"
#include "NIBPZeroPoint.h"
#include "NIBPManometer.h"
#include "NIBPPressureControl.h"



NIBPRepair *NIBPRepair::_selfObj = NULL;

/**************************************************************************************************
 * 功能：初始化。
 *************************************************************************************************/
void NIBPRepair::init()
{
    nibpcalibrate.init();
    nibpmanometer.init();
    nibppressurecontrol.init();
    nibpzeropoint.init();

    _repairError = false;

    if (nibpParamService.isErrorDisable())
    {
        warnShow(true);
    }
    else
    {
        if (nibpParamService.connectWrongFlag)
        {
            warnShow(true);
        }
        else
        {
            warnShow(false);
        }
    }

    _replyFlag = false;

    // 转换到状态。
    nibpParamService.switchState(NIBP_Service_REPAIR);
    nibpParamService.serviceEnter(true);
}

/**************************************************************************************************
 * 进入维护模式的应答。
 *************************************************************************************************/
void NIBPRepair::unPacket(bool flag)
{
    _replyFlag = flag;
}

/**************************************************************************************************
 * 返回列表。
 *************************************************************************************************/
void NIBPRepair::returnMenu()
{
    nibpcalibrate.init();
    nibpmanometer.init();
    nibppressurecontrol.init();
    nibpzeropoint.init();
}

/**************************************************************************************************
 * 提示框显示。
 *************************************************************************************************/
void NIBPRepair::messageBox(void)
{
    messageBoxWait->exec();
}

/**************************************************************************************************
 * 警告框显示。
 *************************************************************************************************/
void NIBPRepair::warnShow(bool enable)
{
    // 模块硬件出错
    if (nibpParamService.isErrorDisable())
    {
        warn->setText(trs("NIBPModuleError"));
    }
    // 模块通信错误
    else
    {
        warn->setText(trs("NIBPServiceModuleErrorQuitTryAgain"));
        if(enable)
        {
            if (messageBoxWait != NULL && messageBoxWait->isVisible())
            {
                messageBoxWait->close();
            }
            messageBoxError->exec();
        }
        else
        {
            if (messageBoxError != NULL && messageBoxError->isVisible())
            {
                messageBoxError->close();
            }
        }
    }

    warn->setVisible(enable);
    _repairError = enable;
}

/**************************************************************************************************
 * 重新聚焦菜单列表。
 *************************************************************************************************/
void NIBPRepair::_returnMenuList()
{
    nibpParamService.serviceReturn();
    MenuGroup::returnMenuList();
}

void NIBPRepair::_closeSlot()
{
    // 转换到状态，发送退出服务模式
    nibpParamService.switchState(NIBP_Service_REPAIR);
    nibpParamService.serviceReturn();

    // 转换到状态，关闭窗口
    nibpParamService.switchState(NIBP_Service_NULL);

    menuManager.returnPrevious();
}

bool NIBPRepair::getRepairError(void)
{
    return _repairError;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPRepair::NIBPRepair() : MenuGroup("SupervisorMenuManager")
{
    _repairError = false;
    _replyFlag = false;
    messageBoxWait = new IMessageBox(trs("Warn"), trs("NIBPServiceWaitAgain"), QStringList(trs("EnglishYESChineseSURE")));
    messageBoxError = new IMessageBox(trs("Warn"), trs("NIBPServiceModuleErrorQuitTryAgain"), QStringList(trs("EnglishYESChineseSURE")));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPRepair::~NIBPRepair()
{
    delete messageBoxWait;
    delete messageBoxError;
}
