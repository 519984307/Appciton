/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/12
 **/


#include "PatientManager.h"
#include "PatientInfoWidget.h"
#include "IConfig.h"
#include "ECGParam.h"
#include "DataStorageDirManager.h"

PatientManager *PatientManager::_selfObj = NULL;

class PatientManagerPrivate
{
public:
    PatientManagerPrivate()
        : patientInfoWidget(NULL),
          patientNew(false),
          relieveFlag(true)
    {}
    PatientInfo patientInfo;
    PatientInfoWidget *patientInfoWidget;

    void loadPatientInfo(PatientInfo &info);

    bool patientNew;                 // 新建病人标志
    bool relieveFlag;                // 解除病人标志
};

/**************************************************************************************************
 * 功能：设置窗体控件。
 * 参数：
 *      widget: 窗体控件。
 *************************************************************************************************/
void PatientManager::setPatientInfoWidget(PatientInfoWidget &widget)
{
    d_ptr->patientInfoWidget = &widget;
}

/**************************************************************************************************
 * 功能：设置病人类型。
 *************************************************************************************************/
void PatientManager::setType(PatientType type)
{
    PatientType oldType = d_ptr->patientInfo.type;

    d_ptr->patientInfo.type = type;
    systemConfig.setNumValue("General|PatientType", static_cast<int>(type));

    if (d_ptr->patientInfo.type == oldType)
    {
//        return;
    }

    // 病人类型被修改了，重新加载配置后，通知需要关注次事件的对象。
    d_ptr->patientInfoWidget->loadPatientInfo();

    // 报警限修改
    QString str = "AlarmSource|";
    str += PatientSymbol::convert(d_ptr->patientInfo.type);
    currentConfig.setNodeValue(str, currentConfig);

    emit signalPatientType(d_ptr->patientInfo.type);

    ecgParam.setPatientType((unsigned char)(d_ptr->patientInfo.type));
}

/**************************************************************************************************
 * 功能： 获取病人类型。
 *************************************************************************************************/
PatientType PatientManager::getType(void)
{
    return d_ptr->patientInfo.type;
}

/**************************************************************************************************
 * 功能： 获取病人类型。
 *************************************************************************************************/
QString PatientManager::getTypeStr(void)
{
    return PatientSymbol::convert(d_ptr->patientInfo.type);
}

/**************************************************************************************************
 * 功能： 设置起搏类型。
 *************************************************************************************************/
void PatientManager::setPacermaker(PatientPacer type)
{
    d_ptr->patientInfo.pacer = type;
//    currentConfig.setNumValue("General|PatientPacer", static_cast<int>(type));
    ecgParam.setPacermaker(static_cast<ECGPaceMode>(type));
}

/**************************************************************************************************
 * 功能： 获取起搏类型。
 *************************************************************************************************/
PatientPacer PatientManager::getPacermaker()
{
    d_ptr->patientInfo.pacer = static_cast<PatientPacer>(ecgParam.getPacermaker());
    return d_ptr->patientInfo.pacer;
}

/**************************************************************************************************
 * 功能： 设置性别。
 *************************************************************************************************/
void PatientManager::setSex(PatientSex sex)
{
    d_ptr->patientInfo.sex = sex;
    systemConfig.setNumValue("PrimaryCfg|PatientInfo|Sex", static_cast<int>(sex));
}

/**************************************************************************************************
 * 功能： 获取性别。
 *************************************************************************************************/
PatientSex PatientManager::getSex(void)
{
    return d_ptr->patientInfo.sex;
}

/**************************************************************************************************
 * 功能： 设置年龄。
 *************************************************************************************************/
void PatientManager::setAge(int age)
{
    d_ptr->patientInfo.age = age;
    systemConfig.setNumValue("PrimaryCfg|PatientInfo|Age", age);
}

/**************************************************************************************************
 * 功能： 获取年龄。
 *************************************************************************************************/
int PatientManager::getAge(void)
{
    return d_ptr->patientInfo.age;
}

void PatientManager::setBlood(PatientBloodType blood)
{
    d_ptr->patientInfo.blood = blood;
    systemConfig.setNumValue("PrimaryCfg|PatientInfo|Blood", static_cast<int>(blood));
}

int PatientManager::getBlood()
{
    return d_ptr->patientInfo.blood;
}

void PatientManager::setWeight(float weight)
{
    d_ptr->patientInfo.weight = weight;
    systemConfig.setStrValue("PrimaryCfg|PatientInfo|Weight", QString::number(weight, 'f', 1));
}

float PatientManager::getWeight()
{
    return d_ptr->patientInfo.weight;
}

void PatientManager::setHeight(float height)
{
    d_ptr->patientInfo.height = height;
    systemConfig.setStrValue("PrimaryCfg|PatientInfo|Height", QString::number(height, 'f', 1));
}

float PatientManager::getHeight()
{
    return d_ptr->patientInfo.height;
}

/**************************************************************************************************
 * 功能： 设置名字。
 *************************************************************************************************/
void PatientManager::setName(const QString &name)
{
    systemConfig.setStrValue("PrimaryCfg|PatientInfo|Name", name);
    ::strncpy(d_ptr->patientInfo.name, name.toUtf8().constData(),
              sizeof(d_ptr->patientInfo.name));

    d_ptr->patientInfoWidget->loadPatientInfo();
}

/**************************************************************************************************
 * 功能： 获取名字。
 *************************************************************************************************/
const char *PatientManager::getName(void)
{
    return d_ptr->patientInfo.name;
}

/**************************************************************************************************
 * 功能： 设置PID。
 *************************************************************************************************/
void PatientManager::setPatID(const QString &id)
{
    systemConfig.setStrValue("PrimaryCfg|PatientInfo|ID", id);
    ::strncpy(d_ptr->patientInfo.id, id.toUtf8().constData(),
              sizeof(d_ptr->patientInfo.id));
}

/**************************************************************************************************
 * 功能： 获取PID。
 *************************************************************************************************/
const char *PatientManager::getPatID()
{
    return d_ptr->patientInfo.id;
}

/**************************************************************************************************
 * 功能： 获取名字的长度。
 *************************************************************************************************/
int PatientManager::getNameLength(void)
{
    return sizeof(d_ptr->patientInfo.name)  - 1;
}

/**************************************************************************************************
 * 功能： 获取ID的长度。
 *************************************************************************************************/
int PatientManager::getIDLength(void)
{
    return sizeof(d_ptr->patientInfo.id)  - 1;
}

/**************************************************************************************************
 * 功能： 获取病人信息。
 *************************************************************************************************/
const PatientInfo &PatientManager::getPatientInfo(void)
{
    return d_ptr->patientInfo;
}

UnitType PatientManager::getWeightUnit()
{
    int unit = UNIT_KG;
    currentConfig.getNumValue("Local|WEIGHTUnit", unit);
    return (UnitType)unit;
}

UnitType PatientManager::getHeightUnit()
{
    int unit = UNIT_CM;
    currentConfig.getNumValue("Local|HEIGHTUnit", unit);
    return (UnitType)unit;
}

void PatientManager::setMonitor(bool status)
{
    d_ptr->relieveFlag = status;
}

bool PatientManager::isMonitoring()
{
    return d_ptr->relieveFlag;
}

void PatientManager::newPatient()
{
    d_ptr->patientNew = true;
    patientManager.setAge(-1);
    patientManager.setBlood(PATIENT_BLOOD_TYPE_NULL);
    patientManager.setHeight(0.0);
    patientManager.setName(QString(""));
    patientManager.setPatID(QString(""));
    patientManager.setSex(PATIENT_SEX_NULL);
    patientManager.setType(getType());
    patientManager.setWeight(0.0);
    patientManager.setPacermaker(PATIENT_PACER_ON);
    dataStorageDirManager.createDir(true);
}

void PatientManager::finishPatientInfo()
{
    d_ptr->patientNew = false;
    setMonitor(true);
}

bool PatientManager::isNewPatient()
{
    return d_ptr->patientNew;
}

/**************************************************************************************************
 * 载入配置信息。
 *************************************************************************************************/
void PatientManagerPrivate::loadPatientInfo(PatientInfo &info)
{
    int numValue = 0;
    QString strValue;

    systemConfig.getNumValue("General|PatientType", numValue);
    info.type = (PatientType)numValue;

    systemConfig.getNumValue("General|PatientPacer", numValue);
    info.pacer = (PatientPacer)numValue;

    systemConfig.getNumValue("PrimaryCfg|PatientInfo|Sex", numValue);
    info.sex = (PatientSex)numValue;

    systemConfig.getNumValue("PrimaryCfg|PatientInfo|Age", numValue);
    info.age = numValue;

    systemConfig.getNumValue("PrimaryCfg|PatientInfo|Blood", numValue);
    info.blood = (PatientBloodType)numValue;

    systemConfig.getStrValue("PrimaryCfg|PatientInfo|Weight", strValue);
    info.weight = strValue.toFloat();

    systemConfig.getStrValue("PrimaryCfg|PatientInfo|Height", strValue);
    info.height = strValue.toFloat();

    systemConfig.getStrValue("PrimaryCfg|PatientInfo|Name", strValue);
    ::strncpy(info.name, strValue.toUtf8().constData(), sizeof(info.name));

    systemConfig.getStrValue("PrimaryCfg|PatientInfo|ID", strValue);
    ::strncpy(info.id, strValue.toUtf8().constData(), sizeof(info.id));
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientManager::PatientManager()
    : d_ptr(new PatientManagerPrivate)
{
    d_ptr->loadPatientInfo(d_ptr->patientInfo);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientManager::~PatientManager()
{
    delete d_ptr;
}
