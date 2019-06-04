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
#include "PatientInfoWindowInterface.h"

class PatientInfoWindowPrivate;
class PatientInfoWindow : public PatientInfoWindowInterface
{
    Q_OBJECT
public:
    static PatientInfoWindow &getInstance(void);
    ~PatientInfoWindow();

private slots:
    /**
     * @brief idReleased id按钮释放
     */
    void idReleased(void);

    /**
     * @brief nameReleased 姓名按钮释放
     */
    void nameReleased(void);

    /**
     * @brief heightReleased 身高按钮释放
     */
    void heightReleased(void);

    /**
     * @brief weightReleased 体重按钮释放
     */
    void weightReleased(void);

    /**
     * @brief onBtnReleased 解除病人按钮释放
     */
    void onBtnReleased(void);

    /**
     * @brief bedNumReleased 修改病人床号
     */
    void bedNumReleased(void);

    void onSpinBoxValueChanged(int, int);

protected:
    /*reimplement*/
    void showEvent(QShowEvent *);

    /*reimplement*/
    void hideEvent(QHideEvent *ev);

private:
    PatientInfoWindow();
    PatientInfoWindowPrivate * const d_ptr;
};

#define patientInfoWindow (PatientInfoWindow::getInstance())
