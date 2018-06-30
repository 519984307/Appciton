#pragma once
#include "WaveWidget.h"
#include <QVector>

class OxyCRGTrendWidgetRuler;

class OxyCRGTrendWidget: public WaveWidget
{
    Q_OBJECT

public:
    // 构造与析构。
    OxyCRGTrendWidget(const QString &waveName, const QString &title);
    ~OxyCRGTrendWidget();
    int getRulerWidth(void)const;
    int getRuleHeight(void)const;
    float getRulerPixWidth(void)const;
    void setRuler(int up, int mid, int low);
    /**
     * @brief _resetBuffer 复位缓冲区
     */
    virtual void _resetBuffer();

    /**
     * @brief addDataBuf  添加数据缓存
     * @param value 缓存的数据
     */
    void addDataBuf(int value);

    /**
     * @brief eventFilter 窗口事件过滤方法
     * @param obj  窗口对象
     * @param ev   事件
     * @return     返回 false
     */
    bool eventFilter(QObject *obj, QEvent *ev);

private slots:
    /**
     * @brief onTimeout  用于将波形数据重新放回waveBuf中
     */
    void onTimeout();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    OxyCRGTrendWidgetRuler *_ruler;          // 标尺对象

    int *_spaceFlag; //画虚线标记缓存
    int *_dataBuf; //波形数据缓存
    int _dataBufIndex; //波形数据缓存下标
    int _dataBufLen; //波形数据长度
    int _dataSizeLast; //上次的波形数据长度

};
