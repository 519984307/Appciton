/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/30
 **/

#include <QByteArray>
#include <QVariant>
#include <QModelIndex>


struct ScreenLayoutItemInfo
{
    ScreenLayoutItemInfo()
        : waveMaxValue(255),
          waveMinValue(0),
          baseLine(128),
          sampleRate(250),
          isWave(false)
    {}

    QModelIndex index;
    QString name;
    QByteArray waveContent;
    unsigned char baseLine;
    unsigned char waveMinValue;
    unsigned char waveMaxValue;
    int sampleRate;
    bool isWave;
};

Q_DECLARE_METATYPE(ScreenLayoutItemInfo)
