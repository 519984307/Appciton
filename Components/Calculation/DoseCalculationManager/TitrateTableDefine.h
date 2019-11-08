/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/8
 **/
#pragma once

enum StepOption
{
    STEP_OPTION_ONE,
    STEP_OPTION_TWO,
    STEP_OPTION_THREE,
    STEP_OPTION_FOUR,
    STEP_OPTION_FIVE,
    STEP_OPTION_SIX,
    STEP_OPTION_SEVEN,
    STEP_OPTION_EIGHT,
    STEP_OPTION_NINE,
    STEP_OPTION_TEN,
    STEP_OPTION_NR,
};

enum DatumTerm
{
    DATUM_TERM_DOSE,
    DATUM_TERM_INFUSIONRATE,
    DATUM_TERM_DRIPRATE,
    DATUM_TERM_NR
};

enum DoseType
{
    DOSE_TYPE_MIN,
    DOSE_TYPE_H,
    DOSE_TYPE_KGMIN,
    DOSE_TYPE_KGH,
    DOSE_TYPE_NR
};

struct SetTableParam
{
    SetTableParam() :step(STEP_OPTION_ONE), datumTerm(DATUM_TERM_DOSE), doseType(DOSE_TYPE_H) {}
    StepOption step;
    DatumTerm datumTerm;
    DoseType doseType;
};
