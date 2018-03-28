#include "BLMSPO2Provider.h"
#include "SPO2Param.h"
#include "SPO2Alarm.h"
#include "SystemManager.h"
#include "Debug.h"
#include <QString>

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool BLMSPO2Provider::attachParam(Param &param)
{
    if (param.getParamID() == PARAM_SPO2)
    {
        spo2Param.setProvider(this);
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 读取数据。
 *************************************************************************************************/
void BLMSPO2Provider::dataArrived(void)
{
    readData();
    if (ringBuff.dataSize() < _packetLen)
    {
        return;
    }

    feed();
    spo2OneShotAlarm.setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, false);

    unsigned char value = 0;
    unsigned char buff[64];
    while (ringBuff.dataSize() >= _packetLen)
    {
        if ((ringBuff.at(0) == 0x55) && (ringBuff.at(1) == 0x02) && (ringBuff.at(2) == 0x05))
        {
            for (int i = 0; i < _packetLen; i++)
            {
                buff[i] = ringBuff.at(0);
                ringBuff.pop(1);
            }

            spo2Param.receivePackage();

            // 棒图。
            spo2Param.addBarData(buff[5] & 0x0F);

            // 脉搏音。
            spo2Param.setPulseAudio(buff[3] & 0x40);

            // 报警信息。
            // searching too long, no sensor or sensor off,
            // 探头脱落
            bool isCableOff = (buff[5] & 0x10) ? true : false;
            // 手指脱落
            bool isFingerOff = (buff[3] & 0x20) ? true : false;
            bool isTooLong = (buff[3] & 0x10) ? true : false;
            bool isSearching = (buff[5] & 0x20) ? true : false;
            // 低弱灌注
            bool isLowPerfusion = (buff[3] & 0x80) ? true : false;

            if (isCableOff)
            {
                spo2Param.setSensorOff(true);
            }
            else
            {
                spo2Param.setSensorOff(false);
            }

            bool isLowPerfusionFlag = false;
            if (isCableOff || isFingerOff)  // 存在报警则不显示searching for pulse。
            {
                spo2Param.setNotify(true,trs("SPO2CheckSensor"));
                spo2Param.setValidStatus(false);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, true);
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, false);

            }
            else
            {
                spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_CHECK_SENSOR, false);
                spo2Param.setNotify(false);
                spo2Param.setValidStatus(true);
                if (isTooLong || isSearching)
                {
                    spo2Param.setSearchForPulse(true);
                    spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, false);
                }
                else
                {
                    spo2Param.setSearchForPulse(false);
                    spo2Param.setOneShotAlarm(SPO2_ONESHOT_ALARM_LOW_PERFUSION, isLowPerfusion);
                    isLowPerfusionFlag = isLowPerfusion;
                }
            }

            if (isLowPerfusionFlag)
            {
                spo2Param.setSPO2(UnknownData());
                spo2Param.setPR(UnknownData());
            }
            else
            {
                // 血氧值。
                value = buff[7] & 0x7F;
                spo2Param.setSPO2((value == 127) ? InvData() : value);

                // 脉率值。
                value = buff[6] & 0x7F;
                value |= (buff[5] << 1) & 0x80;
                spo2Param.setPR((value == 255) ? InvData() : value);
            }
        }
        else
        {
//            debug("BLMSPO2Provider discard data = %d", (int)value);
            ringBuff.pop(1);
        }
    }
}

/**************************************************************************************************
 * 连接中断。
 *************************************************************************************************/
void BLMSPO2Provider::disconnected(void)
{
    spo2OneShotAlarm.clear();
    spo2OneShotAlarm.setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, true);
}

/**************************************************************************************************
 * 连接恢复。
 *************************************************************************************************/
void BLMSPO2Provider::reconnected(void)
{
    spo2OneShotAlarm.setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, false);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
BLMSPO2Provider::BLMSPO2Provider() : Provider("BLM_SPO2"), SPO2ProviderIFace()
{
    UartAttrDesc portAttr(19200, 8, 'N', 1);
    initPort(portAttr);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
BLMSPO2Provider::~BLMSPO2Provider()
{

}
