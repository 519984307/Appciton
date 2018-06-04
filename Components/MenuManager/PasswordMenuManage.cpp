#include "PasswordMenuManage.h"
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "LanguageManager.h"
#include "FontManager.h"
#include "Debug.h"
#include "LabelButton.h"
#include "SystemManager.h"
#include "IConfig.h"
#include "MenuManager.h"
#include "SupervisorMenuManager.h"
#include "ServiceWindowManager.h"
#include "FactoryWindowManager.h"
#include "UserMaintainEntrance.h"
#include "UserMaintainManager.h"
#include "UserMaintainGeneralSet.h"
/**************************************************************************************************
 * 功能: 构造函数
**************************************************************************************************/
PasswordMenuManage::PasswordMenuManage(const QString &title) :
    _title(title)
{
    QPalette p;

    //2s后清除警告信息
    _timer = new QTimer(this);
    _timer->setSingleShot(true);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOutSlot()));

    int fontSize = fontManager.getFontSize(2);

    _titleLabel = new QLabel(title);
    _titleLabel->setFont(fontManager.textFont(fontSize));
    _titleLabel->setAlignment(Qt::AlignCenter);
    p = _titleLabel->palette();
    p.setColor(QPalette::Foreground, Qt::black);
    _titleLabel->setPalette(p);
    _titleLabel->setFixedHeight(30);

    _passwordEdit = new QLineEdit();
    _passwordEdit->setReadOnly(true);
    _passwordEdit->setMaxLength(PASSWORD_LEN);
    p = _passwordEdit->palette();
    p.setColor(QPalette::Foreground, Qt::black);
    p.setColor(QPalette::Background, Qt::white);
    _passwordEdit->setPalette(p);
    _passwordEdit->setFont(fontManager.textFont(fontSize));
    _passwordEdit->setFixedHeight(30);
    _passwordEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _passwordEdit->setFocusPolicy(Qt::NoFocus);
    _passwordEdit->setStyleSheet("border: 1px solid gray;border-radius:3px;");
    _passwordEdit->setPlaceholderText(trs("PassWord"));

    //第一行
    QHBoxLayout *firstRowLayout = new QHBoxLayout();
    firstRowLayout->addWidget(_passwordEdit);

    //第二行
    _infoLabel = new QLabel();
    _infoLabel->setFont(fontManager.textFont(fontSize));
    _infoLabel->setFixedHeight(30);
    _infoLabel->setAlignment(Qt::AlignCenter);
    p.setColor(QPalette::Foreground, Qt::red);
    _infoLabel->setPalette(p);

    //第三行，数字，0-4
    _numBtn[0] = new LButton(0);
    _numBtn[1] = new LButton(1);
    _numBtn[2] = new LButton(2);
    _numBtn[3] = new LButton(3);
    _numBtn[4] = new LButton(4);
    _numBtn[5] = new LButton();
    QHBoxLayout *thirdRowLayout = new QHBoxLayout();
    thirdRowLayout->addWidget(_numBtn[0]);
    thirdRowLayout->addWidget(_numBtn[1]);
    thirdRowLayout->addWidget(_numBtn[2]);
    thirdRowLayout->addWidget(_numBtn[3]);
    thirdRowLayout->addWidget(_numBtn[4]);
    thirdRowLayout->addWidget(_numBtn[5]);

    //第四行，数字，5-9
    _numBtn[6] = new LButton(5);
    _numBtn[7] = new LButton(6);
    _numBtn[8] = new LButton(7);
    _numBtn[9] = new LButton(8);
    _numBtn[10] = new LButton(9);
    _numBtn[11] = new LButton();
    QHBoxLayout *fourthRowLayout = new QHBoxLayout();
    fourthRowLayout->addWidget(_numBtn[6]);
    fourthRowLayout->addWidget(_numBtn[7]);
    fourthRowLayout->addWidget(_numBtn[8]);
    fourthRowLayout->addWidget(_numBtn[9]);
    fourthRowLayout->addWidget(_numBtn[10]);
    fourthRowLayout->addWidget(_numBtn[11]);

    //添加信号-槽函数
    for (int i = 0; i < 12; ++i)
    {
        if (i < 5)
        {
            connect(_numBtn[i], SIGNAL(released(int)), this, SLOT(_numBtnSlot(int)));
            _numBtn[i]->setText(QString::number(i));
        }
        else if (i == 5)
        {
            connect(_numBtn[i], SIGNAL(released(int)), this, SLOT(_backspaceBtnSlot(int)));
            _numBtn[i]->setText(trs("SupervisorDel"));
        }
        else if (i < 11)
        {
            connect(_numBtn[i], SIGNAL(released(int)), this, SLOT(_numBtnSlot(int)));
            _numBtn[i]->setText(QString::number(i - 1));
        }
        else
        {
            connect(_numBtn[i], SIGNAL(released(int)), this, SLOT(_okBtnSlot(int)));
            _numBtn[i]->setText(trs("SupervisorOK"));
        }

        _numBtn[i]->setFont(fontManager.textFont(fontManager.getFontSize(fontSize)));
        p = _numBtn[i]->palette();
        p.setColor(QPalette::Background, Qt::white);
        _numBtn[i]->setPalette(p);
        _numBtn[i]->setFixedSize(30, 30);
    }

    _mainLayout = new QVBoxLayout();
    _mainLayout->setMargin(50);
    _mainLayout->setSpacing(10);
    _mainLayout->addWidget(_titleLabel);
    _mainLayout->addLayout(firstRowLayout);
    _mainLayout->addWidget(_infoLabel);
    _mainLayout->addLayout(thirdRowLayout);
    _mainLayout->addLayout(fourthRowLayout);
    _mainLayout->addStretch();

    setLayout(_mainLayout);

//    setFixedSize(supervisorWindowManager.getSubmenuWidth(),
//                 supervisorWindowManager.getSubmenuHeight());

    setFocusPolicy(Qt::NoFocus);

    _initData();

    _passwordStr.clear();
}

/**************************************************************************************************
 * 功能: 设置密码
**************************************************************************************************/
void PasswordMenuManage::setPassword(const QString &password)
{
    _normalPassword = password;
}

/**************************************************************************************************
 * 功能: 设置用户维护密码
**************************************************************************************************/
void PasswordMenuManage::setUserMaintainPassword(const QString &password)
{
    _userMaintainPassword = password;
}

/**************************************************************************************************
 * 功能: 清除密码和密码输显示
**************************************************************************************************/
void PasswordMenuManage::clearPassword(void)
{
    _passwordStr.clear();
    _passwordEdit->setText("");
}

/**************************************************************************************************
 * 功能: 初始化数据。
**************************************************************************************************/
void PasswordMenuManage::_initData(void)
{
    currentConfig.getStrValue("General|SuperPassword", _superPassword);
}

/**************************************************************************************************
 * 功能: 数字按键处理
**************************************************************************************************/
void PasswordMenuManage::_numBtnSlot(int index)
{
    if (_passwordStr.size() >= PASSWORD_LEN)
    {
        return;
    }

    //添加当前数字
    _passwordStr += QString::number(index);

    QString text(_passwordStr.size(), '*');
    _passwordEdit->setText(text);
}

/**************************************************************************************************
 * 功能: 删除键处理
**************************************************************************************************/
void PasswordMenuManage::_backspaceBtnSlot(int /*index*/)
{
    if (_passwordStr.size() == 0)
    {
        return;
    }

    //删除末尾数字
    _passwordStr.remove(_passwordStr.size() - 1, 1);
    QString text(_passwordStr.size(), '*');
    _passwordEdit->setText(text);
}

/**************************************************************************************************
 * 功能: ok键处理
**************************************************************************************************/
void PasswordMenuManage::_okBtnSlot(int /*index*/)
{
    //超级密码
    if (_passwordStr == _superPassword)
    {
//        emit enterSignal();
        supervisorMenuManager.popup();
        return;
    }

    //普通密码
    else if (_passwordStr == _normalPassword)
    {
        supervisorMenuManager.popup();
        return;
    }

    //服务密码
    else if (_passwordStr == SERVER_PASSWORD)
    {
        serviceWindowManager.popup();
        return;
    }

    //用户维护密码
    else if (_passwordStr == _userMaintainPassword)
    {
        userMaintainManager.popup();
        //
        return;
    }

    //工厂密码
    else if (_passwordStr == FACTORY_PASSWORD)
    {
        //factoryWindowManager.popup();
        factoryMaintainManager.popup();
        return;
    }

    clearPassword();
    _infoLabel->setText(trs("ErrorPassWordERROR"));
    _timer->start(2*1000);
}

/**************************************************************************************************
 * 功能: 析构函数
**************************************************************************************************/
void PasswordMenuManage::_timeOutSlot()
{
    _infoLabel->setText("");
}

/**************************************************************************************************
 * 功能: 析构函数
**************************************************************************************************/
PasswordMenuManage::~PasswordMenuManage()
{
    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}

