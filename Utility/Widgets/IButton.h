#pragma once
#include <QPushButton>

class IButton: public QPushButton
{
    Q_OBJECT

public:
    IButton(const QString &text = QString::null, QWidget *parent = 0);
    ~IButton();

    // 设置边框
    void setBorderEnabled(bool border, bool isAntialiasing = false);

    // 设置边框颜色
    void setBorderColor(QColor color);

    // 设置字体颜色
    void setTextColor(QColor color);
    void setInactivityTextColor(QColor color);
    void setHighLightTextColor(QColor color);

    // 设置背景颜色
    void setBackgroundColor(QColor color);

    // 设置不活动颜色
    void setShdowColor(QColor color);


    // 设置边框倒角
    void setBorderRadius(double radius);

    // 设置背景图片
    void setPicture(const QImage &pic);
    bool isPictureNull();

    // 设置文本水平对齐方式
    void setAlignment(Qt::Alignment align);
//    void setBackgroundEnabled(bool enabled);

    // 设置图片水平对齐方式
    void setPicAlignment(Qt::Alignment align);

    // 使能按键处理
    void setEnableKeyAction(bool flag);

    void setId(int id);
    int id(void);

protected:
    // 获取边框倒角
    double getBorderRadius()
    {
        return _radius;
    }

protected:
    // 响应重绘事件
    void paintEvent(QPaintEvent *e);

    // 响应鼠标出入事件
//    void enterEvent(QEvent *e);
//    void leaveEvent(QEvent *e);

    // 键盘和飞棱事件
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    // 触摸屏点击或按键按下事件
    void mouseReleaseEvent(QMouseEvent *e);

private:
    // 绘制按钮
    void _paintButton();

    // 绘制文本
    void _paintText();

    // 绘制背景图
    void _paintPicture();

signals:
    void clicked(int id);
    void clicked(const QString &text);
    void realReleased(void);

private slots:
    void _onClicked(void);

private:
    int _id;
    double _radius;                                 // 边框倒角
    bool _isBorderEnabled;                          // 边框使能状态
    bool _isAntialiasingWhenNoBorder;               // is Antialiasing when disable border
    bool _keyActionEnable;                          // 使能按键
    bool _isKeyPressed;
    QColor _borderColor;                            // 边框颜色
    QColor _textColor;                              // 文字颜色
    QColor _inactivityTxtColor;                     // 不活动文字颜色
    QColor _highlightTxtColor;                      // 高亮文本颜色
    QImage _picture;	                            // 无效状态图片
    Qt::Alignment _textAlign;                       // 文本水平对齐方式
    Qt::Alignment _picAlign;                        // 图片对齐
};
