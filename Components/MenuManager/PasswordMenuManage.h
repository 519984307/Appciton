/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once
#include <QList>
#include <QTimer>
#include "IWidget.h"
#include "SystemManager.h"
#include "FactoryMaintainManager.h"

class QLabel;
class QVBoxLayout;
class QLineEdit;
class IButton;
class LButton;

#define PASSWORD_LEN (8)                      //密码长度
#define FACTORY_PASSWORD ("12345678")         //工厂密码
#define SERVER_PASSWORD ("11111111")          //服务密码

//密码输入菜单
class PasswordMenuManage : public IWidget
{
    Q_OBJECT

public:
    //构造函数
    explicit PasswordMenuManage(const QString &title);
    ~PasswordMenuManage(); // 析构函数

public:
    void setWorkMode(int mode);
    void setPassword(const QString &password);
    void clearPassword(void);
    void setUserMaintainPassword(const QString &password);
    void setFactoryMaintainPassword(const QString &password);
    void setSuperPassword(const QString &password);

    void setOkBtn(int id);

signals:
    // 返回到菜单列表。
//    void enterSignal(void);

private slots:
    void _numBtnSlot(int index);
    void _backspaceBtnSlot(int index);
    void _okBtnSlot(int index);

    void _timeOutSlot();

private:
    //初始化数据
    void _initData(void);

    QTimer *_timer;                            //定时器，用于清除警告区信息

    QLabel *_titleLabel;                      //标题
    QLabel *_infoLabel;                       //信息提示
    QLineEdit *_passwordEdit;                 //密码输入框
    LButton *_numBtn[12];
    QVBoxLayout *_mainLayout;                 //主布局器

    QString _title;
    QString _passwordStr;                     //密码
    QString _superPassword;                   //超级密码
    QString _normalPassword;                  //通用配置管理密码
    QString _userMaintainPassword;            //用户维护密码
    QString _factoryMaintainPassword;         //工厂维护密码
};
