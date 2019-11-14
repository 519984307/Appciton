/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/12/14
 **/

#pragma once
#include "Provider.h"
#include "SPO2ProviderIFace.h"

class RainbowProviderPrivate;
class RainbowProvider : public Provider, public SPO2ProviderIFace
{
    Q_OBJECT

    friend class RainbowProviderPrivate;
public:
    explicit RainbowProvider(const QString &name, bool isPlugIn = false);
    ~RainbowProvider();

    /* reimplment */
    bool attachParam(Param *param);

    /* reimplment */
    void dataArrived();

    /* reimplment */
    void dataArrived(unsigned char *data, unsigned int length);

    /* reimplment */
    void dispatchPortHasReset();

    /* reimplment */
    void setSensitive(SPO2Sensitive /*sens*/) { }

    /* reimplment */
    // 实际为62.5, 62.5 * 2 = 125  波形数据增加2次
    int getSPO2WaveformSample() {return 125;}

    /* reimplment */
    int getSPO2BaseLine();

    /* reimplment */
    int getSPO2MaxValue();

    /* reimplment */
    void sendVersion();

    /* reimplment */
    void setSensitivityFastSat(SensitivityMode mode, bool fastSat);

    /* reimplment */
    void setAverageTime(AverageTime mode);

    /* reimplment */
    void setSmartTone(bool enable);

    /* reimplment */
    void disconnected();

    /* reimplment */
    void reconnected();

    /* reimplment */
    void initModule();

    /* reimplment */
    void setSpHbPrecisionMode(SpHbPrecisionMode mode);

    /* reimplment */
    void setPVIAveragingMode(AveragingMode mode);

    /* reimplment */
    void setSpHbBloodVesselMode(SpHbBloodVesselMode mode);

    /* reimplment */
    void setSphbAveragingMode(SpHbAveragingMode mode);

private slots:
    /**
     * @brief requestBoardInfo send get board info command
     */
    void requestBoardInfo();

    /**
     * @brief changeBaudrate change the baudrate
     */
    void changeBaudrate();

public:
    /**
     * @brief setLineFrequency
     * @param freq
     */
    void setLineFrequency(RainbowLineFrequency freq);

private:
    RainbowProviderPrivate *const d_ptr;
};
