#pragma once
#include "PopupWidget.h"

class IButton;
class IComboList;
class WidgetSelectMenu : public PopupWidget
{
    Q_OBJECT

public:
    WidgetSelectMenu(const QString &widgetSelectMenuName, bool replaceOnly = false);
    ~WidgetSelectMenu();

public:
    // 设置被操作的波形控件。
    void setWidgetName(const QString &name);

    // 设置是否为操作Top Waveform。
    void setTopWaveform(bool topWaveform);

    // 是否关联了TopWaveform。
    bool getTopWaveform(void) const;

    // 重新刷新。
    void refresh(void);

    // 设置显示坐标
    inline void setShowPoint(int x, int y) {_x = x; _y = y;}

protected slots:
    virtual void _replaceListSlot(int index) = 0;          //替换波形。
    virtual void _insertListSlot(int index) = 0;           //插入新波形。
    virtual void _removeButtonSlot(void) = 0;              //移除。
    virtual void _loadWidget(void) = 0;                 //载入波形控件的名称。

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

    IComboList *_replaceList;
    IComboList *_insertList;
    IButton *_removeButton;

    QString widgetName;
    // 保存替换选项和插入选项中的波形信息。
    QStringList replaceWidget;
    QStringList replaceWidgetTitles;
    QStringList insertWidget;
    QStringList insertWidgetTitles;

    bool isTopWaveform;

    int _x;
    int _y;
};
