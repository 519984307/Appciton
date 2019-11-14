/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/

#include "Framework/UI/Dialog.h"

class MonitorInfoWindowPrivate;
class MonitorInfoWindow : public Dialog
{
    Q_OBJECT
public:
    /**
     * @brief MonitorInfoWindow  构造函数
     */
    MonitorInfoWindow();
    /**
     * @brief ~MonitorInfoWindow  析构函数
     */
    ~MonitorInfoWindow();

    /**
     * @brief readyShow  加载显示参数
     */
    void readyShow();
    /**
     * @brief layoutExec  显示布局
     */
    void layoutExec();
    /**
     * @brief getRunTime  获取系统运行时间
     * @return 系统运行时间
     */
    QString getRunTime(void);

private slots:
    /* reimpelment */
    void showEvent(QShowEvent *e);

    /* reimpelment */
    void hideEvent(QHideEvent *e);

private slots:
    /**
     * @brief onTimeOutExec 定时器执行接口
     */
    void onTimeOutExec(void);

private:
    MonitorInfoWindowPrivate *const d_ptr;
};


