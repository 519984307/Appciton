#pragma once
#include "IWidget.h"

class QLabel;
class PatientInfoWidget : public IWidget
{
    Q_OBJECT

public:
    PatientInfoWidget(QWidget *parent = 0);
    ~PatientInfoWidget();

    void loadPatientInfo(void);

private:
    QLabel *_bedtype;
    QLabel *_bedNum;
    QLabel *_patientName;
};
