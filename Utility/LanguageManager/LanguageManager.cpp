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
    superConfig.getNumAttr("Local|Language", "CurrentOption", langNo);

    _curLanguage = (LanguageName) langNo;

    // 获取语言文件的名称。
    QString language;
    language.sprintf("Local|Language|Opt%d", langNo);

    superConfig.getStrAttr(language, "XmlFileName", language);
    QString path =  LOCALE_FILE_PATH + language + ".xml";

    _xmlParser.open(path);

    // 载入打印的翻译文件。
    path = LOCALE_FILE_PATH;
    path += "PrintEnglish.xml"; // todo:目前只支持英文。
    _xmlPrintParser.open(path);
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
const QString &LanguageManager::translate(const char *id)
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
            //debug("@@\t%s\t@@", id.toAscii().data());
            return retNull;
        }
        it = _languageMap.insert(idStr, value);
    }

    return it.value();
}

const QString &LanguageManager::translate(const QString &id)
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
            //debug("@@\t%s\t@@", id.toAscii().data());
            return retNull;
        }
        it = _languageMap.insert(id, value);
    }

    return it.value();
}

/**************************************************************************************************
 * 功能：将用户给的索引字符串翻译成某种语种的字符串。
 * 参数：
 *      id：待翻译的字符串；
 * 返回值：翻译好的字符串。
 *************************************************************************************************/
const QString &LanguageManager::translatePrint(const char *id)
{
    QString idStr = QString::fromLatin1(id);
    LanguageMap::iterator it = _printLanguageMap.find(idStr);
    if (_printLanguageMap.end() == it)
    {
        QString value;
        bool ret = _xmlPrintParser.getValue(idStr, value);
        if (!ret)
        {
//            debug("@@\t%s\t@@", id.toAscii().data());
            return trs(id);
        }
        it = _printLanguageMap.insert(idStr, value);
    }

    return it.value();
}

const QString &LanguageManager::translatePrint(const QString &id)
{
    LanguageMap::iterator it = _printLanguageMap.find(id);
    if (_printLanguageMap.end() == it)
    {
        QString value;
        bool ret = _xmlPrintParser.getValue(id, value);
        if (!ret)
        {
//            debug("@@\t%s\t@@", id.toAscii().data());
            return trs(id);
        }
        it = _printLanguageMap.insert(id, value);
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

    QString language;
    language.sprintf("Local|Language|Opt%d", index);

    bool ret = superConfig.getStrAttr(language, "XmlFileName", language);
    if (!ret)
    {
        return;
    }
    QString path =  LOCALE_FILE_PATH + language + ".xml";

    _xmlParser.open(path);
    _languageMap.clear();
}

/***************************************************************************************************
 * get the current voice prompt string id
 **************************************************************************************************/
QString LanguageManager::getCurVoicePromptFolderName() const
{
    static const char * voicePromptStrings [Language_NR] = {
        "English",
        "ChinaMainland",
        //"ChinaTaiwan",
        //"ChinaHongKong",
    };
    return voicePromptStrings[_curLanguage];
}
