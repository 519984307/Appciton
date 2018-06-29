////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件工作于回顾模式时, 用于获取数据的模型
////////////////////////////////////////////////////////////////////////////////
#ifndef WAVE_DATA_MODEL_H
#define WAVE_DATA_MODEL_H

#include <QObject>
#include <QVariant>

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形数据信息类型
////////////////////////////////////////////////////////////////////////////////
enum WaveDataInfoType {
	WAVA_IS_VALID,                 // 波形数据是否有效, 期待bool类型
	WAVE_STARTUP_TIME,             // 波形启始时刻, 期待QDateTime类型
	WAVE_DURATION,                 // 波形持续长时, 单位秒, 期待int类型
	WAVE_SAMPLE_RATE,              // 波形采样率, 单位Hz, 期待float类型
	WAVE_HAS_RULER,                // 波形幅度是否由标尺定义, 期待bool类型, 说明:
								   // 若为true, 则需要提供WAVE_RULER_*相关信息
								   // 若为false, 则需要提供WAVE_RANGE_*相关信息
	WAVE_RULER_HEIGHT,             // 标尺高度, 单位像素, 期待int类型
	WAVE_RULER_UPPER,              // 标尺上边界对应的采样值, 期待int类型
	WAVE_RULER_LOWER,              // 标尺下边界对应的采样值, 期待int类型
	WAVE_RANGE_MAX,                // 波形控件上边界对应的采样值, 期待int类型
	WAVE_RANGE_MIN                 // 波形控件下边界对应的采样值, 期待int类型
};

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 波形控件工作于回顾模式时, 用于获取数据的模型
////////////////////////////////////////////////////////////////////////////////
class WaveDataModel: public QObject {
		Q_OBJECT
	public:
        explicit WaveDataModel(QObject *parent): QObject(parent) {}
		// 获取待回顾波形数据的各种信息
		virtual QVariant waveDataInfo(WaveDataInfoType type) = 0;
		// 获取波形数据
		virtual int readWaveData(int index, int *buff, int size) = 0;

	public slots:
		// 通知关联的波形控件刷新数据
		void update() {
			emit modelChanged();
		}

	signals:
		// 波形数据或信息有变
		void modelChanged();
};

#endif
