/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/3
 **/

////////////////////////////////////////////////////////////////////////////////
// 说明：
// WaveWidget是一个波形控件，支持扫描模式，滚动模式，回顾模式及三者对应的级联模式
////////////////////////////////////////////////////////////////////////////////
#include <QFont>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include "WaveWidget.h"
#include "WaveDataModel.h"
#include "WaveScanMode.h"
#include "WaveScanCascadeMode.h"
#include "WaveScrollMode.h"
#include "WaveScrollCascadeMode.h"
#include "WaveReviewMode.h"
#include "WaveReviewCascadeMode.h"
#include "WaveWidgetLabel.h"
#include "LanguageManager.h"
#include "FloatHandle.h"
#include "Framework/Utility/RingBuff.h"
#include "SystemManager.h"
#include "WaveWidgetItem.h"
#include "WaveWidgetLabel.h"
#include "Debug.h"
#include "WindowManager.h"
#include "FreezeManager.h"
#include "FreezeWaveReviewMode.h"
#include "FreezeTimeIndicator.h"
#include "LayoutManager.h"
#include "IConfig.h"

float WaveWidget::_pixelWPitch = 0.248;
float WaveWidget::_pixelHPitch = 0.248;
bool WaveWidget::resetWaveFlag = false;
QList<WaveWidget *> WaveWidget::waveWidgets;

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 构造函数
//
// 参数:
// type: 波形参数ID
////////////////////////////////////////////////////////////////////////////////
WaveWidget::WaveWidget(const QString &widgetName, const QString &title) : IWidget(widgetName),
    _modes(), _mode(NULL), _modeFlag(SCAN_MODE), _isCascade(false), _items(), _labelItems(), _title(title),
    _background(NULL), _isUpdateBackgroundPending(true), _model(NULL),
    _reviewTime(), _isUpdateBufferPending(true), _waveBuf(NULL), _dataBuf(NULL), _xBuf(NULL),
    _dyBuf(NULL), _flagBuf(NULL), _size(0), _head(0), _tail(0), _margin(QMargins(2, 2, 2, 2)), _spacing(2),
    _waveSpeed(12.5), _dataRate(125), _lineWidth(1), _minValue(-32768),
    _maxValue(32767),
    _isFill(false), _isAntialias(false), _isShowGrid(false), _id(-1), _isFreeze(false),
    _queuedDataBuf(NULL), _queuedDataRate(0), _dequeueTimer(), _dequeueSizeEachTime(0)
{
    // 波形区不能被选中
    setEnabled(false);

    _spaceFlag = NULL;
    _pixelWPitch = systemManager.getScreenPixelWPitch();
    _pixelHPitch = systemManager.getScreenPixelHPitch();

    // 禁用Qt背景预绘, 由波形控件自己管理背景图层, 优化性能
    setAttribute(Qt::WA_OpaquePaintEvent, true);

    // 初始化工作模式对象列表
    _modes.append(new WaveScanMode(this));
    _modes.append(new WaveScrollMode(this));
    // _modes.append(new WaveReviewMode(this));
    _modes.append(new FreezeWaveReviewMode(this));

    // 选择默认工作模式
    _mode = _findMode(_modeFlag, _isCascade);
    if (_mode)
    {
        _mode->enter();
    }

    _name = new WaveWidgetLabel(" ", Qt::AlignLeft | Qt::AlignVCenter, this);
    addItem(_name);

    _freezeIndicator = new FreezeTimeIndicator(this);
    _freezeIndicator->setVisible(false);


    // 根据参数类型初始化波形与标签的颜色
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    palette.setColor(QPalette::WindowText, Qt::white);
    setPalette(palette);


    connect(&freezeManager, SIGNAL(freeze(bool)), this, SLOT(freeze(bool)));
    connect(&freezeManager, SIGNAL(freezeReview()), this, SLOT(enterFreezeReviewMode()));

    int lineW = 1;
    systemConfig.getNumValue("Others|WaveLine", lineW);
    if (lineW > 0 && lineW < 10)
    {
        _lineWidth = lineW;
    }

    int drawMode = WAVE_DRAW_MODE_COLOR;
    systemConfig.getNumValue("Others|WaveDrawMode", drawMode);
    if (drawMode == WAVE_DRAW_MODE_MONO)
    {
        colorGradationScanMode = false;
    }
    else
    {
        colorGradationScanMode = true;
    }

    waveWidgets.append(this);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 析构函数
////////////////////////////////////////////////////////////////////////////////
WaveWidget::~WaveWidget()
{
    _dequeueTimer.stop();

    if (_queuedDataBuf)
    {
        delete _queuedDataBuf;
        _queuedDataBuf = NULL;
    }

    qDeleteAll(_items);
    _items.clear();

    qDeleteAll(_labelItems);
    _labelItems.clear();

    qDeleteAll(_modes);
    _modes.clear();
    _mode = NULL;

    if (_model)
    {
        // 注意波形控件不能删除模型对象
        _model = NULL;
    }

    if (_background)
    {
        delete _background;
        _background = NULL;
    }

    if (_waveBuf)
    {
        delete[] _waveBuf;
        _waveBuf = NULL;
    }

    if (_dataBuf)
    {
        delete[] _dataBuf;
        _dataBuf = NULL;
    }

    if (_xBuf)
    {
        delete[] _xBuf;
        _xBuf = NULL;
    }

    if (_dyBuf)
    {
        delete[] _dyBuf;
        _dyBuf = NULL;
    }

    if (_flagBuf)
    {
        delete[] _flagBuf;
        _flagBuf = NULL;
    }

    if (_spaceFlag)
    {
        delete[] _spaceFlag;
        _spaceFlag = NULL;
    }

    waveWidgets.removeOne(this);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 添加绘图元素, 接管其生命周期
//
// 参数:
// item: 绘图元素对象
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::addItem(WaveWidgetItem *item)
{
    if (item && !_items.contains(item))
    {
        _items.append(item);
        item->update();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 添加绘图元素, 接管其生命周期
//
// 参数:
// item: 绘图元素对象
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::addItem(WaveWidgetLabel *item)
{
    if (item && !_labelItems.contains(item))
    {
        _labelItems.append(item);
        item->update();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 移除绘图元素, 放弃管理其生命周期
//
// 参数:
// item: 绘图元素对象
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::removeItem(WaveWidgetItem *item)
{
    if (item)
    {
        if (_items.removeOne(item))
        {
            item->update();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 指定数据模型, 注意波形控件不控制模型对象的生命周期
//
// 参数:
// model: 新数据模型对象
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::setDataModel(WaveDataModel *model)
{
    if (model == _model)
    {
        return;
    }

    if (_mode)
    {
        _mode->exit();
    }

    _model = model;

    if (_mode)
    {
        _mode->enter();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 功能:是否有焦点
////////////////////////////////////////////////////////////////////////////////
bool WaveWidget::isFocus()
{
    bool bret = hasFocus();
    if (bret)
    {
        return bret;
    }

    int count = _labelItems.count();
    if (0 == count)
    {
        return bret;
    }

    for (int i = 0; i < count; ++i)
    {
        QWidget *widget = qobject_cast<QWidget *>(_labelItems.at(i));
        if (NULL != widget && widget->hasFocus())
        {
            bret = true;
            break;
        }
    }

    return bret;
}

////////////////////////////////////////////////////////////////////////////////
// 功能:获取子菜单控件
////////////////////////////////////////////////////////////////////////////////
QList<QWidget *> WaveWidget::getSubFocusWidget() const
{
    QList<QWidget*> subWidget;

    int count = _labelItems.count();
    if (0 == count)
    {
        return subWidget;
    }

    for (int i = count - 1; i >= 0; --i)
    {
        QWidget *widget = qobject_cast<QWidget *>(_labelItems.at(i));
        if (NULL != widget && (Qt::NoFocus != _labelItems.at(i)->focusPolicy()))
        {
            subWidget.append(widget);
        }
    }
    return subWidget;
}

QString WaveWidget::waveLabel() const
{
    return _name->text();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 修改波形控件的空白边距, set all the margin to the same value
//
// 参数:
// margin: 边距值, 单位像素
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::setMargin(int margin)
{
    this->setMargin(QMargins(margin, margin, margin, margin));
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 修改波形控件的空白边距
//
// 参数:
// margin: 边距值, 单位像素
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::setMargin(const QMargins &margin)
{
    _margin = margin;
    updateBuffer();
    updateBackground();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 缓存波形坐标及数值
//
// 参数:
// (x, y): 波形点坐标
// value: 原始波形数值
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::_bufPush(int x, int y, int value, int flag, bool isUpdated)
{
    if (bufIsFull())
    {
        _tail = _bufNext(_tail);
    }

    if (!_waveBuf || !_dataBuf || !_flagBuf)
    {
        return;
    }

    _waveBuf[_head].setX(x);
    _waveBuf[_head].setY(y);
    _dataBuf[_head] = value;
    _flagBuf[_head] = flag;

    _updateRangeInfo(_head);

    _spaceFlag[_head] = false;
    if (flag & 0x4000)
    {
        int spaceNum = 0;
        int j = _head - 1;
        int count = 0;
        while (j > 0)
        {
            spaceNum += _spaceFlag[j--];
            ++count;
            if (_spaceDataNum == count)
            {
                break;
            }
        }

        if (count == _spaceDataNum)
        {
            if (_spaceFlag[_head - 1] && _spaceDataNum != spaceNum)
            {
                _spaceFlag[_head] = true;
            }
            else if (0 == spaceNum)
            {
                _spaceFlag[_head] = true;
            }
        }
    }

    int i = _head;

    _head = _bufNext(_head);

    _setDyBuf(i);

    if (isUpdated == false)
    {
        return;
    }

    QRegion region;
    if (_mode->updateRegion(region) && !region.isEmpty())
    {
        update(region);
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 删除波尾数据
//
// 参数:
// n: 数量
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::_bufPop(int n)
{
    if (0 == _size)
    {
        return;
    }

    if (n > _size) {
        n = _size;
    }

    int startX = _xBuf[_tail];
    _tail = (_tail + n) % _size;
    int endX = _xBuf[_tail];
    /* clear the wave range info */
    if (startX > endX)
    {
        while (startX <= _xBuf[_size - 1])
        {
            _waveRangeInfo[startX].clear();
            startX++;
        }
        startX = _xBuf[0];
    }

    while (startX < endX)
    {
        _waveRangeInfo[startX].clear();
        startX++;
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 计算waveBuf[i - 1].y()与waveBuf[i].y()的差异
//
// 参数:
// i: 波形点索引
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::_setDyBuf(int i)
{
    if (!_waveBuf || !_dyBuf)
    {
        return;
    }

    if (_bufCount() > 1)
    {
        _dyBuf[i] = qAbs(_waveBuf[i].y() - _waveBuf[_bufPrev(i)].y()) > 1;
    }
    else
    {
        _dyBuf[i] = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 请求更新波形缓存, 波形控件能够自动合并多个请求, 然后在合适的时机执行更新操作
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::updateBuffer()
{
    _isUpdateBufferPending = true;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 请求更新背景位图, 波形控件能够自动合并多个请求, 然后在合适的时机执行更新操作
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::updateBackground()
{
    _isUpdateBackgroundPending = true;
    update();
}

void WaveWidget::updatePalette(const QPalette &pal)
{
    setPalette(pal);
    updateBackground();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 调整波形缓存, 确保可以存放足够控件显示的波形数据
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::_resetBuffer()
{
    if (!_mode)
    {
        return;
    }


    int n = _mode->dataSize();
    if ((n > 0) && (_size != n))
    {
        _size = n;

        if (_waveBuf)
        {
            delete[] _waveBuf;
        }
        _waveBuf = new QPoint[_size];

        if (_dataBuf)
        {
            delete[] _dataBuf;
        }
        _dataBuf = new int[_size];

        if (_xBuf)
        {
            delete[] _xBuf;
        }
        _xBuf = new int[_size];

        if (_dyBuf)
        {
            delete[] _dyBuf;
        }
        _dyBuf = new bool[_size];

        if (_flagBuf)
        {
            delete[] _flagBuf;
        }
        _flagBuf = new int[_size];

        if (_spaceFlag)
        {
            delete[] _spaceFlag;
        }
        _spaceFlag = new bool[_size];
    }

    _mode->prepareTransformFactor();

    _head = 0;
    _tail = 0;

    // 重建索引换算X坐标缓存
    for (int i = 0; i < _size; i++)
    {
        _xBuf[i] = _mode->indexToX(i);
    }

    // clear the wave range info
    _waveRangeInfo.clear();

    memset(_spaceFlag, 0, _size);

    // 若已启动了延时模式, 则需要检查是否需要修改延时缓冲区容量
    if (_queuedDataBuf && !isZero(_dataRate)
            && !isEqual(_queuedDataRate, _dataRate))
    {
        delete _queuedDataBuf;
        _queuedDataBuf = new RingBuff<int>(qRound(_dataRate * QUEUE_SECONDS));
        _queuedDataRate = _dataRate;
        _dequeueSizeEachTime = ceilf(_dataRate / DEQUEUE_FREQ);
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 对于给定的数值value, 计算大于等于value且能整除step的数值
////////////////////////////////////////////////////////////////////////////////
int WaveWidget::_roundUp(int value, int step)
{
    while (value % step)
    {
        value++;
    }
    return value;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 对于给定的数值value, 计算小于等于value且能整除step的数值
////////////////////////////////////////////////////////////////////////////////
int WaveWidget::_roundDown(int value, int step)
{
    while (value % step)
    {
        value--;
    }
    return value;
}

void WaveWidget::_updateRangeInfo(int index)
{
    int x = _waveBuf[index].x();
    int y = _waveBuf[index].y();
    if (y > _waveRangeInfo[x].max)
    {
        _waveRangeInfo[x].max = y;
    }
    if (y < _waveRangeInfo[x].min)
    {
        _waveRangeInfo[x].min = y;
    }

    /* perform interpolation */
    if (index > 0) {
        int lastX = _waveBuf[index - 1].x();
        int lastY = _waveBuf[index - 1].y();
        int curX = lastX + 1;
        while (curX < x) {
            _waveRangeInfo[curX].clear();
            int curY = lastY + (y - lastY) * (curX - lastX) / (x - lastX);
            _waveRangeInfo[curX].min = curY;
            _waveRangeInfo[curX].max = curY;
            curX++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 调整背景位图
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::_resetBackground()
{
    // 确保背景位图总是与控件尺寸一致
    if (_background)
    {
        if ((_background->width() != width()) || (_background->height() != height()))
        {
            delete _background;
            _background = NULL;
        }
    }

    if (!_background)
    {
        _background = new QPixmap(width(), height());
    }

    QPainter painter(_background);

    // 底色
    painter.fillRect(_background->rect(), palette().window());

    // 网格
    if (_isShowGrid)
    {
        painter.setPen(QPen(QColor(50, 50, 50), 1, Qt::DotLine));
        int xstep = qRound(5 / _pixelWPitch);       // 网格间隔固定5mm
        int ystep = qRound(5 / _pixelHPitch);
        QPoint gridLeftTop = mapFromParent(QPoint(_roundUp(geometry().left(), xstep),
                                           _roundUp(geometry().top(), ystep)));
        QPoint gridRightBottom = mapFromParent(QPoint(_roundDown(geometry().right(), xstep),
                                               _roundDown(geometry().bottom(), ystep)));
        QPoint parentLeftTop = mapFromParent(QPoint(0, 0));

        for (int x = gridLeftTop.x(); x <= gridRightBottom.x(); x += xstep)
        {
            painter.drawLine(x, parentLeftTop.y(), x, height());
        }
        for (int y = gridLeftTop.y(); y <= gridRightBottom.y(); y += ystep)
        {
            painter.drawLine(parentLeftTop.x(), y, width(), y);
        }
    }

    // 其它背景元素
    foreach(WaveWidgetItem *item, _items)
    {
        if (item && item->isBackground() && item->isVisible())
        {
            item->paintItem(painter);
        }
    }

//    // DEMO字样
//    if (_isInDemo)
//    {
//        QFont font = _titleFont;
//        // 限制字号, 否则SonosatH的大波形模式中, DEMO字号太大
//        font.setPixelSize(qMin(height() / 2, 64));
//        font.setBold(true);
//        painter.setFont(font);
//        painter.setPen(Qt::white);
//        painter.drawText(_background->rect(), Qt::AlignCenter, trs("DEMO"));
//    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 复位波形扫描或滚动
//
// 参数:
// 无
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::resetWave()
{
    _head = 0;
    _tail = 0;
    _waveRangeInfo.clear();

    update();
}

int WaveWidget::xToIndex(int x)
{
    if (!_mode)
    {
        return 0;
    }

    return _mode->xToIndex(x);
}

////////////////////////////////////////////////////////////////////////////////
// 刷新波形
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::resetWaveWidget()
{
    _resetBuffer();
    update();
}

void WaveWidget::updateWidgetConfig()
{
    resetWaveWidget();
    loadConfig();
}

void WaveWidget::setWaveDrawMode(WaveDrawMode mode)
{
    this->colorGradationScanMode = mode == WAVE_DRAW_MODE_COLOR;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 指定波形前进的速率, 将调整波形缓存, 并触发重绘
//
// 参数:
// waveSpeed: 波形速率, 单位mm/s
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::setWaveSpeed(float waveSpeed)
{
    if (isEqual(_waveSpeed, waveSpeed))
    {
        return;
    }

    _waveSpeed = waveSpeed;

    updateBuffer();

    if (_model)
    {
        _model->update();
    }

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 指定原始波形数据的速率, 将调整波形缓存, 并触发重绘
//
// 参数:
// dataRate: 原始波形数据的速率, 单位点/s
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::setDataRate(float dataRate)
{
    if (isEqual(_dataRate, dataRate))
    {
        return;
    }

    _dataRate = dataRate;

    updateBuffer();

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 指定波形曲线宽度
//
// 参数:
// lineWidth: 波形曲线宽度, 单位像素
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::setLineWidth(int lineWidth)
{
    if ((lineWidth > 0) && (lineWidth < 10) && (_lineWidth != lineWidth))
    {
        _lineWidth = lineWidth;
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 指定原始波形数据的数值范围
//
// 参数:
// min: 下限
// max: 上限
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::setValueRange(int min, int max)
{
    if (!_mode)
    {
        return;
    }

    _minValue = min;
    _maxValue = max;
    /* clear range info when the value range is change */
    _waveRangeInfo.clear();

    _mode->prepareTransformFactor();
    _mode->valueRangeChanged();

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 切换波形工作模式
//
// 参数:
// modeFlag: 波形工作模式
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::selectMode(WaveModeFlag modeFlag)
{
    if (_modeFlag == modeFlag)
    {
        return;
    }

    _modeFlag = modeFlag;

    if (_mode)
    {
        _mode->exit();
    }

    _mode = _findMode(_modeFlag, _isCascade);

    if (_mode)
    {
        _mode->enter();
    }

    updateBuffer();
    updateBackground();

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 打开或关闭级联
//
// 参数:
// isEnable: true表示打开级联, false表示关闭级联
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::enableCascade(bool isEnable)
{
    if (_isCascade == isEnable)
    {
        return;
    }

    _isCascade = isEnable;

    if (_mode)
    {
        _mode->exit();
    }

    _mode = _findMode(_modeFlag, _isCascade);

    if (_mode)
    {
        _mode->enter();
    }

    updateBuffer();
    updateBackground();

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 打开或关闭填充
//
// 参数:
// isEnable: true表示打开填充, false表示关闭填充
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::enableFill(bool isEnable)
{
    if ((_isFill == isEnable) || (!_mode))
    {
        return;
    }

    _isFill = isEnable;

    _mode->prepareTransformFactor();

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 指定是否使用波形曲线绘制是否使用抗锯齿
//
// 参数:
// isEnable: true表示使用抗锯齿, false表示不使用抗锯齿
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::enableAntialias(bool isEnable)
{
    if (_isAntialias == isEnable)
    {
        return;
    }

    _isAntialias = isEnable;

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 指定是否显示网格背景
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::enableGrid(bool isEnable)
{
    if (_isShowGrid == isEnable)
    {
        return;
    }

    _isShowGrid = isEnable;

    updateBackground();

    update();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 接收原始波形数据
//
// 参数:
// data: 原始波形数据
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::addData(int value, int flag, bool isUpdated)
{
    if (!isVisible())
    {
        return;
    }

    if (_isFreeze)
    {
        return;
    }

    if (!_mode)
    {
        return;
    }

    if (_isUpdateBufferPending)
    {
        return;
    }

    if (resetWaveFlag)
    {
        return;
    }

    _mode->addData(value, flag, isUpdated);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 缓冲波形数据
//
// 参数:
// buf: 波形数据缓存
// size: 数据量
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::queueData(const int *buf, int size)
{
    if (!_queuedDataBuf)
    {
        _startQueueWaveData();
        if (!_queuedDataBuf)
        {
            return;
        }
    }

    // 防止误差累积
    while (!_queuedDataBuf->isEmpty())
    {
        addData(_queuedDataBuf->takeTail());
    }

    _queuedDataBuf->push(buf, size);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 缓冲波形数据
//
// 参数:
// data: 波形数据数组
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::queueData(const QVector<short> &data)
{
    if (!_queuedDataBuf)
    {
        _startQueueWaveData();
        if (!_queuedDataBuf)
        {
            return;
        }
    }

    // 防止误差累积
    while (!_queuedDataBuf->isEmpty())
    {
        addData(_queuedDataBuf->takeTail());
    }

    int n = data.size();
    for (int i = 0; i < n; i++)
    {
        int value = data[i];
        _queuedDataBuf->push(value);
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 设置波形的ID。
//
// 参数:
// id: 波形ID。
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::setID(int id)
{
    _id = id;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 获取波形的ID。
//
// 参数:
// 无
//
// 返回值:
// ID
////////////////////////////////////////////////////////////////////////////////
int WaveWidget::getID(void) const
{
    return _id;
}

/**
 * @brief WaveWidget::freeze
 * @param flag
 */
void WaveWidget::freeze(bool flag)
{
    if (_isFreeze == flag || !isVisible())
    {
        return;
    }

    _isFreeze = flag;

    if (_isFreeze)
    {
        _freezeDataModel = freezeManager.getWaveDataModel(this->getID());
    }
    else
    {
        // free the data model
        selectMode(SCAN_MODE);
        resetWave();
    }
}

void WaveWidget::enterFreezeReviewMode()
{
    if (_isFreeze && isVisible())
    {
        selectMode(FREEZE_REVIEW_MODE);
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 回顾波形
//
// 参数:
// time: 回顾指定时刻的波形
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::reviewWave(const QDateTime &time)
{
    if (!_mode)
    {
        return;
    }

    _mode->reviewWave(time);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 重绘波形控件, 支持局部重绘
//
// 参数:
// e: 事件对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::paintEvent(QPaintEvent *e)
{
    if (!_mode)
    {
        return;
    }

    if (resetWaveFlag)
    {
        return;
    }

    if (_isUpdateBufferPending)
    {
        _resetBuffer();
        _isUpdateBufferPending = false;
    }

    if (_isUpdateBackgroundPending)
    {
        _resetBackground();
        _isUpdateBackgroundPending = false;
    }

    _mode->preparePaint();

    if (e->region().rectCount() == 1)
    {
        QPainter painter(this);
        QRect rect = e->rect();

        // 绘背景
        painter.drawPixmap(rect, *_background, rect);

        // 绘除背景及波形外的绘图元素
        int n = _items.size();
        for (int i = 0; i < n; i++)
        {
            WaveWidgetItem *item = _items[i];

            if (!item)
            {
                continue;
            }

            if (!item->isBackground() && item->isVisible()
                    && item->geometry().intersects(rect))
            {
                item->paintItem(painter);
            }
        }

        // 绘波形
        _mode->paintWave(painter, rect);
    }
    else
    {
        QPainter painter(this);
        QVector<QRect> rects = e->region().rects();
        int cnt = rects.size();
        for (int i = 0; i < cnt; i++)
        {
            QRect rect = rects.at(i);

            // 绘背景
            painter.drawPixmap(rect, *_background, rect);

            // 绘除背景及波形外的绘图元素
            int n = _items.size();
            for (int i = 0; i < n; i++)
            {
                WaveWidgetItem *item = _items[i];
                if (!item)
                {
                    continue;
                }

                if (!item->isBackground() && item->isVisible()
                        && item->geometry().intersects(rect))
                {
                    item->paintItem(painter);
                }
            }

            // 绘波形
            if (_mode->supportPartialPaint())
            {
                _mode->paintWave(painter, rect);
            }
        }

        // 不支持局部更新
        if (!_mode->supportPartialPaint())
        {
            _mode->paintWave(painter, QRect());
        }
    }

    IWidget::paintEvent(e);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 调整波形控件尺寸
//
// 参数:
// e: 事件对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::resizeEvent(QResizeEvent *e)
{
    IWidget::resizeEvent(e);

    updateBuffer();
    updateBackground();

    if (_model)
    {
        _model->update();
    }

    if (_freezeIndicator)
    {
        _freezeIndicator->resize(this->rect());
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 处理定时器事件
//
// 参数:
// e: 事件对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::timerEvent(QTimerEvent *e)
{
    if (!e || !_mode)
    {
        return;
    }

    if (e->timerId() == _dequeueTimer.timerId())
    {
        for (int i = 0; i < _dequeueSizeEachTime; i++)
        {
            if (_queuedDataBuf->isEmpty())
            {
                break;
            }

            addData(_queuedDataBuf->takeTail());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 显示事件
//
// 参数:
// e: 事件对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::showEvent(QShowEvent *e)
{
    Q_UNUSED(e)
    if (!_mode)
    {
        return;
    }

    if (_mode->maxUpdateRate() > 0)
    {
//        _updateTimer.start(1000 / qMin(_mode->maxUpdateRate(), 30), this);
    }

    if (_queuedDataBuf)
    {
        _dequeueTimer.start(1000 / DEQUEUE_FREQ, this);
    }

    if (_model)
    {
        _model->update();
    }

    if (layoutManager.getUFaceType() == UFACE_MONITOR_BIGFONT ||
            layoutManager.getUFaceType() == UFACE_MONITOR_OXYCRG)
    {
        _name->setFocusPolicy(Qt::NoFocus);
    }
    else
    {
        _name->setFocusPolicy(Qt::StrongFocus);
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 隐藏事件
//
// 参数:
// e: 事件对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e)
    _dequeueTimer.stop();
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 检索波形模式对象
//
// 参数:
// modeFlag: 波形模式
// isCascade: 是否级联
//
// 返回值:
// 波形模式对象
////////////////////////////////////////////////////////////////////////////////
WaveMode *WaveWidget::_findMode(WaveModeFlag modeFlag, bool isCascade)
{
    int n = _modes.size();
    for (int i = 0; i < n; i++)
    {
        if (!_modes.at(i))
        {
            continue;
        }

        if (_modes.at(i)->match(modeFlag, isCascade))
        {
            return _modes.at(i);
        }
    }

    qDebug("Wave mode (%d, %d) not supported yet!", static_cast<int>(modeFlag), isCascade);
    return _modes.at(0);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 启动波形数据缓存
////////////////////////////////////////////////////////////////////////////////
void WaveWidget::_startQueueWaveData()
{
    if (!_queuedDataBuf)
    {
        _queuedDataBuf = new RingBuff<int>(qRound(_dataRate * QUEUE_SECONDS));
        _queuedDataRate = _dataRate;
        _dequeueSizeEachTime = ceilf(_dataRate / DEQUEUE_FREQ);
    }

    if (isVisible())
    {
        _dequeueTimer.start(1000 / DEQUEUE_FREQ, this);
    }
}
