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

    /**
     * @brief widgetChange 根据状态修改按钮控件
     */
    void widgetChange(void);

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
     * @brief ageReleased 年龄按钮释放
     */
    void ageReleased(void);

    /**
     * @brief heightReleased 身高按钮释放
     */
    void heightReleased(void);

    /**
     * @brief weightReleased 体重按钮释放
     */
    void weightReleased(void);

    /**
     * @brief relieveReleased 解除病人按钮释放
     */
    void relieveReleased(void);

    /**
     * @brief saveInfoReleased 新建病人保存信息按钮释放
     */
    void saveInfoReleased(void);

    /**
     * @brief pacerMakerReleased 起博打开按钮释放
     */
    void pacerMakerReleased(int);

    /**
     * @brief dischargeWinExit 解除病人待机窗口退出返回标志
     * @param flag
     */
    void dischargeWinExit(int flag);

protected:
    /*reimplement*/
    void showEvent(QShowEvent *);

private:
    PatientInfoWindow();
    PatientInfoWindowPrivate * const d_ptr;
};

#define patientInfoWindow (PatientInfoWindow::construction())
#define deletePatientInfoWindow() (delete PatientInfoWindow::_selfObj)


