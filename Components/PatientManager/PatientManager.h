#pragma once
#include "PatientDefine.h"
#include <QVector>
#include <QObject>
#include "UnitManager.h"

// 病人管理对象。
class PatientBarWidget;
class PatientInfoWidget;
class PatientManager : public QObject
{
    Q_OBJECT

public:
    static PatientManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PatientManager();
        }
        return *_selfObj;
    }
    static PatientManager *_selfObj;
    ~PatientManager();

public:
    // 设置Widget。
    void setPatientTypeWidget(PatientBarWidget &widget);
    void setPatientInfoWidget(PatientInfoWidget &widget);

public:
    // 设置/获取病人类型。
    void setType(PatientType type);
    PatientType getType(void);
    QString getTypeStr(void);

    // 设置/获取起搏。
    void setPacermaker(PatientPacer type);
    PatientPacer getPacermaker(void);

    // 设置/获取窒息时间。
    void setSex(PatientSex sex);
    PatientSex getSex(void);

    // 设置/获取年龄。
    void setAge(int age);
    int getAge(void);

    // 设置/获取血型。
    void setBlood(PatientBloodType blood);
    int getBlood(void);

    // 设置/获取体重, in unit of g。
    void setWeight(int weight);
    int getWeight(void);

    // 设置/获取身高, in unit of mm
    void setHeight(short height);
    short getHeight(void);

    // 设置/获取名字。
    void setName(const QString &name);
    const char *getName(void);

    // 设置/获取病人ID
    void setPatID(const QString &id);
    const char *getPatID(void);

    // 获取名字的长度。
    int getNameLength(void);
    int getIDLength(void);

    // 获取病人信息。
    const PatientInfo &getPatientInfo(void);

    // 获取单位。
    UnitType getWeightUnit(void);
    UnitType getHeightUnit(void);

signals:
    void signalPatientType(PatientType type);

private:
    PatientManager();

    PatientInfo _patientInfo;
    PatientBarWidget *_patientTypeWidget;
    PatientInfoWidget *_patientInfoWidget;

    void _loadPatientInfo(PatientInfo &info);
};
#define patientManager (PatientManager::construction())
#define deletePatientManager() (delete PatientManager::_selfObj)
