#pragma once
#include "PopupWidget.h"

class IButton;
class IComboList;
class WaveWidgetSelectMenu : public PopupWidget
{
    Q_OBJECT

public:
    static WaveWidgetSelectMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new WaveWidgetSelectMenu();
        }
        return *_selfObj;
    }
    static WaveWidgetSelectMenu *_selfObj;
    ~WaveWidgetSelectMenu();

public:
    // 设置是否为操作Top Waveform。
    void setTopWaveform(bool isTopWaveform);

    // 设置被操作的波形控件。
    void setWaveformName(const QString &name);

    // 是否关联了TopWaveform。
    bool isTopWaveform(void) const;

    // 重新刷新。
    void refresh(void);

    // 更新MFC波形名称
    void updateMFCWaveName(const QString &name);

    // 设置显示坐标
    inline void setShowPoint(int x, int y) {_x = x; _y = y;}

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    void _loadWaveformsSlot(void);
    void _replaceListSlot(int index);
    void _insertListSlot(int index);
    void _removeButtonSlot(void);

private:
    WaveWidgetSelectMenu();
    void _loadWaveforms(void);

    IComboList *_replaceList;
    IComboList *_insertList;
    IButton *_removeButton;

    bool _isTopWaveform;
    QString _waveformName;

    // 保存替换选项和插入选项中的波形信息。
    QStringList _replaceWaveforms;
    QStringList _replaceWaveformTitles;
    QStringList _insertWaveforms;
    QStringList _insertWaveformTitles;

private:
    int _x;
    int _y;
};
#define waveWidgetSelectMenu (WaveWidgetSelectMenu::construction())
#define deleteWaveWidgetSelectMenu() (delete WaveWidgetSelectMenu::_selfObj)
