#include "BarGraphScanMode.h"
#include "WaveDefine.h"
#include "WaveWidget.h"
#include <QPainter>
#include "Debug.h"

#define BAR_GRAPH_WIDTH 14
BarGraphScanMode::BarGraphScanMode(WaveWidget *wave)
    :WaveScanMode(wave), _bufferReset(true)
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
bool BarGraphScanMode::match(int mode, bool isCascade)
{
    Q_UNUSED(isCascade);
    return (mode == BAR_GRAPH_SCAN_MODE);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制波形: 根据绘制区域的X轴跨度来决定绘制曲线的一小部分, 以优化性能
//
// 参数:
// painter: 绘图对象
// rect: 重绘区域
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void BarGraphScanMode::paintWave(QPainter &painter, const QRect &rect)
{
    if(!_wave)
    {
        return;
    }

    if (_wave->bufIsEmpty())
    {
        return;
    }

    painter.setPen(QPen(_wave->palette().windowText().color(), _wave->lineWidth()));
    painter.setRenderHint(QPainter::Antialiasing, false);

    if(rect.left() >= _wave->_margin.left() && rect.top() == _wave->_margin.top())
    {
        //draw bar here
        //the bar might locate at the start and the end position, so it might need to draw twice

        if(!_barRects[0].isNull() && rect.contains(_barRects[0]))
        {
            painter.fillRect(_barRects[0], _wave->palette().windowText());
            _barRects[0] = QRect();
        }
        else if(!_barRects[1].isNull() && rect.contains(_barRects[1]))
        {
            painter.fillRect(_barRects[1], _wave->palette().windowText());
            _barRects[1] = QRect();
        }
        return;
    }

    // 把绘制区域的左右边界换算为波形坐标序列索引
    int leftIndex = qMax(0, xToIndex(rect.left() - _wave->lineWidth()) - 1);
    int rightIndex = qMin((_wave->_size - 1), xToIndex(rect.right() + _wave->lineWidth()) + 1);
    int beginIndex;
    int endIndex;
    int count;

    // 绘制波形曲线
    if (_wave->_head > _wave->_tail)
    {
        // 缓冲连续情形
        beginIndex = qMax(leftIndex, _wave->_tail);
        endIndex = qMin(rightIndex, _wave->_head - 1);
        count = endIndex - beginIndex;
        if (count >= 0)
        {
            _drawBaseLine(painter, beginIndex, endIndex);
        }
    }
    else
    {
        // 缓冲不连续情形
        beginIndex = qMax(leftIndex, 0);
        endIndex = qMin(rightIndex, _wave->_head - 1);
        count = endIndex - beginIndex ;
        if (count >= 0)
        {
            _drawBaseLine(painter, beginIndex, endIndex);
        }

        beginIndex = qMax(leftIndex, _wave->_tail);
        endIndex = qMin(rightIndex, _wave->_size - 1);
        count = endIndex - beginIndex;
        if (count >= 0)
        {
            _drawBaseLine(painter, beginIndex, endIndex);
        }

    }
}

/***************************************************************************************************
 * get the update region
 **************************************************************************************************/
bool BarGraphScanMode::updateRegion(QRegion &region)
{
    if (_pendingCount < 1)
    {
        return false;
    }

    if(!_wave)
    {
        return false;
    }

    int count = _wave->_bufCount();
    if (count < 3)
    {
        return false;
    }

    _pendingCount = qMin(_pendingCount, count);

    QRect rect;
    rect.setTop(0);
    rect.setBottom(_wave->height() - 1);

    // 计算波头更新区域
    //预留画标记长度
    int startX = _wave->_waveBufHead(_pendingCount).x();
    int endX = _wave->_waveBufHead(0).x();

    if (startX <= endX)
    {
        rect.setLeft(startX);
        rect.setRight(endX);
        region += rect;
    }
    else
    {
        rect.setLeft(startX);
        rect.setRight(_wave->width());
        region += rect;

        rect.setLeft(_wave->_margin.left());
        rect.setRight(endX);
        region += rect;
    }

    if(_wave->_waveBufHead(0).y() != _wave->_margin.top())
    {
        _newBarPos = _wave->_waveBufHead(0);
    }

    _pendingCount = 0;

    // 计算波尾更新区域
    int deleteCount = _scanLineSpace - _wave->_bufSpace();
    if (deleteCount > 0)
    {
        startX = _wave->_waveBufTail(0).x();
        endX = _wave->_waveBufTail(deleteCount).x();
        if (startX <= endX)
        {
            rect.setLeft(startX);
            rect.setRight(endX);
            region += rect;
        }
        else
        {
            rect.setLeft(startX);
            rect.setRight(_wave->width());
            region += rect;

            rect.setLeft(_wave->_margin.left());
            rect.setRight(endX);
            region += rect;
        }

        _wave->_bufPop(deleteCount);
    }

    return true;
}

/***************************************************************************************************
 * get the data size, use the floor value
 **************************************************************************************************/
int BarGraphScanMode::dataSize()
{
    if(!_wave)
    {
        return 0;
    }

    int minX = _wave->_margin.left();
    int maxX = _wave->width() - 1 - _wave->_margin.right();
    if(minX > maxX)
    {
        return 0;
    }

    return (int) (maxX - minX + 1) * _wave->dataRate() / _wave->waveSpeed() *
            _wave->pixelWPitch();
}

/***************************************************************************************************
 * add here to check whether the buffer is reset
 **************************************************************************************************/
void BarGraphScanMode::preparePaint()
{
    WaveScanMode::preparePaint();
    if(_wave->_head == 0 && _wave->_tail == 0)
    {
        _bufferReset = true;
    }
}

/***************************************************************************************************
 * add here to adjust the scan line
 **************************************************************************************************/
void BarGraphScanMode::prepareTransformFactor()
{
    WaveScanMode::prepareTransformFactor();
    _scanLineSpace -= 1;
}

/***************************************************************************************************
 * draw the base line, and check whether needed to update again to paint bars
 **************************************************************************************************/
void BarGraphScanMode::_drawBaseLine(QPainter &painter, int beginIndex, int endIndex)
{
    int i = beginIndex;
    int baseLine = _wave->_margin.top();

    int basrLineR = _wave->rect().right() - _wave->_margin.right();
    if (_wave->_waveBuf[endIndex].x() < basrLineR)
    {
        basrLineR = _wave->_waveBuf[endIndex].x();
    }
    //draw baseline first
    painter.drawLine(QPoint(_wave->_waveBuf[beginIndex].x(), baseLine),
                     QPoint(basrLineR, baseLine));

    QRegion barRegion;

    while(i <= endIndex)
    {
        if(_wave->_waveBuf[i].x() == _newBarPos.x())
        {
            //draw a bar
            int minX = _wave->_margin.left();
            if(minX + BAR_GRAPH_WIDTH <= _newBarPos.x())
            {
                _barRects[0] = QRect(_newBarPos.x() - BAR_GRAPH_WIDTH, baseLine, BAR_GRAPH_WIDTH, _newBarPos.y() - _wave->_margin.top());
                barRegion += _barRects[0];

                _barRects[1] = QRect();
            }
            else
            {
                int w1 = _newBarPos.x() - minX;
                int w2 = BAR_GRAPH_WIDTH - w1;

                if(!_bufferReset) //don't draw at the end when reset
                {
                    int maxX = _wave->width() - _wave->_margin.right();
                    _barRects[0] = QRect(maxX - w2, baseLine, w2, _newBarPos.y() - _wave->_margin.top());
                    barRegion += _barRects[0];
                }
                else
                {
                    _barRects[0] = QRect();
                }

                _barRects[1] = QRect(minX, baseLine, w1, _newBarPos.y() - _wave->_margin.top());
                barRegion += _barRects[1];
            }
            _newBarPos = QPoint();
            _bufferReset = false;
            break;
        }
        i++;
    }

    if(barRegion.rectCount())
    {
        _wave->update(barRegion);
    }
}
