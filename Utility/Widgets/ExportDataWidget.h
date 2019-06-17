/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/11/30
 **/


#pragma once
#include "Dialog.h"
#include "LanguageManager.h"
#include "qprogressbar.h"
#include "Button.h"

//数据类型及传输方式
enum Export_Data_Type
{
    EXPORT_RESCUE_DATA_BY_USB,       // 通过U盘导出营救数据
    EXPORT_RESCUE_DATA_BY_SFTP,      // 通过WIFI导出营救数据
    EXPORT_CONFIG_DATA_BY_USB,       // 通过U盘导出配置数据
    EXPORT_CONFIG_DATA_BY_WIFI,      // 通过WIFI导出配置数据
    EXPORT_ECG12LEAD_DATA_BY_USB,    // 通过U盘导出12导心电数据
    EXPORT_ECG12LEAD_DATA_BY_WIFI,   // 通过WIFI导出12导心电数据
    EXPORT_ERROR_LOG_BY_USB,         // Export ErrorLog through USB
    EXPORT_DATA_NR
};

//数据传输基类
class QLabel;
class QProgressBar;
class QTimer;
class ExportDataWidget : public Dialog
{
    Q_OBJECT

public:
    explicit ExportDataWidget(Export_Data_Type type);
    ~ExportDataWidget();

    //是否传输取消
    bool isTransferCancel() const
    {
        return _transferCancel;
    }

    //获取进度条的值
    int getBarValue()
    {
        return _bar->value();
    }

    //初始化进度条的值
    void init();

public slots:
    //设置进度条的值
    void setBarValue(unsigned char value);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _cancleOrOKPressed();

signals:
    void cancel();

protected:
    QLabel *_title;
    QLabel *_info;
    QProgressBar *_bar;
    Button *_cancleOrOK;

    bool _transferCancel;
    Export_Data_Type _curType;
};
