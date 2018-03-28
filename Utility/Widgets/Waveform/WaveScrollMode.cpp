////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的滚动模式
////////////////////////////////////////////////////////////////////////////////
#include <QPainter>
#include "WaveScrollMode.h"
#include "WaveWidget.h"

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 构造滚动模式对象
//
// 参数:
// parent: 波形控件对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveScrollMode::WaveScrollMode(WaveWidget *wave)
        : WaveNormalMode(wave), _isPending(false)
{
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 析构滚动模式对象
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
WaveScrollMode::~WaveScrollMode()
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
bool WaveScrollMode::match(int mode, bool isCascade)
{
    return ((mode == SCROLL_MODE) && !isCascade);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 滚动模式的刷新率上限, 单位Hz
//
// 参数:
// 无
//
// 返回值:
// 最高刷新率, 单位Hz
////////////////////////////////////////////////////////////////////////////////
int WaveScrollMode::maxUpdateRate()
{
    return 10;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 获取重绘区域
//
// 参数:
// region: 需要重绘的区域
//
// 返回值:
// 需要重绘返回true, 否则返回false
////////////////////////////////////////////////////////////////////////////////
bool WaveScrollMode::updateRegion(QRegion &region)
{
    if (!_isPending)
    {
        return false;
    }

    region += _wave->rect();
    _isPending = false;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 响应用户对波形上下限的修改, 该函数重新计算整个波形的坐标序列, 注意调用该函数前, 需要保证
// WaveWidget::_minValue和WaveWidget::_maxValue修改完毕
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScrollMode::valueRangeChanged()
{
    if (_wave->bufIsEmpty())
    {
        return;
    }
    else if (_wave->bufIsContinuous())
    {
        for (int i = _wave->_tail; i < _wave->_head; i++)
        {
            _wave->_waveBuf[i].setY(valueToY(_wave->_dataBuf[i]));
            _wave->_setDyBuf(i);
        }
    }
    else
    {
        for (int i = 0; i < _wave->_head; i++)
        {
            _wave->_waveBuf[i].setY(valueToY(_wave->_dataBuf[i]));
            _wave->_setDyBuf(i);
        }
        for (int i = _wave->_tail; i < _wave->_size; i++)
        {
            _wave->_waveBuf[i].setY(valueToY(_wave->_dataBuf[i]));
            _wave->_setDyBuf(i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 接收原始波形数据, 换算成Y坐标缓存起来
//
// 参数:
// value: 原始波形数据
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScrollMode::addData(int value, int flag)
{
    _wave->_bufPush(0, valueToY(value), value, flag);

    _isPending = true;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制准备: 换算整个波形序列的X坐标
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScrollMode::preparePaint()
{
    int p = _wave->_bufPrev(_wave->_head);
    int cnt = _wave->_bufCount();
    for (int i = 0; i < cnt; i++)
    {
        _wave->_waveBuf[p].setX(_wave->_xBuf[_wave->_size - 1 - i]);
        p = _wave->_bufPrev(p);
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制波形
//
// 参数:
// painter: 绘图对象
// rect: 重绘区域
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveScrollMode::paintWave(QPainter &painter, const QRect &/*rect*/)
{
    painter.setPen(QPen(_wave->palette().windowText(), _wave->lineWidth()));
    painter.setRenderHint(QPainter::Antialiasing, false);

    int cntToEnd = _wave->_bufCountToEnd();
    painter.drawPolyline(&_wave->_waveBuf[_wave->_tail], cntToEnd);

    int cnt = _wave->_bufCount();
    if (cntToEnd < cnt)
    {
        painter.drawLine(_wave->_waveBuf[_wave->_size - 1], _wave->_waveBuf[0]);
        painter.drawPolyline(&_wave->_waveBuf[0], (cnt - cntToEnd));
    }

    _isPending = false;
}
