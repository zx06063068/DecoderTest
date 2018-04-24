//
// Created by zhangxin on 18-4-24.
//

#include "PhiDecoder.h"
#include <android/log.h>

#define TAG "phiFFmpegDecoder"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
extern "C" {
#include <libavutil/frame.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

static int writeFrameToJPEG(AVFrame *pFrame, const char *path, int width, int height) {
    LOGD("----------SaveFrameToJPEG width:%d  height:%d", width, height);
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    pFormatCtx->oformat = av_guess_format("mjpeg", NULL, NULL);
    if (avio_open(&pFormatCtx->pb, path, AVIO_FLAG_READ_WRITE) < 0) {
        LOGD("Couldn't open output file.");
        return -1;
    }
    AVStream *pAVStream = avformat_new_stream(pFormatCtx, 0);
    if (pAVStream == NULL) {
        return -1;
    }

    AVCodec *pCodec = avcodec_find_encoder(pFormatCtx->oformat->video_codec);
    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL) {
        return -1;
    }
    LOGD("writeFrameToJPEG pCodecCtx->pix_fmt %d", AV_PIX_FMT_YUVJ420P);

    pCodecCtx->codec_id = pFormatCtx->oformat->video_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    pCodecCtx->width = width;
    pCodecCtx->height = height;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;
    // Begin Output some information
    av_dump_format(pFormatCtx, 0, path, 1);


    // 设置pCodecCtx的解码器为pCodec
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGD("Could not open codec.");
        return -1;
    }
    //Write Header
    avformat_write_header(pFormatCtx, NULL);
    int y_size = pCodecCtx->width * pCodecCtx->height;
    //Encode
    // 给AVPacket分配足够大的空间
    AVPacket pkt;
    av_new_packet(&pkt, y_size * 3);
    int ret;
    ret = avcodec_send_frame(pCodecCtx, pFrame);
    if (ret < 0) {
        LOGD("Error sending a frame for encoding\n");
    } else {
        while (ret >= 0) {
            ret = avcodec_receive_packet(pCodecCtx, &pkt);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            else if (ret < 0) {
                LOGD("Error during encoding\n");
                break;
            }
            LOGD("av_write_frame");
            av_write_frame(pFormatCtx, &pkt);
            av_packet_unref(&pkt);
        }
    }
    //Write Trailer
    av_write_trailer(pFormatCtx);
    LOGD("Encode Successful.out_file:%s", path);
    avcodec_close(pCodecCtx);
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);
    return 0;
}
static int open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx,
                              enum AVMediaType type,
                              const char *path) {

    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0) {
        LOGD("Could not find %s stream in input file '%s'\n",
             av_get_media_type_string(AVMEDIA_TYPE_VIDEO), path);
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            LOGD("Failed to find %s codec\n",
                 av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            LOGD("Failed to allocate the %s codec context\n",
                 av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            LOGD("Failed to copy %s codec parameters to decoder context\n",
                 av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
            return ret;
        }
        if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0) {
            LOGD("Failed to open %s codec\n",
                 av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
            return ret;
        }
        *stream_idx = stream_index;
    }
    return 0;
}

static int ScaleImg(AVCodecContext *pCodecCtx, AVFrame *src_picture, AVFrame *dst_picture) {
    struct SwsContext *convert_ctx;
    uint8_t *pBuffer;
    int numBytes;
    int destw = pCodecCtx->width;
    int desth = pCodecCtx->height;
    if (destw > 252 || desth > 252) {
        int sca = destw / 252 > desth / 252 ? destw / 252 : desth / 252;
        destw = destw / sca;
        desth = desth / sca;
    }
    numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUVJ420P, destw,
                                        desth, 1);
    pBuffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(dst_picture->data, dst_picture->linesize, pBuffer,
                         AV_PIX_FMT_YUVJ420P, destw,
                         desth, 1);
    convert_ctx = sws_getContext(
            pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
            destw, desth,
            AV_PIX_FMT_YUVJ420P, SWS_BICUBIC, NULL, NULL, NULL);
    sws_scale(convert_ctx, (uint8_t const *const *) src_picture->data,
              src_picture->linesize, 0,
              src_picture->height, dst_picture->data, dst_picture->linesize);
    dst_picture->format = AV_PIX_FMT_YUVJ420P;
    LOGD("ScaleImg pCodecCtx->pix_fmt %d", pCodecCtx->pix_fmt);
    LOGD("ScaleImg dst_picture->format  %d", dst_picture->format);
    dst_picture->width = destw;
    dst_picture->height = desth;
    sws_freeContext(convert_ctx);
    return 0;
}


static int decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                  const char *filename) {
    int ret;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        LOGD("Error sending a packet for decoding\n");
        return ret;
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        else if (ret < 0) {
            LOGD("Error during decoding\n");
            return ret;
        }
        if (frame->pict_type == AV_PICTURE_TYPE_I) {
            LOGD("saving frame %3d\n", dec_ctx->frame_number);

            AVFrame *pFrameRGB = NULL;
            pFrameRGB = av_frame_alloc();
            ScaleImg(dec_ctx, frame, pFrameRGB);
            writeFrameToJPEG(pFrameRGB, filename, pFrameRGB->width, pFrameRGB->height);
            return 1;
        }
    }
    return 0;
}


int processFile(const char *inputpath, const char *outputpath, PhiMediaScannerClient &phiMediaScannerClient) {
    AVFormatContext *fmt_ctx = NULL;
    AVCodecContext *video_dec_ctx = NULL;
    AVStream *video_stream = NULL;
    AVPacket pkt;
    AVDictionaryEntry *tag = NULL;
    AVFrame *frame;
    int64_t mid = 0;
    int ret = 0;
    int video_stream_idx = -1;

    if ((ret = avformat_open_input(&fmt_ctx, inputpath, NULL, NULL))) {
        LOGD("avformat_open_input fail");
        return ret;
    }
    /* retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        LOGD("Could not find stream information\n");
        goto end2;
    }

    if (fmt_ctx->duration != AV_NOPTS_VALUE) {

        int hours, mins, secs, us;
        int64_t duration = fmt_ctx->duration + 5000;
        mid = duration / 2;
        secs = duration / AV_TIME_BASE;
        phiMediaScannerClient.setDuration(secs);
        us = duration % AV_TIME_BASE;
        mins = secs / 60;
        secs %= 60;
        hours = mins / 60;
        mins %= 60;
        LOGD("%02d:%02d:%02d.%02d\n", hours, mins, secs, (100 * us) / AV_TIME_BASE);
    }
    while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
        LOGD("%s=%s\n", tag->key, tag->value);

    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO,
                           inputpath) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];
    }

    if (!video_stream) {
        LOGD("Could not find video stream in the input, aborting\n");
        ret = 1;
        goto end2;
    }
    /* dump input information to stderr */
    av_dump_format(fmt_ctx, 0, inputpath, 0);
    av_seek_frame(fmt_ctx, -1, mid, AVSEEK_FLAG_BACKWARD);
    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    frame = av_frame_alloc();
    if (!frame) {
        LOGD("Could not allocate video frame\n");
        goto end1;
    }

    /* read frames from the file */
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == video_stream_idx)
            ret = decode(video_dec_ctx, frame, &pkt, outputpath);
        if (ret != 0)
            break;
    }

    /* flush cached frames */
    decode(video_dec_ctx, frame, NULL, "");


    av_frame_free(&frame);
    end1:
    avcodec_free_context(&video_dec_ctx);
    end2:
    avformat_close_input(&fmt_ctx);
    return ret;
}
}