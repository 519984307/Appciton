#pragma once
#include <QLabel>

//文本显示方向
enum ITEXTLABEL_DISPLAY_DIRECTION
{
    ILABEL_SINGLE_TEXT,          //单个文本
    ILABEL_SINGLE_PIC,           //单个图片
    ILABEL_HORIZOL_TEXT_PIC,     //水平，文本在前
    ILABEL_HORIZOL_PIC_TEXT,     //水平，图片在前
    ILABEL_HORIZOL_MULTI_TEXT,   //水平，多个文本
    ILABEL_VERTICAL_TEXT_PIC,    //垂直，文本在上
    ILABEL_VERTICAL_PIC_TEXT,    //垂直，图片在上
    ILABEL_VERTICAL_MULTI_TEXT,  //垂直，多个文本
};

//文本标签控件
class ILabel : public QLabel
{
public:
    ILabel();
    ~ILabel();

    //设置是否显示边框
    void setBoarderEnable(bool enable);

    //设置是否抗锯齿
    void setAntialiasingEnable(bool enable);

    // 设置边框宽度。
    void setBoarderWidth(int width);

    //设置边框颜色
    void setBoarderColor(const QColor &color);

    //设置背景颜色
    void setBackgroundColor(const QColor &color);

    //设置字体颜色
    void setFontColor(const QColor &color);

    //设置显示方向
    void setDirection(unsigned char direction);

    //设置对齐方式
    void setAlign(unsigned flag);

    //设置字体大小
    void changeFontSize(int fontSize, int index);
    void addFontSize(int fontSize);

    //设置字体加粗
    void changeFontBold(bool bold, int index);
    void addFontBold(bool bold);

    //设置显示内容
    void setText(const QStringList &textList);
    void setText(const QString &text);
    void setPic(const QPixmap &picture);

    //设置边界
    void setMargin(int left, int top, int right, int bottom);

    //设置项与项之间的间隙
    void setSpacing(int spacing);

    //设置图片宽度
    void setPicSize(int width, int height);

    // 设置圆角的半径。
    void setRadius(double radius);

    //清空
    void clear();

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    void _drawContent(QPainter &painter, QRect r);
    int _adjustFontSizeToWidth(const QString &txt, int width, int fontSize, bool bold);
    int _adjustFontSizeToHeight(int height, int fontSize, bool bold);
    void _drawPic(QPainter &painter, const QRect &r, unsigned alignFlag);
    void _drawHrizonTextPic(QPainter &painter, const QRect &r);
    void _drawHrizonPicText(QPainter &painter, const QRect &r);
    void _drawHrizonText(QPainter &painter, const QRect &r);
    void _drawVerticalTextPic(QPainter &painter, const QRect &r);
    void _drawVerticalPicText(QPainter &painter, const QRect &r);
    void _drawVerticalText(QPainter &painter, const QRect &r);

private:
    bool _boarder;                //是否显示边框
    bool _antialiasing;           //是否抗锯齿
    int _boarderWidth;            //边框宽度
    QColor _boarderColor;         //边框颜色
    QColor _bgColor;              //背景颜色
    QColor _fontColor;            //字体颜色
    unsigned char _direction;     //显示方向
    unsigned char _status;        //显示状态
    double _radius;               //边框圆角幅度
    unsigned _alignFlag;          //文本图片对齐方式
    int _leftMargin;
    int _topMargin;
    int _rightMargin;
    int _bottomMargin;
    int _spacing;
    int _picWidth;                //图片宽度
    int _picHight;                //图片高度
    QStringList _textList;        //显示文本，当有多个文本时以逗号隔开
    QVector<int> _fontSize;       //文本字体大小
    QVector<bool> _fontBold;      //加粗标志
    QPixmap _pic;                 //图片
};
