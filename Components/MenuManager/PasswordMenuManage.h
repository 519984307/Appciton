#pragma once
#include <QList>
#include <QTimer>
#include "IWidget.h"
#include "SystemManager.h"

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
    PasswordMenuManage(const QString &title);
    ~PasswordMenuManage(); // 析构函数

public:
    void setWorkMode(int mode);
    void setPassword(const QString &password);
    void clearPassword(void);
    void setUserMaintainPassword(const QString &password);

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
    QString _normalPassword;                  //普通密码
    QString _userMaintainPassword;            //用户维护密码
};
