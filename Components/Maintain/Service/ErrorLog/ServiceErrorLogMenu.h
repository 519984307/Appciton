#pragma once
#include "MenuWidget.h"
#include <QIcon>

class QLabel;
class IButton;
class ITableWidget;
class TableItemDelegate;
class ServiceErrorLogMenu  : public MenuWidget
{
    Q_OBJECT

public:
    static ServiceErrorLogMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ServiceErrorLogMenu();
        }
        return *_selfObj;
    }
    static ServiceErrorLogMenu *_selfObj;

    ~ServiceErrorLogMenu();

    void init();

protected:
//    bool focusNextPrevChild(bool next);
    void keyPressEvent(QKeyEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private slots:
    void summaryBtnClick();
    void exportBtnClick();
    void eraseBtnClick();
    void upBtnClick();
    void downBtnClick();
    void itemClick(int index);
    void USBCheckTimeout();

private:
    ServiceErrorLogMenu();

    void titleInit();
    void loadData();
    void refreshTitle();

    QLabel *_title;
    ITableWidget *_table;
    IButton *_summaryBtn;
    IButton *_exportBtn;
    IButton *_eraseBtn;
    IButton *_upBtn;
    IButton *_downBtn;
    QLabel *_info;                    //提示信息
    TableItemDelegate *_delegate;
    int currentPage;
    int totalPage;
    QString title;
    QIcon icons[2];

    QTimer *_USBCheckTimer;
};
#define serviceErrorLogMenu (ServiceErrorLogMenu::construction())

