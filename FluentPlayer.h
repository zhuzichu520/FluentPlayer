#ifndef FLUENTPLAYER_H
#define FLUENTPLAYER_H

#include <QObject>
#include <QUrl>
#include <QPointer>
#include <QVideoFrame>
#include <QSharedPointer>
#include <QQueue>
#include <QMutex>
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#else
#include <QVideoSink>
#endif

#include "stdafx.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <libavfilter/avfilter.h>
}

class QAudioFrame{
public:
    QAudioFrame(){};
    ~QAudioFrame(){
        if(pcm){
            av_free(pcm);
            pcm = nullptr;
        }
    };
    unsigned char *pcm = nullptr;
    int count = 0;
    qint64 startTime = 0;
    qint64 endTime = 0;
};

class FluentPlayer : public QObject
{
    Q_OBJECT
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface CONSTANT)
#else
    Q_PROPERTY_AUTO(QObject*,videoOutput)
#endif
    Q_PROPERTY_AUTO(QUrl,source)
    Q_PROPERTY_AUTO(int,volume)
    Q_PROPERTY_READONLY_AUTO(double,duration)
    Q_PROPERTY_READONLY_AUTO(double,position)
    Q_PROPERTY_READONLY_AUTO(bool,playing)
public:
    explicit FluentPlayer(QObject *parent = nullptr);
    ~FluentPlayer();
    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void seek(qint64 seek);
protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void loadSource(bool autoPlay,qint64 seek = 0);
    QSharedPointer<QVideoFrame> getVideoFrame();
    QSharedPointer<QAudioFrame> getAudioFrame();
    void cleanVideoFrame();
    void cleanAudioFrame();
    void enqueueVideoFrame(QSharedPointer<QVideoFrame> frame);
    void enqueueAudioFrame(QSharedPointer<QAudioFrame> frame);
    void startDeocde(qint64 seek);
    void stopThreadAwit();
    void doInWorkVideoRender();
    void doInWorkAudioRender();
    void doInWorkVideoDecode(qint64 seek);
    void doInWorkAudioDecode(qint64 seek);
    void updateVideoFrame(QSharedPointer<QVideoFrame> frame);
    QString getUrl();
    void checkThreadCompleted();
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    QAbstractVideoSurface* videoSurface();
    void setVideoSurface(QAbstractVideoSurface *surface);
    void setVideoFormat(int width, int heigth, QVideoFrame::PixelFormat format = QVideoFrame::PixelFormat::Format_YUV420P);
#endif
private:
    QQueue<QSharedPointer<QVideoFrame>> m_videoFrameCache;
    QQueue<QSharedPointer<QAudioFrame>> m_audioFrameCache;
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    QAbstractVideoSurface *m_videoSink = nullptr;
    QVideoSurfaceFormat m_format;
#else
    QPointer<QVideoSink> m_videoSink = nullptr;
#endif
    qint64 m_clockMilliseconds = 0;
    int m_threading = false;
    int m_threadCount = 0;
    int m_fpsCount = 0;
    bool m_threadCompleted = false;
    QMutex m_videoMutex;
    QMutex m_audioMutex;
    int m_channels = 0;
    int m_sampleRate = 0;
};

#endif // FLUENTPLAYER_H
