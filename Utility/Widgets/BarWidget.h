#pragma once
#include <QWidget>

// 条状控件。
class BarWidget : public QWidget
{
    Q_OBJECT

public:
    BarWidget(QWidget *parent = 0);
    ~BarWidget();

protected:
    // 重绘事件。
    void paintEvent(QPaintEvent *e);
};
