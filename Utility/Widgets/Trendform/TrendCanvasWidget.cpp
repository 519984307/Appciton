////////////////////////////////////////////////////////////////////////////////
// 说明：
// WaveWidget是一个波形控件，支持扫描模式，滚动模式，回顾模式及三者对应的级联模式
////////////////////////////////////////////////////////////////////////////////

#include <QPainter>
#include <QPaintEvent>
#include "TrendCanvasWidget.h"
#include "SystemManager.h"
#include "ColorManager.h"

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 构造函数
//
// 参数:
// type: 波形参数ID
////////////////////////////////////////////////////////////////////////////////
TrendCanvasWidget::TrendCanvasWidget(QWidget *parent) : IWidget("TrendCanvasWidget", parent),
    _parent(parent),_background(NULL),
    _lineWidth(1),_isShowGrid(true),
    _isShowFrame(true), _isShowScale(true), _totalTrend(60)
{
    setFocusPolicy(Qt::NoFocus);

    _pixelWPitch = systemManager.getScreenPixelWPitch();
    _pixelHPitch = systemManager.getScreenPixelHPitch();

    // 禁用Qt背景预绘, 由波形控件自己管理背景图层, 优化性能
    setAttribute(Qt::WA_OpaquePaintEvent, true);

    // 根据参数类型初始化波形与标签的颜色
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    palette.setColor(QPalette::WindowText, Qt::white);
    setPalette(palette);
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 析构函数
////////////////////////////////////////////////////////////////////////////////
TrendCanvasWidget::~TrendCanvasWidget()
{
    if (_background)
    {
        delete _background;
        _background = NULL;
    }

    TrendDataMap::iterator it = _trendDataItem.begin();
    for (; it != _trendDataItem.end(); ++it)
    {
        QList<TrendDataType> trendData = it.value();
        trendData.clear();
    }
    _trendDataItem.clear();
}

/***************************************************************************************************
 * 注册一个TrendDataItem。
 **************************************************************************************************/
void TrendCanvasWidget::registerTrendData(SubParamID id)
{
    TrendDataMap::iterator trendItem = _trendDataItem.find(id);
    if(trendItem == _trendDataItem.end())
    {
        trendItem = _trendDataItem.insert(id, QList<TrendDataType>());
    }
}

/***************************************************************************************************
 * 添加数据。
 **************************************************************************************************/
void TrendCanvasWidget::addTrendData(SubParamID id, TrendDataType data)
{
    TrendDataMap::iterator trendItem = _trendDataItem.find(id);
    if(trendItem == _trendDataItem.end())
    {
        return;
    }
    trendItem.value().append(data);
}


////////////////////////////////////////////////////////////////////////////////
// 说明:
// 重绘波形控件, 支持局部重绘
//
// 参数:
// e: 事件对象
//
// 返回值:
// 无
////////////////////////////////////////////////////////////////////////////////
void TrendCanvasWidget::paintEvent(QPaintEvent *e)
{
    _resetBackground();

    if (e->region().rectCount() == 1)
    {
        QPainter painter(this);
        QRect rect = e->rect();

        // 绘背景
        painter.drawPixmap(rect, *_background, rect);

        // 绘波形
        paintWave(painter, rect);
    }
    else
    {
        QPainter painter(this);
        QVector<QRect> rects = e->region().rects();
        int cnt = rects.size();
        for (int i = 0; i < cnt; i++)
        {
            QRect rect = rects.at(i);

            // 绘背景
            painter.drawPixmap(rect, *_background, rect);

            // 绘波形
            paintWave(painter, rect);
        }
    }

    IWidget::paintEvent(e);
}

void TrendCanvasWidget::_resetBackground()
{
    // 确保背景位图总是与控件尺寸一致
    if (_background)
    {
        if ((_background->width() != width()) || (_background->height() != height()))
        {
            delete _background;
            _background = NULL;
        }
    }

    if (!_background)
    {
        _background = new QPixmap(_parent->width(), _parent->height());
    }

    QPainter painter(_background);

    // 底色
    painter.fillRect(_background->rect(), palette().window());

    int xstep = qRound(5 / _pixelWPitch);       // 网格间隔固定5mm
    int ystep = qRound(5 / _pixelHPitch);
    QPoint gridLeftTop = mapFromParent(QPoint(_roundUp(geometry().left(), xstep),
            _roundUp(geometry().top(), ystep)));
    QPoint gridRightBottom = mapFromParent(QPoint(_roundDown(geometry().right(), xstep),
            _roundDown(geometry().bottom(), ystep)));

    // 边框
    if (_isShowFrame)
    {
        painter.setPen(QPen(Qt::white, 1, Qt::SolidLine));
        painter.drawRect(gridLeftTop.x(),gridLeftTop.y(),gridRightBottom.x(),gridRightBottom.y());
    }
    // 刻度
    if (_isShowScale)
    {
        painter.setPen(QPen(Qt::white, 1, Qt::SolidLine));

        //画上x轴刻度线
        for(int i=0;i<5;i++)//分成10份
        {
            //选取合适的坐标，绘制一段长度为4的直线，用于表示刻度
            painter.drawLine(gridLeftTop.x() + (i * (gridRightBottom.x() / 5)),gridRightBottom.y(),
                             gridLeftTop.x() + (i * (gridRightBottom.x() / 5)),(gridRightBottom.y() + 4));

            painter.drawText(gridLeftTop.x() + (i * (gridRightBottom.x() / 5)) + 2,  (gridRightBottom.y() + 10)
                             ,QString::number(i));
        }

        //y轴刻度线
        for(int i=0;i<3;i++)
        {
            //主要就是确定一个位置，然后画一条短短的直线表示刻度。
            painter.drawLine(gridLeftTop.x(),(gridLeftTop.y() + (i * (gridRightBottom.y() / 3))),
                             gridLeftTop.x()-4,(gridLeftTop.y() + (i * (gridRightBottom.y() / 3))));


            painter.drawText(gridLeftTop.x()-8,(gridLeftTop.y() + (i * (gridRightBottom.y() / 3)) - 2),
                             QString::number(i));
        }
    }
    // 网格
    if (_isShowGrid)
    {
        painter.setPen(QPen(Qt::white, 1, Qt::DotLine));


        for (int x = (gridLeftTop.x() + xstep); x <= (gridRightBottom.x() - xstep); x += xstep)
        {
            painter.drawLine(x, gridLeftTop.y(), x, gridRightBottom.y());
        }
        for (int y = (gridLeftTop.y() + ystep); y <= (gridRightBottom.y() - ystep); y += ystep)
        {
            painter.drawLine(gridLeftTop.x(), y, gridRightBottom.x(), y);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// 说明:
// 对于给定的数值value, 计算大于等于value且能整除step的数值
////////////////////////////////////////////////////////////////////////////////
int TrendCanvasWidget::_roundUp(int value, int step)
{
    while (value % step)
    {
        value++;
    }
    return value;
}

////////////////////////////////////////////////////////////////////////////////
// 说明:
// 对于给定的数值value, 计算小于等于value且能整除step的数值
////////////////////////////////////////////////////////////////////////////////
int TrendCanvasWidget::_roundDown(int value, int step)
{
    while (value % step)
    {
        value--;
    }
    return value;
}

void TrendCanvasWidget::paintWave(QPainter &painter, const QRect &/*rect*/)
{
    int xstep = qRound(5 / _pixelWPitch);       // 网格间隔固定5mm
    int ystep = qRound(5 / _pixelHPitch);
    QPoint gridLeftTop = mapFromParent(QPoint(_roundUp(geometry().left(), xstep),
            _roundUp(geometry().top(), ystep)));
    QPoint gridRightBottom = mapFromParent(QPoint(_roundDown(geometry().right(), xstep),
            _roundDown(geometry().bottom(), ystep)));

    //确定坐标轴起点坐标
    int pointx=gridLeftTop.x();
    int pointy=gridRightBottom.y();
    //确定坐标轴宽度跟高度
    int width=gridRightBottom.x() - gridLeftTop.x();
//    int height=gridRightBottom.y() - gridLeftTop.y();

    double kx=(double)width/(_totalTrend-1);   //x轴的系数
    double ky=0.5;                             //y方向的比例系数

    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(_lineWidth);
    ParamID paramID;

    TrendDataMap::iterator it = _trendDataItem.begin();
    for (; it != _trendDataItem.end(); ++it)
    {
        for(int i = 0; i < (it.value().count() - 1); i++)
        {
            paramID = paramInfo.getParamID(it.key());
            pen.setColor(colorManager.getColor(paramInfo.getParamName(paramID)));

            if (it.value().at(i) == InvData() || it.value().at(i + 1)== InvData())
            {
//                painter.setPen(penPoint);//蓝色的笔，用于标记各个点
//                painter.drawPoint(pointx + kx * i, pointy - (it.value().at(i) * ky));
                continue;
            }
            //由于y轴是倒着的，所以y轴坐标要pointy-a[i]*ky 其中ky为比例系数
            painter.setPen(pen);//黑色笔用于连线
            painter.drawLine(pointx + kx * i, pointy - (it.value().at(i) * ky),
                             pointx + kx * (i + 1), pointy - (it.value().at(i + 1) * ky));
//            painter.setPen(penPoint);//蓝色的笔，用于标记各个点
//            painter.drawPoint(pointx + kx * i, pointy - (it.value().at(i) * ky));
        }

//        painter.drawPoint(pointx + (kx * (it.value().count() - 1)),
//                          pointy - (it.value().at(it.value().count() - 1) * ky));//绘制最后一个点

        it.value().clear();
    }
}
