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
#include "PatientInfoWidgetInterface.h"
#include "IConfig.h"
#include "ConfigManagerInterface.h"
#include "ECGParamInterface.h"
#include "DataStorageDirManagerInterface.h"
#include "NIBPParamInterface.h"
#include "NIBPProviderIFace.h"
#include "AlarmIndicatorInterface.h"
#include "O2ParamInterface.h"
#include "SystemManagerInterface.h"
#include <QFile>
#include "ECGDupParamInterface.h"
#include "ECGParamInterface.h"
#include "Framework/Config/XmlParser.h"
#include <QDebug>
// #include "ECGDupParam.h"

#define XML_FILE_SUFFIX QString::fromLatin1(".xml")
#define PATIENT_INFO_PATH QString("/usr/local/nPM/etc")
#define PATIENT_INFO_FILENAME QString("/PatientInfo.xml")

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
    PatientInfoWidgetInterface *patientInfoWidget;

    void loadPatientInfo(PatientInfo &info);    /* NOLINT */
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
void PatientManager::setPatientInfoWidget(PatientInfoWidgetInterface &widget)
{
    d_ptr->patientInfoWidget = &widget;
    d_ptr->patientInfoWidget->loadPatientInfo(d_ptr->patientInfo, getBedNum());
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
    if (d_ptr->patientInfoWidget)
    {
        d_ptr->patientInfoWidget->loadPatientInfo(d_ptr->patientInfo, getBedNum());
    }

    // 报警限修改
    ConfigManagerInterface *config = ConfigManagerInterface::getConfigManager();
    if (config)
    {
        QString str = "AlarmSource";
        Config &currentConfig = config->getCurConfig();
        currentConfig.setNodeValue(str, currentConfig);
    }

    emit signalPatientType(d_ptr->patientInfo.type);

    ECGParamInterface *ecgParamInterface = ECGParamInterface::getECGParam();
    if (ecgParamInterface)
    {
        ecgParamInterface->setPatientType((unsigned char)(d_ptr->patientInfo.type));
    }
    SystemManagerInterface *systemManagerInterface = SystemManagerInterface::getSystemManager();
    if (systemManagerInterface && systemManagerInterface->isSupport(PARAM_NIBP))
    {
        NIBPParamInterface *nibpParam = NIBPParamInterface::getNIBPParam();
        if (nibpParam)
        {
            nibpParam->provider().setPatientType(type);
        }
    }
    if (systemManagerInterface && systemManagerInterface->isSupport(PARAM_ECG))
    {
        ECGDupParamInterface *ecgDupParam =  ECGDupParamInterface::getECGDupParam();
        if (ecgDupParam) {
            ecgDupParam->updateHRSource();
        }
        ECGParamInterface *ecgParam = ECGParamInterface::getECGParam();
        if (ecgParam) {
            ecgParam->updatePacermaker();    // 更新起博标志
        }
    }
    if (config)
    {
        config->loadConfig(type);
    }
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
void PatientManager::setPacermaker(PatientPacer pacer)
{
    d_ptr->patientInfo.pacer = pacer;
    systemConfig.setNumValue("PrimaryCfg|PatientInfo|PatientPacer", static_cast<int>(pacer));
}

/**************************************************************************************************
 * 功能： 获取起搏类型。
 *************************************************************************************************/
PatientPacer PatientManager::getPacermaker()
{
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
    systemConfig.setStrValue("PrimaryCfg|PatientInfo|Weight", QString::number(weight, 'f', 2));
}

float PatientManager::getWeight()
{
    return d_ptr->patientInfo.weight;
}

void PatientManager::setHeight(float height)
{
    d_ptr->patientInfo.height = height;
    systemConfig.setStrValue("PrimaryCfg|PatientInfo|Height", QString::number(height, 'f', 2));
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

    if (d_ptr->patientInfoWidget)
    {
        d_ptr->patientInfoWidget->loadPatientInfo(d_ptr->patientInfo, getBedNum());
    }
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
    if (d_ptr->patientInfoWidget)
    {
        d_ptr->patientInfoWidget->loadPatientInfo(d_ptr->patientInfo, bedNum);
    }
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

const PatientInfo &PatientManager::getHistoryPatientInfo(const QString &path)
{
    static PatientInfo *patientInfo = NULL;
    if (patientInfo == NULL)
    {
        patientInfo = new PatientInfo;
    }
    XmlParser xmlFile;
    QString str;
    if (!xmlFile.open(path + XML_FILE_SUFFIX))
    {
        qDebug() << "history patient info file open fail!";
        return *patientInfo;
    }
    // patient type
    xmlFile.getValue("PatientType", str);
    patientInfo->type = static_cast<PatientType>(str.toInt());
    str.clear();

    // pacermaker
    xmlFile.getValue("PacerMaker", str);
    patientInfo->pacer = static_cast<PatientPacer>(str.toInt());
    str.clear();

    // sex
    xmlFile.getValue("Sex", str);
    patientInfo->sex = static_cast<PatientSex>(str.toInt());
    str.clear();

    // bornDate
    xmlFile.getValue("BornDate", str);
    patientInfo->bornDate = QDate::fromString(str, "yyyy/MM/dd");
    str.clear();

    // blood
    xmlFile.getValue("Blood", str);
    patientInfo->blood = static_cast<PatientBloodType>(str.toInt());
    str.clear();

    // weight
    xmlFile.getValue("Weight", str);
    patientInfo->weight = str.toFloat();
    str.clear();

    // height
    xmlFile.getValue("Height", str);
    patientInfo->height = str.toFloat();
    str.clear();

    // id
    xmlFile.getValue("ID", str);
    ::strncpy(patientInfo->id, str.toUtf8().constData(), sizeof(patientInfo->id));
    str.clear();

    // name
    xmlFile.getValue("Name", str);
    ::strncpy(patientInfo->name, str.toUtf8().constData(), sizeof(patientInfo->name));
    str.clear();
    return *patientInfo;
}

UnitType PatientManager::getWeightUnit()
{
    int unit = UNIT_KG;
    systemConfig.getNumValue("Unit|WeightUnit", unit);
    if (unit != UNIT_KG && unit != UNIT_LB)
    {
        unit = UNIT_KG;
    }
    return (UnitType)unit;
}

UnitType PatientManager::getHeightUnit()
{
    int unit = UNIT_CM;
    systemConfig.getNumValue("Unit|HeightUnit", unit);
    if (unit != UNIT_CM && unit != UNIT_INCH)
    {
        unit = UNIT_CM;
    }
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
    patientManager.setPacermaker(PATIENT_PACER_OFF);
    DataStorageDirManagerInterface *dataStorageDirManager = DataStorageDirManagerInterface::getDataStorageDirManager();
    if (dataStorageDirManager)
    {
        dataStorageDirManager->createDir(true);
    }
    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
    if (alarmIndicator)
    {
        alarmIndicator->delAllPhyAlarm();        // 新建病人时，应清空上一个病人的生理报警
    }
    SystemManagerInterface *systemManagerInterface = SystemManagerInterface::getSystemManager();
    if (systemManagerInterface && systemManagerInterface->isSupport(PARAM_NIBP))
    {
        NIBPParamInterface *nibpParam = NIBPParamInterface::getNIBPParam();
        if (nibpParam)
        {
            nibpParam->clearResult();
            nibpParam->clearTrendListData();
            if (nibpParam->isMeasuring())
            {
                nibpParam->stopMeasure();
            }
        }
    }
    ECGParamInterface *ecgParam = ECGParamInterface::getECGParam();
    if (ecgParam)
    {
        ecgParam->setPacermaker(ECG_PACE_ON);
    }
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

void PatientManager::updatePatientInfo()
{
    XmlParser xmlFile;
    PatientInfo patientInfo = getPatientInfo();
    QString str;
    DataStorageDirManagerInterface *dataStorageDirManager = DataStorageDirManagerInterface::getDataStorageDirManager();
    if (!xmlFile.open(dataStorageDirManager->getCurFolder() + PATIENT_INFO_FILENAME))
    {
        qDebug() << "patient info file open fail!";
        return;
    }

    // patient type
    str = QString::number(patientInfo.type);
    xmlFile.setValue("PatientType", str);

    // pacermaker
    str = QString::number(patientInfo.pacer);
    xmlFile.setValue("PacerMaker", str);

    // sex
    str = QString::number(patientInfo.sex);
    xmlFile.setValue("Sex", str);

    // bornDate
    str = patientInfo.bornDate.toString("yyyy/MM/dd");
    xmlFile.setValue("BornDate", str);

    // blood
    str = QString::number(patientInfo.blood);
    xmlFile.setValue("Blood", str);

    // weight
    str = QString::number(patientInfo.weight, 'f', 1);
    xmlFile.setValue("Weight", str);

    // height
    str = QString::number(patientInfo.height, 'f', 1);
    xmlFile.setValue("Height", str);

    // id
    str = QString("%1").arg(patientInfo.id);
    xmlFile.setValue("ID", str);

    // name
    str = QString("%1").arg(patientInfo.name);
    xmlFile.setValue("Name", str);

    xmlFile.saveToFile();
}

void PatientManager::onNewPatientHandle()
{
    DataStorageDirManagerInterface *dataStorageDirManager = DataStorageDirManagerInterface::getDataStorageDirManager();
    QString fileName = QString(PATIENT_INFO_PATH + PATIENT_INFO_FILENAME);
    if (!dataStorageDirManager)
    {
        return;
    }
    QString newFileName = QString(dataStorageDirManager->getCurFolder() + PATIENT_INFO_FILENAME);
    // 如果文件已经创建不再拷贝新的模版
    if (!QFile::exists(newFileName))
    {
        if (!QFile::copy(fileName, newFileName))
        {
            qDebug() << "creat patient info file fail!";
        }
    }
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

    systemConfig.getNumValue("PrimaryCfg|PatientInfo|PatientPacer", numValue);
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
        DataStorageDirManagerInterface *dataStorageDirManager
                = DataStorageDirManagerInterface::getDataStorageDirManager();
        if (dataStorageDirManager)
        {
            dataStorageDirManager->cleanCurData();
        }
        SystemManagerInterface *systemManagerInterface = SystemManagerInterface::getSystemManager();
        if (systemManagerInterface && systemManagerInterface->isSupport(PARAM_NIBP))
        {
            NIBPParamInterface *nibpParam = NIBPParamInterface::getNIBPParam();
            if (nibpParam)
            {
                nibpParam->clearResult();
                nibpParam->clearTrendListData();
            }
        }
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PatientManager::PatientManager()
    : d_ptr(new PatientManagerPrivate(this))
{
    onNewPatientHandle();
    d_ptr->loadPatientInfo(d_ptr->patientInfo);
    DataStorageDirManagerInterface *dataStorageDirManager = DataStorageDirManagerInterface::getDataStorageDirManager();
    if (dataStorageDirManager)
    {
        connect(dataStorageDirManager, SIGNAL(newPatient()), this, SLOT(onNewPatientHandle()));
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/

PatientManager &PatientManager::getInstance()
{
    static PatientManager *instance = NULL;

    if (!instance)
    {
        instance = new PatientManager;
    }
    return *instance;
}

PatientManager::~PatientManager()
{
    delete d_ptr;
}
