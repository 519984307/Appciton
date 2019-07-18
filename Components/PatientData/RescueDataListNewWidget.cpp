/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/22
 **/

#include "RescueDataListNewWidget.h"
#include <QVBoxLayout>
#include "FontManager.h"
#include <QVariant>
#include "DataStorageDirManager.h"
#include <QDateTime>
#include "TimeDate.h"
#include "ThemeManager.h"

#define PIC_WIDTH (16)

class RescueDataListNewWidgetPrivate
{
public:
    RescueDataListNewWidgetPrivate()
        :singleSelect(false),
          isShowCurRescue(true),
          totalPage(0),
          curPage(0),
          pageNum(-1),
          widgetHeight(themeManger.getAcceptableControlHeight())
    {}

    ~RescueDataListNewWidgetPrivate(){}

    QString convertTimeStr(const QString &str);
    bool singleSelect;              // only select one item, default select multi items
    bool isShowCurRescue;           // 显示当前营救，默认显示
    int totalPage;                  // 总页数
    int curPage;                    // 当前页
    int pageNum;                    // 每一页的个数
    int widgetHeight;

    QList<QString> strList;         // 字符
    QList<unsigned char> checkFlag; // 选中标志
    QList<Button *> btnList;        // 按钮列表
};

RescueDataListNewWidget::RescueDataListNewWidget(int w, int h)
    :d_ptr(new RescueDataListNewWidgetPrivate())
{
    d_ptr->pageNum = h / (d_ptr->widgetHeight + 2);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setMargin(1);
    leftLayout->setSpacing(2);

    int fontSize = fontManager.getFontSize(3);
    for (int i = 0; i < d_ptr->pageNum; ++i)
    {
        QHBoxLayout *layout = new QHBoxLayout();
        layout->setMargin(0);
        layout->setSpacing(2);

        Button *btn1 = new Button();
        btn1->setProperty("Id" , qVariantFromValue(i * 2));
        btn1->setFont(fontManager.textFont(fontSize));
        btn1->setButtonStyle(Button::ButtonTextBesideIcon);
        connect(btn1, SIGNAL(released()), this, SLOT(_btnPressed()));

        Button *btn2 = new Button();
        btn2->setProperty("Id" , qVariantFromValue(i * 2 + 1));
        btn2->setFont(fontManager.textFont(fontSize));
        btn2->setButtonStyle(Button::ButtonTextBesideIcon);
        connect(btn2, SIGNAL(released()), this, SLOT(_btnPressed()));

        layout->addWidget(btn1);
        layout->addWidget(btn2);

        leftLayout->addLayout(layout);

        d_ptr->btnList.append(btn1);
        d_ptr->btnList.append(btn2);
    }

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(2);
    mainLayout->addLayout(leftLayout);

    setLayout(mainLayout);
    setFixedSize(w, h);
    setFocusPolicy(Qt::NoFocus);
}

RescueDataListNewWidget::~RescueDataListNewWidget()
{
    delete d_ptr;
}

void RescueDataListNewWidget::getCheckList(QList<int> &list)
{
    list.clear();

    int count = d_ptr->strList.count();
    for (int i = 0; i < count; ++i)
    {
        if (d_ptr->checkFlag.at(i))
        {
            list << i;
        }
    }
}

void RescueDataListNewWidget::getCheckList(QStringList &list)
{
    list.clear();

    int count = d_ptr->strList.count();
    for (int i = 0; i < count; ++i)
    {
        if (d_ptr->checkFlag.at(i))
        {
            list << d_ptr->strList.at(i);
        }
    }
}

void RescueDataListNewWidget::getStrList(QStringList &strList)
{
    strList.clear();
    strList.append(d_ptr->strList);
}

void RescueDataListNewWidget::pageChange(bool upPage)
{
    if (d_ptr->totalPage == 0)
    {
        return;
    }

    if (upPage)
    {
        if (0 == d_ptr->curPage)
        {
            return;
        }
        else if (d_ptr->curPage > 0)
        {
            --d_ptr->curPage;
        }
    }
    else
    {
        if ((d_ptr->totalPage - 1) == d_ptr->curPage)
        {
            return;
        }
        else if (d_ptr->curPage < (d_ptr->totalPage - 1))
        {
            ++d_ptr->curPage;
        }
    }

    _loadData();
    emit pageInfoChange();
}

void RescueDataListNewWidget::reload()
{
    _caclInfo();
    _loadData();
}

void RescueDataListNewWidget::setShowCurRescue(bool flag)
{
    d_ptr->isShowCurRescue = flag;
}

void RescueDataListNewWidget::setSelectSingle(bool flag)
{
    d_ptr->singleSelect = flag;
}

int RescueDataListNewWidget::getCurPage() const
{
    return d_ptr->curPage;
}

int RescueDataListNewWidget::getTotalPage() const
{
    return d_ptr->totalPage;
}

void RescueDataListNewWidget::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    _caclInfo();
    _loadData();
}

void RescueDataListNewWidget::_btnPressed()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (NULL == btn)
    {
        return;
    }

    int id = btn->property("Id").toInt();
    if (id >= d_ptr->checkFlag.count())
    {
        return;
    }

    int btnCount = d_ptr->btnList.count();
    CheckIter iter = d_ptr->checkFlag.begin();
    int index = d_ptr->curPage * btnCount + id;
    int strCount = d_ptr->strList.count();

    if (index + 1 > strCount)
    {
        return;
    }

    if (0 == *(iter + index))
    {
        QIcon pixmap = themeManger.getIcon(ThemeManager::IconChecked);
        btn->setIcon(pixmap);
        *(iter + index) = 1;
    }
    else
    {
        btn->setIcon(QIcon());
        *(iter + index) = 0;
    }

    if (d_ptr->singleSelect)
    {
        if (d_ptr->checkFlag.count(1) <= 1)
        {
            return;
        }

        for (int i = 0; i < btnCount; ++i)
        {
            if (i == id)
            {
                continue;
            }

            Button *clearbtn = d_ptr->btnList.at(i);
            if (NULL != clearbtn)
            {
                clearbtn->setIcon(QIcon());
            }
        }

        int tmpIndex = 0;
        while (iter != d_ptr->checkFlag.end())
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
    emit btnRelease();
}

void RescueDataListNewWidget::_loadData()
{
    int count = d_ptr->strList.count();
    int btnNum = d_ptr->btnList.count();
    int index = d_ptr->curPage * btnNum;

    for (int i = 0; i < btnNum; ++i)
    {
        if (index < count)
        {
            d_ptr->btnList.at(i)->setText(d_ptr->convertTimeStr(d_ptr->strList.at(index)));

            if (0 == d_ptr->checkFlag.at(index))
            {
                d_ptr->btnList.at(i)->setIcon(QIcon());
            }
            else
            {
                QIcon pixmap("/usr/local/nPM/icons/select.png");
                d_ptr->btnList.at(i)->setIcon(pixmap);
            }

            d_ptr->btnList.at(i)->setFocusPolicy(Qt::StrongFocus);
        }
        else
        {
            d_ptr->btnList.at(i)->setText("");
            d_ptr->btnList.at(i)->setIcon(QIcon());
            d_ptr->btnList.at(i)->setFocusPolicy(Qt::NoFocus);
        }

        index++;
    }
}

void RescueDataListNewWidget::_caclInfo()
{
    d_ptr->strList.clear();
    QStringList list;
    dataStorageDirManager.getRescueEvent(list);
    d_ptr->strList.append(list);
    list.clear();

    if (!d_ptr->isShowCurRescue)
    {
        d_ptr->strList.takeAt(0);
    }

    //计算页码
    int count = d_ptr->strList.count();
    d_ptr->totalPage = count / (d_ptr->pageNum * 2);
    d_ptr->totalPage += (count % (d_ptr->pageNum * 2)) ? 1 : 0;
    d_ptr->curPage = 0;

    //重新初始化选择标记
    d_ptr->checkFlag.clear();
    for (int i = 0; i < count; ++i)
    {
        d_ptr->checkFlag.append(0);
    }
    emit pageInfoChange();
    emit btnRelease();
}

QString RescueDataListNewWidgetPrivate::convertTimeStr(const QString &str)
{
    QString timeStr;
    QDateTime dt = QDateTime::fromString(str, "yyyyMMddHHmmss");
    timeDate.getDateTime(dt.toTime_t(), timeStr, true, true);
    return timeStr;
}
