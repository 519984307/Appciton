/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/12/19
 **/

#include "JSONParser.h"
#include "Framework/SOUP/Json/parser.h"

class JSONParserPrivate
{
public:
    JSONParserPrivate()
    {
    }

    QJson::Parser parser;
};

JSONParser::JSONParser()
    : pimpl(new JSONParserPrivate())
{
}

JSONParser::~JSONParser()
{
    delete pimpl;
}

void JSONParser::parse(const QString &jsonMessage)
{
    QVariant var = pimpl->parser.parse(jsonMessage.toUtf8());
    emit dataReady(var.toMap());
}
