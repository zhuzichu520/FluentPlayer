#include "FluentPlayer.h"

#include <QDebug>
#include <QVariant>
#include <QThreadPool>
#include <QAudioFormat>

Q_DECLARE_METATYPE(QMargins)

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#else
#include <QAudioSink>
#include <QAudioDevice>
#include <QMediaDevices>
#endif

#include <QTimerEvent>
#include <QDateTime>
#include <QTimer>

const int MaxAudioFrameSize = 192000;
const int MaxFrameCacheSize = 32;

extern "C" {
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/samplefmt.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
}

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)

QAbstractVideoSurface* FluentPlayer::videoSurface(){
    return m_videoSink;
}

void FluentPlayer::setVideoSurface(QAbstractVideoSurface *surface){
    if (m_videoSink && m_videoSink != surface  && m_videoSink->isActive()) {
        m_videoSink->stop();
    }
    m_videoSink = surface;
    if (m_videoSink && m_format.isValid())
    {
        m_format = m_videoSink->nearestFormat(m_format);
        m_videoSink->start(m_format);
    }
    loadSource(false);
}

#endif

FluentPlayer::FluentPlayer(QObject *parent)
    : QObject{parent}
{
    position(0);
    duration(0);
    volume(1);
    speed(1.0);
    playing(false);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    connect(this,&FluentPlayer::sendVideoFrame,this,[this](const QVideoFrame &frame){
        if(m_videoSink){
            m_videoSink->present(frame);
        }
    });
#else
    connect(this,&FluentPlayer::videoOutputChanged,this,[this](){
        m_videoSink = _videoOutput->property("videoSink").value<QVideoSink*>();
        loadSource(false);
    });
#endif
    connect(this,&FluentPlayer::positionChanged,this,[this](){
        if(position() == duration()){
            stop();
        }
    });
    connect(this,&FluentPlayer::speedChanged,this,[this](){
        seek(position());
    });
    connect(this,&FluentPlayer::firstVideoFrameCompeletd,this,[this](QVideoFrame frame){
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
        QVideoSurfaceFormat format(frame.size(), frame.pixelFormat());
        m_videoSink->stop();
        m_videoSink->start(format);
        QTimer::singleShot(100,this,[this,frame]{updateVideoFrame(frame);});
#endif
        updateVideoFrame(frame);
    });
    startTimer(1000);
}

FluentPlayer::~FluentPlayer(){
    stop();
}

void FluentPlayer::timerEvent(QTimerEvent *event){
    //    qDebug()<<m_fpsCount;
    m_fpsCount = 0;
}

void FluentPlayer::play(){
    if(!_playing && m_threading){
        playing(true);
        return;
    }
    loadSource(true);
}

void FluentPlayer::pause(){
    playing(false);
}

void FluentPlayer::stop(){
    playing(false);
    stopThreadAwit();
}

void FluentPlayer::stopThreadAwit(){
    if(m_threadCount == 0){
        return;
    }
    if(m_threading){
        m_threading = false;
    }
    while(!(m_threading == false && m_threadCompleted == true)){
        QThread::msleep(10);
    }
}

void FluentPlayer::seek(qint64 seek){
    stopThreadAwit();
    cleanVideoFrame();
    cleanAudioFrame();
    m_seek = seek;
    position(seek);
    startDeocde(seek);
}

void FluentPlayer::checkThreadCompleted(){
    m_threadCount-=1;
    if(m_threadCount == 0){
        m_threading = false;
        m_threadCompleted = true;
        qDebug()<<"all thread finished";
    }
}

QString FluentPlayer::getUrl(){
    QString url(_source.toString());
    if(_source.isLocalFile()){
        url = _source.toLocalFile();
    }
    return url;
}

void FluentPlayer::updateVideoFrame(QSharedPointer<QVideoFrame> frame){
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    Q_EMIT sendVideoFrame(*frame);
#else
    m_videoSink->setVideoFrame(*frame);
#endif
}

void FluentPlayer::updateVideoFrame(const QVideoFrame& frame){
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    Q_EMIT sendVideoFrame(frame);
#else
    m_videoSink->setVideoFrame(frame);
#endif
}

void FluentPlayer::startDeocde(qint64 seek){
    QString url(_source.toString());
    if(_source.isLocalFile()){
        url = _source.toLocalFile();
    }
    m_threading = true;
    m_threadCompleted = false;
    m_clockMilliseconds = 0;

    m_threadCount+=1;
    QThreadPool::globalInstance()->start([this,seek](){doInWorkVideoDecode(seek);});

    m_threadCount+=1;
    QThreadPool::globalInstance()->start([this,seek](){doInWorkAudioDecode(seek);});

    m_threadCount+=1;
    QThreadPool::globalInstance()->start([this](){doInWorkVideoRender();});

    m_threadCount+=1;
    QThreadPool::globalInstance()->start([this](){doInWorkAudioRender();});
}

void FluentPlayer::doInWorkVideoRender(){
    auto weakThis = QPointer<FluentPlayer>(this);
    while(weakThis && m_threading){
        while(weakThis && m_threading && _playing){
            while(weakThis && m_threading && _playing && m_clockMilliseconds==0){}
            auto frame = getVideoFrame();
            if(frame){
                bool isSkip = false;
                while(weakThis && m_threading &&  _playing){
                    qint64 delay = frame->startTime() - m_clockMilliseconds;
                    if(delay<-100){
                        isSkip = true;
                        break;
                    }
                    if(delay<0){
                        break;
                    }
                    delay = delay > 1 ? 1 : delay;
                    QThread::msleep(delay);
                }
                if(weakThis && m_threading && frame && !isSkip){
                    updateVideoFrame(frame);
                    m_fpsCount++;
                    position(m_clockMilliseconds);
                    if(frame->endTime() >= duration()){
                        position(duration());
                    }
                }
            }
        }
    }
    if(weakThis){
        checkThreadCompleted();
    }
    qDebug()<< "Video render thread completed";
}
void FluentPlayer::doInWorkAudioRender(){
    auto weakThis = QPointer<FluentPlayer>(this);
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(2);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    QAudioDeviceInfo device(QAudioDeviceInfo::defaultOutputDevice());
    using AudioSink = QAudioOutput;
#else
    format.setSampleFormat(QAudioFormat::Int16);
    QAudioDevice device = QMediaDevices::defaultAudioOutput();
    using AudioSink = QAudioSink;
#endif
    if(!device.isFormatSupported(format)){
        format = device.preferredFormat();
    }
    auto audioSink = new AudioSink(format);
    audioSink->setVolume(volume());
    auto audioStream = audioSink->start();
    auto closeAudio = [&audioStream,&audioSink](){
        if(audioStream){
            audioStream->close();
            audioStream = nullptr;
        }
        if(audioSink){
            audioSink->deleteLater();
            audioSink = nullptr;
        }
    };
    while(weakThis && m_threading){
        while(weakThis && m_threading && _playing){
            auto frame = getAudioFrame();
            if(frame){
                auto delay = frame->endTime - frame->startTime;
                if(audioStream){
                    while(audioSink->bytesFree() < frame->count){}
                    if(audioSink->volume() != volume()){
                        audioSink->setVolume(volume());
                    }
                    audioStream->write((const char *)frame->pcm,frame->count);
                }
                QThread::msleep(delay);
                if(weakThis){
                    m_clockMilliseconds = m_seek+ (frame->endTime-m_seek+delay)*speed();
                }
            }
        }
    }
    if(weakThis){
        closeAudio();
        checkThreadCompleted();
    }
    qDebug()<< "Audio render thread completed";
};

void FluentPlayer::doInWorkVideoDecode(qint64 seek){
    auto weakThis = QPointer<FluentPlayer>(this);
    auto url = getUrl();
    AVFormatContext *formatCtx = nullptr;
    AVStream *videoStream = nullptr;
    AVCodecContext *videoCodecCtx = nullptr;
    int videoStreamIndex = -1;
    auto release = [&formatCtx,&videoCodecCtx,&videoStream](){
        if(formatCtx){
            avformat_close_input(&formatCtx);
        }
        if(videoCodecCtx){
            avcodec_free_context(&videoCodecCtx);
        }
        formatCtx = nullptr;
        videoStream = nullptr;
        videoCodecCtx = nullptr;
    };
    auto init = [this,release,&formatCtx,&videoCodecCtx,&videoStream,&url,&videoStreamIndex,seek]{
        formatCtx = avformat_alloc_context();
        if (!formatCtx) {
            release();
            qWarning()<<"Could not allocate AVFormatContext";
            return false;
        }
        int ret = avformat_open_input(&formatCtx,url.toUtf8(),NULL, NULL);;
        if(ret<0){
            release();
            qWarning()<<"Unable to open input file";
            return false;
        }
        ret = avformat_find_stream_info(formatCtx, nullptr);
        if(ret<0){
            release();
            qWarning()<<"Cannot find stream information";
            return false;
        }
        av_dump_format(formatCtx, 0, url.toUtf8(), 0);
        int streamSize = formatCtx->nb_streams;
        for(int i=0;i<streamSize; i++){
            auto type = formatCtx->streams[i]->codecpar->codec_type;
            if(type == AVMEDIA_TYPE_VIDEO){
                videoStreamIndex=i;
                break;
            }
        }
        if(videoStreamIndex == -1){
            release();
            qWarning()<<"Cannot find video stream";
            return false;
        }
        videoStream = formatCtx->streams[videoStreamIndex];
        if(seek != 0){
            qint64 position = qMin((qint64)(seek),(qint64)duration()-1000)/(av_q2d(videoStream->time_base)*1000);
            av_seek_frame(formatCtx, videoStreamIndex, position, AVSEEK_FLAG_BACKWARD);
        }
        const AVCodec * videoCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
        if (videoCodec == nullptr) {
            release();
            qWarning()<<"Cannot find video decoder";
            return false;
        }
        videoCodecCtx = avcodec_alloc_context3(videoCodec);
        ret = avcodec_parameters_to_context(videoCodecCtx,videoStream->codecpar);
        if (ret<0){
            release();
            qWarning()<<"Cannot find video codec parameters";
            return false;
        }
        ret = avcodec_open2(videoCodecCtx,videoCodec,NULL);
        if(ret < 0){
            release();
            qWarning()<<"Cannot Open video codec";
            return false;
        }
        duration(formatCtx->duration * 1000 / AV_TIME_BASE);
        return true;
    };
    if(!init()){
        qWarning()<<"Error video init";
        checkThreadCompleted();
        return;
    }
    AVPacket *packet = av_packet_alloc();
    AVFrame *avframe = av_frame_alloc();
    bool isFirst = true;
    QSharedPointer<QVideoFrame> frame;
    while(weakThis && m_threading && av_read_frame(formatCtx, packet)>=0) {
        if(packet->stream_index == videoStreamIndex){
            if (avcodec_send_packet(videoCodecCtx, packet) == 0)
            {
                while (avcodec_receive_frame(videoCodecCtx, avframe) == 0)
                {
                    if(avframe->format == AV_PIX_FMT_YUV420P){
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                        int frameBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, avframe->width, avframe->height, 1);
                        frame = QSharedPointer<QVideoFrame>(new QVideoFrame(frameBytes,QSize(avframe->width,avframe->height),avframe->width,QVideoFrame::PixelFormat::Format_YUV420P));
                        if (frame->map(QAbstractVideoBuffer::WriteOnly))
                        {
                            uchar * fdata = frame->bits();
                            av_image_copy_to_buffer(fdata, frameBytes, avframe->data, avframe->linesize, AV_PIX_FMT_YUV420P, avframe->width, avframe->height, 1);
                        }
#else
                        QVideoFrameFormat format(QSize( avframe->width, avframe->height), QVideoFrameFormat::Format_YUV420P);
                        format.setViewport(QRect(0, 0, avframe->width, avframe->height));
                        frame = QSharedPointer<QVideoFrame>(new QVideoFrame(format));
                        if (frame->map(QVideoFrame::WriteOnly))
                        {
                            uint8_t *srcData[3] = { avframe->data[0], avframe->data[1], avframe->data[2] };
                            int srcLinesize[3] = { avframe->linesize[0], avframe->linesize[1], avframe->linesize[2] };
                            uint8_t *dstData[3] = { frame->bits(0), frame->bits(1), frame->bits(2) };
                            int dstLinesize[3] = { frame->bytesPerLine(0), frame->bytesPerLine(1), frame->bytesPerLine(2) };
                            av_image_copy2(dstData, dstLinesize, srcData, srcLinesize, AV_PIX_FMT_YUV420P, frame->width(), frame->height());
                        }
#endif
                        frame->setStartTime(avframe->pts*av_q2d(videoStream->time_base)*1000);
                        frame->setEndTime((avframe->pts+avframe->duration)*av_q2d(videoStream->time_base)*1000);
                        frame->unmap();
                        if(weakThis){
                            if(isFirst){
                                Q_EMIT firstVideoFrameCompeletd(*frame);
                                isFirst = false;
                            }
                            weakThis->enqueueVideoFrame(frame);
                            while(weakThis && m_threading && m_videoFrameCache.size() >= MaxFrameCacheSize){}
                        }
                    }
                }
            }
        }
        av_packet_unref(packet);
    }
    av_packet_free(&packet);
    av_frame_free(&avframe);
    release();
    if(weakThis){
        if(!frame.isNull()){
            frame->setEndTime(_duration);
        }
        checkThreadCompleted();
    }
    qDebug()<< "Video decode thread completed";

}
void FluentPlayer::doInWorkAudioDecode(qint64 seek){
    auto weakThis = QPointer<FluentPlayer>(this);
    auto url = getUrl();
    AVFormatContext *formatCtx = nullptr;
    AVStream *audioStream = nullptr;
    AVCodecContext *audioCodecCtx = nullptr;
    int audioStreamIndex = -1;
    SwrContext *swrCtx = nullptr;
    AVFilterGraph *graph = nullptr;
    AVFilterContext *bufferCtx = nullptr;
    AVFilterContext *sinkCtx = nullptr;
    int channels = 0;
    enum AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;
    auto release = [&formatCtx,&audioCodecCtx,&audioStream,&graph,&swrCtx,&bufferCtx,&sinkCtx](){
        if(formatCtx){
            avformat_close_input(&formatCtx);
        }
        if(audioCodecCtx){
            avcodec_free_context(&audioCodecCtx);
        }
        if(graph){
            avfilter_graph_free(&graph);
        }
        if(swrCtx){
            swr_free(&swrCtx);
        }
        formatCtx = nullptr;
        audioStream = nullptr;
        audioCodecCtx = nullptr;
        graph = nullptr;
        swrCtx = nullptr;
        bufferCtx = nullptr;
        sinkCtx = nullptr;
    };
    auto init = [this,release,&formatCtx,&audioCodecCtx,&audioStream,&graph,&swrCtx,&bufferCtx,&sinkCtx,&url,&audioStreamIndex,&outSampleFmt,&channels,seek]{
        formatCtx = avformat_alloc_context();
        if (!formatCtx) {
            release();
            qWarning()<<"Could not allocate AVFormatContext";
            return false;
        }
        int ret = avformat_open_input(&formatCtx,url.toUtf8(),NULL, NULL);;
        if(ret<0){
            release();
            qWarning()<<"Unable to open input file";
            return false;
        }
        ret = avformat_find_stream_info(formatCtx, nullptr);
        if(ret<0){
            release();
            qWarning()<<"Cannot find stream information";
            return false;
        }
        int streamSize = formatCtx->nb_streams;
        for(int i=0;i<streamSize; i++){
            auto type = formatCtx->streams[i]->codecpar->codec_type;
            if(type == AVMEDIA_TYPE_AUDIO){
                audioStreamIndex = i;
                break;
            }
        }
        if(audioStreamIndex == -1){
            release();
            qWarning()<<"Cannot find audio stream";
            return false;
        }
        audioStream = formatCtx->streams[audioStreamIndex];
        const AVCodec * audioCodec = avcodec_find_decoder(audioStream->codecpar->codec_id);
        if (audioCodec == nullptr) {
            release();
            qWarning()<<"Cannot find audio decoder";
            return false;
        }
        audioCodecCtx = avcodec_alloc_context3(audioCodec);
        ret = avcodec_parameters_to_context(audioCodecCtx,audioStream->codecpar);
        if (ret<0){
            release();
            qWarning()<<"Cannot find audio codec parameters";
            return false;
        }
        ret = avcodec_open2(audioCodecCtx, audioCodec,NULL);
        if(ret < 0){
            release();
            qWarning()<<"Cannot audio video codec";
            return false;
        }
        duration(formatCtx->duration * 1000 / AV_TIME_BASE);
        auto sampleRate = audioCodecCtx->sample_rate;
        channels = audioCodecCtx->ch_layout.nb_channels;
        auto outChannelLayout = audioCodecCtx->ch_layout;
        ret = swr_alloc_set_opts2(&swrCtx,&outChannelLayout,outSampleFmt,sampleRate,&audioCodecCtx->ch_layout,audioCodecCtx->sample_fmt,audioCodecCtx->sample_rate,0,nullptr);
        if(ret < 0){
            release();
            qWarning()<<"Could not allocate SwrContext";
            return false;
        }
        ret =swr_init(swrCtx);
        if(ret < 0){
            release();
            qWarning()<<"Error SwrContext init";
            return false;
        }
        if(seek != 0){
            qint64 position = qMin((qint64)(seek),(qint64)duration()-1000)/(av_q2d(audioCodecCtx->time_base)*1000);
            av_seek_frame(formatCtx, audioStreamIndex, position, AVSEEK_FLAG_BACKWARD);
        }
        std::string sampleRateStr = std::to_string(audioCodecCtx->sample_rate);
        std::string sampleFmtStr = std::string(av_get_sample_fmt_name(audioCodecCtx->sample_fmt));
        std::string sampleChannelStr;
        if (audioCodecCtx->ch_layout.nb_channels == 1) {
            sampleChannelStr = "mono";
        } else {
            sampleChannelStr = "stereo";
        }
        std::string abufferString = "sample_rate=" + sampleRateStr + ":sample_fmt=" + sampleFmtStr + ":channel_layout=" + sampleChannelStr;
        std::string aformatString = "sample_fmts=" + sampleFmtStr + ":sample_rates=" + sampleRateStr + ":channel_layouts=" + sampleChannelStr;
        graph = avfilter_graph_alloc();
        const AVFilter *abuffer = avfilter_get_by_name("abuffer");
        bufferCtx = avfilter_graph_alloc_filter(graph, abuffer, "src");
        if (avfilter_init_str(bufferCtx, abufferString.c_str()) < 0) {
            qWarning()<<"Error init abuffer filter";
            return false;
        }
        const AVFilter *atempo = avfilter_get_by_name("atempo");
        AVFilterContext *atempoCtx = avfilter_graph_alloc_filter(graph, atempo, "atempo");
        AVDictionary *argsA = nullptr;
        av_dict_set(&argsA, "tempo", std::to_string(speed()).c_str(), 0);
        if (avfilter_init_dict(atempoCtx, &argsA) < 0) {
            qWarning()<<"Error init atempo filter";
            return false;
        }
        const AVFilter *aformat = avfilter_get_by_name("aformat");
        AVFilterContext *aformatCtx = avfilter_graph_alloc_filter(graph, aformat, "aformat");
        if (avfilter_init_str(aformatCtx, aformatString.c_str()) < 0) {
            qWarning()<<"Error init aformat filter";
            return false;
        }
        const AVFilter *sink = avfilter_get_by_name("abuffersink");
        sinkCtx = avfilter_graph_alloc_filter(graph, sink, "sink");
        if (avfilter_init_str(sinkCtx, NULL) < 0) {
            qWarning()<<"Error init sink filter";
            return false;
        }
        if (avfilter_link(bufferCtx, 0, atempoCtx, 0) != 0) {
            qWarning()<<"Error link to atempo filter";
            return false;
        }
        if (avfilter_link(atempoCtx, 0, aformatCtx, 0) != 0) {
            qWarning()<<"Error link to aformat filter";
            return false;
        }
        if (avfilter_link(aformatCtx, 0, sinkCtx, 0) != 0) {
            qWarning()<<"Error link to sink filter";
            return false;
        }
        if (avfilter_graph_config(graph, NULL) < 0) {
            qWarning()<<"Error config filter graph";
            return false;
        }
        return true;
    };
    if(!init()){
        qWarning()<<"Error audio init";
        checkThreadCompleted();
        return;
    }
    AVPacket *packet = av_packet_alloc();
    AVFrame *avframe = av_frame_alloc();
    while(weakThis && m_threading && av_read_frame(formatCtx, packet)>=0) {
        if(packet->stream_index == audioStreamIndex){
            if (avcodec_send_packet(audioCodecCtx, packet) == 0){
                while (avcodec_receive_frame(audioCodecCtx, avframe) == 0){
                    if (av_buffersrc_add_frame(bufferCtx, avframe) < 0) {
                        av_log(NULL, AV_LOG_ERROR, "Failed to allocate filtered frame\n");
                        break;
                    }
                    while(true){
                        int ret = av_buffersink_get_frame(sinkCtx, avframe);
                        if(ret<0){
                            break;
                        }
                        if(av_sample_fmt_is_planar(audioCodecCtx->sample_fmt)){
                            QSharedPointer<QAudioFrame> frame(new QAudioFrame());
                            frame->pcm = (uint8_t *) av_malloc(MaxAudioFrameSize * 2);
                            int len = swr_convert(swrCtx,&frame->pcm,MaxAudioFrameSize * 2,(const uint8_t **) avframe->data,avframe->nb_samples);
                            if (len <= 0)
                            {
                                continue;
                            }
                            int count = av_samples_get_buffer_size(nullptr,channels,len,outSampleFmt,1);
                            frame->count = count;
                            frame->startTime = avframe->pts*av_q2d(audioCodecCtx->time_base)*1000;
                            frame->endTime = (avframe->pts+avframe->duration)*av_q2d(audioCodecCtx->time_base)*1000;
                            if(weakThis){
                                weakThis->enqueueAudioFrame(frame);
                                while(weakThis && m_threading && m_audioFrameCache.size() >= MaxFrameCacheSize){}
                            }
                        }
                    }
                }
            }
        }
        av_packet_unref(packet);
    }
    av_packet_free(&packet);
    av_frame_free(&avframe);
    release();
    if(weakThis){
        checkThreadCompleted();
    }
    qDebug()<< "Audio decode thread completed";
}

QSharedPointer<QVideoFrame> FluentPlayer::getVideoFrame(){
    QMutexLocker locker(&m_videoMutex);
    if(m_videoFrameCache.isEmpty()){
        return nullptr;
    }
    return m_videoFrameCache.dequeue();
}

QSharedPointer<QAudioFrame> FluentPlayer::getAudioFrame(){
    QMutexLocker locker(&m_audioMutex);
    if(m_audioFrameCache.isEmpty()){
        return nullptr;
    }
    return m_audioFrameCache.dequeue();
}

void FluentPlayer::cleanVideoFrame(){
    QMutexLocker locker(&m_videoMutex);
    m_videoFrameCache.clear();
}

void FluentPlayer::cleanAudioFrame(){
    QMutexLocker locker(&m_audioMutex);
    m_audioFrameCache.clear();
}

void FluentPlayer::enqueueVideoFrame(QSharedPointer<QVideoFrame> frame){
    QMutexLocker locker(&m_videoMutex);
    m_videoFrameCache.enqueue(frame);
}

void FluentPlayer::enqueueAudioFrame(QSharedPointer<QAudioFrame> frame){
    QMutexLocker locker(&m_audioMutex);
    m_audioFrameCache.enqueue(frame);
}

void FluentPlayer::loadSource(bool autoPlay,qint64 seek){
    if(m_threading){
        return;
    }
    cleanVideoFrame();
    cleanAudioFrame();
    position(0);
    duration(0);
    playing(autoPlay);
    startDeocde(seek);
}
