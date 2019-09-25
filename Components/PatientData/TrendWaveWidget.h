/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/3
 **/

#pragma once
#include "IWidget.h"
#include "TrendSubWaveWidget.h"
#include "TrendDataDefine.h"
#include <QScrollArea>
#include "EventStorageManager.h"
#include "PatientDefine.h"

class QHBoxLayout;
class QVBoxLayout;
class TrendWaveWidget : public IWidget
{
    Q_OBJECT
public:
    TrendWaveWidget();
    ~TrendWaveWidget();

public:
    /**
     * @brief setWidgetSize 设置波形区窗口的宽度和高度
     * @param w 宽度
     * @param h 高度
     */
    void setWidgetSize(int w, int h);

    /**
     * @brief pageUpParam 上翻页
     */
    void pageUpParam();

    /**
     * @brief pageDownParam 下翻页
     */
    void pageDownParam();

    /**
     * @brief setTimeInterval 设置时间间隔
     * @param timeInterval 时间间隔
     */
    void setTimeInterval(ResolutionRatio timeInterval);

    /**
     * @brief setWaveNumber 设置显示波形数
     * @param num 波形数
     */
    void setWaveNumber(int num);

    /**
     * @brief setRulerLimit 设置子窗口上下标尺限
     * @param down 下标尺限
     * @param up 上标尺限
     */
    void setRulerLimit(int index, int down, int up, int scale);

    /**
     * @brief loadTrendData 根据子参数ID将一页趋势数据载入容器
     * @param id 子参数ID
     * @param startIndex 开始数据的索引
     * @param endIndex 结束数据的索引
     */
    void loadTrendData(SubParamID id);

    /**
     * @brief dataIndex 获取开始与结束数据的索引
     * @param startIndex 开始数据的索引
     * @param endIndex 结束数据的索引
     */
    void dataIndex(int &startIndex, int &endIndex);


    /**
     * @brief getTrendData 对应位置的趋势图数据包
     * @param startIndex
     * @param endIndex
     */
    void trendDataPack(int startIndex, int endIndex);

    /**
     * @brief updateTimeRange 根据当前数据页和时间间隔更新显示数据时间范围
     */
    void updateTimeRange(void);

    /**
     * @brief trendWaveReset 趋势波形复位
     */
    void trendWaveReset(void);

    /**
     * @brief 获取趋势图打印的数据及其事件发生时刻列表
     */
    const QList<TrendGraphInfo> getTrendGraphPrint();
    const QList<BlockEntry> getEventList();

    /**
     * @brief getPatientInfo 获取病人信息
     * @return
     */
    const PatientInfo &getPatientInfo();

    /**
     * @brief setHistoryDataPath 设置历史回顾数据的文件路径
     * @param path 文件路径
     */
    void setHistoryDataPath(QString path);

    /**
     * @brief setHistoryData 此刻的趋势表是否为历史数据
     * @param flag 标志
     */
    void setHistoryData(bool flag);

    QList<SubParamID> getCurParamList(void);

    void setAllParamAutoRuler(void);

public slots:
    /**
     * @brief leftMoveCoordinate 左移坐标
     */
    void leftMoveCoordinate();

    /**
     * @brief rightMoveCoordinate 右移坐标
     */
    void rightMoveCoordinate();

    /**
     * @brief leftMoveCursor 左移游标
     */
    void leftMoveCursor();

    /**
     * @brief rightMoveCursor 右移游标
     */
    void rightMoveCursor();

    /**
     * @brief leftMoveEvent 左移事件
     */
    void leftMoveEvent();

    /**
     * @brief rightMoveEvent 右移事件
     */
    void rightMoveEvent();

protected:
    void paintEvent(QPaintEvent *event);
    void showEvent(QShowEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    /**
     * @brief _trendLayout 将趋势数据加载入子窗口
     */
    void _trendLayout(void);

    /**
     * @brief _getTrendData 获取趋势图数据
     */
    void _getTrendData(void);

    /**
     * @brief _initWaveSubWidget 初始化一次波形窗口中子窗口控件
     */
    void _initWaveSubWidget(void);

    /**
     * @brief _getCursorPos 根据此时游标索引的时间获取在窗口的像素位置
     * @param t 游标索引的时间
     * @return 像素位置
     */
    double _getCursorPos(unsigned t);               // 获取坐标位置

    /**
     * @brief _getTimeIndex 获取时间对应的数据索引
     * @param t
     */
    int _getTimeIndex(unsigned t);

    /**
     * @brief _getCursorTime 获取游标位置时间
     * @param pos   游标位置
     */
    unsigned _getCursorTime(double pos);

    /**
     * @brief _calculationPage 获取当前时间间隔的总数据页并设置当前页为1
     */
    void _calculationPage(void);                    // 计算当前数据量的页数

    /**
     * @brief _updateEventIndex 刷新存储事件发生时间
     */
    void _updateEventIndex();

    /**
     * @brief _findIndex 获取当前时刻在趋势数据中的索引
     * @param timeStamp
     * @return
     */
    int _findIndex(unsigned timeStamp);

    /**
     * @brief _findPackIndex 查找当前时刻在Pack中的索引
     * @param timeStamp
     * @return
     */
    int _findPackIndex(unsigned timeStamp);

    /**
     * @brief getTrendGraphType 获取id对应的趋势图类型
     * @param id 子参数ID
     * @return
     */
    TrendGraphType getTrendGraphType(SubParamID id);

    /**
     * @brief _loadEventInfoList 加载事件信息列表
     */
    void _loadEventInfoList();

    /**
     * @brief getTrendDataPackage 获取索引对应的趋势数据
     * @param index
     * @return
     */
    TrendDataPackage getTrendDataPackage(int index);

private:
    QVBoxLayout *_mainLayout;
    QVBoxLayout *_hLayoutTrend;

    ResolutionRatio _timeInterval;
    int _waveRegionWidth;
    int _oneFrameWidth;

    int _cursorPosIndex;                            //游标位置在存储数据结构容器中的索引值
    unsigned _currentCursorTime;

    IWidget *_subWidget;

    int _displayGraphNum;                           // 一屏显示的波形数
    int _totalGraphNum;                             // 趋势组显示的总波形数
    int _curVScroller;

    QList<TrendDataPackage *> _trendDataPack;       // 趋势数据包
    int _totalPage;                                 // 总数据页数 *
    int _currentPage;                               // 当前页数 *

    unsigned _leftTime;                             // 趋势图左边时间 *
    unsigned _rightTime;                            // 趋势图右边时间 *

    QList<TrendSubWaveWidget *> _subWidgetList;     // 子波形窗口容器
    QList<SubParamID> _subParams;                   // 子参数组
    QList<SubParamID> _curDisplaySubList;           // 当前显示子参数集合
    int _curIndex;
    TrendGraphInfo _trendGraphInfo;                 // 趋势图数据集合
    QList<BlockEntry> _eventList;            // 报警类型和时间
    QList<TrendGraphInfo> _infosList;                    // 打印趋势图数据链表
    PatientInfo _patientInfo;                        // 病人信息

    bool _isHistory;                                // 历史回顾标志
    QString _historyDataPath;                       // 历史数据路径
    QVector<BlockEntry> eventBlockList;               // 事件信息列表
    QVector<BlockEntry> trendBlockList;               // 趋势数据信息列表
    IStorageBackend *_backend;
};
