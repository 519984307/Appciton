////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件的回顾模式
////////////////////////////////////////////////////////////////////////////////
#include <QPainter>
#include "WaveReviewMode.h"
#include "WaveWidget.h"
#include "WaveDataModel.h"
#include "FloatHandle.h"

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
WaveReviewMode::WaveReviewMode(WaveWidget *wave)
        : QObject(), WaveNormalMode(wave), _paintWaveFunc(_drawSparseCurve),
        _reviewTime(), _needLoadWaveData(false)
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
WaveReviewMode::~WaveReviewMode()
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
bool WaveReviewMode::match(int mode, bool isCascade)
{
    return ((mode == REVIEW_MODE) && !isCascade);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 回顾指定时刻开始的波形
//
// 参数:
// time: 回顾启始时刻
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveReviewMode::reviewWave(const QDateTime &time)
{
    if (!_wave)
    {
        return;
    }

    _reviewTime = time;

    _needLoadWaveData = true;

    _wave->update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 调整波形坐标换算参数
////////////////////////////////////////////////////////////////////////////////
void WaveReviewMode::prepareTransformFactor()
{
    if (!_wave)
    {
        return;
    }

    if (_wave->_model && _wave->_model->waveDataInfo(WAVA_IS_VALID).toBool())
    {
        // 更新波形采样率
        float sampleRate =
                _wave->_model->waveDataInfo(WAVE_SAMPLE_RATE).toFloat();
        if (!isZero(sampleRate))
        {
            _wave->_dataRate = sampleRate;
        }

        // 更新波形数值范围
        if (_wave->_model->waveDataInfo(WAVE_HAS_RULER).toBool())
        {
            // 波形幅度由标尺定义
            int rulerHeight =
                    _wave->_model->waveDataInfo(WAVE_RULER_HEIGHT).toInt();
            int rulerUpper =
                    _wave->_model->waveDataInfo(WAVE_RULER_UPPER).toInt();
            int rulerLower =
                    _wave->_model->waveDataInfo(WAVE_RULER_LOWER).toInt();
            int rulerTop = _wave->_margin.top()
                    + ((_wave->height() - _wave->_margin.top() - _wave->_margin.bottom()
                            - rulerHeight) / 2);
            int rulerBottom = rulerTop + rulerHeight - 1;
            _wave->_maxValue = (rulerUpper - rulerLower)
                    * (_wave->_margin.bottom() - rulerBottom) / (rulerTop - rulerBottom)
                    + rulerLower;
            _wave->_minValue = (rulerUpper - rulerLower)
                    * (_wave->height() - 1 - _wave->_margin.top() - rulerBottom)
                    / (rulerTop - rulerBottom) + rulerLower;
        }
        else
        {
            // 波形幅度由最大值最小值定义
            _wave->_maxValue =
                    _wave->_model->waveDataInfo(WAVE_RANGE_MAX).toInt();
            _wave->_minValue =
                    _wave->_model->waveDataInfo(WAVE_RANGE_MIN).toInt();
        }
    }

    // 更新波形数值范围后, 才能计算波形坐标换算因子
    WaveNormalMode::prepareTransformFactor();

    // 更新波形采样率后, 才能访问数据量
    if (dataSize() >= _wave->width())
    {
        _paintWaveFunc = _wave->isFill() ? _fillDenseCurve : _drawDenseCurve;
    }
    else
    {
        _paintWaveFunc = _wave->isFill() ? _fillSparseCurve : _drawSparseCurve;
    }
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
int WaveReviewMode::maxUpdateRate()
{
    // 回顾模式不需要控制刷新频率
    return -1;
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
void WaveReviewMode::paintWave(QPainter &painter, const QRect &rect)
{
    if (!_wave || !_paintWaveFunc)
    {
        return;
    }

    if (_needLoadWaveData)
    {
        _loadWaveData();
        _needLoadWaveData = false;
    }

    painter.setBrush(_wave->palette().windowText());

    // 把绘制区域的左右边界换算为波形坐标序列索引
    int leftIndex = qMax(0, xToIndex(rect.left() - _wave->lineWidth()) - 1);
    int rightIndex = qMin((_wave->_size - 1),
            xToIndex(rect.right() + _wave->lineWidth()) + 1);
    int beginIndex = qMax(leftIndex, _wave->_tail);
    int endIndex = qMin(rightIndex, _wave->_head - 1);
    if (endIndex > beginIndex)
    {
        _paintWaveFunc(this, painter, beginIndex, endIndex);
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 进入回顾模式
////////////////////////////////////////////////////////////////////////////////
void WaveReviewMode::enter()
{
    if (!_wave || !_wave->_model)
    {
        return;
    }

    connect(_wave->_model, SIGNAL(modelChanged()), this,
            SLOT(_onModelChanged()));

    _onModelChanged();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 退出回顾模式
////////////////////////////////////////////////////////////////////////////////
void WaveReviewMode::exit()
{
    if (!_wave || !_wave->_model)
    {
        return;
    }

    disconnect(_wave->_model, SIGNAL(modelChanged()), this,
            SLOT(_onModelChanged()));
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 响应数据模型变更
////////////////////////////////////////////////////////////////////////////////
void WaveReviewMode::_onModelChanged()
{
    if (!_wave || !_wave->_model)
    {
        return;
    }

    _wave->updateBuffer();

    _reviewTime = QDateTime();

    _needLoadWaveData = true;

    _wave->update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 在窗口尺寸,采样率,波速,数值范围等参数已确定的前提下重新加载波形数据
////////////////////////////////////////////////////////////////////////////////
void WaveReviewMode::_loadWaveData()
{
    if (!_wave || !_wave->_waveBuf || !_wave->_dataBuf || !_wave->_flagBuf
            || !_wave->_dyBuf)
    {
        return;
    }

    // 清空波形缓冲
    _wave->_head = 0;
    _wave->_tail = 0;

    // 无有效波形数据时, 不执行数据加载
    if (!_wave->_model || !_wave->_model->waveDataInfo(WAVA_IS_VALID).toBool())
    {
        return;
    }

    // 确定回顾启始时刻
    QDateTime startupTime = _wave->_model->waveDataInfo(WAVE_STARTUP_TIME).toDateTime();
    int seconds = (startupTime.isValid() && _reviewTime.isValid()) ?
            startupTime.secsTo(_reviewTime) : 0;

    // 加载波形数据
    if (seconds >= 0)
    {
        // 回顾时刻晚于或等于波形启始时刻的情况
        int skip = seconds * _wave->_dataRate;
        int n = _wave->_model->readWaveData(skip, _wave->_dataBuf,
                _wave->_bufSpaceToEnd());
        _wave->_head = (_wave->_head + n) % _wave->_size;
    }
    else
    {
        // 回顾时刻早于波形启始时刻的情况
        int lack = -seconds * _wave->_dataRate;
        if (lack >= _wave->_bufSpaceToEnd())
        {
            // 有效波形超出波形控件显示范围
            return;
        }

        _wave->_head = lack;
        _wave->_tail = _wave->_head;
        int n = _wave->_model->readWaveData(0, _wave->_dataBuf + _wave->_head,
                _wave->_bufSpaceToEnd());
        _wave->_head = (_wave->_head + n) % _wave->_size;
    }

    // 计算波形座标
    for (int i = _wave->_tail; i != _wave->_head; i = _wave->_bufNext(i))
    {
        _wave->_waveBuf[i].setX(_wave->_xBuf[i]);
        _wave->_waveBuf[i].setY(valueToY(_wave->_dataBuf[i]));
        _wave->_flagBuf[i] = 0;
        if (i == _wave->_tail)
        {
            _wave->_dyBuf[i] = false;
        }
        else
        {
            _wave->_dyBuf[i] = qAbs(_wave->_waveBuf[i].y() -
                    _wave->_waveBuf[_wave->_bufPrev(i)].y()) > 1;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制稀疏曲线, 把波形分解为折线段
//
// 参数:
// painter: 绘图对象
// beginIndex: 波形起始索引
// endIndex: 波形结束索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveReviewMode::_drawSparseCurve(WaveReviewMode *self, QPainter &painter,
        int beginIndex, int endIndex)
{
    if (!self || !self->_wave)
    {
        return;
    }
    painter.setPen(QPen(self->_wave->palette().windowText(), self->_wave->lineWidth()));
    painter.setRenderHint(QPainter::Antialiasing, self->_wave->isAntialias());

    if (beginIndex < endIndex)
    {
        painter.drawPolyline(&self->_wave->_waveBuf[beginIndex], endIndex - beginIndex + 1);
    }
    else if (beginIndex == endIndex)
    {
        painter.drawPoints(&self->_wave->_waveBuf[beginIndex], 1);
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制稠密曲线, 根据相邻点Y坐标的差异把波形分解为点和线
//
// 参数:
// painter: 绘图对象
// beginIndex: 波形起始索引
// endIndex: 波形结束索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveReviewMode::_drawDenseCurve(WaveReviewMode *self, QPainter &painter,
        int beginIndex, int endIndex)
{
    if (!self || !self->_wave)
    {
        return;
    }

    painter.setPen(QPen(self->_wave->palette().windowText(), self->_wave->lineWidth()));
    painter.setRenderHint(QPainter::Antialiasing, self->_wave->isAntialias());

    int i = beginIndex;
    while (i <= endIndex)
    {
        int j = i + 1;
        if (j > endIndex)
        {
            painter.drawPoints(&self->_wave->_waveBuf[i], 1);
            break;
        }
        else if (self->_wave->_dyBuf[j])
        {
            while (((j + 1) <= endIndex) && self->_wave->_dyBuf[j + 1])
            {
                j++;
            }
            painter.drawPolyline(&self->_wave->_waveBuf[i], (j - i + 1));
            i = j + 1;
        }
        else
        {
            while (((j + 1) <= endIndex) && !self->_wave->_dyBuf[j + 1])
            {
                j++;
            }
            painter.drawPoints(&self->_wave->_waveBuf[i], (j - i));
            i = j;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制填充曲线, 分解为三角形和矩形
//
// 参数:
// painter: 绘图对象
// beginIndex: 波形起始索引
// endIndex: 波形结束索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveReviewMode::_fillSparseCurve(WaveReviewMode *self, QPainter &painter,
        int beginIndex, int endIndex)
{
    if (!self || !self->_wave)
    {
        return;
    }

    painter.setPen(self->_wave->palette().color(QPalette::WindowText));
    painter.setRenderHint(QPainter::Antialiasing, false);

    int i = beginIndex;
    int yBottom = self->_wave->height() - 1 - self->_wave->_margin.bottom();
    while (i <= endIndex)
    {
        int j = i + 1;
        if (j > endIndex)
        {
            // 单数据点
            painter.drawLine(self->_wave->_waveBuf[i], QPoint(self->_wave->_xBuf[i], yBottom));
            break;
        }
        else if (self->_wave->_waveBuf[i].y() == self->_wave->_waveBuf[j].y())
        {
            // 矩形
            while (((j + 1) <= endIndex) && (self->_wave->_waveBuf[i].y() ==
                    self->_wave->_waveBuf[j + 1].y()))
            {
                j++;
            }
            QRect rect;
            rect.setTopLeft(self->_wave->_waveBuf[i]);
            rect.setWidth(self->_wave->_xBuf[j] - self->_wave->_xBuf[i] + 1);
            rect.setBottom(yBottom);
            painter.fillRect(rect,
                    self->_wave->palette().color(QPalette::WindowText));
            i = j;
        }
        else if (self->_wave->_waveBuf[i].y() > self->_wave->_waveBuf[j].y())
        {
            // 梯形
            QPoint tri[3];
            tri[0] = self->_wave->_waveBuf[i];
            tri[1] = self->_wave->_waveBuf[j];
            tri[2].setX(tri[1].x());
            tri[2].setY(tri[0].y());
            painter.drawConvexPolygon(tri, 3);

            QRect rect;
            rect.setTopLeft(tri[0]);
            rect.setWidth(tri[1].x() - tri[0].x() + 1);
            rect.setBottom(yBottom);
            painter.fillRect(rect, self->_wave->palette().color(QPalette::WindowText));
            i = j;
        }
        else
        {
            // 梯形
            QPoint tri[3];
            tri[0] = self->_wave->_waveBuf[i];
            tri[1] = self->_wave->_waveBuf[j];
            tri[2].setX(tri[0].x());
            tri[2].setY(tri[1].y());
            painter.drawConvexPolygon(tri, 3);

            QRect rect;
            rect.setTopLeft(tri[2]);
            rect.setWidth(tri[1].x() - tri[0].x() + 1);
            rect.setBottom(yBottom);
            painter.fillRect(rect,
                    self->_wave->palette().color(QPalette::WindowText));
            i = j;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 绘制稠密填充曲线
//
// 参数:
// painter: 绘图对象
// beginIndex: 波形起始索引
// endIndex: 波形结束索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveReviewMode::_fillDenseCurve(WaveReviewMode *self, QPainter &painter,
        int beginIndex, int endIndex)
{
    if (!self || !self->_wave)
    {
        return;
    }

    painter.setPen(self->_wave->palette().color(QPalette::WindowText));
    painter.setRenderHint(QPainter::Antialiasing, false);

    int yBottom = self->_wave->height() - 1 - self->_wave->_margin.bottom();
    for (int i = beginIndex; i <= endIndex; i++)
    {
        painter.drawLine(self->_wave->_waveBuf[i],
                QPoint(self->_wave->_xBuf[i], yBottom));
    }

    _drawDenseCurve(self, painter, beginIndex, endIndex);
}
