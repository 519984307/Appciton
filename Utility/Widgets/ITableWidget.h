#pragma once
#include <QTableWidget>

class QIcon;
//重新实现表格控件
class ITableWidget : public QTableWidget
{
    Q_OBJECT

public:
    ITableWidget(bool autoCheckWhenEnterPress = true);
    ~ITableWidget();

    void setCheck(QTableWidgetItem *item);
    void clearCheck(QTableWidgetItem *item);
    bool isEmpty() const;

signals:
    void itemEnter(int index);

protected:
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

private:
    int _index;                            // 当前索引
    QIcon *_pic;                           // 选择图表
    bool _autoCheckWhenEnterPress;      //auto check
};

