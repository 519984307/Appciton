#pragma once
#include <QWidget>
#include <QLabel>
#include <QVariant>
#include <QKeyEvent>
#include "ComboList.h"

#define ICOMBOLIST_SPACE (5)

// Label + ComboList的组合控件。
class Configer;
class IComboList: public QWidget
{
    Q_OBJECT

public:
    IComboList(const QString &label, bool isVertical = false);
    ~IComboList();

public:
    // 设置文本和下拉框的水平拉伸因子
    void setStretch(int labelStretch, int combolistStretch);

    //设置layout中控件间隙
    void setSpacing(int space);

    // 设置文本的水平位置：靠左、居中、靠右
    void setLabelAlignment(Qt::Alignment align);

    // 设置当前Item。
    void setCurrentItem(const QString &item);

    // 设置当前的选项。
    void setCurrentIndex(int index);

    // 设置ID。
    void SetID(int id);

    QLabel *label;                                        // 文本。
    ComboList *combolist;                                 // 下拉框。

public:// --以下接口均复用ComboList类的接口，暂时只实现部分接口，以后有需要再继续补充
    // 添加Item。
    void addItem(const QString &text, const QVariant &userData = QVariant());
    void addItems(const QStringList &texts);

    // 设置和获取后缀。
    void setSuffix(const QString &suffix);
    QString getSuffix();

    // 获取Item信息。
    int currentIndex() const;
    QString currentText() const;
    QString itemText(int index) const;
    QVariant itemData(int index, int role = Qt::UserRole) const;

    // 插入Item。
    void insertItem(int index, const QString &text, const QVariant &userData = QVariant());
    void insertItems(int index, const QStringList &texts);

    // 设置Item。
    void setItemText(int index, const QString &text);
    void setItemData(int index, const QVariant &value, int role = Qt::UserRole);

    // 移除Item。
    void removeItem(int index);

    // 查找Item。
    int findText(const QString &text) const;
    int findData(const QVariant &data) const;

    // Item数量。
    int count() const;

    // 清空。
    void clear();

signals:
    void currentIndexChanged(int index);         // 转发ComboList的currentIndexChanged。
    void currentIndexChanged(const QString &);   // 转发ComboList的currentIndexChanged。
    void currentIndexChanged(int id, int index); // 扩展一个带ID编号的信号。

private slots:
    void _currentIndexChanged(int index);

private:
    bool _isVertical;                                      // 文本下拉框垂直排列。
    int _id;                                               // ID号。
};
