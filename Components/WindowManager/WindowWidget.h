#pragma once
#include "PopupWidget.h"
#include "PatientDefine.h"
#include "IComboList.h"
#include "ParamDefine.h"

class LabelButton;
class ISpinBox;
class IButton;
class WindowWidget : public QWidget
{
    Q_OBJECT

public:
    WindowWidget();
    ~WindowWidget();

    void layoutExec(void);

    void focusOrder(bool flag);

protected:
    void keyPressEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *event);
    void focusInEvent(QFocusEvent *e);
    bool focusPreviousChild(void);
    bool focusNextChild(void);

private:
    static const int _itemH = 30;     // 子项高度
    QImage *resultImage;
    QPixmap *_pixmap[48];

    QPainter *_painter;
    QPixmap *_wavePixmap[WAVE_NR];
    QFile _demoFile[WAVE_NR];

    void _drawGrid(QPainter *painter);
    void _drawCurves(WaveformID id, QPainter *painter,int maxWaveform);


    static const int _focusedBorderWidth = 4;     // 聚焦边框宽度

    int _focusNum;
};
