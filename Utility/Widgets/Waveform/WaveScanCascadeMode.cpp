////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的级联扫描模式
////////////////////////////////////////////////////////////////////////////////
#include <QPainter>
#include "WaveScanCascadeMode.h"
#include "WaveWidget.h"

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 构造函数
//
// 参数:
// parent: 波形控件对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveScanCascadeMode::WaveScanCascadeMode(WaveWidget *wave)
        : WaveCascadeMode(wave)
{

}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 析构函数
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveScanCascadeMode::~WaveScanCascadeMode()
{

}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 模式匹配检查
//
// 参数:
// mode: 模式标志
// isCascade: 是否级联
//
// 返回值:
// 匹配返回true, 否则返回false
////////////////////////////////////////////////////////////////////////////////
bool WaveScanCascadeMode::match(int mode, bool isCascade)
{
    return ((mode == SCAN_MODE) && isCascade);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 接收原始波形数据
//
// 参数:
// value: 原始波形数据
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanCascadeMode::addData(int /*value*/, int /*flag*/, bool /*isUpdated*/)
{

}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 获取更新区域
//
// 参数:
// region: 区域
//
// 返回值:
// 需要重绘返回true, 不需要重绘返回false
////////////////////////////////////////////////////////////////////////////////
bool WaveScanCascadeMode::updateRegion(QRegion &/*region*/)
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 返回刷新速率上限
//
// 参数:
// 无
//
// 返回值:
// 刷新速率上限
////////////////////////////////////////////////////////////////////////////////
int WaveScanCascadeMode::maxUpdateRate()
{
    return 30;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制波形
//
// 参数:
// painter: 绘图对象
// rect: 更新区域
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScanCascadeMode::paintWave(QPainter &/*painter*/,
        const QRect &/*rect*/)
{

}

