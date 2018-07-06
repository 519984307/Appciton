#include "NellcorProvider.h"
#include "SPO2Param.h"
#include "SPO2Alarm.h"
#include "Debug.h"

class NellcorSetProviderPrivate
{
public:
#if 0
    union DataField{
        unsigned char dataField[31];
        struct{
            unsigned char key; //密钥
            unsigned char len; //实际长度 0-29
            unsigned char value[31-2]; //接收值
        }s;
    };

    union DataPackField{
        unsigned char dataPackField[sizeof(DataField)+5];
        struct{
            unsigned char fixedHead; // 固定头 0x55
            unsigned char stx; //固定字符 0x02
            unsigned char size; //实际数据包长度 0-31
            DataField data; //数据包
            unsigned char validityCheck; //数据包有效性检查 crc校验
            unsigned char etx; //固定尾 0x03
        }s;
    };
#endif
    union OxismartReport{
        unsigned char oxismartReport[8];
        struct{
            unsigned char key; // 密钥 默认为'j'
            unsigned char len; //报告长度 默认为6
            unsigned char satDispType; //显示行为信息
            unsigned char spo2Value;  //血氧值
            unsigned char rateDispType:7;  //显示脉率信息
            unsigned char rateBpmValueNinth:1; //脉率值的第9位
            unsigned char rateBpmValueTopEight;  //脉率值的前8位
            unsigned short statusAndAlarmWord;  //状态值和报警值
        }s;
    };

    static const int _minPacketLen = 14;      // 最小数据包长度。

    bool _isLowPerfusionFlag;                     // 低弱冠注

};

#define SOM             (0x02)
#define EOM             (0x03)

/**************************************************************************************************
 * 模块与参数对接。
 *************************************************************************************************/
bool NellcorSetProvider::attachParam(Param &param)
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
void NellcorSetProvider::dataArrived(void)
{
    readData();
    if (ringBuff.dataSize() < _minPacketLen)
    {
        return;
    }

    unsigned char buff[64];
    while (ringBuff.dataSize() >= _minPacketLen)
    {
        if (ringBuff.at(0) != SOM)
        {
            //debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        if (ringBuff.at(13) != EOM)
        {
            //debug("discard (%s:%d)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        // 将数据包读到buff中。
        for (int i = 0; i < _minPacketLen; i++)
        {
            buff[i] = ringBuff.at(0);
            ringBuff.pop(1);
        }

        unsigned char sum = _calcCheckSum(&buff[1],_minPacketLen-3);
        if (buff[_minPacketLen-2] == sum)
        {
            handlePacket(&buff[1], _minPacketLen - 3);
        }
        else
        {
            outHex(buff, _minPacketLen);
            debug("0x%02x",sum);
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

/**************************************************************************************************
 * send to code.
 *************************************************************************************************/
void NellcorSetProvider::_sendCmd(const unsigned char *data, unsigned int len)
{
    int index = 0;
    unsigned char sendBuf[15] = {0};

    sendBuf[index++] = SOM;
    for (unsigned int i = 0; i < len; i++)
    {
        sendBuf[index++] = data[i];
    }

    sendBuf[index++] = _calcCheckSum(data,len);
    sendBuf[index++] = EOM;

    writeData(sendBuf, len + 3);
}

/**************************************************************************************************
 * calculate check sum.
 *************************************************************************************************/
unsigned char NellcorSetProvider::_calcCheckSum(const unsigned char *data, int len)
{
    unsigned char sum = 0;
    for (int i = 0; i < len; i++)
    {
        sum = sum + data[i];
    }
    sum = sum & 0xFF;
    return sum;
}

void NellcorSetProvider::sendVersion()
{
    sendCmd(0, 0, 0);
}

bool NellcorSetProvider::sendCmd(unsigned char cmdKey, const unsigned char *data, unsigned int len)
{

}

/**************************************************************************************************
 * 设置灵敏度和FastSat。
 *************************************************************************************************/
void NellcorSetProvider::setSensitivityFastSat(SensitivityMode mode, bool fastSat)
{
    unsigned char cmd[7] = {0};
    cmd[0] = 0x03;
    cmd[1] = 0x00;
    cmd[2] = 0x00;
    // bit0 for SensitivityMode, 1-normal, 0-max
    if (mode == (int)SPO2_MASIMO_SENS_NORMAL)
    {
        cmd[2] |= 0x01;
    }

    // bit1 for Fast Sat, 1-on, 0-off.
    if (fastSat)
    {
        cmd[2] |= 0x02;
    }

    // bit2 for APOD, 1-on, 0-ignore.
    if (mode == SPO2_MASIMO_SENS_APOD)
    {
        cmd[2] |= 0x04;
    }
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = 0x00;
    cmd[6] = 0x00;

    _sendCmd(cmd, 7);
}

/**************************************************************************************************
 * 设置平均时间。
 *************************************************************************************************/
void NellcorSetProvider::setAverageTime(AverageTime mode)
{
    unsigned char cmd[7] = {0};
    cmd[0] = 0x02;
    cmd[1] = (unsigned char)mode;
    cmd[2] = 0x00;  // D2
    cmd[3] = 0x00;  // D3
    cmd[4] = 0x00;  // D4
    cmd[5] = 0x00;  // D5
    cmd[6] = 0x00;  // D6

    _sendCmd(cmd, 7);
}

/**************************************************************************************************
 * 设置SMart Tone使能选项。
 *************************************************************************************************/
void NellcorSetProvider::setSmartTone(bool enable)
{
    unsigned char cmd[7] = {0};
    cmd[0] = 0x01;
    cmd[1] = 0x18;
    cmd[2] = 0x38;
    if (enable)
    {
        cmd[3] = 0x50;
    }
    else
    {
        cmd[3] = 0x40;
    }
    cmd[4] = 0x00;
    cmd[5] = 0x00;
    cmd[6] = 0x00;

    _sendCmd(cmd, 7);
}

/**************************************************************************************************
 * 功能：构造函数
 * 参数：
 *
 *************************************************************************************************/
NellcorSetProvider::NellcorSetProvider() : Provider("NUCELL_SPO2"), SPO2ProviderIFace()
{
    UartAttrDesc portAttr(9600, 8, 'N', 1);
    initPort(portAttr);

    _isLowPerfusionFlag = false;
}

/**************************************************************************************************
 * 功能：析构函数
 * 参数：
 *
 *************************************************************************************************/
NellcorSetProvider::~NellcorSetProvider()
{

}

/**************************************************************************************************
 * 连接中断。
 *************************************************************************************************/
void NellcorSetProvider::disconnected()
{
    spo2OneShotAlarm.clear();
    spo2OneShotAlarm.setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, true);
}

/**************************************************************************************************
 * 连接恢复。
 *************************************************************************************************/
void NellcorSetProvider::reconnected()
{
    spo2OneShotAlarm.setOneShotAlarm(SPO2_ONESHOT_ALARM_COMMUNICATION_STOP, false);
}

void NellcorSetProvider::handlePacket(unsigned char *data, int /*len*/)
{
    feed();
    short temp;
    switch (data[0])
    {
    case 'j':  //oxismart 报告 包含spo2、pr、状态等相关信息
        NellcorSetProviderPrivate::OxismartReport *oxismartInfo = (NellcorSetProviderPrivate::OxismartReport *)data;
        temp = oxismartInfo->s.key;
        if(temp){}
        temp = oxismartInfo->s.len;if(temp){}
        temp = oxismartInfo->s.rateBpmValueNinth;if(temp){}
        temp = oxismartInfo->s.rateBpmValueTopEight;if(temp){}
        temp = oxismartInfo->s.rateDispType;if(temp){}
        temp = oxismartInfo->s.satDispType;if(temp){}
        temp = oxismartInfo->s.statusAndAlarmWord;if(temp){}
        temp = oxismartInfo->s.spo2Value;if(temp){}
        spo2Param.setSPO2(temp);
        break;
    }

    // 接收PLE波形。
    char pleWaveformValue = 0;
    pleWaveformValue = data[3];
//    spo2Param.addWaveformData(100 - (unsigned char)(pleWaveformValue + 128) / 3);
    spo2Param.addWaveformData(128 - pleWaveformValue);

    // 棒图，使用PLETH为原始数据。
    unsigned char barData;
    barData = data[3];
    spo2Param.addBarData(barData / 15);

    // BEEP音。
    if ((data[4] != 0) && !(data[4] & 0x80))
    {
        spo2Param.setPulseAudio(true);
    }

    // 接收SIQ波形。
//    unsigned char siqWaveformValue = 0;
//    siqWaveformValue = data[5];
//    spo2Param.updataSiqWaveformValue((unsigned char)siqWaveformValue);
}
