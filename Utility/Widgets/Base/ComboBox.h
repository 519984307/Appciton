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
     * @brief setPlaySoundType  在聚焦的item改变时，设置播放声音类型
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

signals:
    /**
     * @brief itemFocusChanged  聚焦改变信号触发
     * @param value
     * @param type
     */
    void itemFocusChanged(int value, SoundManager::SoundType type);

private slots:
    void onPopupDestroyed();

    /**
     * @brief onItemFocusChanged
     * @param value
     */
    void onItemFocusChanged(int value);

private:
    QScopedPointer<ComboBoxPrivate> d_ptr;
};
