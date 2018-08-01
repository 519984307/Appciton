/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/26
 **/



#pragma once
#include <string.h>
#include "BaseDefine.h"
#include <QString>

enum PatientType
{
    PATIENT_TYPE_ADULT,
    PATIENT_TYPE_PED,
    PATIENT_TYPE_NEO,
    PATIENT_TYPE_NULL,
    PATIENT_TYPE_NR
};

enum PatientSex
{
    PATIENT_SEX_NULL,                 // 性别为空主要用于开机时性别框要显示为空白
    PATIENT_SEX_MALE,
    PATIENT_SEX_FEMALE,
    PATIENT_SEX_NR
};

enum PatientPacer
{
    PATIENT_PACER_OFF,
    PATIENT_PACER_ON,
    PATIENT_PACER_NR
};

enum PatientBloodType
{
    PATIENT_BLOOD_TYPE_NULL,
    PATIENT_BLOOD_TYPE_A,
    PATIENT_BLOOD_TYPE_B,
    PATIENT_BLOOD_TYPE_AB,
    PATIENT_BLOOD_TYPE_O,
    PATIENT_BLOOD_TYPE_NR
};

enum PatientWeightUnit
{
    PATIENT_WEIGHT_UNIT_KG,
    PATIENT_WEIGHT_UNIT_LB,
    PATIENT_WEIGHT_UNIT_NR
};

// 描述病人信息。
struct PatientInfo
{
    PatientInfo()
    {
        pacer = PATIENT_PACER_ON;
        type = PATIENT_TYPE_ADULT;
        sex = PATIENT_SEX_NULL;      //  性别为空主要用于开机时性别框要显示为空白
        age = -1;                     //  年纪设置为-1主要用于开机时年纪框不让设置数据要显示为空白
        blood = PATIENT_BLOOD_TYPE_NULL;
        weight = 0;
        height  = 0;
        weightUnit = PATIENT_WEIGHT_UNIT_KG;
        memset(name, 0, sizeof(name));
        memset(id, 0, sizeof(id));
    }

    PatientInfo &operator=(const PatientInfo &other)
    {
        pacer = other.pacer;
        type = other.type;
        sex = other.sex;
        age = other.age;
        blood = other.blood;
        weight = other.weight;
        height = other.height;
        weightUnit = other.weightUnit;
        ::memcpy(name, other.name, sizeof(name));
        ::memcpy(id, other.id, sizeof(id));
        return *this;
    }

    bool operator==(const PatientInfo &other)
    {
        return ((pacer == other.pacer) &&
                (type == other.type) &&
                (sex == other.sex) &&
                (age == other.age) &&
                (blood == other.blood) &&
                (weight == other.weight) &&
                (height == other.height) &&
                (::strcmp(name, other.name) == 0) &&
                (::strcmp(id, other.id) == 0));
    }

    PatientType type;
    PatientPacer pacer;
    PatientSex sex;
    int age;
    PatientBloodType blood;
    PatientWeightUnit weightUnit;
    int weight;
    int height;
    char id[MAX_PATIENT_ID_LEN];
    char name[MAX_PATIENT_NAME_LEN];
};

class PatientSymbol
{
public:
    static const char *convert(PatientType index)
    {
        static const char *symbol[PATIENT_TYPE_NR] =
        {
            "Adult", "PED", "NEO", "NULL"
        };
        return symbol[index];
    }

    static const char *convert(PatientSex index)
    {
        static const char *symbol[PATIENT_SEX_NR] =
        {
            "", "Male", "Female"
        };
        return symbol[index];
    }

    static const char *convert(PatientPacer index)
    {
        static const char *symbol[PATIENT_PACER_NR] =
        {
            "Off", "On"
        };
        return symbol[index];
    }

    static const char *convert(PatientBloodType index)
    {
        static const char *symbol[PATIENT_BLOOD_TYPE_NR] =
        {
            "", "A", "B", "AB", "O"
        };
        return symbol[index];
    }

    static const char *convert(PatientWeightUnit index)
    {
        static const char *symbol[PATIENT_WEIGHT_UNIT_NR] =
        {
            "kg", "lb"
        };
        return symbol[index];
    }
};
