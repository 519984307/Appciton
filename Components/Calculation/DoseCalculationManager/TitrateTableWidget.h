#pragma once
#include "PopupWidget.h"
#include "DoseCalculationDefine.h"
#include <QLabel>

class LabelButton;
class IButton;
class ITableWidget;
class TitrateTableWidget : public PopupWidget
{
    Q_OBJECT

public:
    static TitrateTableWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TitrateTableWidget();
        }
        return *_selfObj;
    }
    ~TitrateTableWidget();

    void layoutExec(void);

    void updateTitrateTableData(void);

    void datumTermDose(void);
    void datumTermInfusionRate(void);
    void datumTermDripRate(void);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _upReleased(void);
    void _downReleased(void);
    void _tableSetReleased(void);
    void _recordReleased(void);

private:
    TitrateTableWidget();

private:
    static TitrateTableWidget *_selfObj;
    static const int _itemH = 30;  // 子项高度

    QLabel *_paramName[TITRATE_TABLE_NR];       // 参数名称
    QLabel *_paramValue[TITRATE_TABLE_NR];      // 参数值
    QLabel *_paramUnit[TITRATE_TABLE_NR];       // 参数单位

    ITableWidget *_titrateTable;                // 滴定表格

    IButton *_up;
    IButton *_down;
    IButton *_tableSet;
    IButton *_record;
};
#define titrateTableWidget (TitrateTableWidget::construction())
#define deleteTitrateTableWidget() (delete &titrateTableWidget)
