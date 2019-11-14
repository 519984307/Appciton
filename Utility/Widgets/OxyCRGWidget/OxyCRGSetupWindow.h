/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/20
 **/
#pragma  once

#include "Framework/UI/Dialog.h"

class OxyCRGSetupWindowPrivate;
class OxyCRGSetupWindow : public Dialog
{
    Q_OBJECT
public:
    OxyCRGSetupWindow();
    ~OxyCRGSetupWindow();

public :
    /**
     * @brief getWaveTypeIndex
     * @return
     */
    int getWaveTypeIndex(void)const;

    /**
     * @brief getHRLow
     * @return
     */
    int getHRLow()const;
    /**
     * @brief getHRHigh
     * @return
     */
    int getHRHigh()const;
    /**
     * @brief getSPO2Low
     * @return
     */
    int getSPO2Low()const;
    /**
     * @brief getSPO2High
     * @return
     */
    int getSPO2High()const;
    /**
     * @brief getCO2Low
     * @return
     */
    int getCO2Low()const;
    /**
     * @brief getCO2High
     * @return
     */
    int getCO2High()const;
    /**
     * @brief getRRLow
     * @return
     */
    int getRRLow()const;
    /**
     * @brief getRRHigh
     * @return
     */
    int getRRHigh()const;

protected:
    /**
     * @brief showEvent
     * @param ev
     */
    virtual void showEvent(QShowEvent *ev);

public slots:
    /**
     * @brief onComboUpdated
     * @param index
     */
    void onComboUpdated(int index);

private:
    OxyCRGSetupWindowPrivate    *const d_ptr;
};

