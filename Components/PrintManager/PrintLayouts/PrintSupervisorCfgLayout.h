#pragma once
#include "PrintLayoutIFace.h"

//配置项
enum ConfigPrintItemType
{
    CONFIG_ITEM_START,
    CONFIG_ITEM_GENERAL,
    CONFIG_ITEM_ECG,
    CONFIG_ITEM_RESP,
    CONFIG_ITEM_12L,
    CONFIG_ITEM_NIBP,
    CONFIG_ITEM_RESP_CO2,
    CONFIG_ITEM_ALARM,
    CONFIG_ITEM_ALARM_LIMIT,
    CONFIG_ITEM_LOCAL,
    CONFIG_ITEM_DISPLAY,
    CONFIG_ITEM_TRIGGER_PRINT,
    CONFIG_ITEM_DATETIME,
    CONFIG_ITEM_CODEMARKER,
    CONFIG_ITEM_NETWORK,
    CONFIG_ITEM_SFTP,
    CONFIG_ITEM_MAIL,
    CONFIG_ITEM_NR
};

class PrintSupervisorCfgLayout : public PrintLayoutIFace
{
public:
    PrintSupervisorCfgLayout();
    ~PrintSupervisorCfgLayout();

    void start(void); // 开始布局。
    void abort(void); // 布局中止。

    // 对打印内容进行布局排版，构建相应布局的打印页
    PrintPage *exec(PrintContent *content);

private:
    // 构建不同类型的打印页
    PrintPage *_buildPage();
    void _buildStartPage();
    void _buildGeneralPage();
    void _buildECGPage();
    void _buildRESPPage();
    void _buildNIBPPage();
    void _buildRESPAndCo2Page();
    void _buildAlarmPage();
    void _buildAlarmLimitPage();
    void _buildLocalPage();
    void _buildDisplayPage();
    void _buildTriggerPrintPage();
    void _build12LPage();
    void _buildNetworkPage();
    void _buildSFTPPage();
    void _buildDateTimePage();
    void _buildCodeMarkerPage();
    void _buildMailPage();

    // 获取打印页的最大宽度
    int _getPageMaxWidth(const QFont &font);

private:
    int _curType;
    int _curItem;
    QStringList _content;
    PrintPage *_page;
};


