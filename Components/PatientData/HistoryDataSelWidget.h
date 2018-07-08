#pragma once
#include "PopupWidget.h"
#include <QModelIndex>

class ITableWidget;
class HistoryDataSelWidget : public PopupWidget
{
    Q_OBJECT

public:
    HistoryDataSelWidget();
    ~HistoryDataSelWidget();

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _upReleased();
    void _downReleased();

    /**
     * @brief _dataTimeReleased 单击表格项,触发的槽函数
     * @param index 单击的表格项索引位置
     */
    void _dataTimeReleased(QModelIndex index);

private:
    /**
     * @brief _caclInfo 计算并将历史数据目录装填入表格
     */
    void _caclInfo();

    /**
     * @brief _convertTimeStr 将时间字符串转换成显示可以的字符串
     * @param str 需要转换的字符串
     * @return
     */
    QString _convertTimeStr(const QString &str);

private:
    ITableWidget *_reviewTable;
    IButton *_up;
    IButton *_down;

    QList<QString> _strList;                    // 字符
};
