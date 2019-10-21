/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/10/17
 **/

#ifndef WAVE_DEFINE_H
#define WAVE_DEFINE_H

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件工作模式
////////////////////////////////////////////////////////////////////////////////
enum WaveModeFlag
{
    SCAN_MODE = 0,        // 扫描模式
    SCROLL_MODE = 1,      // 滚动模式
    REVIEW_MODE = 2,       // 回顾模式
    BAR_GRAPH_SCAN_MODE = 3,  // bar graph scan mode
    FREEZE_REVIEW_MODE = 4,  // freeze review mode
};

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 波形绘制方式：阶梯、色阶。
////////////////////////////////////////////////////////////////////////////////
enum WaveDrawMode
{
    WAVE_DRAW_MODE_MONO = 0,    // 阶梯
    WAVE_DRAW_MODE_COLOR = 1     // 色阶
};

#endif  // WAVE_DEFINE_H
