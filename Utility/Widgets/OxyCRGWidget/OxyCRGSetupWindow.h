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

#include "Window.h"

class OxyCRGSetupWindowPrivate;
class OxyCRGSetupWindow : public Window
{
    Q_OBJECT
public:
    OxyCRGSetupWindow();
    ~OxyCRGSetupWindow();

public :
    /**
     * @brief setWaveTypeIndex
     * @param index
     */
    void setWaveTypeIndex(int index);

    /**
     * @brief getWaveTypeIndex
     * @return
     */
    int getWaveTypeIndex(void)const;
    /**
     * @brief getHRLow
     * @param status
     * @return
     */
    int getHRLow(bool &status)const;
    /**
     * @brief getHRHigh
     * @param status
     * @return
     */
    int getHRHigh(bool &status)const;
    /**
     * @brief getSPO2Low
     * @param status
     * @return
     */
    int getSPO2Low(bool &status)const;
    /**
     * @brief getSPO2High
     * @param status
     * @return
     */
    int getSPO2High(bool &status)const;
    /**
     * @brief getCO2Low
     * @param status
     * @return
     */
    int getCO2Low(bool &status)const;
    /**
     * @brief getCO2High
     * @param status
     * @return
     */
    int getCO2High(bool &status)const;
    /**
     * @brief getRRLow
     * @param status
     * @return
     */
    int getRRLow(bool &status)const;
    /**
     * @brief getRRHigh
     * @param status
     * @return
     */
    int getRRHigh(bool &status)const;

protected:
    /**
     * @brief showEvent
     * @param ev
     */
    virtual void showEvent(QShowEvent *ev);
    /**
     * @brief resizeEvent
     * @param e
     */
    virtual void resizeEvent(QResizeEvent *e);

public slots:
    /**
     * @brief onComboUpdated
     * @param index
     */
    void onComboUpdated(int index);

private:
    OxyCRGSetupWindowPrivate    *const d_ptr;
};

