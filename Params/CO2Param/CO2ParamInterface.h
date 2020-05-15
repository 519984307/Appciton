/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/12/9
 **/

#pragma once
#include "CO2ProviderIFace.h"

class Co2ParamInterface
{
public:
    ~Co2ParamInterface() {}

    /**
     * @brief setEtCO2 set the EtCo2 value
     * @param etCo2
     */
    virtual void setEtCO2(short etCo2) = 0;

    /**
     * @brief setFiCO2 set the FiCo2 value
     * @param fiCo2
     */
    virtual void setFiCO2(short fiCo2) = 0;

    /**
     * @brief setBR set the br value
     * @param br
     */
    virtual void setBR(short br) = 0;

    /**
     * @brief addWaveformData add co2 wave data
     * @param wave wave value
     * @param invalid
     */
    virtual void addWaveformData(short wave, bool invalid) = 0;

    /**
     * @brief getCo2ModuleType get the attach provider type
     * @return the provider type
     */
    virtual CO2ModuleType getCo2ModuleType() const = 0;
};
