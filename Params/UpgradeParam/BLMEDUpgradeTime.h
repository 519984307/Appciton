#pragma once
#include <QObject>

// 升级过程中，应答的返回时间
class QTimer;
class BLMEDUpgradeTime : public QObject
{
    Q_OBJECT

public:
    static BLMEDUpgradeTime &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new BLMEDUpgradeTime();
        }
        return *_selfObj;
    }
    static BLMEDUpgradeTime *_selfObj;
    ~BLMEDUpgradeTime();

public:
    void startEnter(int sec);
    void startFile(int sec);
    void startSend(int sec);
    void stopEnter(void);
    void stopFile(void);
    void stopSend(void);
    bool isTimeout(void) const;

private slots:
    void _timeoutEnter(void);
    void _timeoutFile(void);
    void _timeoutSend(void);

private:
    BLMEDUpgradeTime();
    QTimer *_timerEnter;
    QTimer *_timerFile;
    QTimer *_timerSend;
    bool _isTimeout;
};
#define blmedUpgradeTime (BLMEDUpgradeTime::construction())
#define deleteblmedupgradetime() (delete BLMEDUpgradeTime::_selfObj)
