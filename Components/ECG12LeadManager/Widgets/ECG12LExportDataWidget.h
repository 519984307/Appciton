#pragma once
#include "ExportDataWidget.h"

class ECG12LExportDataWidget : public ExportDataWidget
{
public:
    static ECG12LExportDataWidget &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ECG12LExportDataWidget();
        }

        return *_selfObj;
    }

    static ECG12LExportDataWidget *_selfObj;

    ECG12LExportDataWidget();
    ~ECG12LExportDataWidget();

public:
    //设置进度条的文本显示
    void setBarText();

    //是否传输取消
    void setTransferCancleFlag(bool flag) {_transferCancel = flag;}

    //设置当前传送类型
    void setCurType(Export_Data_Type type) {_curType = type;}

    //获取传输进度。
    unsigned char getBarValue();
};

#define ecg12LExportDataWidget (ECG12LExportDataWidget::construction())
#define deleteecg12LExportDataWidget() (delete ECG12LExportDataWidget::_selfObj)
