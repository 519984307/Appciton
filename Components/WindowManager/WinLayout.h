#pragma once

#include <QRect>
#include <QList>
#include <QWidget>

class IWidget;

class WinLayout
{
public:
    virtual ~WinLayout() {}

    // 布置窗体
    virtual bool layout(const QRect &/*rect*/,
                        const QList<IWidget *> &/*winList*/,
                        QWidget */*parent*/)
    {
        return false;
    }

    // 交换窗体
    virtual bool swapWidget(IWidget */*w1*/, IWidget */*w2*/)
    {
        return false;
    }

    // 退出布局
    virtual void exitLayout() {}

    // 布局名称
    const QString &name()
    {
        return _name;
    }

protected:
    WinLayout(const QString &name)
    {
        _name = name;
    }

private:
    QString _name; // 布局名称
};
