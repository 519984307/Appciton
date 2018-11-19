/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/

#pragma once
#include <QComboBox>
#include "SoundManager.h"

class ComboBoxPrivate;
class ComboBox: public QComboBox
{
    Q_OBJECT
public:
    explicit ComboBox(QWidget *parent = NULL);
    ~ComboBox();

    /* reimplement */
    virtual void showPopup();

    /* reimplement */
    QSize sizeHint() const;

    /**
     * @brief setPlaySoundType  在combobox item 设置聚焦改变时播放声音的类型
     * @param type 声音类型
     */
    void setPlaySoundType(SoundManager::SoundType type);

protected:
    /* reimplement */
    void keyPressEvent(QKeyEvent *ev);
    /* reimplement */
    void keyReleaseEvent(QKeyEvent *ev);
    /* reimplement */
    void hideEvent(QHideEvent *ev);
    /* reimplement */
    void paintEvent(QPaintEvent *ev);

private slots:
    void onPopupDestroyed();

private:
    QScopedPointer<ComboBoxPrivate> d_ptr;
};
