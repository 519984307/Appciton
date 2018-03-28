#pragma once
#include <QWidget>

class IPrograssBar : public QWidget
{
public:
    IPrograssBar();
    ~IPrograssBar();

    // 设置当前进度条的值
    void setPrograssValue(int value);

    // 设置进度条背景色
    void setBarBackgroundColor(QColor color);

    // 设置进度条前景色
    void setBarForegroundColor(QColor color);

    // 设置边框幅度
    void setBoardRadius(double radius);

    // 设置边框宽度
    void setBoardWidth(int width);

    // 设置控件宽度
    void setWidth(int w);

protected:
    void paintEvent(QPaintEvent *e);

private:
    int _curValue;
    int _boardWidth;
    double _radius;
    QColor _prograssBgColor;
    QColor _prograssFgColor;
};


