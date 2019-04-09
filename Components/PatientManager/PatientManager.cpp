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
#include "ConfigManager.h"
#include "ECGParam.h"
#include "DataStorageDirManager.h"
#include "NIBPParam.h"
#include "NIBPProviderIFace.h"
#include "TimeDate.h"
#include "AlarmIndicator.h"
#include "RunningStatusBar.h"
#include "O2ParamInterface.h"

PatientManager *PatientManager::_selfObj = NULL;

class PatientManagerPrivate
{
public:
    explicit PatientManagerPrivate(PatientManager *const q_ptr)
        : q_ptr(q_ptr),
          patientInfoWidget(NULL),
          patientNew(false),
          relieveFlag(true),
          bornDateFormat(QString())
    {}
    PatientManager * const q_ptr;
    PatientInfo patientInfo;
    PatientInfoWidget *patientInfoWidget;

    void loadPatientInfo(PatientInfo &info);
    /**
     * @brief handleDischarge 解除病人后，刷新标志状态
     */
    void handleDischarge();

    bool patientNew;                 // 新建病人标志
    bool relieveFlag;                // 解除病人标志
    QString bornDateFormat;          // 出生日期格式
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
    if (type == d_ptr->patientInfo.type)
    {
        return;
    }

#ifdef ENABLE_O2_APNEASTIMULATION
    // 窒息唤醒功能状态
    O2ParamInterface *o2Param = O2ParamInterface::getO2ParamInterface();
    if (o2Param)
    {
        bool sta = false;
        systemConfig.getNumValue("PrimaryCfg|O2|ApneaAwake", sta);
        if (type == PATIENT_TYPE_NEO && sta)
        {
            o2Param->updateApneaStimulationStatus();
        }
        else
        {
            o2Param->setVibration(false);
            runningStatus.setShakeStatus(SHAKE_OFF);
        }
    }
#endif

    d_ptr->patientInfo.type = type;
    systemConfig.setNumValue("General|PatientType", static_cast<int>(type));

    // 病人类型被修改了，重新加载配置后，通知需要关注次事件的对象。
    d_ptr->patientInfoWidget->loadPatientInfo();

    // 报警限修改
    QString str = "AlarmSource";
    currentConfig.setNodeValue(str, currentConfig);

    emit signalPatientType(d_ptr->patientInfo.type);

    ecgParam.setPatientType((unsigned char)(d_ptr->patientInfo.type));
    if (systemManager.isSupport(PARAM_NIBP))
    {
        nibpParam.provider().setPatientType(type);
    }
    configManager.loadConfig(type);
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

void PatientManager::setBornDate(QDate bornDate)
{
    systemConfig.setStrValue("PrimaryCfg|PatientInfo|BornDate", bornDate.toString("yyyy/MM/dd"));
    d_ptr->patientInfo.bornDate = bornDate;
}

QDate PatientManager::getBornDate()
{
    return d_ptr->patientInfo.bornDate;
}

void PatientManager::getBornDate(unsigned int &year, unsigned int &month, unsigned int &day)
{
    QDate bornDate = d_ptr->patientInfo.bornDate;
    if (!bornDate.isValid())
    {
        return;
    }
    year = static_cast<unsigned int>(bornDate.year());
    month = static_cast<unsigned int>(bornDate.month());
    day = static_cast<unsigned int>(bornDate.day());
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

void PatientManager::setBedNum(const QString &bedNum)
{
    systemConfig.setStrValue("General|BedNumber", bedNum);
    d_ptr->patientInfoWidget->loadPatientInfo();
}

const QString PatientManager::getBedNum()
{
    QString bedNumStr;
    systemConfig.getStrValue("General|BedNumber", bedNumStr);
    return bedNumStr;
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
    systemConfig.getNumValue("Unit|WeightUnit", unit);
    return (UnitType)unit;
}

UnitType PatientManager::getHeightUnit()
{
    int unit = UNIT_CM;
    systemConfig.getNumValue("Unit|HeightUnit", unit);
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
    patientManager.setBornDate(QDate());
    patientManager.setBlood(PATIENT_BLOOD_TYPE_NULL);
    patientManager.setHeight(0.0);
    patientManager.setName(QString(""));
    patientManager.setPatID(QString(""));
    patientManager.setSex(PATIENT_SEX_NULL);
    patientManager.setType(getType());
    patientManager.setWeight(0.0);
    patientManager.setPacermaker(PATIENT_PACER_ON);
    dataStorageDirManager.createDir(true);
    alarmIndicator.delAllPhyAlarm();        // 新建病人时，应清空上一个病人的生理报警
}

void PatientManager::dischargePatient()
{
    d_ptr->handleDischarge();
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

    systemConfig.getStrValue("PrimaryCfg|PatientInfo|BornDate", strValue);
    info.bornDate = QDate::fromString(strValue, "yyyy/MM/dd");
}

void PatientManagerPrivate::handleDischarge()
{
    if (relieveFlag)
    {
        relieveFlag = false;
        q_ptr->newPatient();
    }
    else
    {
        dataStorageDirManager.cleanCurData();
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientManager::PatientManager()
    : d_ptr(new PatientManagerPrivate(this))
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
