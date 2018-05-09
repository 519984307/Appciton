#include <QHBoxLayout>
#include <QKeyEvent>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "LabelButton.h"
#include "SupervisorCodeMarker.h"
#include "SupervisorConfigManager.h"
#include "CodeMarkerList.h"
#include "SupervisorMenuManager.h"

SupervisorCodeMarker *SupervisorCodeMarker::_selfobj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorCodeMarker::SupervisorCodeMarker()
    : SubMenu(trs("SupervisorCodeMarker"))
{
    setDesc(trs("SupervisorCodeMarkerDesc"));

    startLayout();
}

void SupervisorCodeMarker::_loadOptions()
{
    _LanguageIndex = 0;
    currentConfig.getNumAttr("Local|Language", "CurrentOption", _LanguageIndex);
    if (_LanguageIndex != languageManager.getCurLanguage())
    {
        languageManager.reload(_LanguageIndex);
    }

    QString codemarkerStr;
    QString indexStr = "CodeMarker|Marker";
    indexStr += QString::number(_LanguageIndex, 10);

    currentConfig.getStrValue(indexStr, codemarkerStr);
    _allCodeMarkers = codemarkerStr.split(',');

    codemarkerStr.clear();
    QString markerStr = "CodeMarker|SelectMarker|Language";
    markerStr += QString::number(_LanguageIndex, 10);
    currentConfig.getStrValue(markerStr, codemarkerStr);
    _selectedCodeMarkers = codemarkerStr.split(',');
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void SupervisorCodeMarker::readyShow()
{
    _loadOptions();
    _tmpcombolist = NULL;
    int totalSize = _allCodeMarkers.size();
    int selectSize = _selectedCodeMarkers.size();
    for (int i = 0; i < CODEMARKER_MAX; ++i)
    {
        // 根据配置添加可选项。
        _codeMarkerList[i]->clear();
        for (int j = 0; j < totalSize; ++j)
        {
            _codeMarkerList[i]->addItem(trs(_allCodeMarkers.at(j)));
        }

        if (i < selectSize)
        {
            _codeMarkerList[i]->setCurrentItem(trs(_selectedCodeMarkers[i]));
        }
        else
        {
            _codeMarkerList[i]->setCurrentItem(" ");
        }
    }
}

/**************************************************************************************************
 * 按键。
 *************************************************************************************************/
void SupervisorCodeMarker::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusNextPrevChild(false);
            break;
        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;
        default:
            break;
    }
}

/**************************************************************************************************
 * 按键。
 *************************************************************************************************/
void SupervisorCodeMarker::hideEvent(QHideEvent *e)
{
    SubMenu::hideEvent(e);

    if (_LanguageIndex != languageManager.getCurLanguage())
    {
        languageManager.reload(languageManager.getCurLanguage());
    }
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SupervisorCodeMarker::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE - 5;
    itemW /= 2;
    int fontSize = fontManager.getFontSize(1);
    int labelWidth = itemW / 4;
    int btnWidth = itemW - labelWidth;

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setMargin(0);
    leftLayout->setSpacing(2);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setMargin(0);
    rightLayout->setSpacing(2);

    layout->addLayout(leftLayout);
    layout->addStretch();
    layout->addLayout(rightLayout);

    _tmpcombolist = NULL;
    for (int i = 0; i < CODEMARKER_MAX; ++i)
    {
        _tmpcombolist = new IComboList(QString::number(i + 1));
        _tmpcombolist->setFont(fontManager.textFont(fontSize));
        _tmpcombolist->label->setFixedSize(labelWidth, ITEM_H - 2);
        _tmpcombolist->combolist->setFixedSize(btnWidth, ITEM_H - 2);
        _tmpcombolist->SetID(i);


        connect(_tmpcombolist, SIGNAL(currentIndexChanged(int, int)), this,
                SLOT(_codeMarkerChangeSlot(int, int)));

        if (i < CODEMARKER_MAX / 2)
        {
            leftLayout->addWidget(_tmpcombolist);
        }
        else
        {
            rightLayout->addWidget(_tmpcombolist);
        }
        _codeMarkerList.append(_tmpcombolist);
        _tmpcombolist = NULL;
    }

    mainLayout->addLayout(layout);
}

/**************************************************************************************************
 * code marker change。
 *************************************************************************************************/
void SupervisorCodeMarker::_codeMarkerChangeSlot(int id, int index)
{
    if (id >= _selectedCodeMarkers.size())
    {
        return;
    }

    _selectedCodeMarkers[id] = _allCodeMarkers[index];  // 新的code marker。
    QStringList tmpList(_selectedCodeMarkers);
    QString strValue = tmpList.join(",");
    int num = 0;
    currentConfig.getNumAttr("Local|Language", "CurrentOption", num);
    QString markerStr = "CodeMarker|SelectMarker|Language";
    markerStr += QString::number(num, 10);
    currentConfig.setStrValue(markerStr, strValue);
}

/**************************************************************************************************
 * code marker change。
 *************************************************************************************************/
void SupervisorCodeMarker::_returnBtnSlot()
{
    emit returnItemList();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorCodeMarker::~SupervisorCodeMarker()
{

}

