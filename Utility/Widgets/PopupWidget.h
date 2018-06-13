#pragma once
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class IButton;
class PopupWidget : public QDialog
{
    Q_OBJECT

public:
    // 设置标题栏的提示文字。
    void setTitleBarText(const QString &str);

    // 读取标题栏的提示文字。
    QString readTitleBarText(void){return _titleLabel->text();}

    // 设置标题栏的字体。
    void setTitleBarFont(const QFont &font);

    // 设置关闭按钮焦点
    void setCloseBtnFocus(void);

    // 设置关闭按钮显示
    void setCloseBtnEnable(bool enable);

    // 读取标题栏高度
    int getTitleBarhight()const;

    //设置文本及图片
    void setCloseBtnTxt(const QString &txt);
    void setCloseBtnPic(const QImage &pic);
    void setCloseBtnColor(QColor color);

    // 自动弹出与关闭，如果窗体显示则关闭，如果关闭则显示。
    virtual void autoShow(void);
    virtual void autoShow(int x, int y);
    virtual void autoShow(int x, int y, int w, int h);

    PopupWidget(bool isBlank = false);
    virtual ~PopupWidget();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);
    virtual void exit(void);
    QVBoxLayout *contentLayout;

    // 设置边框宽度。
    void setBorderWidth(int width);

    // 隐藏退出按钮。
    void hideExitButton(void);

protected slots:
    virtual void exitButtonRealsed(void);

private:
    static const int _titleBarHeight = 29;
    int _borderWidth;
    bool _fixedDimension;               // 标记为指定的尺寸和位置。

    QLabel *_titleLabel;
    IButton *_exitButton;
};
