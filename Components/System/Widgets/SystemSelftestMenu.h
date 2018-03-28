#pragma once
#include "PopupWidget.h"
#include "SystemManager.h"
#include <QMap>

class IButton;
class QTextEdit;
class SystemSelftestMenu : public PopupWidget
{
    Q_OBJECT

public:
    SystemSelftestMenu();
    ~SystemSelftestMenu() {}

    // 追加信息
    void appendInfo(ModulePoweronTestResult module, ModulePoweronTestStatus status, const QString &info);

    // 测试结束
    void testOver(bool testOK, const QString &testResult);

protected:
    void hideEvent(QHideEvent *e);

private slots:
    void _yesReleased();
    void _displayFailedInfo();

private:
    IButton *_ok;
    QTextEdit *_info;
    QString _testResult;
    QMap<ModulePoweronTestResult, ModulePoweronTestStatus> _moduleTestStatus;
};
