/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/7
 **/

#pragma once

#include <QString>
class Translator;
class LanguageManagerPrivate;
class LanguageManager
{
public:
    enum LanguageId
    {
        English = 0,
        ChineseSimplified,
        Language_NR = 10,
    };

    static LanguageManager *getInstance();
    ~LanguageManager();

    /**
     * @brief registerTranslator register a language translator
     * @param langId the language id
     * @param translator the new translator
     * @return true if success, otherwise, return false;
     * @note the exist translator of the same language will be deleted
     */
    bool registerTranslator(LanguageId langId, Translator *translator);

    /**
     * @brief translate translate the string
     * @param strId the string id
     * @return  the correspond language string to the language id
     */
    QString translate(const QString &strId);

    /**
     * @brief translate translate the string
     * @param strId the string id
     * @return the correspond language string to the language id
     */
    QString translate(const char *strId);

    /**
     * @brief setCurlanguage set the current translate language
     * @param langId the lanaguage id
     * @return true if setting successfully
     */
    bool setCurLanguage(const LanguageId &langId);

    /**
     * @brief getCurLanguage get the current language id
     * @return  the current language id
     */
    LanguageId getCurLanguage();

protected:
    static LanguageManager *instance;
    LanguageManager();

private:
    Q_DISABLE_COPY(LanguageManager)
    LanguageManagerPrivate * const d_ptr;
};

#define trs(str) (LanguageManager::getInstance()->translate(str))

