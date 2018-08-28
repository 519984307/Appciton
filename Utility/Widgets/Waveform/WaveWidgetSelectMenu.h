/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/27
 **/



#pragma once
#include "Window.h"

class WaveWidgetSelectMenuPrivate;
class WaveWidgetSelectMenu : public Window
{
    Q_OBJECT
public:
    static WaveWidgetSelectMenu &getInstance(void);

    ~WaveWidgetSelectMenu();

public:
    /**
     * @brief setTopWaveform 设置是否为操作Top Waveform。
     * @param isTopWaveform
     */
    void setTopWaveform(bool isTopWaveform);
    /**
     * @brief setWaveformName  设置被操作的波形控件
     * @param name
     */
    void setWaveformName(const QString &name);

    /**
     * @brief isTopWaveform  是否关联了TopWaveform
     * @return
     */
    bool isTopWaveform(void) const;

    /**
     * @brief refresh  重新刷新
     */
    void refresh(void);

    /**
     * @brief updateMFCWaveName 更新MFC波形名称
     * @param name
     */
    void updateMFCWaveName(const QString &name);

    /**
     * @brief setShowPoint 设置显示坐标
     * @param x
     * @param y
     */
    void setShowPoint(int x, int y);

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    /**
     * @brief loadWaveformsSlot
     */
    void  loadWaveformsSlot(void);
    /**
     * @brief replaceListSlot
     * @param index
     */
    void  replaceListSlot(int index);
    /**
     * @brief insertListSlot
     * @param index
     */
    void  insertListSlot(int index);
    /**
     * @brief removeButtonSlot
     */
    void  removeButtonSlot(void);

private:
    WaveWidgetSelectMenu();
    WaveWidgetSelectMenuPrivate *const d_ptr;
};
#define waveWidgetSelectMenu (WaveWidgetSelectMenu::getInstance())
