#pragma once
#include <QLabel>
#include "IWidget.h"
#include "PatientManager.h"

class PatientSelectWidget;
class PatientBarWidget : public IWidget
{
    Q_OBJECT

public:
    PatientBarWidget(QWidget *parent = 0);
    ~PatientBarWidget();

    // 设置病人类型。
    void setPatientType(PatientType newType);

    // 设置病人信息。
    void setPatientInfo(const PatientInfo &info);

private slots:
    void _releaseHandle(IWidget *);

private:
    QLabel *_bedtype;
    QLabel *_bedNum;
    QLabel *_patientName;
    QLabel *_patientTypeLabel;
    IWidget *_gain;         // 增益标签
    PatientSelectWidget *_patientTypeSelWidget;
};
