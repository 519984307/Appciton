#pragma once
#include <QWidget>

//营救事件列表
class IButton;
class QScrollBar;
class RescueDataListWidget : public QWidget
{
    Q_OBJECT

public:
    RescueDataListWidget(int w, int h);
    ~RescueDataListWidget();

    //获取选择标记字串
    void getCheckList(QList<int> &list);
    void getCheckList(QStringList &list);

    //获取全部字串
    void getStrList(QStringList &strList);

    //翻页
    void pageChange(bool upPage);

    // 重新加载数据
    void reload();

    // 是否显示当前营救
    void setShowCurRescue(bool flag);

    // is single selected
    void setSelectSingle(bool flag) {_singleSelect = flag;}

    int getCurPage() const;
    int getTotalPage() const;
    void setScrollbarVisiable(bool visiable);

signals:
    void pageInfoChange();

protected:
    void showEvent(QShowEvent *e);

private slots:
    void _btnPressed();

private:
    void _loadData();
    void _caclInfo();
    QString _convertTimeStr(const QString &str);

private:
    bool _singleSelect;              //only select one item, default select multi items
    bool _isShowCurRescue;           //显示当前营救，默认显示
    int _totalPage;                  //总页数
    int _curPage;                    //当前页
    int _PageNum;                    //每一页的个数

    QScrollBar *_bar;
    typedef QList<QString>::Iterator StrIter;
    QList<QString> _strList;         //字符

    typedef QList<unsigned char>::Iterator CheckIter;
    QList<unsigned char> _checkFlag; //选中标志

    typedef QList<IButton *>::Iterator BtnIter;
    QList<IButton *> _btnList;       //按钮列表
};


