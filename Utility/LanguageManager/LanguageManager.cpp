/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/6
 **/


#include "LanguageManager.h"
#include "IConfig.h"

#define LOCALE_FILE_PATH "/usr/local/nPM/locale/"

LanguageManager *LanguageManager::_selfObj = NULL;

/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
LanguageManager::LanguageManager()
{
    // 获得当前使用的语言。
    int langNo = 0;
    systemConfig.getNumAttr("General|Language", "CurrentOption", langNo);
    int langNext = 0 ;
    systemConfig.getNumAttr("General|Language", "NextOption", langNext);
    if (langNo != langNext)
    {
        systemConfig.setNumAttr("General|Language", "CurrentOption", langNext);
        langNo = langNext;
    }

    _curLanguage = (LanguageName)langNo;

    // 获取语言文件的名称。
    QString language = QString("General|Language|Opt%1").arg(langNo);

    systemConfig.getStrAttr(language, "XmlFileName", language);
    QString path =  LOCALE_FILE_PATH + language + ".xml";

    _xmlParser.open(path);
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
LanguageManager::~LanguageManager()
{
}

/**************************************************************************************************
 * 功能：将用户给的索引字符串翻译成某种语种的字符串。
 * 参数：
 *      id：待翻译的字符串；
 * 返回值：翻译好的字符串。
 *************************************************************************************************/
QString LanguageManager::translate(const char *id)
{
    static QString retNull("null");
    if (NULL == id)
    {
        retNull = "null";
        return retNull;
    }

    QString idStr = QString::fromLatin1(id);
    LanguageMap::iterator it = _languageMap.find(idStr);
    if (_languageMap.end() == it)
    {
        QString value;
        bool ret = _xmlParser.getValue(idStr, value);
        if (!ret)
        {
            retNull = idStr;
            return retNull;
        }
        it = _languageMap.insert(idStr, value);
    }

    return it.value();
}

QString LanguageManager::translate(const QString &id)
{
    static QString retNull("null");

    LanguageMap::iterator it = _languageMap.find(id);
    if (_languageMap.end() == it)
    {
        QString value;
        bool ret = _xmlParser.getValue(id, value);
        if (!ret)
        {
            retNull = id;
            return retNull;
        }
        it = _languageMap.insert(id, value);
    }

    return it.value();
}

/***************************************************************************************************
 * get the current language name
 **************************************************************************************************/
LanguageManager::LanguageName LanguageManager::getCurLanguage() const
{
    return _curLanguage;
}

/***************************************************************************************************
 * reload language
 **************************************************************************************************/
void LanguageManager::reload(int index)
{
    // 获取语言文件的名称。
    if (index == getCurLanguage())
    {
        return;
    }

    if (index >= Language_NR)
    {
        // invaild input
        index = 0;
    }

    QString language = QString("General|Language|Opt%1").arg(index);

    bool ret = systemConfig.getStrAttr(language, "XmlFileName", language);
    if (!ret)
    {
        return;
    }
    QString path =  LOCALE_FILE_PATH + language + ".xml";

    _curLanguage = static_cast<LanguageName>(index);
    _xmlParser.open(path);
    _languageMap.clear();
}

/***************************************************************************************************
 * get the current voice prompt string id
 **************************************************************************************************/
QString LanguageManager::getCurVoicePromptFolderName() const
{
    static const char *voicePromptStrings [Language_NR] =
    {
        "English",
        "ChinaMainland",
    };
    return voicePromptStrings[_curLanguage];
}
