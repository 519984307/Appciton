#pragma once
#include <QObject>
#include <QVector>
#include <sys/time.h>

#define USE_VECTOR
typedef void (*SystemTickHook)(void);

class QTimer;
class SystemTick : public QObject
{
    Q_OBJECT

public:
    static SystemTick &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SystemTick();
        }
        return *_selfObj;
    }
    static SystemTick *_selfObj;

    // 添加一个入口。
    void addHook(int ms, int initCount, SystemTickHook hook);

    // 启动运行。
    void startTick(void);

    void resetLastTime(void);
    // 析构。
    virtual ~SystemTick();

private slots:
    void _timerOutSlot(void);

private:
    SystemTick();

private:
    // 管理回调。
    struct SystemTickCtrl
    {
        SystemTickCtrl (int ms, int initCount, SystemTickHook entry)
        {
            fixedMS = ms;
            curMS = initCount > ms ? ms : initCount;
            hook = entry;
        }

        SystemTickCtrl()
        {
            fixedMS = 0;
            curMS = 0;
            hook = NULL;
        }

        int fixedMS;
        int curMS;
        SystemTickHook hook;
    };

#ifdef USE_VECTOR
    QVector<SystemTickCtrl> _systemTickCtrl;
#else
    #define LEN 50
    SystemTickCtrl _systemTickCtrl[LEN];
#endif

    QTimer *_sysTimer;
    struct timeval _lastTime;
};

#define systemTick (SystemTick::construction())
#define deleteSystemTick() (delete SystemTick::_selfObj)
