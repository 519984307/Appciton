#pragma once
#include "PopupWidget.h"
#include "IListWidget.h"
#include "SystemManager.h"

class SystemModeSelectWidget : public PopupWidget
{
    Q_OBJECT

public:
    SystemModeSelectWidget();
    ~SystemModeSelectWidget();

protected:
    virtual void showEvent(QShowEvent *event);

private slots:
    void _listItemClicked(QListWidgetItem *item);  // 鼠标点击IListWidget的item槽。
    void _listReleased(void);                      // enter按键释放IListWidget的item槽。

private:
    void _typeChanged(UserFaceType t);

    IListWidget *_list;
};
