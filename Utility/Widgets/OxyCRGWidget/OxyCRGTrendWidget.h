#pragma once
#include "WaveWidget.h"
#include "RingBuff.h"

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
     * @param value  缓存的数据
     * @param flag  缓存的标记位
     */
    void addDataBuf(int value, int flag);

private slots:
    /**
     * @brief onTimeout  用于将波形数据重新放回waveBuf中
     */
    void onTimeout();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    virtual void showEvent(QShowEvent * event );
    virtual void hideEvent(QHideEvent * event );
    OxyCRGTrendWidgetRuler *_ruler;          // 标尺对象

    RingBuff<int> *_falgBuf;//给波形打标记的缓存
    RingBuff<int> *_dataBuf;//波形数据缓存
    int _dataBufIndex; //波形数据缓存下标
    int _dataBufLen; //波形数据长度
    int _dataSizeLast; //上次的波形数据长度

};
