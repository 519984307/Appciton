/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/8
 **/

#pragma once

#include <QScopedPointer>
#include "Provider.h"
#include "COProviderIFace.h"

class SmartCOProviderPrivate;
class SmartCOProvider : public Provider, public COProviderIFace
{
public:
    /**
     * @brief SmartCOProvider constructor
     * @param port the uart port
     */
    explicit SmartCOProvider(const QString &port);
    ~SmartCOProvider();

    /* Provider interfaces */
public:
    /* reimplement */
    bool attachParam(Param *param);

    /* reimplement */
    void sendVersion();
    /* reimplement */
    void disconnected();
    /* reimplement */
    void reconnected();

protected:
    void dataArrived();

private:
    const QScopedPointer<SmartCOProviderPrivate> pimpl;
    friend class SmartCOProviderPrivate;
};
