/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/12/19
 **/

#pragma once
#include <QObject>
#include <QVariantMap>

/**
 * @brief The JSONSerializer class serialize the QVariantMap into a json message,
 */
class JSONSerializerPrivate;
class JSONSerializer : public QObject
{
    Q_OBJECT
public:
    JSONSerializer();
    ~JSONSerializer();

public slots:
    /**
     * @brief serialze serialize a qvariantMap
     * @param map the qvariant map
     */
    void serialize(const QVariantMap &map);

signals:
    /* emit when serialaize one QVariantMap */
    void dataReady(const QString &json);

private:
    JSONSerializerPrivate * const pimpl;
};
