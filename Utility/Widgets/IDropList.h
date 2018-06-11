#pragma once
#include <QList>
#include "LabelButton.h"

class ComboListPopup;
class IDropList : public LButton
{
    Q_OBJECT
public:
    IDropList(const QString &title);
    ~IDropList();

    //添加子项
    void addItem(const QString &title);
    void addItem(const QStringList &titleList);

    //清空子项
    void clearItem();

    //当前选择项
    int currentIndex();

    //项数
    int count();

    //子项名称
    QString itemText(int index);

    //设置当前子项
    void setCurrentIndex(int index);

signals:
    void currentIndexChange(int index);

protected:
    void hideEvent(QHideEvent *e);

    // 处理键盘和飞棱事件。
    void keyPressEvent(QKeyEvent *e)
    {
        switch (e->key())
        {
            case Qt::Key_Up:
            case Qt::Key_Down:
                e->ignore();
                return;
            default:
                break;
        }

        QPushButton::keyPressEvent(e);
    }

private slots:
    void _showPopup();
    void _popupDestroyed();

private:
    ComboListPopup *_popup;

private:
    int _curIndex;
    QStringList _strList;
};

