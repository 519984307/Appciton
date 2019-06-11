/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/6/4
 **/

#pragma once
#include <QObject>

class PlugInProviderPrivate;
class PlugInProvider : public QObject
{
    Q_OBJECT
public:
    enum PlugInType
    {
        PLUGIN_TYPE_SPO2,
        PLUGIN_TYPE_CO2,
        PLUGIN_TYPE_IBP
    };

public:
    explicit PlugInProvider(const QString &name, QObject *parent = NULL);
    virtual ~PlugInProvider();

private slots:
    // 接收数据
    void dataArrived();

private:
    PlugInProviderPrivate *const d_ptr;
};
