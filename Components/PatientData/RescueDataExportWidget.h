#pragma once
#include "PopupWidget.h"

//传输类型
enum TransferType
{
    TRANSFER_TYPE_USB,
    TRANSFER_TYPE_WIFI,
    TRANSFER_TYPE_NR
};

//营救事件导出
class IButton;
class IComboList;
class LButtonEx;
class RescueDataListWidget;
class QLabel;
class RescueDataExportWidget : public PopupWidget
{
    Q_OBJECT

public:
    static RescueDataExportWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new RescueDataExportWidget();
        }

        return *_selfObj;
    }

    static RescueDataExportWidget *_selfObj;
    ~RescueDataExportWidget();

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _transferTypeReleased(int);
    void _transferSelectReleased();
    void _transferAllReleased();
    void _upReleased();
    void _downReleased();
    void _updateWindowTitle();

private:
    RescueDataExportWidget();
    bool _transferData(QStringList &list);

private:
    RescueDataListWidget *_listWidget;
    IComboList *_transferType;
    LButtonEx *_transferSelect;
    LButtonEx *_transferAll;
    QLabel *_spaceLabel;
    IButton *_up;
    IButton *_down;

    TransferType _type;
};
#define rescueDataExportWidget (RescueDataExportWidget::construction())
#define deleteRescueDataExportWidget() (delete RescueDataExportWidget::_selfObj)

