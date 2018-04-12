#pragma once
#include <QObject>
#include <QScopedPointer>

class WavPlayerPrivate;
class WavFile;
class WavPlayer : public QObject
{
    Q_OBJECT
public:
    WavPlayer();
    ~WavPlayer();

    /**
     * @brief isPlaying check whehter is in playing status
     * @return
     */
    bool isPlaying() const;

    /**
     * @brief isValid check whether the player is valid
     * @return
     */
    bool isValid() const;

public slots:

    /**
     * @brief setVolume set the volume, in range of 0~100
     * @param vol volume
     */
    void setVolume(int vol);

    /**
     * @brief play play a wavfile object
     * @param wavfile wav file object
     */
    void play(WavFile *wavfile);

    /**
     * @brief stop stop current playing wav file
     */
    void stop();

signals:
    /**
     * @brief finished finished signal
     */
    void finished();

private:
    Q_DISABLE_COPY(WavPlayer)
    QScopedPointer<WavPlayerPrivate> d_ptr;
};
