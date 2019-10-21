/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/20
 **/

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件，支持扫描模式，滚动模式，回顾模式及三者对应的级联模式
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_WIDGET_H
#define WAVE_WIDGET_H

#include <math.h>
#include <QList>
#include <QDateTime>
#include <QBasicTimer>
#include "IWidget.h"
#include "WaveDefine.h"
#include <FreezeDataModel.h>
#include <QPointer>
#include <QHash>

class QFont;
class QPixmap;
class WaveMode;
class WaveDataModel;
class WaveWidgetItem;
class WaveWidgetLabel;
class Configer;
class FreezeTimeIndicator;

template<typename T> class QVector;
template<typename T> class RingBuff;

struct WaveRangeInfo
{
    WaveRangeInfo() : min(SHRT_MAX), max(SHRT_MIN) {}
    WaveRangeInfo(short min, short max) : min(min), max(max) {}

    /**
     * @brief isValid check whether range info is valid
     * @return true when valid
     */
    bool isValid() const { return min != SHRT_MAX && max != SHRT_MIN; }

    /**
     * @brief clear clear the range info
     */
    void clear() { min = SHRT_MAX; max = SHRT_MIN;}

    /**
     * @brief midvalue get the middle value of the range
     * @return the middle value
     */
    short midvalue() const { return (min + max) >> 1;}

    short min;
    short max;
};

#define WAVE_X_OFFSET       (0)                 // 波形起始偏移
#define RULER_X_OFFSET      (65)                // 标尺线起始偏移
////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件，支持扫描模式，滚动模式，回顾模式及三者对应的级联模式
////////////////////////////////////////////////////////////////////////////////
class WaveWidget: public IWidget
{
    Q_OBJECT
public:
    WaveWidget(const QString &widgetName, const QString &title);
    ~WaveWidget();

    void addItem(WaveWidgetItem *item);
    void addItem(WaveWidgetLabel *item);
    void removeItem(WaveWidgetItem *item);
    void setDataModel(WaveDataModel *model);

    void updateBuffer();
    void updateBackground();

    /**
     * @brief updatePalette 更新调色板
     * @param pal
     */
    virtual void updatePalette(const QPalette &pal);

    bool isFocus();
    void getSubFocusWidget(QList<QWidget *> &subWidget) const;  // NOLINT

    /**
     * @brief waveLabel get the wave labels
     * @return  the wave label string
     */
    QString waveLabel() const;


    // 窗口是否使能
    virtual bool waveEnable()
    {
        return true;
    }

    // 获取工作模式标志
    inline int modeFlag() const
    {
        return _modeFlag;
    }

    // 获取级联状态
    inline bool isCascade() const
    {
        return _isCascade;
    }

    // 获取索引映射X坐标缓冲
    inline int xBuf(int i)
    {
        return _xBuf[i];
    }

    // get the wave range info at specific x value
    inline const WaveRangeInfo &rangeInfo(int x)
    {
        return _waveRangeInfo[x];
    }

    // 获取波形标志
    inline int flagBuf(int i) const
    {
        return _flagBuf[i];
    }

    // 获取缓冲区容量
    inline int bufSize() const
    {
        return _size;
    }

    // 获取缓冲区头索引
    inline int bufHead() const
    {
        return _head;
    }

    // 获取缓冲区尾索引
    inline int bufTail() const
    {
        return _tail;
    }

    // 获取波形区距离获取边界间距, 单位像素
    // Warning: return the left margin only, call this function when all margins are equaled.
    inline int margin() const
    {
        return _margin.left();
    }

    // 获取波形区距离获取边界间距, 单位像素
    inline QMargins qmargins() const
    {
        return _margin;
    }

    // 修改波形控件的空白边距
    // Note: set the margin to the same value
    void setMargin(int margin);

    // set wave margin
    void setMargin(const QMargins &margin);

    // 获取级联模式下, 上下波形的间距
    inline int spacing() const
    {
        return _spacing;
    }

    // 获取波形速率, 单位mm/s
    inline float waveSpeed() const
    {
        return _waveSpeed;
    }

    // 获取原始波形速率
    inline float dataRate() const
    {
        return _dataRate;
    }

    // 获取曲线宽度, 单位像素
    inline int lineWidth() const
    {
        return _lineWidth;
    }

    // 获取显示屏像素点距, 单位mm
    static float pixelWPitch()
    {
        return _pixelWPitch;
    }
    static float pixelHPitch()
    {
        return _pixelHPitch;
    }

    // 原始波形数值下限
    inline int minValue() const
    {
        return _minValue;
    }

    // 原始波形数值上限
    inline int maxValue() const
    {
        return _maxValue;
    }

    // 是否使用向下填充波形
    inline bool isFill() const
    {
        return _isFill;
    }

    // 绘制波形曲线时是否使用抗锯齿
    inline bool isAntialias() const
    {
        return _isAntialias;
    }

    // 环形缓冲空判断
    inline bool bufIsEmpty() const
    {
        return (_head == _tail);
    }

    // 环形缓冲满判断
    inline bool bufIsFull() const
    {
        return (_bufNext(_head) == _tail);
    }

    // 环形缓冲连续性判断
    inline bool bufIsContinuous() const
    {
        return (_head >= _tail);
    }

    // 获取显示的名称。
    const QString &getTitle(void) const
    {
        return _title;
    }

    // set display Title
    void setTitle(const QString &title)
    {
        if (title != _title)
        {
            _title = title;
        }
    }

    const QPoint getWavePoint(int x) const
    {
        if (x < _size)
        {
            return _waveBuf[x];
        }
        else
        {
            return QPoint(0, 0);
        }
    }

    void resetWave();
    int xToIndex(int x);

    // 重新刷新波形
    void resetWaveWidget();

    // set reset flag
    static void setWaveReset(bool flag)
    {
        resetWaveFlag = flag;
    }

    /* reimplment */
    void updateWidgetConfig();

    /**
     * @brief getAllWaveWidgets the all the created wave widgets
     * @return all
     */
    static QList<WaveWidget *> getAllWaveWidgets()
    {
        return waveWidgets;
    }

    /**
     * @brief setWaveDrawMode set the wave draw mode
     * @param mode draw mode
     */
    void setWaveDrawMode(WaveDrawMode mode);

public slots:
    void setWaveSpeed(float waveSpeed);
    void setDataRate(float dataRate);
    void setLineWidth(int lineWidth);
    void setValueRange(int min, int max);
    void selectMode(WaveModeFlag modeFlag);
    void enableCascade(bool isEnable);
    void enableFill(bool isEnable);
    void enableAntialias(bool isEnable);
    void enableGrid(bool isEnable);
    void addData(int value, int flag = 0, bool isUpdated = true);
    void reviewWave(const QDateTime &time);
    void queueData(const int *buf, int size);
    void queueData(const QVector<short> &data);
    void setID(int id);
    int getID(void) const;
    void freeze(bool flag);
    void enterFreezeReviewMode();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void timerEvent(QTimerEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

    // 返回缓冲区中的波形数据量
    inline int _bufCount() const
    {
        return (_head >= _tail) ? (_head - _tail) : (_head + _size - _tail);
    }

    // 返回缓冲区的空闲空间
    inline int _bufSpace() const
    {
        return (_head >= _tail) ?
               (_tail + _size - _head - 1) : (_tail - _head - 1);
    }

    // 返回缓冲区中地址连续的数据量
    inline int _bufCountToEnd() const
    {
        return (_head >= _tail) ? (_head - _tail) : (_size - _tail);
    }

    // 返回缓冲区中地址连续的空闲空间
    inline int _bufSpaceToEnd() const
    {
        return (_head >= _tail) ? (_size - _head - 1) : (_tail - _head - 1);
    }

    // 环形缓冲索引自加
    inline int _bufNext(int index) const
    {
        return (index == (_size - 1)) ? 0 : (index + 1);
    }

    // 环形缓冲索引自减
    inline int _bufPrev(int index) const
    {
        return (index == 0) ? (_size - 1) : (index - 1);
    }

    // 访问波形坐标序列缓冲头
    inline QPoint &_waveBufHead(int i)
    {
        return _waveBuf[(_head - 1 - i + _size) % _size];
    }

    // 访问波形坐标序列缓冲尾
    inline QPoint &_waveBufTail(int i)
    {
        return _waveBuf[(_tail + i) % _size];
    }

    void _bufPush(int x, int y, int value, int flag, bool isUpdated = true);
    void _bufPop(int n = 1);
    void _setDyBuf(int i);

protected:
    WaveWidgetLabel *_name;
    QList<WaveMode *> _modes;                 // 支持的工作模式对象列表
    /**
     * @brief _resetBuffer 复位波形
     */
    virtual void _resetBuffer();

private:
    WaveMode *_findMode(WaveModeFlag modeFlag, bool isCascade);
    void _resetBackground();
    void _startQueueWaveData();
    int _roundUp(int value, int step);
    int _roundDown(int value, int step);
    /**
     * @brief _updateRangeInfo udpate the wave range info with the point at index
     * @param index the data index
     */
    void _updateRangeInfo(int index);

    WaveMode *_mode;                          // 当前使用的工作模式
    WaveModeFlag _modeFlag;                   // 工作模式标志
    bool _isCascade;                          // 是否级联
    QList<WaveWidgetItem *> _items;           // 绘图元素对象列表
    QList<WaveWidgetLabel *> _labelItems;
    QString _title;                           // 波形控制标名
    QPixmap *_background;                     // 背景位图
    bool _isUpdateBackgroundPending;          // 是否等待更新背景位图
    WaveDataModel *_model;                    // 回顾模式时使用的数据源
    QDateTime _reviewTime;                    // 回顾的时刻, 用于回顾模式
    bool _isUpdateBufferPending;              // 是否等待更新波形缓存
    QPoint *_waveBuf;                         // 波形坐标序列缓存
    int *_dataBuf;                            // 原始波形数据缓存
    int *_xBuf;                               // 索引换算X坐标缓存
    bool *_dyBuf;                             // Y坐标是否差异大标志
    int *_flagBuf;                            // 给波形打标记的缓存
    bool *_spaceFlag;                         // 画虚线标记
    int _size;                                // 波形坐标序列容量
    int _head;                                // 波头坐标索引
    int _tail;                                // 波尾坐标索引
    QMargins _margin;                         // 波形区边缘的间距
    int _spacing;                             // 级联模式时上下道波形的间距
    float _waveSpeed;                         // 波形前进的速率, 单位mm/s
    float _dataRate;                          // 原始波形数据的速率, 单位点/s
    int _lineWidth;                           // 波形曲线宽度
    static float _pixelWPitch;                // 屏幕像素点距, 单位mm
    static float _pixelHPitch;                // 屏幕像素点距, 单位mm
    int _minValue;                            // 波形数值下限
    int _maxValue;                            // 波形数值上限
    bool _isFill;                             // 是否使用填充
    bool _isAntialias;                        // 是否使用抗锯齿绘制波形
    bool _isShowGrid;                         // 是否显示网格
    int  _id;                                 // 波形的唯一ID。
    int _spaceDataNum;                        // 画虚线所需数据个数
    bool _isFreeze;                           // isFreeze or not
    QPointer<FreezeDataModel> _freezeDataModel;  // the freeze wave data model
    FreezeTimeIndicator *_freezeIndicator;      // the freeze time indicator

    RingBuff<int> *_queuedDataBuf;            // 波形延时缓冲区
    float _queuedDataRate;                    // 创建波形延时缓冲时参照的速度率
    QBasicTimer _dequeueTimer;                // 模拟采样间隔的定时器
    int _dequeueSizeEachTime;                 // 模拟采样间隔的数据量

    bool colorGradationScanMode;              // 色阶法扫描画波形
    QHash<int, WaveRangeInfo> _waveRangeInfo;  // 记录了每个整形x坐标所对应的Y坐标的值的范围

    static QList<WaveWidget*> waveWidgets;    // store the created wave widgets

    static const int DEQUEUE_FREQ = 25;       // 模拟采样频率, 单位Hz
    static const int QUEUE_SECONDS = 4;       // 允许的最大延时时长, 单位秒
    static const int scanLineSpace = 12;         // 空白扫描宽度12个像素点
    static bool resetWaveFlag;                 // reset wave not draw wave

    friend class WaveMode;
    friend class WaveNormalMode;
    friend class WaveCascadeMode;
    friend class WaveScanMode;
    friend class BarGraphScanMode;
    friend class FreezeWaveReviewMode;
    friend class WaveScanCascadeMode;
    friend class WaveScrollMode;
    friend class WaveScrollCascadeMode;
    friend class WaveReviewMode;
    friend class WaveReviewCascadeMode;
    friend class WaveWidgetItem;
};

#endif
