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

class TranslatorPrivate;
class Translator
{
public:
    explicit Translator(const QString &languageFile);
    ~Translator();

    /**
     * @brief isValid check whether the translator is valid
     * @return true if valid, otherwise, return false
     */
    bool isValid() const;

    /**
     * @brief translate translate the string ID
     * @param strId the string id
     * @return the language string of the string ID
     */
    QString translate(const QString &strId);

private:
    Q_DISABLE_COPY(Translator)
    TranslatorPrivate * const d_ptr;
};
