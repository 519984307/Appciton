/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/12
 **/



#pragma once
#include "MenuGroup.h"
#include "NIBPProviderIFace.h"
#include "NIBPServiceStateDefine.h"
#include "NIBPMonitorStateDefine.h"
#include "NIBPState.h"
#include "IMessageBox.h"
#include "MessageBox.h"

#define InvStr() ("---")

class QLabel;
class QStackedWidget;
class SubMenu;
class NIBPRepairMenuManager : public MenuGroup
{
    Q_OBJECT

public:
    static NIBPRepairMenuManager &Construation()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new NIBPRepairMenuManager();
        }

        return *_selfObj;
    }
    static NIBPRepairMenuManager *_selfObj;

    ~NIBPRepairMenuManager();

    //初始化
    void init();

    //进入模式的应答
    void unPacket(bool flag);

    // 返回到菜单列表项。
    void returnMenu(void);

    void messageBox(void);

    void warnShow(bool enable);

    // 获取维护模式是否错误
    bool getRepairError(void);

    void setMonitorState(NIBPMonitorStateID id);
    NIBPMonitorStateID getMonitorState(void);

signals:
    void foucsChange();

protected:
    void hideEvent(QHideEvent *event);

private slots:
    // 返回到菜单列表项。
    virtual void returnMenuList();

    //关闭窗口
    void _closeSlot();

private:
    NIBPRepairMenuManager();

    static const int _titleBarHeight = 29;
    static const int _listItemHeight = 26;
    static const int _borderWidth = 4;

    MessageBox *messageBoxWait;
    MessageBox *messageBoxError;

    bool _replyFlag;                     //进入维护模式标志
    bool _repairError;                   //维护模式错误
    NIBPMonitorStateID monitorState;
};
#define nibpRepairMenuManager (NIBPRepairMenuManager::Construation())
