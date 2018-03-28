#pragma once
#include "PopupWidget.h"
#include <QPointer>


//营救事件导出
class IButton;
class LButtonEx;
class RescueDataListWidget;
class RescueDataDeleteWidget : public PopupWidget
{
    Q_OBJECT

public:
    static RescueDataDeleteWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new RescueDataDeleteWidget();
        }

        return *_selfObj;
    }

    static RescueDataDeleteWidget *_selfObj;
    ~RescueDataDeleteWidget();

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _deleteSelectReleased();
    void _deleteAllReleased();
    void _upReleased();
    void _downReleased();
    void _updateWindowTitle();

private:
    RescueDataDeleteWidget();

private:
    RescueDataListWidget *_listWidget;
    LButtonEx *_delete;
    LButtonEx *_deleteAll;
    IButton *_up;
    IButton *_down;
    QPointer<QThread> _deleteThreadPtr;
};
#define rescueDataDeleteWidget (RescueDataDeleteWidget::construction())
#define deleteRescueDataDeleteWidget() (delete RescueDataDeleteWidget::_selfObj)



