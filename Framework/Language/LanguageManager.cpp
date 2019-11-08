/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/7
 **/

#include "LanguageManager.h"
#include "Translator.h"

LanguageManager *LanguageManager::instance = NULL;

class LanguageManagerPrivate
{
public:
    LanguageManagerPrivate()
        : curLang(LanguageManager::Language_NR)
    {
        for (int i = 0; i < LanguageManager::Language_NR; ++i)
        {
            translators[i] = NULL;
        }
    }

    ~LanguageManagerPrivate()
    {
        for (int i = 0; i < LanguageManager::Language_NR; ++i)
        {
            if (translators[i])
            {
                delete translators[i];
            }
        }
    }

    Translator* translators[LanguageManager::Language_NR];
    LanguageManager::LanguageId curLang;
};

LanguageManager *LanguageManager::getInstance()
{
    if (instance == NULL)
    {
        instance = new LanguageManager();
    }
    return instance;
}

LanguageManager::~LanguageManager()
{
    delete d_ptr;
}

bool LanguageManager::registerTranslator(LanguageManager::LanguageId langId, Translator *translator)
{
    if (langId >= Language_NR || translator == NULL)
    {
        return false;
    }

    Translator *old = d_ptr->translators[langId];
    if (old == translator)
    {
        return false;
    }

    d_ptr->translators[langId] = translator;
    if (d_ptr->curLang == Language_NR)
    {
        d_ptr->curLang = langId;
    }

    if (old)
    {
        delete old;
    }

    return true;
}

QString LanguageManager::translate(const QString &strId)
{
    if (d_ptr->curLang < Language_NR )
    {
        Translator *translator = d_ptr->translators[d_ptr->curLang];
        if (translator)
        {
            return translator->translate(strId);
        }
    }

    /* return the string id if the translator is not existed */
    return strId;
}

QString LanguageManager::translate(const char *strId) {
    return translate(QString::fromLatin1(strId));
}

bool LanguageManager::setCurLanguage(const LanguageManager::LanguageId &langId)
{
    if (langId >= Language_NR || d_ptr->translators[langId] == NULL)
    {
        return false;
    }

    d_ptr->curLang = langId;
    return true;
}

LanguageManager::LanguageId LanguageManager::getCurLanguage()
{
    return d_ptr->curLang;
}

LanguageManager::LanguageManager()
    :d_ptr(new LanguageManagerPrivate())
{
}
