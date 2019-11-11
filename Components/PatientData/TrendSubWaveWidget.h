/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/17
 **/

#pragma once
#include "IWidget.h"
#include "TrendDataDefine.h"
#include "QDebug"

struct TrendParamDesc
{
    TrendParamDesc() : max(0), min(0),
        start(0), end(0), scale(1)
    {}
    double max;        // 数据范围最大值
    double min;        // 数据范围最小值
    int start;      // 坐标开始像素点
    int end;        // 坐标结束像素点
    int scale;
};

class TrendSubWaveWidget : public IWidget
{
    Q_OBJECT
public:
    explicit TrendSubWaveWidget(SubParamID id = SUB_PARAM_HR_PR, TrendGraphType type = TREND_GRAPH_TYPE_NORMAL);
    ~TrendSubWaveWidget();

    void setWidgetParam(SubParamID id, TrendGraphType type);

    /**
     * @brief trendDataInfo 载入趋势图数据
     * @param info 趋势数据
     */
    void trendDataInfo(const TrendGraphInfo &info);

    /**
     * @brief loadTrendSubWidgetInfo 载入趋势图数据的范围值
     * @param info 横纵坐标的实际像素范围
     */
    void loadTrendSubWidgetInfo(const TrendSubWidgetInfo &info);

    /**
     * @brief getLimitMax getLimitMin getLimitScale 获取超限信息
     * @return
     */
    int getLimitMax();
    int getLimitMin();
    int getLimitScale();

public:
    /**
     * @brief setThemeColor 设置子窗口主题色
     * @param color 主题颜色
     */
    void setThemeColor(QColor color);

    /**
     * @brief setRulerRange 设置标尺上下限
     * @param down 下限
     * @param up 上限
     */
    void setRulerRange(int down, int up, int scale);

    /**
     * @brief getUnitType
     * @return
     */
    UnitType getUnitType();

    /**
     * @brief getAutoRuler 获取是否为自动标尺
     * @return
     */
    int getAutoRuler(void);

    /**
     * @brief setTimeRange 设置两端时间范围
     * @param leftTime 左侧时间
     * @param rightTime 右侧时间
     */
    void setTimeRange(unsigned leftTime, unsigned rightTime);

    /**
     * @brief cursorMove 游标移动
     * @param position 游标位置
     */
    void cursorMove(int position);

    /**
     * @brief getSubParamID 获取子参数ID
     * @return 子参数ID
     */
    SubParamID getSubParamID();

    QList<QPainterPath> generatorPainterPath(const TrendGraphInfo &graphInfo);

    /**
     * @brief getTrendPainterPath 获取参数趋势数据Path
     * @param dataVertor 数据容器
     * @param index 参数在容器中的索引
     * @return
     */
    QPainterPath getTrendPainterPath(const QVector<TrendGraphDataV2> &dataVertor, int index);
    QPainterPath getTrendPainterPath(const QVector<TrendGraphDataV3> &dataVertor, int index);

protected:
    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *e);

private:
    /**
     * @brief _mapValue 根据传入的数据信息获取像素位置
     * @param desc 坐标描述
     * @param data 数据
     * @return 像素位置
     */
    double _mapValue(TrendParamDesc desc, int data);

    /**
     * @brief _autoRulerCal 计算自动标尺
     */
    void _autoRulerCal(void);

    /**
     * @brief _updateAutoRuler 更新自动标尺
     * @param data 数据
     */
    void _updateAutoRuler(TrendDataType data);

private:
    TrendSubWidgetInfo _info;
    SubParamID _id;
    TrendGraphType _type;
    TrendGraphInfo _trendInfo;      // 趋势图数据信息
    TrendParamDesc _timeX;
    TrendParamDesc _valueY;         // 默认单位的上下标尺信息
    TrendParamDesc _rulerY;         // 当前单位的上下标尺信息
    int _xSize;                     // 趋势x坐标长度
    int _ySize;                     // 趋势y坐标长度
    int _trendDataHead;             // 趋势数据开始位置
    QColor _color;                  // 窗体主题颜色
    QString _paramName;             // 参数名称
    QString _paramUnit;             // 参数单位
    int _cursorPosIndex;            // 游标位置
    int _maxValue;                  // 数据中最大值
    int _minValue;                  // 数据中最小值
    bool _fristValue;               // 是否为第一个数据
};
