#include "PatientManager.h"
#include "PatientBarWidget.h"
#include "PatientInfoWidget.h"
#include "IConfig.h"
#include "ECGParam.h"
#include "AlarmLimitMenu.h"

PatientManager *PatientManager::_selfObj = NULL;

/**************************************************************************************************
 * 功能：设置窗体控件。
 * 参数：
 *      widget: 窗体控件。
 *************************************************************************************************/
void PatientManager::setPatientTypeWidget(PatientBarWidget &widget)
{
    _patientTypeWidget = &widget;
    _patientTypeWidget->setPatientInfo(_patientInfo);
}

void PatientManager::setPatientInfoWidget(PatientInfoWidget &widget)
{
    _patientInfoWidget = &widget;
}

/**************************************************************************************************
 * 功能：设置病人类型。
 *************************************************************************************************/
void PatientManager::setType(PatientType type)
{
    PatientType oldType = _patientInfo.type;

    _patientInfo.type = type;
    systemConfig.setNumValue("General|PatientType", (int)type);

    if (_patientInfo.type == oldType)
    {
//        return;
    }

    // 病人类型被修改了，重新加载配置后，通知需要关注次事件的对象。
    _patientTypeWidget->setPatientType(_patientInfo.type);

    //报警限修改
    QString str = "AlarmSource|";
    str += PatientSymbol::convert(_patientInfo.type);
    currentConfig.setNodeValue(str, currentConfig);

    emit signalPatientType(_patientInfo.type);

    ecgParam.setPatientType((unsigned char)(_patientInfo.type));

    alarmLimitMenu.checkAlarmEnableStatus();
}

/**************************************************************************************************
 * 功能： 获取病人类型。
 *************************************************************************************************/
PatientType PatientManager::getType(void)
{
    return _patientInfo.type;
}

/**************************************************************************************************
 * 功能： 获取病人类型。
 *************************************************************************************************/
QString PatientManager::getTypeStr(void)
{
    return PatientSymbol::convert(_patientInfo.type);
}

/**************************************************************************************************
 * 功能： 设置起搏类型。
 *************************************************************************************************/
void PatientManager::setPacermaker(PatientPacer type)
{
    _patientInfo.pacer = type;
    currentConfig.setNumValue("General|PatientPacer", (int)type);
//    ecgParam.setPacermaker(type);
}

/**************************************************************************************************
 * 功能： 获取起搏类型。
 *************************************************************************************************/
PatientPacer PatientManager::getPacermaker()
{
    return _patientInfo.pacer;
}

/**************************************************************************************************
 * 功能： 设置性别。
 *************************************************************************************************/
void PatientManager::setSex(PatientSex sex)
{
    _patientInfo.sex = sex;
    systemConfig.setNumValue("PrimaryCfg|PatientInfo|Sex", (int)sex);
}

/**************************************************************************************************
 * 功能： 获取性别。
 *************************************************************************************************/
PatientSex PatientManager::getSex(void)
{
     return _patientInfo.sex;
}

/**************************************************************************************************
 * 功能： 设置年龄。
 *************************************************************************************************/
void PatientManager::setAge(int age)
{
    _patientInfo.age = age;
    systemConfig.setNumValue("PrimaryCfg|PatientInfo|Age", age);
}

/**************************************************************************************************
 * 功能： 获取年龄。
 *************************************************************************************************/
int PatientManager::getAge(void)
{
    return _patientInfo.age;
}

void PatientManager::setBlood(PatientBloodType blood)
{
    _patientInfo.blood = blood;
    systemConfig.setNumValue("PrimaryCfg|PatientInfo|Blood", (int)blood);
}

int PatientManager::getBlood()
{
    return _patientInfo.blood;
}

void PatientManager::setWeight(int weight)
{
    _patientInfo.weight = weight;
//    QString prefix = "Patient|Weight|";
//    prefix += Unit::localeSymbol(getWeightUnit());
//    currentConfig.setStrValue(prefix + "|Value", weight);

    systemConfig.setNumValue("PrimaryCfg|PatientInfo|Weight", weight);
}

int PatientManager::getWeight()
{
    return _patientInfo.weight;
}

void PatientManager::setHeight(short height)
{
    _patientInfo.height = height;
//    QString prefix = "Patient|Height|";
//    prefix += Unit::localeSymbol(getHeightUnit());
//    currentConfig.setStrValue(prefix + "|Value", height);

    systemConfig.setNumValue("PrimaryCfg|PatientInfo|Height", height);
}

short PatientManager::getHeight()
{
    return _patientInfo.height;
}

/**************************************************************************************************
 * 功能： 设置名字。
 *************************************************************************************************/
void PatientManager::setName(const QString &name)
{
    systemConfig.setStrValue("PrimaryCfg|PatientInfo|Name", name);
    ::strncpy(_patientInfo.name, name.toUtf8().constData(), sizeof(_patientInfo.name));

    _patientInfoWidget->loadPatientInfo();
}

/**************************************************************************************************
 * 功能： 获取名字。
 *************************************************************************************************/
const char *PatientManager::getName(void)
{
    return _patientInfo.name;
}

/**************************************************************************************************
 * 功能： 设置PID。
 *************************************************************************************************/
void PatientManager::setPatID(const QString &id)
{
    systemConfig.setStrValue("PrimaryCfg|PatientInfo|ID", id);
    ::strncpy(_patientInfo.id, id.toUtf8().constData(), sizeof(_patientInfo.id));
}

/**************************************************************************************************
 * 功能： 获取PID。
 *************************************************************************************************/
const char *PatientManager::getPatID()
{
    return _patientInfo.id;
}

/**************************************************************************************************
 * 功能： 获取名字的长度。
 *************************************************************************************************/
int PatientManager::getNameLength(void)
{
    return sizeof(_patientInfo.name)  - 1;
}

/**************************************************************************************************
 * 功能： 获取ID的长度。
 *************************************************************************************************/
int PatientManager::getIDLength(void)
{
    return sizeof(_patientInfo.id)  - 1;
}

/**************************************************************************************************
 * 功能： 获取病人信息。
 *************************************************************************************************/
const PatientInfo &PatientManager::getPatientInfo(void)
{
    return _patientInfo;
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

/**************************************************************************************************
 * 载入配置信息。
 *************************************************************************************************/
void PatientManager::_loadPatientInfo(PatientInfo &info)
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

//    QString prefix = "Patient|Weight|";
//    prefix += Unit::localeSymbol(getWeightUnit());
//    currentConfig.getStrValue(prefix + "|Value", strValue);
    systemConfig.getNumValue("PrimaryCfg|PatientInfo|Weight", numValue);
    info.weight = numValue;

//    prefix = "Patient|Height|";
//    prefix += Unit::localeSymbol(getHeightUnit());
//    currentConfig.getStrValue(prefix + "|Value", strValue);
    systemConfig.getNumValue("PrimaryCfg|PatientInfo|Height", numValue);
    info.height = numValue;

    systemConfig.getStrValue("PrimaryCfg|PatientInfo|Name", strValue);
    ::strncpy(info.name, strValue.toUtf8().constData(), sizeof(info.name));

    systemConfig.getStrValue("PrimaryCfg|PatientInfo|ID", strValue);
    ::strncpy(info.id, strValue.toUtf8().constData(), sizeof(info.id));
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientManager::PatientManager()
{
    _loadPatientInfo(_patientInfo);
    _patientTypeWidget = NULL;
    _patientInfoWidget = NULL;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PatientManager::~PatientManager()
{

}
