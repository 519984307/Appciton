/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/30
 **/
#pragma once
#include "Framework/UI/Dialog.h"

class DoseCalculationWindowPrivate;
class DoseCalculationWindow : public Dialog
{
    Q_OBJECT
public:
    static DoseCalculationWindow *getInstance();
    ~DoseCalculationWindow();
    /**
     * @brief layoutExec
     */
    void layoutExec(void);
    /**
     * @brief initDrugParam
     */
    void initDrugParam(void);
    /**
     * @brief updateDrugParam
     */
    void updateDrugParam(void);
    /**
     * @brief updataParamDefault
     */
    void updataParamDefault(void);
    /**
     * @brief weightHandle
     */
    void weightHandle(float);
    /**
     * @brief totalHandle
     */
    void totalHandle(float);
    /**
     * @brief volumeHandle
     */
    void volumeHandle(float);
    /**
     * @brief concentrationHandle
     */
    void concentrationHandle(float);
    /**
     * @brief dose1Handle
     */
    void dose1Handle(float);
    /**
     * @brief dose2Handle
     */
    void dose2Handle(float);
    /**
     * @brief dose3Handle
     */
    void dose3Handle(float);
    /**
     * @brief dose4Handle
     */
    void dose4Handle(float);
    /**
     * @brief infusionRateHandle
     */
    void infusionRateHandle(float);
    /**
     * @brief dripRateHandle
     */
    void dripRateHandle(float);
    /**
     * @brief dropSizeHandle
     */
    void dropSizeHandle(float);
    /**
     * @brief durationHandle
     */
    void durationHandle(float);

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void onBtnParamReleased();
    void onComboUpdataParamDefault(int);
    void onBtnTitrateTableReleased(void);
private:
    DoseCalculationWindow();

    DoseCalculationWindowPrivate *const d_ptr;
};
