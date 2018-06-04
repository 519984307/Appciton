#include "SelectStarterLogo.h"
#include <QVBoxLayout>
#include <QRegExp>
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "IButton.h"
#include "IComboList.h"
#include "SupervisorMenuManager.h"
#include "SupervisorConfigManager.h"
#include "PatientDefine.h"
#include "KeyBoardPanel.h"
#include "Debug.h"
#include "UserMaintainManager.h"
#include <QDir>
#include <QProcess>

#define usb0DeviceNode     ("/dev/sda2")
#define selectLogoPath     ("/mnt/usb0/logo")

SelectStarterLogo *SelectStarterLogo::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SelectStarterLogo::SelectStarterLogo() : SubMenu(trs("SelectStarterLogoMenu"))
{
    QString stringTemp[]={
        "SelectStarterLogo", "NULL"
    };

    for(unsigned int i=SELECT_STATRTER_LOGO; i<SELECT_STARTER_TYPE_NR && i < sizeof(stringTemp); i++)
    {
        _stringTypeName[i] = stringTemp[i];
    }

    for(int i=SELECT_STATRTER_LOGO; i<SELECT_STARTER_TYPE_NR; i++)
    {
        _selectLogoCombo[i] = new IComboList(trs(_stringTypeName[i]));
    }
    _returnFlag = 1;
    setDesc(trs("SelectStarterLogoMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void SelectStarterLogo::readyShow()
{
    int index;

    //读取usb内logo
    if(_returnFlag!=0)
    {
        _returnFlag = QProcess::execute(QString("mount -t vfat %1 %2").arg(usb0DeviceNode).arg(selectLogoPath));
        QProcess::execute("sync");
    }

    if(_returnFlag==0)//挂载成功
    {
        QDir dir(QString("%1%2").arg(selectLogoPath).arg("/"));
        QStringList nameFilter;
        nameFilter.append("*.bmp");
        _logoNames = dir.entryList(nameFilter,QDir::Files|QDir::Readable,QDir::Name);
    }

    QDir dir(QString("%1%2").arg(selectLogoPath).arg("/"));
    QStringList nameFilter;
    nameFilter.append("*.bmp");
    _logoNames = dir.entryList(nameFilter,QDir::Files|QDir::Readable,QDir::Name);

    for(int i=SELECT_STATRTER_LOGO; i<SELECT_STARTER_TYPE_NR; i++)
    {
        for(int j=0; j<_logoNames.count(); j++)
        {
            _selectLogoCombo[i]->combolist->addItem(trs(_logoNames[j]));
        }
        if(_logoNames.isEmpty())
        {
            _selectLogoCombo[i]->combolist->addItem(trs("null"));
        }
    }

    for(int i=SELECT_STATRTER_LOGO; i<SELECT_STARTER_TYPE_NR; i++)
    {
        index = 0;
        systemConfig.getNumValue(QString("SelectStarterLogo"), index);
        if(index<_logoNames.count())
        {
             _selectLogoCombo[i]->combolist->setCurrentIndex(index);
        }
        else
        {
            _selectLogoCombo[i]->combolist->setCurrentIndex(0);
        }

    }
}

bool SelectStarterLogo::eventFilter(QObject *obj, QEvent *ev)
{
//    if(obj == d_ptr->configList)
//    {
//        if (ev->type() == QEvent::FocusIn)
//        {
//            QFocusEvent *e = static_cast<QFocusEvent *>(ev);
//            if(e->reason() == Qt::TabFocusReason)
//            {
//                d_ptr->configList->setCurrentRow(0);
//            }
//            else if (e->reason() == Qt::BacktabFocusReason)
//            {
//                d_ptr->configList->setCurrentRow(d_ptr->configList->count() - 1);
//            }
//        }

//        if (ev->type() == QEvent::Hide)
//        {
//            if(!d_ptr->selectItems.isEmpty())
//            {
//                d_ptr->importBtn->setEnabled(true);
//                d_ptr->exportBtn->setEnabled(false);
//                d_ptr->selectItems.clear();
//            }
//        }
//    }
    if(obj == this)
    {
        if (ev->type() == QEvent::FocusIn)
        {

        }
        else if (ev->type() == QEvent::Hide)
        {

        }
    }
    return false;
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SelectStarterLogo::layoutExec()
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;


    for(int i=SELECT_STATRTER_LOGO; i<SELECT_STARTER_TYPE_NR; i++)
    {
        QHBoxLayout *hlayout=new QHBoxLayout;

        _selectLogoCombo[i]->setFont(fontManager.textFont(fontSize));
        _selectLogoCombo[i]->label->setFixedSize(labelWidth, ITEM_H);
        _selectLogoCombo[i]->combolist->setFixedSize(btnWidth, ITEM_H);
        hlayout->addWidget(_selectLogoCombo[i]);

        mainLayout->addLayout(hlayout,Qt::AlignRight);
        mainLayout->addStretch();
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SelectStarterLogo::~SelectStarterLogo()
{

}
