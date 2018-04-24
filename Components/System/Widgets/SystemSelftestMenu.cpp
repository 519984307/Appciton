#include <QTextEdit>
#include <QDesktopWidget>
#include <QApplication>
#include <QTimer>
#include "SystemSelftestMenu.h"
#include "IButton.h"
#include "FontManager.h"
#include "LanguageManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SystemSelftestMenu::SystemSelftestMenu() : PopupWidget()
{
    //将系统自检窗口定义为最顶端显示、无边框
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
   //设置窗口栏标题文本
    setTitleBarText(trs("SystemSelftestResult"));
    //隐藏存在的按钮
    hideExitButton();

    //窗口调色
    QPalette p = this->palette();
    p.setColor(QPalette::Window, Qt::white);
    setPalette(p);

    //文本框编辑
    _info = new QTextEdit();
    _info->setFrameStyle(Qt::FramelessWindowHint);
    _info->setFocusPolicy(Qt::NoFocus);
    _info->setReadOnly(true);
    _info->setFont(fontManager.textFont(15));
    _info->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //按钮编辑 添加按钮槽函数_yesReleased
    _ok = new IButton(trs("SupervisorOK"));
    _ok->setFixedHeight(30);
    _ok->setVisible(false);
    _ok->setBorderEnabled(true);
    _ok->setBorderColor(QColor(120, 120, 120));
    _ok->setFont(fontManager.textFont(15));
    connect(_ok, SIGNAL(realReleased()), this, SLOT(_yesReleased()));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, 0, 4);
    hlayout->setSpacing(0);
    hlayout->addStretch(1);
    hlayout->addWidget(_ok);
    hlayout->addStretch(1);

    contentLayout->addWidget(_info);
    contentLayout->addLayout(hlayout);
    contentLayout->setSpacing(2);

    QDesktopWidget *w = QApplication::desktop();
    setFixedSize(w->width() / 2, 180);
    _testResult.clear();
}

/**************************************************************************************************
 * 追加信息。
 *************************************************************************************************/
void SystemSelftestMenu::appendInfo(ModulePoweronTestResult module, ModulePoweronTestStatus status, const QString &info)
{
    if (_ok->isVisible())
    {
        return;
    }

    _info->append(info);
    _moduleTestStatus[module] = status;
}

/**************************************************************************************************
 * 测试结束。
 *************************************************************************************************/
void SystemSelftestMenu::testOver(bool testOK, const QString &testResult)
{
    if (testOK)
    {
        //display center
        QFont font = fontManager.textFont(30);
        int fonth = fontManager.textHeightInPixels(font);
        int vPadding = (this->rect().height() - 30 - 30 - fonth) / 2;
        int hPadding = (this->rect().width() - fontManager.textWidthInPixels(testResult, font)) / 2;
        _info->clear();
        _info->setStyleSheet(QString("QTextEdit {background-color:white; padding-top:%1; "
                                     "padding-left:%2}").arg(vPadding).arg(hPadding));
        _info->setCurrentFont(font);
        _info->setTextColor(QColor(0, 0x60, 0));
        _info->setText(testResult);
        _ok->setVisible(true);
        _ok->setFocus();
        QTimer::singleShot(3000, this, SLOT(_yesReleased()));
        systemManager.systemSelftestOver();
    }
}

/**************************************************************************************************
 * hide event。
 *************************************************************************************************/
void SystemSelftestMenu::hideEvent(QHideEvent *e)
{
    PopupWidget::hideEvent(e);
}

/**************************************************************************************************
 * press enter。
 *************************************************************************************************/
void SystemSelftestMenu::_yesReleased()
{
    done(1);
    systemManager.systemSelftestOver();
}

/**************************************************************************************************
 * display failed info。
 *************************************************************************************************/
void SystemSelftestMenu::_displayFailedInfo()
{
    _info->clear();
    _info->setText(_testResult);
    _ok->setVisible(true);
    _ok->setFocus();
    systemManager.systemSelftestOver();
}
