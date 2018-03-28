#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>

/***************************************************************************************************
 * 对QTableWidget进行二次封装，主要添加飞梭按键响应，触摸屏拖动和焦点处理
 **************************************************************************************************/
class TableWidget : public QTableWidget {
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = 0);

signals:
    // cell点击事件
    void cellClicked(int row, int column);

protected:
    // 处理焦点事件
    void focusOutEvent(QFocusEvent *e);
    void focusInEvent(QFocusEvent *e);
    // 键盘和飞梭点击事件
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    // 触摸屏或鼠标点击事件
    void mousePressEvent(QMouseEvent *e);
    // 触摸屏或鼠标点击完毕事件
    void mouseReleaseEvent(QMouseEvent *e);
    // 鼠标移动事件
    void mouseMoveEvent(QMouseEvent *e);

private:
    // 以下拖动相关
    bool _isMoveStart;                  // 是否开始移动
    bool _isMoved;                      // 是否发生移动
    QPoint _lastMousePos;               // 鼠标的坐标
    int _pressedRow;                    // 鼠标按下时的Item所在行
};

#endif // TABLEWIDGET_H
