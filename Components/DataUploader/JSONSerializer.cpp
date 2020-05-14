/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/12/19
 **/

#include "JSONSerializer.h"
#include "Framework/SOUP/Json/serializer.h"

class JSONSerializerPrivate
{
public:
    JSONSerializerPrivate()
    {
    }

    QJson::Serializer serializer;
};

JSONSerializer::JSONSerializer()
    : pimpl(new JSONSerializerPrivate())
{
    pimpl->serializer.setIndentMode(QJson::IndentCompact);
}

JSONSerializer::~JSONSerializer()
{
    delete pimpl;
}

void JSONSerializer::serialize(const QVariantMap &map)
{
     QString message = pimpl->serializer.serialize(map);

     emit dataReady(message);
}
