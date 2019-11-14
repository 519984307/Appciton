/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/7
 **/

#include "Translator.h"
#include <QMap>
#include "../Config/XmlParser.h"

class TranslatorPrivate
{
public:
    TranslatorPrivate()  : valid(false) {}

    QMap<QString, QString> caches;
    XmlParser parser;
    bool valid;
};

Translator::Translator(const QString &languageFile)
    : d_ptr(new TranslatorPrivate())
{
    d_ptr->valid = d_ptr->parser.open(languageFile);
}

bool Translator::isValid() const
{
    return d_ptr->valid;
}

QString Translator::translate(const QString &strId)
{
    if (strId.isEmpty())
    {
        return QString();
    }

    QMap<QString, QString>::iterator it = d_ptr->caches.find(strId);
    if (it == d_ptr->caches.end())
    {
        QString value;
        bool ret = d_ptr->parser.getValue(strId, value);
        if (!ret)
        {
            /* if the string not found, return the string id */
            value = strId;
        }
        it = d_ptr->caches.insert(strId, value);
    }

    return it.value();
}

Translator::~Translator()
{
    delete d_ptr;
}
