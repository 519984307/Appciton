/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/6
 **/



#pragma once

#include <QObject>
#include <fstream>
#include <QString>
#include <QMap>
#include "XmlParser.h"

class XmlParser;
class LanguageManager
{
public:
    enum LanguageName
    {
        English = 0,
        Chinese_Simplify,
        Language_NR
    };

    static LanguageManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new LanguageManager();
        }
        return *_selfObj;
    }
    static LanguageManager *_selfObj;
    ~LanguageManager();

public:
    QString translate(const char *id);
    QString translate(const QString &id);
    LanguageName getCurLanguage() const;
    void reload(int index);
    QString getCurVoicePromptFolderName() const;

private:
    LanguageManager();

    XmlParser _xmlParser;
    typedef QMap<QString, QString> LanguageMap;
    LanguageMap _languageMap;    // 运行期间存放翻译好字符串的容器。
    LanguageName _curLanguage;

#ifdef CONFIG_UNIT_TEST
    friend class TestLanguageManager;
#endif
};
#define languageManager (LanguageManager::construction())
#define deleteLanguageManager() (delete LanguageManager::_selfObj)

#define trs(str) languageManager.translate(str)
// #define ptrs(str) languageManager.translatePrint(str)

