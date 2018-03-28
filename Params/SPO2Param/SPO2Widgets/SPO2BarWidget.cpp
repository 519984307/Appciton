#include <QPainter>
#include <QPaintEvent>
#include "SPO2BarWidget.h"
#include "ParamInfo.h"
#include "ColorManager.h"
#include "Debug.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SPO2BarWidget::SPO2BarWidget(int minValue, int maxValue, QWidget *parent)
        : QWidget(parent), _minValue(minValue), _maxValue(maxValue),
        _cellHeight(0), _cellNumber(0), _curShowCellMaxIndex(0), _lastShowCellMaxIndex(0)
{
    // 颜色
    QPalette &palette = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    setPalette(palette);
    setCellHeight(2);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SPO2BarWidget::~SPO2BarWidget()
{
}

/**************************************************************************************************
 * 设置单元高度。
 *************************************************************************************************/
void SPO2BarWidget::setCellHeight(int cellHeight)
{
    if (cellHeight > 0)
    {
        _cellHeight = cellHeight;
        _cellNumber = rect().height() / _cellHeight; // 重新计算单元数目
    }
}

/**************************************************************************************************
 * 设置当前棒图值。
 *************************************************************************************************/
void SPO2BarWidget::setValue(int value)
{

    if ((_minValue >= _maxValue) || (value > _maxValue) || (value < _minValue))
    {
        return;
    }
    // 计算当前显示单元的最大索引
    _curShowCellMaxIndex = _cellNumber * (value - _minValue) / (_maxValue - _minValue);

    // 显示单元数没有变化时，不需要刷新
    if (_curShowCellMaxIndex == _lastShowCellMaxIndex)
    {
        return;
    }
    // 刷新棒图
    update();
}

/**************************************************************************************************
 * 窗口重绘事件。
 *************************************************************************************************/
void SPO2BarWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    QRect updateRect; // 刷新的单元矩形区域
    updateRect.setLeft(rect().left());
    updateRect.setWidth(rect().width());
    for (int i = 0; i < _curShowCellMaxIndex; ++i)
    {
        // 只填充索引为偶数的单元矩形区域
        if (0 == (i % 2))
        {
            updateRect.setTop(rect().bottom() - i * _cellHeight - 1);
            updateRect.setHeight(_cellHeight);
            painter.fillRect(updateRect.adjusted(0, 0, -1, -1), palette().foreground());
        }
    }

    // 更新显示单元的最大索引
    _lastShowCellMaxIndex = _curShowCellMaxIndex;
    QWidget::paintEvent(e);
}

/**************************************************************************************************
 * 窗口大小改变事件。
 *************************************************************************************************/
void SPO2BarWidget::resizeEvent(QResizeEvent *e)
{
    if (!e || (0 == _cellHeight))
    {
        return;
    }

    _cellNumber = rect().height() / _cellHeight; // 重新计算单元数目

    QWidget::resizeEvent(e);
}
