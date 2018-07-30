/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/27
 **/
#pragma once
#include "Window.h"

class PatientInfoWindowPrivate;
class PatientInfoWindow : public Window
{
    Q_OBJECT
public:
    static PatientInfoWindow* getInstance();
    ~PatientInfoWindow();

public:
    /**
     * @brief widgetChange
     */
    void widgetChange(void);
    /**
     * @brief relieveStatus
     */
    void relieveStatus(bool);
    /**
     * @brief newPatientStatus
     */
    void newPatientStatus(bool);
protected:
    virtual void showEvent(QShowEvent *ev);
    virtual void keyPressEvent(QKeyEvent *ev);

private slots :
    /**
     * @brief onBtnReleased
     */
    void onBtnReleased();
    /**
     * @brief onComboIndexChanged
     */
    void onComboIndexChanged();

private:
    PatientInfoWindow();
    PatientInfoWindowPrivate *const d_ptr;
};

