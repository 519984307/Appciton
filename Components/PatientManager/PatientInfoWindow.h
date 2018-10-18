/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/7/31
 **/

#pragma once
#include "Window.h"

class PatientInfoWindowPrivate;
class PatientInfoWindow : public Window
{
    Q_OBJECT
public:
    static PatientInfoWindow &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PatientInfoWindow();
        }
        return *_selfObj;
    }
    static PatientInfoWindow *_selfObj;
    ~PatientInfoWindow();
    void widgetChange(void);
    void relieveStatus(bool);
    void newPatientStatus(bool);
private slots:
    void idReleased(void);
    void nameReleased(void);
    void ageReleased(void);
    void heightReleased(void);
    void weightReleased(void);
    void relieveReleased(void);
    void saveInfoReleased(void);
    void pacerMakerReleased(int);
    void dischargeWinExit(bool flag);

protected:
    void showEvent(QShowEvent *);

private:
    PatientInfoWindow();
    PatientInfoWindowPrivate * const d_ptr;
};

#define patientInfoWindow (PatientInfoWindow::construction())
#define deletePatientInfoWindow() (delete PatientInfoWindow::_selfObj)


