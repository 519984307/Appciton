/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/18
 **/

#pragma once
#include "PatientDefine.h"
#include <QVector>
#include <QObject>
#include "UnitManager.h"

// 病人管理对象。
class PatientInfoWidgetInterface;
class PatientManagerPrivate;
class PatientManager : public QObject
{
    Q_OBJECT

public:
    static PatientManager &getInstance(void);
    ~PatientManager();

public:
    // 设置Widget。
    void setPatientInfoWidget(PatientInfoWidgetInterface &widget);

public:
    /**
     * @brief setType 设置病人类型。
     * @param type
     */
    void setType(PatientType type);

    /**
     * @brief getType 获取病人类型。
     * @return
     */
    PatientType getType(void);
    QString getTypeStr(void);

    /**
     * @brief setPacermaker 设置起搏。
     * @param type
     */
    void setPacermaker(PatientPacer pacer);

    /**
     * @brief getPacermaker 获取起博
     * @return
     */
    PatientPacer getPacermaker(void);

    /**
     * @brief setSex 设置性别
     * @param sex
     */
    void setSex(PatientSex sex);

    /**
     * @brief getSex 获取性别
     * @return
     */
    PatientSex getSex(void);

    /**
     * @brief setBornDate 设置出生日期
     * @param bornDate
     */
    void setBornDate(QDate bornDate);

    /**
     * @brief getBornDate 获取出生日期
     * @return
     */
    QDate getBornDate(void);

    void getBornDate(unsigned int &year, unsigned int &month, unsigned int &day);

    /**
     * @brief setBlood 设置血型
     * @param blood
     */
    void setBlood(PatientBloodType blood);

    /**
     * @brief getBlood 获取血型
     * @return
     */
    int getBlood(void);

    /**
     * @brief setWeight 设置体重
     * @param weight
     */
    void setWeight(float weight);

    /**
     * @brief getWeight 获取体重
     * @return
     */
    float getWeight(void);

    /**
     * @brief setHeight 设置身高
     * @param height
     */
    void setHeight(float height);

    /**
     * @brief getHeight 获取身高
     * @return
     */
    float getHeight(void);

    /**
     * @brief setName 设置病人姓名
     * @param name
     */
    void setName(const QString &name);

    /**
     * @brief getName 获取病人姓名
     * @return
     */
    const char *getName(void);

    // 设置/获取病人ID
    /**
     * @brief setPatID 设置病人ID
     * @param id
     */
    void setPatID(const QString &id);

    /**
     * @brief getPatID 获取病人ID
     * @return
     */
    const char *getPatID(void);

    /**
     * @brief setBedNum 设置病床号
     * @param bedNum
     */
    void setBedNum(const QString &bedNum);
    const QString getBedNum();

    /**
     * @brief getNameLength 获取名字的长度。
     * @return
     */
    int getNameLength(void);

    /**
     * @brief getIDLength 获取ID长度
     * @return
     */
    int getIDLength(void);

    /**
     * @brief getPatientInfo 获取病人信息。
     * @return
     */
    const PatientInfo &getPatientInfo(void);

    /**
     * @brief getHistoryPatientInfo 获取指定路径的历史病人信息
     * @param path
     * @return
     */
    const PatientInfo &getHistoryPatientInfo(const QString &path);

    /**
     * @brief getWeightUnit 获取体重单位
     * @return
     */
    UnitType getWeightUnit(void);

    /**
     * @brief getHeightUnit 获取身高单位
     * @return
     */
    UnitType getHeightUnit(void);

    /**
     * @brief setMonitor 设置监护状态
     * @param status
     */
    void setMonitor(bool status);

    /**
     * @brief isMonitoring 是否正在监护病人
     * @return
     */
    bool isMonitoring();

    /**
     * @brief newPatient 新建病人
     */
    void newPatient();

    /**
     * @brief dischargePatient 解除病人
     */
    void dischargePatient();

    /**
     * @brief finishPatientInfo 完成病人信息处理
     */
    void finishPatientInfo();

    /**
     * @brief isNewPatient 是否新建病人
     * @return
     */
    bool isNewPatient();

    /**
     * @brief updatePatientInfo 更新病人信息
     */
    void updatePatientInfo();

signals:
    void signalPatientType(PatientType type);

private slots:
    /**
     * @brief onNewPatientHandle 新建病人后响应函数
     */
    void onNewPatientHandle();

private:
    PatientManager();
    PatientManagerPrivate *const d_ptr;
};
#define patientManager (PatientManager::getInstance())
