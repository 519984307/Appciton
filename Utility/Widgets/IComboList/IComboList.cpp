#include <QHBoxLayout>
#include "IComboList.h"
#include "IConfig.h"
#include "ColorManager.h"

/**************************************************************************************************
 * CurrentIndexChanged的槽函数。
 *************************************************************************************************/
void IComboList::_currentIndexChanged(int index)
{
    if (!signalsBlocked())
    {
        emit currentIndexChanged(index);
        emit currentIndexChanged(_id, index);
    }
}

/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
IComboList::IComboList(const QString &text, bool isVertical) : QWidget(NULL),
        label(NULL), combolist(NULL), _isVertical(isVertical)
{
    _id = 0;
    label = new QLabel(text, this);
    combolist = new ComboList(this);
    combolist->setBorderRadius(4);

    // 链接信号槽。
    connect(combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_currentIndexChanged(int)));
    connect(combolist, SIGNAL(currentIndexChanged(const QString &)),
            this, SIGNAL(currentIndexChanged(const QString &)));
    setLabelAlignment(Qt::AlignVCenter | Qt::AlignRight);

    // 设置能接受焦点
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocusProxy(combolist);

    QBoxLayout *layout;
    if (isVertical)
    {
        layout = new QVBoxLayout();
        layout->addWidget(label, 0, Qt::AlignLeft);
        layout->addWidget(combolist);
    }
    else
    {
        layout = new QHBoxLayout();
        layout->setSpacing(ICOMBOLIST_SPACE);
        layout->setMargin(0);
        layout->addWidget(label);
        layout->addWidget(combolist);
    }

    setLayout(layout);
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
IComboList::~IComboList()
{
    delete label;
    label = NULL;

    delete combolist;
    combolist = NULL;
}

/**************************************************************************************************
 * 设置文本和下拉框的水平拉伸因子。
 *************************************************************************************************/
void IComboList::setStretch(int labelStretch, int combolistStretch)
{
    if (_isVertical)
    {
        return;
    }

    QHBoxLayout *hLayout = (QHBoxLayout *) layout();
    if (NULL == hLayout)
    {
        return;
    }

    hLayout->setStretch(0, labelStretch);
    hLayout->setStretch(1, combolistStretch);
}

/**************************************************************************************************
 * 设置间隙。
 *************************************************************************************************/
void IComboList::setSpacing(int space)
{
    if (_isVertical)
    {
        QVBoxLayout *vlayout = (QVBoxLayout *)layout();
        if (NULL == vlayout)
        {
            return;
        }

        vlayout->setSpacing(space);
    }
    else
    {
        QHBoxLayout *hlayout = (QHBoxLayout *)layout();
        if (NULL == hlayout)
        {
            return;
        }

        hlayout->setSpacing(space);
    }
}

/**************************************************************************************************
 * 设置文本的水平位置：靠左、居中、靠右。
 *************************************************************************************************/
void IComboList::setLabelAlignment(Qt::Alignment align)
{
    if (NULL == label)
    {
        return;
    }

    label->setAlignment(align);
}

/**************************************************************************************************
 * 添加Item。
 *************************************************************************************************/
void IComboList::addItem(const QString &text, const QVariant &userData)
{
    if (NULL == combolist)
    {
        return;
    }

    combolist->blockSignals(true);
    combolist->addItem(text, userData);
    combolist->blockSignals(false);
}

/**************************************************************************************************
 * 添加多个Item。
 *************************************************************************************************/
void IComboList::addItems(const QStringList &texts)
{
    if (NULL == combolist)
    {
        return;
    }

    combolist->blockSignals(true);
    combolist->addItems(texts);
    combolist->blockSignals(false);
}

/**************************************************************************************************
 * 获取当前Item的索引。
 *************************************************************************************************/
int IComboList::currentIndex() const
{
    if (NULL == combolist)
    {
        return -1;
    }

    return combolist->currentIndex();
}

/**************************************************************************************************
 * 获取当前Item的文本信息。
 *************************************************************************************************/
QString IComboList::currentText() const
{
    if (NULL == combolist)
    {
        return "";
    }

    return combolist->currentText();
}

/**************************************************************************************************
 * 获取Item的文本信息。
 *************************************************************************************************/
QString IComboList::itemText(int index) const
{
    if (NULL == combolist)
    {
        return "";
    }

    return combolist->itemText(index);
}

/**************************************************************************************************
 * 获取Item的变量信息。
 *************************************************************************************************/
QVariant IComboList::itemData(int index, int role) const
{
    if (NULL == combolist)
    {
        QVariant temp(QVariant::Invalid);
        return temp;
    }

    return combolist->itemData(index, role);
}

/**************************************************************************************************
 * 设置当前Item。
 *************************************************************************************************/
void IComboList::setCurrentItem(const QString &item)
{
    combolist->blockSignals(true);
    int len = combolist->count();
    for (int i = 0; i < len; i++)
    {
        if (combolist->itemText(i) == item)
        {
            combolist->setCurrentIndex(i);
            break;
        }
    }
    combolist->blockSignals(false);
}

/**************************************************************************************************
 * 插入Item。
 *************************************************************************************************/
void IComboList::insertItem(int index, const QString &text, const QVariant &userData)
{
    if (NULL == combolist)
    {
        return;
    }

    combolist->insertItem(index, text, userData);
}

/**************************************************************************************************
 * 插入多个Item。
 *************************************************************************************************/
void IComboList::insertItems(int index, const QStringList &texts)
{
    if (NULL == combolist)
    {
        return;
    }

    combolist->insertItems(index, texts);
}

/**************************************************************************************************
 * 设置Item文本。
 *************************************************************************************************/
void IComboList::setItemText(int index, const QString &text)
{
    if (NULL == combolist)
    {
        return;
    }

    combolist->setItemText(index, text);
}

/**************************************************************************************************
 * 设置Item值。
 *************************************************************************************************/
void IComboList::setItemData(int index, const QVariant &value, int role)
{
    if (NULL == combolist)
    {
        return;
    }

    combolist->setItemData(index, value, role);
}

/**************************************************************************************************
 * 移除Item。
 *************************************************************************************************/
void IComboList::removeItem(int index)
{
    if (NULL == combolist)
    {
        return;
    }

    combolist->removeItem(index);
}

/**************************************************************************************************
 * 查找Item。
 *************************************************************************************************/
int IComboList::findText(const QString &text) const
{
    if (NULL == combolist)
    {
        return -1;
    }

    return combolist->findText(text);
}

/**************************************************************************************************
 * 查找Item。
 *************************************************************************************************/
int IComboList::findData(const QVariant &data) const
{
    if (NULL == combolist)
    {
        return -1;
    }

    return combolist->findData(data);
}

/**************************************************************************************************
 * 选项数量。
 *************************************************************************************************/
int IComboList::count() const
{
    if (NULL == combolist)
    {
        return 0;
    }

    return combolist->count();
}

/**************************************************************************************************
 * 设置ID。
 *************************************************************************************************/
void IComboList::SetID(int id)
{
    _id = id;
}

/**************************************************************************************************
 * 设置后缀。
 *************************************************************************************************/
void IComboList::setSuffix(const QString &suffix)
{
    if (NULL == combolist)
    {
        return;
    }

    combolist->setSuffix(suffix);
}

/**************************************************************************************************
 * 获取后缀。
 *************************************************************************************************/
QString IComboList::getSuffix()
{
    if (NULL == combolist)
    {
        return "";
    }

    return combolist->getSuffix();
}

/**************************************************************************************************
 * 移除所有Item。
 *************************************************************************************************/
void IComboList::clear()
{
    if (NULL == combolist)
    {
        return;
    }

    combolist->blockSignals(true);
    combolist->clear();
    combolist->blockSignals(false);
}

/**************************************************************************************************
 * 设置当前Item。
 *************************************************************************************************/
void IComboList::setCurrentIndex(int index)
{
    if (NULL == combolist)
    {
        return;
    }

    combolist->blockSignals(true);
    combolist->setCurrentIndex(index);
    combolist->blockSignals(false);
}
