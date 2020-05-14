/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/12/10
 **/

#pragma once

#include "ParamDefine.h"
#include <QByteArray>

/**
 * @brief The ParamPreProcessor class
 * @note use to PrePrecess data between provider and param
 */
class ParamPreProcessor
{
public:
    virtual ~ParamPreProcessor(){}

    /**
     * @brief preProcessData pre process the data
     * @param paramID param id
     * @param subParamID sub param id
     * @param value param vlaue
     * @return the value after preprocess
     */
    virtual int preProcessData(ParamID paramID, SubParamID subParamID, int value) = 0;

    /**
     * @brief preProcessData pre process data
     * @param paramID param id
     * @param name name of change value
     * @param value change value
     * @return
     */
    virtual int preProcessData(ParamID paramID, const QString &name, int value) = 0;

    /**
     * @brief PreProcessWave pre prcess the wave data
     * @param waveID the wave id
     * @param wave the wave value
     * @param flag the wave flag
     */
    virtual void preProcessWave(WaveformID waveID, int *wave, int *flag) = 0;
};
