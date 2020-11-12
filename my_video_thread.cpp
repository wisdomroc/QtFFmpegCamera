#include "my_video_thread.h"
#include <QApplication>
#include <QTime>
#include <QDebug>
#include <QThread>
#include <QImage>
#include <QCameraInfo>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
}

MyVideoThread::MyVideoThread(QObject *parent)
    : QThread(parent)
{
}

void MyVideoThread::openCamera()
{
    QCameraInfo camerainfo = QCameraInfo::defaultCamera();
    QString devicename = camerainfo.description();
    _cameraName = "video=" + devicename;
    qDebug() << "use camera: " << devicename;

    start();
}

void MyVideoThread::run()
{
    AVFormatContext *pFormatCtx;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;

    //注册ffmpeg音视频设备
    av_register_all();
    avformat_network_init();
    avdevice_register_all();

    //参数设置 设置的参数要看摄像头是否支持 如果不支持设置后是无法打开的 没有特别需求默认打开即可
    //比如切换分辨率的时候 就需要设置对应的framerate 否则打开错误，可以使用amcap 工具打开摄像头查看不同分辨率对应的framerate
    //在比如专业的摄像头支持的分辨率比较高800万 这个时候要设置摄像头的format格式为mjpeg 会比较流畅
    AVDictionary* pOptions = NULL;
    //设置编码格式
    //av_dict_set(&pOptions, "input_format", "mjpeg", 0);
    //设置分辨率
    //av_dict_set(&pOptions,"video_size","800x600",0);
    //设置帧率
    //av_dict_set(&pOptions,"framerate","30",0);

    std::string str = _cameraName.toStdString();
    // 打开摄像头
    pFormatCtx = avformat_alloc_context();
    //AVInputFormat *ifmt = av_find_input_format("vfwcap"); // vfwcap方式打开
    //if(avformat_open_input(&pFormatCtx, 0, ifmt, &pOptions))
    AVInputFormat *ifmt = av_find_input_format("dshow"); // dshow方式打开
    if(avformat_open_input(&pFormatCtx,str.data(),ifmt,&pOptions)!=0)
    {
        qDebug("Couldn't open input stream.\n");
        return;
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0)
    {
        qDebug("Couldn't find stream information.\n");
        return;
    }
    // 查找视频流id
    int videoindex = -1;
    for(int i=0; i < pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoindex=i;
            break;
        }
    }
    if(videoindex==-1)
    {
        qDebug("Couldn't find a video stream.\n");
        return;
    }
    // 获取对应视频流编码格式
    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL)
    {
        qDebug("Codec not found.\n");
        return;
    }
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)
    {
        qDebug("Could not open codec.\n");
        return;
    }

    AVFrame *pFrame,*pFrameRGB;
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    // 创建图片缓存
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width,pCodecCtx->height);
    uint8_t * rgbBuffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, rgbBuffer, AV_PIX_FMT_RGB32,pCodecCtx->width, pCodecCtx->height);

    // 创建包缓存
    int ret, got_picture;
    AVPacket *packet=(AVPacket *)av_malloc(sizeof(AVPacket));

    // 创建图片转换格式结构体RGB32
    struct SwsContext *img_convert_ctx;
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    while(1)// 循环读取视频帧数据、解析为QIamge 发送到主线程UI显示
    {
        if(av_read_frame(pFormatCtx, packet) >= 0){
            if(packet->stream_index==videoindex){
                ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
                if(ret < 0)
                {
                    qDebug("Decode Error.\n");
                    return;
                }
                if(got_picture)
                {
                    // AVFrame 转RGB32数据
                    sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                    QImage tmpImg((uchar *)rgbBuffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                    QImage img = tmpImg.copy();
                    emit capImg(img);
                    QThread::msleep(10);
                }
            }
            av_free_packet(packet);
        }
    }
    // 释放内存
    sws_freeContext(img_convert_ctx);
    av_free(rgbBuffer);
    av_free(pFrame);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}
