#pragma once
#include "SubMenu.h"
#include <QList>

#define CODEMARKER_MAX (28)//codemarker数量

class IComboList;
class SupervisorCodeMarker : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorCodeMarker &construction()
    {
        if (NULL == _selfobj)
        {
            _selfobj = new SupervisorCodeMarker();
        }

        return *_selfobj;
    }
    static SupervisorCodeMarker *_selfobj;

    ~SupervisorCodeMarker();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);
    void keyPressEvent(QKeyEvent *e);
    void hideEvent(QHideEvent *e);

private slots:
    void _codeMarkerChangeSlot(int id, int index);
    void _returnBtnSlot();

private:
    SupervisorCodeMarker();

    // 载入可选项的值。
    void _loadOptions(void);
    IComboList *_tmpcombolist;

    typedef QStringList::Iterator StrListIter;
    QStringList _allCodeMarkers;       //codemarker类型列表
    QStringList _selectedCodeMarkers;    //当前codemerker类型列表
    QList<IComboList *> _codeMarkerList;   //codemarker列表
    int _LanguageIndex;
};

#define supervisorCodeMarker (SupervisorCodeMarker::construction())
