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

#define MAX_ROWCOUNT            9
#define NUM_ABREAST             3
class TitrateTableWindowPrivate;
class TitrateTableWindow : public Dialog
{
   Q_OBJECT
public:
    static TitrateTableWindow* getInstance();
    ~TitrateTableWindow();

    /**
     * @brief layoutExecIt
     */
    void layoutExecIt(void);
    /**
     * @brief updateTitrateTableData
     */
    void updateTitrateTableData(void);
    /**
     * @brief datumTermDose
     */
    void datumTermDose(void);
    /**
     * @brief datumTermInfusionRate
     */
    void datumTermInfusionRate(void);
    /**
     * @brief datumTermDripRate
     */
    void datumTermDripRate(void);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void  onBtnUpReleased(void);
    void  onBtnDownReleased(void);
    void  onBtnTableSetReleased(void);
    void  onBtnRecordReleased(void);

private:
    TitrateTableWindow();
    TitrateTableWindowPrivate *const d_ptr;
};
