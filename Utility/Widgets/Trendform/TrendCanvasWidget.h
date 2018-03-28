#include "IWidget.h"
#include "BaseDefine.h"
#include "ParamDefine.h"

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件，支持扫描模式，滚动模式，回顾模式及三者对应的级联模式
////////////////////////////////////////////////////////////////////////////////

class WaveMode;
class TrendCanvasWidget: public IWidget
{
    Q_OBJECT
public:
    TrendCanvasWidget(QWidget *parent = 0);
    ~TrendCanvasWidget();

    int getTotalTrend() { return _totalTrend; }

    // 注册一个Qlist
    void registerTrendData(SubParamID id);

    // 添加一个波形数据。
    void addTrendData(SubParamID id, TrendDataType data);

protected:
    void paintEvent(QPaintEvent *e);

private:
    void _resetBackground();
    int _roundUp(int value, int step);
    int _roundDown(int value, int step);
    void paintWave(QPainter &painter, const QRect &rect);

    QWidget *_parent;                 // 关联的父控件
    WaveMode *_mode;                          // 当前使用的工作模式
    QPixmap *_background;                     // 背景位图
    int _lineWidth;                           // 波形曲线宽度
    float _pixelWPitch;                // 屏幕像素点距, 单位mm
    float _pixelHPitch;                // 屏幕像素点距, 单位mm
    bool _isShowGrid;                         // 是否显示网格
    bool _isShowFrame;                         // 是否显示边框
    bool _isShowScale;                         // 是否显示刻度
    int _totalTrend;                           //显示的trendData数

    typedef QMap<SubParamID, QList<TrendDataType> > TrendDataMap;
    TrendDataMap _trendDataItem;
};
