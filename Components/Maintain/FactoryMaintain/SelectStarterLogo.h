#pragma once
#include "SubMenu.h"
#include <QLabel>
#include "IComboList.h"

class SelectStarterLogo : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static SelectStarterLogo &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SelectStarterLogo();
        }
        return *_selfObj;
    }
    static SelectStarterLogo *_selfObj;
    ~SelectStarterLogo();
    /**
     * @brief eventFilter  事件过滤方法
     * @param obj  作用对象
     * @param ev  对象中事件
     * @return
     */
    virtual bool eventFilter(QObject *obj, QEvent *ev);
protected:
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow  加载数据方法
     */
    virtual void readyShow(void);
private slots:

private:
    SelectStarterLogo();

    enum SelectStarterLogoType
    {
        SELECT_STATRTER_LOGO=0,
        SELECT_STARTER_TYPE_NR
    };

    QString     _stringTypeName[SELECT_STARTER_TYPE_NR];

    IComboList  *_selectLogoCombo[SELECT_STARTER_TYPE_NR];

    QStringList _logoNames;

    int         _returnFlag;
};
#define selectStarterLogo (SelectStarterLogo::construction())
