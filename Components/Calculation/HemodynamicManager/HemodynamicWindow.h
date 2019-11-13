/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/7
 **/
#pragma once
#include "Framework/UI/Dialog.h"

class HemodynamicWindowPrivate;
class HemodynamicWindow : public Dialog
{
    Q_OBJECT
public:
    static HemodynamicWindow* getInstance();
    ~HemodynamicWindow();

    /**
     * @brief layoutExec
     */
    void layoutExec();
    /**
     * @brief initCalInput
     */
    void initCalInput();
    /**
     * @brief updateData
     */
    void updateData(void);
    /**
     * @brief defaultInput
     */
    void defaultInput(void);

signals:
    void checkOutputSignal(int);
    void checkInputSignal(int);

protected:
    void resizeEvent(QResizeEvent *ev);

private slots:
    /**
     * @brief onBtnParamReleased
     */
    void onBtnParamReleased();
    /**
     * @brief onCheckOutputReleased
     */
    void onCheckOutputReleased(void);
    /**
     * @brief onCheckInputReleased
     */
    void onCheckInputReleased(void);
    /**
     * @brief onRefRangeReleased
     */
    void onRefRangeReleased(void);
    /**
     * @brief onCalcReleased
     */
    void onCalcReleased(void);
    /**
     * @brief onCalcReviewReleased
     */
    void onCalcReviewReleased(void);

private:
    HemodynamicWindow();
    HemodynamicWindowPrivate *const d_ptr;
};

