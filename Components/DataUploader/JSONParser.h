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
 * @brief The JSONParser class parse a json message into a variant map,
 */
class JSONParserPrivate;
class JSONParser : public QObject
{
    Q_OBJECT
public:
    JSONParser();
    ~JSONParser();

public slots:
    /**
     * @brief parse parse a json message into a variant map
     * @param jsonMessage the json message
     */
    void parse(const QString &jsonMessage);

signals:
    /* emit when parse one json message*/
    void dataReady(const QVariantMap &map);

private:
    JSONParserPrivate * const pimpl;
};
