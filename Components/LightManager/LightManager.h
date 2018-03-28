#pragma once
#include "AlarmDefine.h"

class LightProviderIFace;
class LightManager
{
public:
    static LightManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new LightManager();
        }
        return *_selfObj;
    }
    static LightManager *_selfObj;
    ~LightManager();

public:
    void setProvider(LightProviderIFace *iface);
    void updateAlarm(bool hasAlarm, AlarmPriority priority);
    void enableAlarmAudioMute(bool enable);

    // 开启/关闭除颤准备灯
    void enableDefibReadyLED(bool enable);

    // 发送协议命令
    void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

private:
    LightManager();
    LightProviderIFace *_provider;
};
#define lightManager (LightManager::construction())
#define deleteLightManager() (delete LightManager::_selfObj)
