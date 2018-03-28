#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QScrollBar>
#include <qmath.h>
#include "RescueDataListWidget.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "IButton.h"
#include "DataStorageDirManager.h"
#include "TimeDate.h"
#include <QDateTime>

#define ITEM_HEIGHT (30)
#define PIC_WIDTH (16)

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RescueDataListWidget::RescueDataListWidget(int w, int h) : QWidget()
{
    _singleSelect = false;
    _isShowCurRescue = true;
    _PageNum = h / (ITEM_HEIGHT + 2);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setMargin(1);
    leftLayout->setSpacing(2);

    int fontSize = fontManager.getFontSize(1);
    for (int i = 0; i < _PageNum; ++i)
    {
        QHBoxLayout *layout = new QHBoxLayout();
        layout->setMargin(0);
        layout->setSpacing(2);

        IButton *btn1 = new IButton();
        btn1->setId(i * 2);
        btn1->setFixedHeight(ITEM_HEIGHT);
        btn1->setFont(fontManager.textFont(fontSize));
        btn1->setPicAlignment(Qt::AlignRight | Qt::AlignVCenter);
        btn1->setPicture(QImage());
        connect(btn1, SIGNAL(realReleased()), this, SLOT(_btnPressed()));

        IButton *btn2 = new IButton();
        btn2->setId(i * 2 + 1);
        btn2->setFixedHeight(ITEM_HEIGHT);
        btn2->setFont(fontManager.textFont(fontSize));
        btn2->setPicAlignment(Qt::AlignRight | Qt::AlignVCenter);
        btn2->setPicture(QImage());
        connect(btn2, SIGNAL(realReleased()), this, SLOT(_btnPressed()));

        layout->addWidget(btn1);
        layout->addWidget(btn2);

        leftLayout->addLayout(layout);

        _btnList.append(btn1);
        _btnList.append(btn2);
    }

    _bar = new QScrollBar();
    _bar->setOrientation(Qt::Vertical);
    _bar->setFixedHeight(_PageNum * (ITEM_HEIGHT + 2) - 2);
    _bar->setMinimum(0);
    _bar->setMaximum(0);
    _bar->setSingleStep(1);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(2);
    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(_bar);

    setLayout(mainLayout);
    setFixedSize(w, h);
    setFocusPolicy(Qt::NoFocus);

    _totalPage = 0;
    _curPage = 0;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RescueDataListWidget::~RescueDataListWidget()
{
    _strList.clear();
    _checkFlag.clear();
    _btnList.clear();
}

/**************************************************************************************************
 * 数据加载。
 *************************************************************************************************/
void RescueDataListWidget::_caclInfo()
{
    _strList.clear();
    QStringList list;
    dataStorageDirManager.getRescueEvent(list);
    _strList.append(list);
    list.clear();

    if (!_isShowCurRescue)
    {
        _strList.takeAt(0);
    }

    //计算页码
    int count = _strList.count();
    _totalPage = count / (_PageNum * 2);
    _totalPage += (count % (_PageNum * 2)) ? 1 : 0;
    _curPage = 0;

    //重新初始化选择标记
    _checkFlag.clear();
    for (int i = 0; i < count; ++i)
    {
        _checkFlag.append(0);
    }

    _bar->setMaximum(_totalPage - 1);
    emit pageInfoChange();
}

/**************************************************************************************************
 * 转换时间字符串。
 *************************************************************************************************/
QString RescueDataListWidget::_convertTimeStr(const QString &str)
{
    QString timeStr;
    QDateTime dt = QDateTime::fromString(str, "yyyyMMddHHmmss");
    timeDate.getDateTime(dt.toTime_t(), timeStr, true, true);
    return timeStr;
}

/**************************************************************************************************
 * 数据加载。
 *************************************************************************************************/
void RescueDataListWidget::_loadData()
{
    int count = _strList.count();
    int btnNum = _btnList.count();
    int index = _curPage * btnNum;

    for (int i = 0; i < btnNum; ++i)
    {
        if (index < count)
        {
            _btnList.at(i)->setText(_convertTimeStr(_strList.at(index)));

            if (0 == _checkFlag.at(index))
            {
                _btnList.at(i)->setPicture(QImage());
            }
            else
            {
                QImage pixmap("/usr/local/nPM/icons/select.png");
                pixmap = pixmap.scaled(PIC_WIDTH, PIC_WIDTH);
                _btnList.at(i)->setPicture(pixmap);
            }

            _btnList.at(i)->setFocusPolicy(Qt::StrongFocus);
        }
        else
        {
            _btnList.at(i)->setText("");
            _btnList.at(i)->setPicture(QImage());
            _btnList.at(i)->setFocusPolicy(Qt::NoFocus);
        }

        index++;
    }

    _bar->setValue(_curPage);
}

/**************************************************************************************************
 * 按钮回调。
 *************************************************************************************************/
void RescueDataListWidget::_btnPressed()
{
    IButton *btn = (IButton *)sender();
    if (NULL == btn)
    {
        return;
    }

    int id = btn->id();
    if (id >= _checkFlag.count())
    {
        return;
    }

    int btnCount = _btnList.count();
    CheckIter iter = _checkFlag.begin();
    int index = _curPage * btnCount + id;
    if (0 == *(iter + index))
    {
        QImage pixmap("/usr/local/nPM/icons/select.png");
        pixmap = pixmap.scaled(PIC_WIDTH, PIC_WIDTH);
        btn->setPicture(pixmap);
        *(iter + index) = 1;
    }
    else
    {
        btn->setPicture(QImage());
        *(iter + index) = 0;
    }

    if (_singleSelect)
    {
        if (_checkFlag.count(1) <= 1)
        {
            return;
        }

        for (int i = 0; i < btnCount; ++i)
        {
            if (i == id)
            {
                continue;
            }

            IButton *clearbtn = _btnList.at(i);
            if (NULL != clearbtn)
            {
                if (!clearbtn->isPictureNull())
                {
                    clearbtn->setPicture(QImage());
                }
            }
        }

        int tmpIndex = 0;
        while (iter != _checkFlag.end())
        {
            if (tmpIndex++ == index)
            {
                iter++;
                continue;
            }

            *iter = 0;
            iter++;
        }
    }
}

/**************************************************************************************************
 * 获取选择标志
 *************************************************************************************************/
void RescueDataListWidget::getCheckList(QList<int> &list)
{
    list.clear();

    int count = _strList.count();
    for (int i = 0; i < count; ++i)
    {
        if (_checkFlag.at(i))
        {
            list << i;
        }
    }
}

/**************************************************************************************************
 * 获取选择标志
 *************************************************************************************************/
void RescueDataListWidget::getCheckList(QStringList &list)
{
    list.clear();

    int count = _strList.count();
    for (int i = 0; i < count; ++i)
    {
        if (_checkFlag.at(i))
        {
            list << _strList.at(i);
        }
    }
}

/**************************************************************************************************
 * 获取全部字串
 *************************************************************************************************/
void RescueDataListWidget::getStrList(QStringList &strList)
{
    strList.clear();
    strList.append(_strList);
}

/**************************************************************************************************
 * 设置是否显示当前营救
 *************************************************************************************************/
void RescueDataListWidget::setShowCurRescue(bool flag)
{
    _isShowCurRescue = flag;
}

int RescueDataListWidget::getCurPage() const
{
    return _curPage;
}

int RescueDataListWidget::getTotalPage() const
{
    return _totalPage;
}

void RescueDataListWidget::setScrollbarVisiable(bool visiable)
{
    return _bar->setVisible(visiable);
}

/**************************************************************************************************
 * 重新加载数据
 *************************************************************************************************/
void RescueDataListWidget::reload()
{
    _caclInfo();
    _loadData();
}

/**************************************************************************************************
 * 翻页
 *************************************************************************************************/
void RescueDataListWidget::pageChange(bool upPage)
{
    if(_totalPage == 0)
    {
        return;
    }

    if (upPage)
    {
        if (0 == _curPage)
        {
            return;
        }
        else if(_curPage > 0)
        {
            --_curPage;
        }
    }
    else
    {
        if ((_totalPage - 1) == _curPage)
        {
            return;
        }
        else if(_curPage < (_totalPage - 1))
        {
            ++_curPage;
        }
    }

    _loadData();
    emit pageInfoChange();
}

/**************************************************************************************************
 * 显示事件
 *************************************************************************************************/
void RescueDataListWidget::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);

    _caclInfo();
    _loadData();
}
