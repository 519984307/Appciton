#include "ServiceUpgrade.h"
#include "LabelButton.h"
#include "Debug.h"
#include "IButton.h"
#include "SubMenu.h"
#include <QScrollBar>
#include <QTextTable>
#include "SystemManager.h"
#include "BLMEDUpgradeParam.h"
#include "IConfig.h"
#include "USBManager.h"
#include "IMessageBox.h"

ServiceUpgrade *ServiceUpgrade::_selfObj = NULL;

/**************************************************************************************************
 * 功能：初始化。
 *************************************************************************************************/
void ServiceUpgrade::init()
{
    btnShow(true);
    _progressBar->reset();
    _progressBar->hide();
    _info->hide();
    _upgradeType->setFocus();
    _upgradeType->setCurrentIndex(0);
    _typl = UPGRADE_TYPE_HOST;
    _testEdit->clear();
    m_status_count = 0;

    //显示框显示行数计算
    _testEdit->setFont(fontManager.textFont(_fontsize));
    int textH = fontManager.textHeightInPixels(fontManager.textFont(_fontsize));
    int editH = _testEdit->height();
    m_status_all = editH / (textH + 5);
}

/**************************************************************************************************
 * 状态显示。
 *************************************************************************************************/
void ServiceUpgrade::setDebugText(QString str)
{
    cur = _testEdit->textCursor();
    cur.movePosition(QTextCursor::End);
    QTextTableFormat tableformate;
    tableformate.setBorder(0);
    //插入table，用于显示状态信息
    QTextTable *table = cur.insertTable(1,1,tableformate);
    table->cellAt(0,0).firstCursorPosition().insertText(str);
    cur.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor,1);
    cur.select(QTextCursor::BlockUnderCursor);
    cur.clearSelection();
    //滚动条置底
    QScrollBar *bar = _testEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
    //状态信息条数+1
    m_status_count ++;
    //如果信息数>50，则删除最早的一条记录
    if(m_status_count > m_status_all)
    {
        //将选择光标移至第一个字符
        cur.setPosition(1,QTextCursor::MoveAnchor);
        //选择最早含有最早状态信息的table
        cur.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor,1);
        cur.select(QTextCursor::BlockUnderCursor);
        //删除该信息
        cur.removeSelectedText();
        //光标移至QTextEdit末行
        cur.movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
        m_status_count --;
    }
}

/**************************************************************************************************
 * 功能:飞梭控制
 *************************************************************************************************/
void ServiceUpgrade::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusNextPrevChild(false);
            return;
        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            return;
        default:
            break;
    }

    QWidget::keyPressEvent(e);
}

/**************************************************************************************************
 * 开始升级。
 *************************************************************************************************/
void ServiceUpgrade::_startBtnReleased()
{
    _testEdit->clear();
    _progressBar->reset();
    m_status_count = 0;
    blmedUpgradeParam.startUpgrade();
}

/**************************************************************************************************
 * 升级类型选择。
 *************************************************************************************************/
void ServiceUpgrade::_upgradeTypeReleased(int index)
{
    QMap<int, UpgradeType>::Iterator iter = _upgradeModule.find(index);
    if (iter != _upgradeModule.end())
    {
        _typl = iter.value();
    }
    else
    {
        _typl = UPGRADE_TYPE_NR;
    }
}

/**************************************************************************************************
 * 按钮的显示与置灰。
 *************************************************************************************************/
void ServiceUpgrade::btnShow(bool flag)
{
    if (flag)
    {
        upgrading = false;
        _upgradeType->setEnabled(true);
        _btnStart->setEnabled(true);
        _btnReturn->setEnabled(true);
        _btnReturn->setFocus();
    }
    else
    {
        upgrading = true;
        _upgradeType->setEnabled(false);
        _btnStart->setEnabled(false);
        _btnReturn->setEnabled(false);
    }
}

/**************************************************************************************************
 * 进步条显示值。
 *************************************************************************************************/
void ServiceUpgrade::progressBarValue(int value)
{
    _progressBar->show();
    _progressBar->setValue(value);
}

/**************************************************************************************************
 * 提示信息显示。
 *************************************************************************************************/
void ServiceUpgrade::infoShow(bool flag)
{
    if (flag)
    {
        _info->show();
    }
    else
    {
        _info->hide();
    }
}

/**************************************************************************************************
 * 模式模块名。
 *************************************************************************************************/
UpgradeType ServiceUpgrade::getType(void)
{
    return _typl;
}

/**************************************************************************************************
 * 是否正在升级。
 *************************************************************************************************/
bool ServiceUpgrade::isUpgrading(void)
{
    return upgrading;
}

/**************************************************************************************************
 * 退出。
 *************************************************************************************************/
void ServiceUpgrade::exit(void)
{
    blmedUpgradeParam.fileClose();
    close();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceUpgrade::ServiceUpgrade() : QWidget()
{
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = 150;
    int labWidth = 500;
    int labHigh = 150;

    QVBoxLayout *labelLayout = new QVBoxLayout();
    labelLayout->setContentsMargins(50, 0, 50, 0);
    labelLayout->setSpacing(5);
    labelLayout->setAlignment(Qt::AlignTop);

    // 标题栏。
    _title = new QLabel("Upgrade");
    _title->setAlignment(Qt::AlignCenter);
    _title->setFixedHeight(30 + 10);
    _title->setFont(fontManager.textFont(fontManager.getFontSize(4)));
    QPalette p;
    p.setColor(QPalette::Foreground, Qt::black);
    _title->setPalette(p);
    labelLayout->addWidget(_title);

    _upgradeType = new IComboList(trs("UpgradeModule") + " :");
    _upgradeType->label->setFixedSize(btnWidth+20, ITEM_H);
    _upgradeType->label->setAlignment(Qt::AlignCenter);
    _upgradeType->combolist->setFixedSize(btnWidth+20, ITEM_H);
    _upgradeType->setFont(fontManager.textFont(fontManager.getFontSize(3)));
    _upgradeType->setFocusPolicy(Qt::StrongFocus);
    _upgradeType->addItem(trs(convert(UPGRADE_TYPE_HOST)));
    _upgradeType->addItem(trs(convert(UPGRADE_TYPE_TE3)));
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        _upgradeType->addItem(trs(convert(UPGRADE_TYPE_TN3)));
    }
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        _upgradeType->addItem(trs(convert(UPGRADE_TYPE_TS3)));
    }
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        _upgradeType->addItem(trs(convert(UPGRADE_TYPE_TT3)));
    }
    _upgradeType->addItem(trs(convert(UPGRADE_TYPE_PRT72)));
    _upgradeType->addItem(trs(convert(UPGRADE_TYPE_nPMBoard)));
    connect(_upgradeType->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_upgradeTypeReleased(int)));


    _btnStart= new LButtonEn();
    _btnStart->setText(trs("UpgradeStart"));
    _btnStart->setFont(fontManager.textFont(fontSize));
    _btnStart->setFixedSize(btnWidth, ITEM_H);
    connect(_btnStart, SIGNAL(realReleased()), this, SLOT(_startBtnReleased()));

    _testEdit = new QTextEdit();
    _testEdit->setFixedSize(labWidth, labHigh+60);
    _testEdit->setFont(fontManager.textFont(fontSize));
    _testEdit->setFocusPolicy(Qt::NoFocus);
    _testEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _testEdit->setReadOnly(true);

//    _progressBar=new QProgressBar;
//    _progressBar->setFixedSize(labWidth, ITEM_H);
//    _progressBar->setFont(fontManager.textFont(fontSize));
//    _progressBar->setRange(0,100);
//    _progressBar->setStyleSheet("QProgressBar{text-align: center;}");
    _progressBar = new QProgressBar();
    _progressBar->setStyleSheet("QProgressBar{background:rgb(255,255,255);border-radius:5px;text-align: center;}"
                                "QProgressBar::chunk{background:green;border-radius:5px;}");
//    _progressBar->setTextVisible(false);
    _progressBar->setFont(fontManager.textFont(fontSize));
    _progressBar->setRange(0,100);
    _progressBar->setValue(50);
    _progressBar->setFixedSize(labWidth, ITEM_H);

    _btnReturn = new LButtonEn();
    _btnReturn->setText(trs("UpgradeReturn"));
    _btnReturn->setFont(fontManager.textFont(fontSize));
    _btnReturn->setFixedSize(btnWidth, ITEM_H);
    connect(_btnReturn, SIGNAL(realReleased()), this, SLOT(_returnBtnReleased()));

    QGridLayout *gridlayout = new QGridLayout();
    gridlayout->setContentsMargins(20, 0, 20, 0);
    gridlayout->addWidget(_upgradeType, 0, 0, 1, 2, Qt::AlignLeft);
    gridlayout->addWidget(_btnStart, 0, 2, 1, 1, Qt::AlignRight);
    gridlayout->addWidget(_testEdit, 1, 0, 2, 3, Qt::AlignCenter);
    gridlayout->addWidget(_progressBar, 4, 0, 1, 3, Qt::AlignCenter);
    gridlayout->addWidget(_btnReturn, 5, 2, 1, 1, Qt::AlignRight);
    labelLayout->addLayout(gridlayout);

    setLayout(labelLayout);

    p.setColor(QPalette::Window, QColor(209, 203, 183));
    setPalette(p);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceUpgrade::~ServiceUpgrade()
{
}




