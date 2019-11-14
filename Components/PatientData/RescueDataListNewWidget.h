/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/22
 **/

#include <QWidget>
#include "Framework/UI/Button.h"

class RescueDataListNewWidgetPrivate;
class RescueDataListNewWidget : public QWidget
{
    Q_OBJECT

public:
    RescueDataListNewWidget(int w, int h);
    ~RescueDataListNewWidget();

    //获取选择标记字串
    void getCheckList(QList<int> *list);
    void getCheckList(QStringList *list);

    //获取全部字串
    void getStrList(QStringList *strList);

    //翻页
    void pageChange(bool upPage);

    // 重新加载数据
    void reload();

    // 是否显示当前营救
    void setShowCurRescue(bool flag);

    // is single selected
    void setSelectSingle(bool flag);

    int getCurPage() const;
    int getTotalPage() const;

protected:
    void showEvent(QShowEvent *e);

signals:
    void pageInfoChange();
    void btnRelease();
private slots:
    void _btnPressed();
private:
    RescueDataListNewWidgetPrivate *d_ptr;

    void _loadData();
    void _caclInfo();

    typedef QList<QString>::Iterator StrIter;
    typedef QList<unsigned char>::Iterator CheckIter;
    typedef QList<Button *>::Iterator BtnIter;
};
