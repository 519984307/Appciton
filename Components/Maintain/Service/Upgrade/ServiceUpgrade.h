#pragma once
#include "PopupWidget.h"
#include "MenuGroup.h"
#include <QPushButton>
#include "NIBPProviderIFace.h"
#include "NIBPState.h"
#include "LabelButton.h"
#include <QProgressBar>
#include "IComboList.h"
#include <QTextEdit>

#define InvStr() ("---")
#define PD_CPLD_FILE_NAME ("/ZOLLImage/PDCPLD")   //PD CPLD文件名
#define PD_APP_FILE_NAME ("/ZOLLImage/PDImage")   //PD APP文件名

enum UpgradeType
{
    UPGRADE_TYPE_HOST,
    UPGRADE_TYPE_TE3,
    UPGRADE_TYPE_TN3,
    UPGRADE_TYPE_TN3Daemon,
    UPGRADE_TYPE_TS3,
    UPGRADE_TYPE_TT3,
    UPGRADE_TYPE_PRT72,
    UPGRADE_TYPE_nPMBoard,
    UPGRADE_TYPE_NR
};

class QLabel;
class IListWidget;
class QStackedWidget;
class LButtonEx;
class ServiceUpgrade : public QWidget
{
    Q_OBJECT

public:
    static ServiceUpgrade &Construation()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ServiceUpgrade();
        }

        return *_selfObj;
    }
    static ServiceUpgrade *_selfObj;
    ~ServiceUpgrade();

    static const char *convert(UpgradeType index)
    {
        static const char *symbol[UPGRADE_TYPE_NR] =
        {
            "HOST", "TE3", "TN3", "TN3Daemon", "TS3", "TT3","PRT72","nPMBoard"
        };
        return symbol[index];
    }

    //初始化
    void init();

    //显示
    void setDebugText(QString str);

    //按钮的显示与置灰
    void btnShow(bool flag);

    //进步条值
    void progressBarValue(int value);

    //提示信息
    void infoShow(bool flag);

    //升级模块名
    UpgradeType getType(void);

    //是否在升级中
    bool isUpgrading(void);

    // PD CPLD完成升级等待标记
    void setWaitPDCPLDRunCompletion(bool flag) {waitPDCPLDRunCompletion = flag;}
    bool isWaitPDCPLDRunCompletion() {return waitPDCPLDRunCompletion;}

protected:
    virtual void exit(void);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void _startBtnReleased(void);
    void _upgradeTypeReleased(int index);

private:
    ServiceUpgrade();

    QLabel *_title;                      //标题
    QTextEdit *_testEdit;                //文本框
    QTextCursor cur;                     //光标
    QProgressBar *_progressBar;          //进步条
    IComboList *_upgradeType;            //下拉选框
    LButtonEn *_btnStart;                //开始按钮
    LButtonEn *_btnReturn;               //返回按钮
    QLabel *_info;                       //提示信息

    int _fontsize;                       //字体大小
    int m_status_count;                  //当前显示行数
    int m_status_all  ;                  //可显示的行数
    bool upgrading;                      //升级中的标志
    bool waitPDCPLDRunCompletion;        //等待PD CPLD运行完成
    UpgradeType _typl;
    QMap<int, UpgradeType> _upgradeModule;        // 支持升级的模块。

};
#define serviceUpgrade (ServiceUpgrade::Construation())
