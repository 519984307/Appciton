#include <QLabel>
#include <QProgressBar>
#include "LabelButton.h"
#include "Debug.h"
#include "ECG12LExportDataWidget.h"


ECG12LExportDataWidget *ECG12LExportDataWidget::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECG12LExportDataWidget::ECG12LExportDataWidget() : ExportDataWidget(EXPORT_RESCUE_DATA_BY_USB)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECG12LExportDataWidget::~ECG12LExportDataWidget()
{
}

/**************************************************************************************************
 * 获取传输进度。
 *************************************************************************************************/
unsigned char ECG12LExportDataWidget::getBarValue()
{
    return _bar->value();
}

/**************************************************************************************************
 * 设置进度条的文本显示
 *************************************************************************************************/
void ECG12LExportDataWidget::setBarText()
{
    _title->setText(startTitleString[_curType]);
    _info->setText(startInfoString[_curType]);
    _cancleOrOK->setText(trs("Cancel"));
}
